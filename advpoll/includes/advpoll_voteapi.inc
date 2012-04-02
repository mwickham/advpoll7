<?php

/*
 * Handles function calls related to voting and selection of votes.
 */

/**
 * Get all votes related to a node by content type and node id.
 *
 * @param $content_type
 *        $nid,
 *        $behavior (whether poll is tabulated approval (default) or pooled.
 * @return
 *   An array containing the key:
 *     'choices' which matches the index of each choice with its tallied votes.
 *     'total' which is the total of all votes
 */
function advpoll_get_votes($content_type, $nid, $behavior = 'approval') {
    $criteria = array();
    $criteria['entity_id'] = $nid;
    $criteria['entity_type'] = $content_type;

    $results = votingapi_select_votes($criteria);

    if ($behavior === 'approval') {
        $tabulated = advpoll_approval_vote($results);
    } else if ($behavior === 'pool') {
        $tabulated = advpoll_pooled_vote($results);
    } else {
        $tabulated = advpoll_default_vote($results);
    }

    return $tabulated;
}

/**
 * Tally votes by approval method.  This mainly matters for polls in which
 * user can vote on more than one choice.  Two votes submitted at the same time
 * only counts as one vote.
 *
 * @param $results - results returned from voting api.
 * @return
 *   Keyed array containing vote choices in descending order and total votes.
 *   $choices contains:
 *      index = the index of the choice
 *      percentage = percentage of votes choice received.
 *      votes = the number of votes received.
 *
 */
function advpoll_approval_vote($results) {
    $tallied = array();
    $source = array();
    $total = 0;

    foreach ($results as $result) {
        if (isset($tallied[$result['tag']])) {
            $tallied[$result['tag']]++;
        } else {
            $tallied[$result['tag']] = 1;
        }

        if (!in_array($result['timestamp'], $source)) {
            $source[] = $result['timestamp'];
        }
    }

    $total = count($source);
    $tabulated = advpoll_calculate_percentage($tallied, $total);

    return array('choices' => $tabulated, 'total' => $total);
}

/**
 * Tally votes by pooling them.  All votes, whether cast by single or multichoice
 * bear the same weight.
 *
 * @param $results - results returned from voting api.
 * @return
 *   Keyed array containing vote choices in descending order and total votes.
 *   $choices contains:
 *      index = the index of the choice
 *      percentage = percentage of votes choice received.
 *      votes = the number of votes received.
 *
 */
function advpoll_pooled_vote($results) {
    $tallied = array();
    $total = 0;
    foreach ($results as $result) {
        if (isset($tallied[$result['tag']])) {
            $tallied[$result['tag']]++;
        } else {
            $tallied[$result['tag']] = 1;
        }
        $total++;
    }

    $tabulated = advpoll_calculate_percentage($tallied, $total);

    return array('choices' => $tabulated, 'total' => $total);
}

/**
 * The default voting count uses the 'value' field returned by the voting API
 * to tally votes and arrive at percentages.
 *
 * @param $results - results returned from voting api.
 * @return
 *   Keyed array containing vote choices in descending order and total votes.
 *   $choices contains:
 *      index = the index of the choice
 *      percentage = percentage of votes choice received.
 *      votes = the number of votes received.
 *
 */
function advpoll_default_vote($results) {
    $tallied = array();
    $total = 0;
    $points = 0;
    $totalpoints = 0;
    foreach ($results as $result) {
        $points = (int) $result['value'];
        if (isset($tallied[$result['tag']])) {
            $tallied[$result['tag']] += $points;;
        } else {
            $tallied[$result['tag']] = $points;
        }
        $totalpoints += $points;
        $total++;
    }

    $tabulated = advpoll_calculate_percentage($tallied, $totalpoints);

    return array('choices' => $tabulated, 'total' => $total);
    
}

/**
 * Return keyed array with percentages for each index.
 *
 * @param $tallied - array containing totals per choice index
 *        $total - total number of votes
 * @return
 *   A keyed array containing:
 *   index = the index of the choice
 *   percentage = the percentage of votes
 *   votes = the number of votes received
 */
function advpoll_calculate_percentage($tallied, $total) {
    $tabulated = array();

    foreach ($tallied as $key => $value) {
        $percentage = floor((int) $value / (int) $total * 100);
        $tabulated[] = array('index' => $key, 'percentage' => $percentage, 'votes' => $value);
    }

    usort($tabulated, "advpoll_compare");
    $tabulated = array_reverse($tabulated);

    return $tabulated;
}

/*
 * callback for usort to order items by percentage.
 */

function advpoll_compare($a, $b) {
    if ($a['percentage'] > $b['percentage']) {
        return 1;
    } else if ($a['percentage'] < $b['percentage']) {
        return -1;
    } else {
        return 0;
    }
}

function advpoll_user_eligibility($node) {
    if (!user_access('vote on polls')) {
        return false;
    }

    global $user;
    $data = advpoll_get_data($node);

    if ($data->write_in && !user_access('add write-ins')) {
        return false;
    }

    if ($data->electoral) {
        if (!advpoll_check_electoral_list($user->uid, $node->nid)) {
            return false;
        }
    }

    if ($data->state !== 'open') {
        return false;
    }

    if ($data->start_date > time() || $data->end_date < time()) {
        return false;
    }

    if ($data->mode === 'cookie' && isset($_COOKIE[$node->type . $node->nid])) {
        return false;
    }

    if ($data->mode === 'normal') {
        $criteria = array();
        $criteria['entity_id'] = $node->nid;
        $criteria['entity_type'] = $node->type;

        $criteria['uid'] = $user->uid;

        $results = votingapi_select_votes($criteria);
        if ($results) {
            return false;
        }
    }

    return true;
}

function advpoll_check_electoral_list($uid, $nid) {
    $result = db_query('SELECT uid FROM {advpoll_electoral_list} WHERE uid = :uid AND nid = :nid', array(':uid' => $uid, ':nid' => $nid))->fetchField();

    if ($result) {
        return true;
    }

    return false;
}

/**
 * Add votes for a given poll.
 *
 * @param $vote
 *   A keyed array that is used to determine the method in which the vote will be
 *   tracked and what values will be passed to votingapi
 *          $vote['type'] The content type - either advpoll or advpoll_ranking
 *          $vote['tag'] = Tag corresponds to the index of the selected choice
 *          $vote['nid'] = The node ID of poll being voted on
 *          $vote['mode'] = normal, cookie, or unlimited
 *          $vote['duration'] = duration is minutes that the cookie will last if one is set.
 */
function advpoll_add_votes($vote) {

    // normal voting uses the voteapi to record user id or ip based on authentication
    if ($vote['mode'] === 'normal') {
        $votes = array(
            'entity_type' => $vote['type'],
            'entity_id' => $vote['nid'],
            'value' => $vote['value'],
            'tag' => $vote['tag'],
        );
    } else {
        // unlimited and cookie voting simply add values to the votingapi and bypass user id and ip as source.
        // time stamp instead of ip will prevent a unique id from being tied to these voters.
        if ($vote['mode'] === 'cookie') {
            // necessary to pass Drupal's $cookie_domain to get this to stick.
            // Raw cookies are safe in this case as we're only passing a static value to mark that this
            // user voted on their machine.
            setrawcookie($vote['type'] . $vote['nid'], 'vote', time() + (60 * $vote['duration']), '/', $cookie_domain);
        }

        $votes = array(
            'entity_type' => $vote['type'],
            'entity_id' => $vote['nid'],
            'value' => $vote['value'],
            'tag' => $vote['tag'],
            'uid' => '',
            'vote_source' => time(),
        );
    }

    votingapi_set_votes($votes, $criteria = NULL);
}

/*
 * Checkbox values returned by form_state have a different structure
 * than radio buttons.  We need an array of indexes representing
 * items selected from list of choices.
 */

function advpoll_checkbox_selected($choices, $votes) {
    $selected = array();
    $count = count($choices);

    for ($i = 0; $i < $count; $i++) {
        $choice = strip_tags($choices[$i]['choice']);
        if (!is_numeric($votes[$choice]) && !empty($votes[$choice])) {
            $selected[] = $choices[$i]['choice_id'];
        }
    }
    return $selected;
}

/*
 * Radio buttons returns a string rather than an array.  Need to return
 * an array containing the index equivalent to this choice.
 */

function advpoll_radio_selected($choices, $vote) {

    $selected = array();
    $count = count($choices);

    for ($i = 0; $i < $count; $i++) {
        $choice = strip_tags($choices[$i]['choice']);

        if ($choice == strip_tags($vote)) {
            $selected[] = $choices[$i]['choice_id'];
        }
    }

    return $selected;
}

/**
 * Return unique choice ids for a given node id and user id.
 *
 * @param $vote
 *   A keyed array that is used to determine the method in which the vote will be
 *   tracked and what values will be passed to votingapi
 *          $vote['type'] The content type - either advpoll or advpoll_ranking
 *          $vote['tag'] = Tag corresponds to the index of the selected choice
 *          $vote['nid'] = The node ID of poll being voted on
 *          $vote['mode'] = normal, cookie, or unlimited
 *          $vote['duration'] = duration is minutes that the cookie will last if one is set.
 */
function advpoll_get_user_votes($nid, $content_type) {
    global $user;
    $votes = array();
    $criteria = array();
    $criteria['entity_id'] = $nid;
    $criteria['entity_type'] = $content_type;

    if ($user->uid) {
        $criteria['uid'] = $user->uid;
    } else {
        $criteria['vote_source'] = ip_address();
    }

    $results = votingapi_select_votes($criteria);

    if ($results) {
        foreach ($results as $result) {
            $votes[] = $result['tag'];
        }
    }

    return $votes;
}