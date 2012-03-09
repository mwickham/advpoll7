<?php

/*
 * Page for displaying vote administration tab pages.
 */

function advpoll_votes_page() {
  $output = t('This table lists all the recorded votes for this poll.' );
  $output = t('If anonymous users are allowed to vote, they will be identified by the IP address of the computer they used when they voted.');

  $header = array();
  $header[] = array('data' => t('Visitor'), 'field' => 'uid');
  $header[] = array('data' => t('Date'), 'field' => 'timestamp', 'sort' => 'asc');
  $header[] = array('data' => t('Choice'), 'tag');

  $query = db_select('votingapi_vote', 'v')
                  ->condition('entity_id', arg(1))
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
            t('Choice ') . ($item->tag + 1),
        )
    );
  }

  $output .= theme('table',
                  array(
                      'header' => $header,
                      'rows' => $rows,
                  )
  );
  $output .= theme('pager', array('tags' => array()));

  // TODO - find out what the method is for printing out a form element
  // as html (drupal_render is bad apparently in d7).  Also figure out how to use
  // confirm_delete on the submit button.
  $output .= drupal_render(drupal_get_form('advpoll_clear_votes_form'));


  return $output;
}

function advpoll_clear_votes_form($form, &$form_state) {
    $form['reset'] = array(
        '#value' => t('Clear all votes'),
        '#type' => 'submit',
    );
   $form['#redirect'] = 'node/' . arg(1) . '/votes/clear';
    return $form;
}

function advpoll_electoral_list_page() {
  return 'electoral list';
}

function advpoll_remove_voter() {
  return 'remove voter';
}

function advpoll_clear_votes_page() {
  return 'clear votes';
}

function advpoll_writeins_page() {
  return 'write-ins page';
}

function advpoll_results_page() {
  return 'results page';
}