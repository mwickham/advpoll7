<?php
/**
 * @file
 * Default template for an advanced poll bar - based on default Drupal Poll template.
 *
 * Variables available:
 * - $data: 
 *   object containing the following fields.
 *   choices:
 *      array containing:
 *        choice_id = the unique hex id of the choice
 *        choice = The text for a given choice.
 *        write_in = a boolean value indicating whether or not the choice was a write in.
 *   start_date: Start date of the poll
 *   end_date: End date of the poll
 *   mode: The mode used to store the vote: normal, cookie, unlimited
 *   cookie_duration: If mode is cookie, the number of minutes to delay votes.
 *   state: Is the poll 'open' or 'closed'
 *   behavior: approval or poll - determines how to treat multiple vote/user tally.
 *   max_choices: How many choices a user can select per vote.
 *   show_results: When to display results - aftervote, afterclose or never.
 *   electoral: Boolean - voting restricted to users in an electoral list.
 *   show_votes: Boolean - allow user with appropriate permission to view voting node page.
 *   write_in: Allow write-in voting.
 *   block: Poll can be displayed as a block.
 */
?>
<div class="poll">
  <div class="poll-noresult">
      <p><?php print t('Thank you for participating in this poll.'); ?></p>

      <?php if ($data->show_results == 'afterclose'): ?>
        <?php $date = date('F j, Y g:ia', $data->end_date); ?>
        <p><?php print t('The results of this poll will be available after @date.', array('@date'=>$date)); ?></p>
      <?php endif; ?>
      
  
  
  </div>
</div>