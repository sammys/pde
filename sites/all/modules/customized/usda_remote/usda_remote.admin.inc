<?php

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
