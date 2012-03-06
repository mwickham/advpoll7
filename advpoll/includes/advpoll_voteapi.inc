<?php

/*
 * Handles function calls related to voting and selection of votes.
 */

function advpoll_get_votes($content_type, $content_id) {
    // TODO: return all votes for a given poll
    return 'woot';
}

function advpoll_user_eligibility($node) {
    // TODO: check to see if a given user is eligible to vote on a poll.
    global $user;
    
    $mode = $node->advpoll_mode['und'][0]['value'];

    if ($node->advpoll_closed['und'][0]['value'] !== 'Open poll') {
        return false;
    }

    if (strtotime($node->advpoll_dates['und'][0]['value2']) < time()) {
        return false;
    }
    
    if ($mode === 'cookie' && isset($_COOKIE[$node->type. $node->nid])) {    
        return false;
    }

    if ($node->advpoll_mode['und'][0]['value'] === 'normal') {
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

/**
 * Select individual votes from the database.
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
            'value' => 1,
            'tag' => $vote['tag'],
        );
    } else {
        // unlimited and cookie voting simply add values to the votingapi and bypass user id and ip as source.
        // time stamp instead of ip will prevent a unique id from being tied to these voters.
        if ($vote['mode'] === 'cookie') {
            // necessary to pass Drupal's $cookie_domain to get this to stick.
            // Raw cookies are safe in this case as we're only passing a static value to mark that this
            // user voted on their machine.
            setrawcookie($vote['type'].$vote['nid'], 'vote', time()+(60 * $vote['duration']), '/', $cookie_domain); 
        }
        
        $votes = array(
            'entity_type' => $vote['type'],
            'entity_id' => $vote['nid'],
            'value' => 1,
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
        $choice = strip_tags($choices[$i]['value']);

        if (!is_numeric($votes[$choice])) {
            $selected[] = $i;
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
        $choice = strip_tags($choices[$i]['value']);

        if ($choice == strip_tags($vote)) {
            $selected[] = $i;
        }
    }

    return $selected;
}