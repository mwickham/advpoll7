<?php

/**
 * @file
 * Provides a FieldAPI field type for presenting an Advanced Poll choice field with
 * a Boolean checkbox to indicate write-in status.
 */


/**
 * Implements hook_field_info().
 *
 * Provides the description of the field.
 */
function advpoll_field_info() {
  return array(
    'advpoll' => array(
      'label' => t('Advanced Poll Choice'),
      'description' => t('A compound field for displaying a choice field and its write-in status.'),
      'default_widget' => 'advpoll_write_in',
      'default_formatter' => 'advpoll_choice',
    ),
  );
}

/**
 * Implements hook_field_is_empty().
 */
function advpoll_field_is_empty($item, $field) {
  return (empty($item['choice']));
}

/**
 * Implements hook_field_formatter_info().
 */
function advpoll_field_formatter_info() {
  return array(
    'advpoll_choice' => array(
      'label' => t('Advanced Poll Widget'),
      'field types' => array('advpoll'),
    ),
  );
}

/**
 * Implements hook_field_formatter_view().
 * Write-in flag is not visible.
 */
function advpoll_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();

  switch ($display['type']) {
    // This formatter simply outputs the field as text and with a color.
    case 'advpoll_choice':
      foreach ($items as $delta => $item) {
        $element[$delta]['#markup'] = filter_xss($item['choice']);
      }
      break;
  }

  return $element;
}



/**
 * Implements hook_field_widget_info().
 *
 * Create appropriate FAPI widget for display in form.
 */
function advpoll_field_widget_info() {
  return array(
    'advpoll_write_in' => array(
      'label' => t('Advanced Poll Choice'),
      'field types' => array('advpoll'),
    ),
  );
}


/**
 * Implements hook_field_widget_form().
 *
 */
function advpoll_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  $element += array(
    '#delta' => $delta,
  );
  
  switch ($instance['widget']['type']) {
    case 'advpoll_write_in':
      $element['choice'] = array(
        '#title' => $element['#title'],
        '#type' => 'textfield',
        '#size' => 110,
        '#default_value' => empty($items[$delta]['choice']) ? '' : $items[$delta]['choice'],
        '#description' => $element['#description'],
        '#required' => $element['#required']
      );
      $element['write_in'] = array(
        '#type' => 'checkbox',
        '#title' => t('Write-in (When checked, item will not be displayed for voting until unchecked.)'),
        '#default_value' => empty($items[$delta]['write_in']) ? 0 : 1,
      );

  }
  return $element;
}



/**
 * Implements hook_field_views_data().
 */
function advpoll_field_views_data($field) {
  // An attempt to make a Views friendly display.
    
  $views_fields = field_views_field_default_views_data($field);
  foreach ($views_fields as $key => $data) {
    $views_fields[$key][$field['field_name'] . '_choice']['argument']['name field'] = $field['field_name'] . '_choice';
  }
  return $views_fields;
}