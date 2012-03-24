<?php

/**
 * @file
 * Default template for wrapping bar results - includes count of votes.
 *
 * Variables available:
 * - $bars: The output of all styled bars displaying votes.
 * - $total: Total number of votes.
 * - $voted: An array indicating which unique choice ids were selected by the user.
 *
 */
?>
<div class="poll">

    <?php print $bars; ?>
    <div class="total"><?php print t('Total votes: @total', array('@total' => $total)); ?></div>

    <?php if ($voted): ?>
    <div class="poll-message"><?php print t('Thank you for voting.'); ?></div>
    <?php endif; ?>
</div>
