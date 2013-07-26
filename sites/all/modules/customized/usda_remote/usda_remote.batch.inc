<?php

function usda_remote_import_data($entity_type, &$context) {
  if (!count($context['sandbox'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current'] = 0;
    // TODO: Implement an entity count endpoint and use that
    $context['sandbox']['max'] = 8194;
    $context['sandbox']['info'] = entity_get_info($entity_type);
  }

  $info = &$context['sandbox']['info'];

  $controller = entity_get_controller($entity_type);
  $query = $controller->getRemoteEntityQuery();
  $query->pager['page'] = $context['sandbox']['progress'] / $query->pager['limit'];

  try {
    $entities = $controller->executeRemoteEntityQuery($query);
    $context['sandbox']['current'] = count($entities);
    $context['sandbox']['progress'] += $context['sandbox']['current'];
  }
  catch (Exception $e) {
    ;
  }

  // For some reason modifications to $context within the catch {} above is ignored by
  // the Drupal batch API. We workaround by checking if we received the full number
  // of entities requested.
  if ($context['sandbox']['current'] == $query->pager['limit'] && $context['sandbox']['progress'] < $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / (float) $context['sandbox']['max'];
    $context['message'] = t('Progress: @count <em>!label</em> entities', array('@count' => $context['sandbox']['progress'], '!label' => $info['label']));
  }
  else {
    $context['results'][] = t('Imported @count !label entities', array('@count' => $context['sandbox']['progress'], '!label' => $info['label']));
  }
}

function usda_remote_import_complete($success, $results, $operations, $duration) {
  if ($success) {
    foreach ($results as $message) {
      drupal_set_message($message);
    }
  }
  else {
    $message = t('Finished with an error.');
  }
}
