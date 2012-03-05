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
  return true;
}

/*
 *  Passes votes to the voting api.
 *
 */
function advpoll_add_votes($vote) {

  $votes = array(
      'entity_type' => $vote['type'],
      'entity_id' => $vote['nid'],
      'value' => 1,
      'tag' => $vote['tag'],
      'uid' => $vote['uid'],
      'vote_source' => $vote['source'],
  );

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