<?php
/**
 * @file
 * Default template for displaying messaging when a user is not eligible to vote on
 * a poll due to not being in the electoral list.
 * 
 * - $data: 
 *   object containing the following fields.
 *   choices:
 *      array containing:
 *        choice_id = the unique hex id of the choice
 *        choice =    The text for a given choice.
 *        write_in =  a boolean value indicating whether or not the choice was a 
 *                    write in.
 *   start_date:      (int) Start date of the poll
 *   end_date:        (int) End date of the poll
 *   mode:            The mode used to store the vote: normal, cookie, unlimited
 *   cookie_duration: (int) If mode is cookie, the number of minutes to delay votes.
 *   state:           Is the poll 'open' or 'close'
 *   behavior:        approval or poll - determines how to treat multiple vote/user 
 *                    tally.
 *   max_choices:     (int) How many choices a user can select per vote.
 *   show_results:    When to display results - aftervote, afterclose or never.
 *   electoral:       Boolean - voting restricted to users in an electoral list.
 *   show_votes:      Boolean - allow user with appropriate permission to view 
 *                    voting node page.
 *   write_in:        Boolean - all write-in voting.
 *   block:           Boolean - Poll can be displayed as a block.
 */
?>
<div class="poll">
    <div class="ineligible"><p><?php print t('You are not eligible to vote in this poll.'); ?>
</div>