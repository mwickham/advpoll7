<?php

/*
 * Page for displaying vote administration tab pages.
 */

function advpoll_votes_page($node) {
    $mode = $node->advpoll_mode[$node->language][0]['value'];
    $choices = $node->advpoll_choice[$node->language];
    $output = t('This table lists all the recorded votes for this poll.');
    if ($mode == 'unlimited') {
        $output = t('With unlimited voting, a timestamp is used to identify unique votes.  If it is important to identify users by ID or IP, switch to normal voting mode which will use your Voting API settings to record votes.');
    } else if ($mode == 'cookie') {
        $output = t('With cookie-based voting, a timestamp is used to identify unique votes while the poll\'s id is set in the cookie to limit votes for a limited time.  If it is important to identify users by ID or IP, switch to normal voting mode which will use your Voting API settings to record votes.');
    } else {
        $output = t('If anonymous users are allowed to vote, they will be identified by the IP address of the computer they used when they voted.');
    }

    $header = array();
    $header[] = array('data' => t('Visitor'), 'field' => 'uid');
    $header[] = array('data' => t('Date'), 'field' => 'timestamp', 'sort' => 'asc');
    $header[] = array('data' => t('Choice'), 'tag');

    $nid = $node->nid;

    $query = db_select('votingapi_vote', 'v')
            ->condition('entity_id', $nid)
            ->extend('PagerDefault')
            ->limit(20)
            ->extend('TableSort')
            ->orderByHeader($header)
            ->fields('v', array(
        'uid',
        'timestamp',
        'tag',
        'vote_source',
            ));

    $results = $query->execute();
    $userObj = null;
    $rows = array();
    foreach ($results as $item) {
        $userID = $item->uid;

        if (!$userID) {
            $userID = $item->vote_source;
        } else {
            $userObj = user_load($userID);
            if ($userObj) {
                $userID = l($userObj->name, '/user/' . $userID);
            }
        }

        $rows[] = array(
            'data' => array(
                $userID,
                format_date($item->timestamp),
                advpoll_match_tag_to_choice($choices, $item->tag),
            )
        );
    }

    if ($rows) {
        $output .= theme('table', array(
            'header' => $header,
            'rows' => $rows,
                )
        );

        $output .= theme('pager', array('tags' => array()));
        $output .= drupal_render(drupal_get_form('advpoll_clear_votes_form'));
    } else {
        $output .= '<hr /><p>' . t('No votes are currently recorded for %title', array('%title' => $node->title)) . '</p>';
    }

    return $output;
}

function advpoll_match_tag_to_choice($choices, $tag) {

    foreach ($choices as $choice) {
        if ($choice['choice_id'] == $tag) {
            return $choice['choice'];
        }
    }

    return;
}

function advpoll_clear_votes_form($form, &$form_state) {
    $nid = (int) check_plain(arg(1));

    $form['reset'] = array(
        '#value' => t('Clear all votes'),
        '#type' => 'submit',
    );
    $form['#action'] = url('node/' . $nid . '/votes/clear');
    return $form;
}

function advpoll_electoral_list_page($node) {
    return 'electoral list';
}

function advpoll_remove_voter($node) {



    return 'remove voter';
}

function advpoll_clear_votes_page($node) {

    $output = drupal_get_form('advpoll_clear_votes_confirm_form', $node->nid, $node->title);

    return $output;
}

/**
 * Display a clear votes confirmation form.
 */
function advpoll_clear_votes_confirm_form($form, &$form_state, $nid, $title) {
    $form = array();
    $form['#nid'] = $nid;
    $confirm_question = t('Are you sure you want to clear all votes for %title?', array('%title' => $title));
    $form['question'] = array('#value' => '<h2>' . $confirm_question . '</h2>');
    $form = confirm_form($form, $confirm_question, 'node/' . $nid . '/votes', t('This will delete all votes that have been cast for %title.', array('%title' => $title)), t('Clear all votes'), t('Cancel'));
    unset($form['#theme']);
    return $form;
}

function advpoll_clear_votes_confirm_form_submit($form, &$form_state) {
    $nid = (int) check_plain(arg(1));
    db_delete('votingapi_vote')->condition('entity_id', $nid)->execute();
    drupal_goto('node/' . $nid . '/votes');
}

/*
 * Menu callback for Write-in node management tab page.
 */

function advpoll_writeins_page($node) {
    $output = '';

    $output .= drupal_render(drupal_get_form('advpoll_promote_writein_form', $node));
    $output .= drupal_render(drupal_get_form('advpoll_merge_writein_form', $node));

    return $output;
}

/*
 * Generates form used to promote write-in options to normal status on the write-in node
 * management tab page.
 */

function advpoll_promote_writein_form($form, &$form_state, $node) {

    $choices = $node->advpoll_choice[$node->language];
    $count = count($choices);

    $options = array();

    for ($i = 0; $i < $count; $i++) {
        if ($choices[$i]['write_in']) {
            $options[] = strip_tags($choices[$i]['choice']);
        }
    }
    $form['#id'] = 'advpoll-promote_writein';

    if ($options) {

        $form['promote_writein'] = array(
            '#type' => 'fieldset',
            '#title' => t('Promote write-ins'),
            '#description' => t('Write-ins can be converted to regular choices. This is useful if users cannot see past write-ins but you want to promote specific write-ins so that they can be seen by users who vote in the future.'),
        );

        $form['promote_writein']['choices'] = array(
            '#type' => 'checkboxes',
            '#title' => '',
            '#options' => drupal_map_assoc($options),
        );

        $form['promote_writein']['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Promote'),
        );
    } else {
        $form['promote_writein'] = array(
            '#type' => 'fieldset',
            '#title' => t('Promote write-ins'),
            '#description' => t('There are currently no write-ins available to promote.'),
        );
    }

    return $form;
}

/*
 * Submit function for advpoll_promote_writein_form.
 */

function advpoll_promote_writein_form_submit($form, &$form_state) {
    $selected = array();
    $nid = $form_state['build_info']['args'][0]->nid;

    $options = $form['promote_writein']['choices']['#options'];

    foreach ($options as $option) {
        if ($form['promote_writein']['choices'][$option]['#checked']) {
            $selected[] = strtolower(strip_tags($option));
        }
    }

    if ($selected) {

        $node = node_load($nid);

        if ($node) {
            $nodeChoices = $node->advpoll_choice[$node->language];
            $updatedChoices = array();
            foreach ($nodeChoices as $item) {
                $choice = strtolower(strip_tags($item['choice']));
                if (in_array($choice, $selected)) {
                    $updatedChoices[] = array('choice' => $item['choice'], 'write_in' => 0, 'choice_id' => $item['choice_id']);
                } else {
                    $updatedChoices[] = array('choice' => $item['choice'], 'write_in' => $item['write_in'], 'choice_id' => $item['choice_id']);
                }
            }
            $node->advpoll_choice[$node->language] = $updatedChoices;
            node_save($node);
        }
    }
}

function advpoll_merge_writein_form($form, &$form_state, $node) {

    $choices = $node->advpoll_choice[$node->language];
    $count = count($choices);

    $options = array();
    $all = array();

    $form['#id'] = 'advpoll-merge_writein';
    for ($i = 0; $i < $count; $i++) {
        if ($choices[$i]['write_in']) {
            $options[] = strip_tags($choices[$i]['choice']);
        } else {
            $all[] = strip_tags($choices[$i]['choice']);
        }
    }

    if ($options && $all) {

        $form['merge_writein'] = array(
            '#type' => 'fieldset',
            '#title' => t('Merge write-ins'),
            '#description' => t('This will delete the write-in and change any votes for it into votes for the selected choice.'),
        );

        $form['merge_writein']['choices'] = array(
            '#type' => 'select',
            '#title' => t('Merge'),
            '#options' => drupal_map_assoc($options),
        );
        $form['merge_writein']['all_choices'] = array(
            '#type' => 'select',
            '#title' => t('into'),
            '#options' => drupal_map_assoc($all),
        );


        $form['merge_writein']['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Merge'),
        );
    } else {
        $form['merge_writein'] = array(
            '#type' => 'fieldset',
            '#title' => t('Merge write-ins'),
            '#description' => t('There are currently no write-ins to promote.'),
        );
    }

    return $form;
}

/*
 * Submit function for merging write-in votes.
 * 
 */

function advpoll_merge_writein_form_submit($form, &$form_state) {
    $lang = $form_state['build_info']['args'][0]->language;
    $mergeFrom = strtolower($form['merge_writein']['choices']['#value']);
    $mergeTo = strtolower($form['merge_writein']['all_choices']['#value']);
    $nid = $form_state['build_info']['args'][0]->nid;
    $type = $form_state['build_info']['args'][0]->type;
    $choices = $form_state['build_info']['args'][0]->advpoll_choice[$lang];

    $fromTag = '';
    $toTag = '';
    foreach ($choices as $choice) {
        if (strtolower(strip_tags($choice['choice'])) == $mergeFrom) {
            $fromTag = $choice['choice_id'];
        } else if (strtolower(strip_tags($choice['choice'])) == $mergeTo) {
            $toTag = $choice['choice_id'];
        }
    }

    if (!$fromTag || !$toTag) {
        drupal_set_message(t('Unidentifed index for index merging.'));
        return;
    }

    $criteria = array('entity_id' => $nid, 'tag' => $fromTag);
    $mergeFromVotes = votingapi_select_votes($criteria);

    // transfer merge from votes to new vote object and update votingapi
    foreach ($mergeFromVotes as $vote) {
        $votes = array(
            'entity_type' => $type,
            'entity_id' => $nid,
            'value' => $vote['value'],
            'tag' => $toTag,
            'uid' => $vote['uid'],
            'vote_source' => $vote['vote_source'],
        );

        votingapi_set_votes($votes);
    }
    // remove votes for merge from tag
    votingapi_delete_votes($mergeFromVotes);

    // remove choice from node.
    $node = node_load($nid);

    if ($node) {
        $nodeChoices = $node->advpoll_choice[$node->language];
        $updatedChoices = array();
        foreach ($nodeChoices as $item) {
            $id = $item['choice_id'];
            if ($id != $fromTag) {
                $updatedChoices[] = $item;
            }
        }
        $node->advpoll_choice[$node->language] = $updatedChoices;
        node_save($node);
    }
}

/*
 * Displays results of poll for results page tab.
 */

function advpoll_results_page($node) {
    drupal_add_css(drupal_get_path('module', 'advpoll') . '/css/advpoll.css', array('group' => CSS_DEFAULT, 'every_page' => TRUE));

    $output = '';

    if ($node) {
        $choices = $node->advpoll_choice[$node->language];
        $content_type = $node->type;
        $behavior = $node->advpoll_behavior[$node->language][0]['value'];
        $output = advpoll_display_results($choices, $content_type, $node->nid, $behavior);
    }

    return $output;
}