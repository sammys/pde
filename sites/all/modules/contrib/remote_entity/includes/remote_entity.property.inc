<?php

/**
 * Entity metadata controller.
 *
 * Registers the properties declared in $entity_info['remote entity keys'] and
 * $entity_info['property map'].
 *
 * See remote_entity_hook_entity_info() for details on using these.
 */
class RemoteEntityAPIDefaultMetadataController extends EntityDefaultMetadataController {

  public function entityPropertyInfo() {
    // Let the parent class handle the schema properties.
    $parent_info = parent::entityPropertyInfo();

    // This is so awkward I am surely doing it wrong :/
    $properties = $parent_info[$this->type]['properties'];

    // Get the property type map out of the connection for our remote entity
    // type.
    // First get the resource for our entity type, and then the connection.
    $resource = clients_resource_get_for_component('remote_entity', $this->type);
    $connection = $resource->getConnection();
    $property_type_map = $connection->entity_property_type_map();

    // Get the remote table info.
    $table_info = remote_entity_get_query_table_info($connection->name, $this->info['remote base table']);

    // Properties from the property map.
    foreach ($this->info['property map'] as $property => $remote_property) {
      // Type defaults to 'text'.
      $type = 'text';
      // Now see if that should actually be something else, by inspecting the
      // remote table info and the remote property type map.
      if (isset($table_info['fields'][$remote_property])) {
        // If the table info defines the field...
        $remote_type = $table_info['fields'][$remote_property]['type'];
        if (isset($property_type_map[$remote_type])) {
          // ...and the property map has the type, take our type from the map.
          $type = $property_type_map[$remote_type];
        }
      }

      $properties[$property] = array(
        'label' => $property,
        'description' => t("A remote property."),
        'type' => $type,
        'getter callback' => 'remote_entity_entity_mapped_property_get',
        'setter callback' => 'remote_entity_entity_mapped_property_set',
        // Custom property to help our getters and setters.
        'remote_property' => $remote_property,
      );
    }

    // Remove the setter callback from the property for the remote id: it
    // should not be set locally.
    // Get the remote id property.
    $id_property = $this->info['remote entity keys']['remote id'];
    // Flip the property map so we can find its local name.
    $flipped_property_map = array_flip($this->info['property map']);
    // Get the local property name that maps to the remote id property.
    $local_id_property = $flipped_property_map[$id_property];
    // Unset the callback.
    unset($properties[$local_id_property]['setter callback']);

    $info = array();
    $info[$this->type]['properties'] = $properties;

    return $info;
  }

}
