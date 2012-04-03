<?php

function advpoll_converter_form($form, $form_state) {
    $form = array(
        '#attributes' => array('class' => array('advpoll-converter-form')),
    );
    $form['info'] = array(
        '#title' => t('Advanced Poll Converter'),
        '#description' => t('Select standard Drupal polls to convert into Advanced polls. As always with any conversion process, be sure to back up your data.'),
        '#type' => 'fieldset',
        '#tree' => FALSE,
    );

    $options = array(0 => t('No'), 1 => t('Yes'));
    $form['info']['advpoll_converter_delete'] = array(
        '#title' => t('Delete source polls?'),
        '#type' => 'radios',
        '#options' => $options,
        '#default_value' => 1,
        '#description' => t('When the process is run, should original polls be deleted?'),
    );

    $polls = _advpoll_converter_get_polls();

    if ($polls) {
        $form['info']['advpoll_converter_select'] = array(
            '#type' => 'select',
            '#title' => t('Select polls to convert.'),
            '#options' => $polls,
            '#multiple' => true,
            '#description' => t('Select the polls to convert.'),
            '#size' => 12,
        );

        $form['info']['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Convert'),
        );
    } else {
        $form['info']['message'] = array(
            '#type' => 'item',
            '#markup' => t('There are currently no polls to convert.'),
        );
    }

    return $form;
}

function advpoll_converter_form_submit ($form, $form_state) {
    $nids = array();
    $delete = 0;
    
    if(isset($form_state['values']['advpoll_converter_select'])) {
        $delete = $form_state['values']['advpoll_converter_delete'];
        $selections = $form_state['values']['advpoll_converter_select'];
        
        if ($selections) {
            foreach($selections as $nid) {
                $nids[] = $nid;
            }
            _advpoll_converter_create_nodes($nids);
        } else {
           form_set_error('advpoll_converter_select', t('Please select at least one poll to convert.'));
        }
    }

}


function _advpoll_converter_get_polls() {
    $result = db_query("SELECT title, nid FROM {node} WHERE type='poll'");
    $options = array();
    foreach ($result as $record) {
        $options[$record->nid] = $record->title;
    }
    return $options;
}

function _advpoll_converter_create_nodes($nids) {
$poll_nodes = db_query("SELECT n.nid, n.title, n.language, n.created, n.status, n.uid, 
                        p.runtime, p.active FROM {node} n JOIN poll p ON n.nid = p.nid 
                        WHERE type='poll' AND n.nid IN (:ids)", array(':ids' => $nids));
   $new_nodes = array();
   foreach($poll_nodes as $record) {
      $node = new stdClass();
      $node->title = $record->title;
      $node->uid = $record->uid;
      $node->status = $record->status;
      $node->type = 'advpoll';
      $node->language = $record->language;
      
      /* suggested query for both poll items and votes
       * SELECT pc.nid, pc.chid, pc.chtext, pv.uid, pv.hostname, pv.timestamp  FROM poll_choice pc JOIN poll_vote pv ON pc.chid = pv.chid WHERE pc.nid = 19
       */
      
      
      $new_nodes[] = $node;
   }   
   
   print_r($new_nodes);
   die;
}