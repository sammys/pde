<?php

/**
 * @file
 * Provides a controller building upon the Entity exportable controller but
 * providing features for handler objects.
 */

/**
 * A controller for entities that function as handlers.
 *
 * Not specific to Clients module -- could be abstracted.
 *
 * The main job of this controller class is to implement a factory-by-row
 * pattern, that is, where the class of each object returned by a load is
 * determined by the value of a database column, in this case, its type.
 */
class ClientsHandlerEntityController extends EntityAPIControllerExportable {

  /**
   * Overridden to not use 'entity class': our class is variable.
   *
   * @return The results in a Traversable object.
   */
  public function query($ids, $conditions, $revision_id = FALSE) {
    // Build the query.
    $query = $this->buildQuery($ids, $conditions, $revision_id);
    $result = $query->execute();
    return $result;
  }

  // buildQuery() takes care of loading by name as well as id.

  /**
   * Overridden to implement factory-by-row pattern.
   *
   * @see DrupalDefaultEntityController#load($ids, $conditions)
   */
  public function load($ids = array(), $conditions = array()) {
    // Call the parent to load the entities.
    // This gets us StdClass objects.
    $entities = parent::load($ids, $conditions);

    // Copy the loaded entity object to the right class for its type.
    $return = array();
    foreach ($entities as $name => $entity_data) {
      // Get the class to use.
      $class = $this->getClass($entity_data);

      // The connection's __construct() takes over from here.
      $entity = new $class((array) $entity_data, $this->entityType);
      $return[$name] = $entity;
    }

    return $return;
  }

  /**
   * Implements EntityAPIControllerInterface.
   *
   * Overridden to implement factory-by-row pattern.
   */
  public function create(array $values = array()) {
    // Add is_new property if it is not set.
    $values += array('is_new' => TRUE);

    // Get the class to use.
    $class = $this->getClass($values);

    return new $class($values, $this->entityType);
  }

  /**
   * Helper to get the class to create for an entity.
   *
   * Uses data in $entity_info['factory'].
   *
   * @param $entity_data
   *  Either an array or an object of entity data.
   *
   * @return
   *  The name of a class.
   */
  function getClass($entity_data) {
    if (is_object($entity_data)) {
      $type = $entity_data->type;
    }
    else {
      $type = $entity_data['type'];
    }

    $prefix = $this->entityInfo['factory']['class prefix'];
    $class = $prefix . $type;

    if (class_exists($class)) {
      return $class;
    }
    else {
      return $this->entityInfo['factory']['broken class'];
    }
  }

}
