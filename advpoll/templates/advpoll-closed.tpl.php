<?php
/**
 * @file
 * Default template that displays closed messaging when a poll is not available.
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
 *   behavior:        approval or pool - determines how to treat multiple vote/user 
 *                    tally. When plugin is installed, voting will default to tabulating
 *                    values returned from voting API.
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
    <div class="poll-closed">
    <?php if ($data->start_date > time()): ?>
        <?php $date = date('F j, Y g:ia', $data->start_date); ?>
       <p><?php print t('This poll will open on @date.', array('@date' => $date)); ?></p>
    <?php else: ?>
       <p><?php print t('Thank you for participating.  The poll is now closed.'); ?></p>
    <?php endif; ?>
    </div>
</div>