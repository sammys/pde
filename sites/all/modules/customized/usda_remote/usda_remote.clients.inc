<?php

class clients_connection_usda_services extends clients_connection_drupal_services_rest_7
implements
ClientsConnectionAdminUIInterface,
ClientsRemoteEntityInterface {
  // ================================= ClientsRemoteEntityInterface

  /**
   * Load a remote entity.
   *
   * @param $entity_type
   *  The entity type to load.
   * @param $id
   *  The (remote) ID of the entity.
   *
   * @return
   *  An entity object.
   */
  function remote_entity_load($entity_type, $id) {
    $query = $this->getRemoteEntityQuery('select');
    $query->base($entity_type);
    $query->entityCondition('entity_id', $id);
    $result = $query->execute();
    //dsm($result, 'remote_entity_load $result');

    // There's only one. Same pattern as entity_load_single().
    return reset($result);
  }

  /**
   * Save a remote entity.
   *
   * @param $entity_type
   *  The entity type to save.
   * @param $entity
   *  The entity to save.
   *
   * @return
   *  If the entity is being created remotely, the new remote GUID.
   */
  function remote_entity_save($entity_type, $entity, $remote_properties = array()) {
    // Determine whether this is an insert or an update. An entity not yet
    // saved remotely will have no remote ID property.
    $action = empty($entity->remote_id) ? 'insert' : 'update';
    //dsm($action, '$action');

    $query = $this->getRemoteEntityQuery($action);
    $query->setEntity($entity_type, $entity);

    if ($action == 'insert') {
      $guid = $query->execute();
      return $guid;
    }
    else {
      $query->addFields($remote_properties);

      $query->execute();
    }
  }

  /**
   * Provide a map of remote property types to Drupal types.
   *
   * Roughly analogous to _entity_metadata_convert_schema_type().
   *
   * @return
   *  An array whose keys are remote property types as used as types for fields
   *  in hook_remote_entity_query_table_info(), and whose values are types
   *  recognized by the Entity Metadata API (as listed in the documentation for
   *  hook_entity_property_info()).
   *  If a remote property type is not listed here, it will be mapped to 'text'
   *  by default.
   */
  function entity_property_type_map() {
    return array(
      'EntityCollection' => 'list<string>',
    );
  }

  /**
   * Get a new RemoteEntityQuery object appropriate for the connection.
   *
   * @param $query_type
   *  (optional) The type of the query. Defaults to 'select'.
   *
   * @return
   *  A remote query object of the type appropriate to the query type.
   */
  function getRemoteEntityQuery($query_type = 'select') {
    ////////////////// Code goes here
  }
}
