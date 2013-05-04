<?php

/**
 * Generates the usda_food_des editing form.
 */
function usda_food_des_form($form, &$form_state, $usda_food_des, $op = 'edit') {
  $form = array();

  // Form API code for entity editing goes here

  return $form;
}

/**
 * Form API submit callback for the save button.
 */
function usda_food_des_form_submit(&$form, &$form_state) {
  entity_ui_get_form();
  $usda_food_des
    = entity_ui_form_submit_build_entity($form, $form_state);
  // Save and go back.
  $usda_food_des->save();
  $form_state['redirect'] = 'admin/content/usda';
}

function usda_remote_import_form() {
  $form['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Import'),
    ),
  );

  return $form;
}

function usda_remote_import_form_submit($form, &$form_state) {
  $path = drupal_get_path('module', 'usda_remote');
  $fns = array('usda_remote_import_data');

  $ops = array();
  foreach (array_keys(usda_remote_entity_info()) as $entity_type) {
    foreach ($fns as $fn) {
      $ops[] = array($fn, array($entity_type));
    }
  }

  $batch = array(
    'title' => t('Import USDA data from remote service'),
    'init_message' => t('Preparing to import USDA data'),
    'operations' => $ops,
    'finished' => 'usda_remote_import_complete',
    'file' => $path . '/usda_remote.batch.inc',
  );

  batch_set($batch);
}
