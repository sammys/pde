<?php

/**
 * @file
 * Provides base classes for clients handler entities.
 */

/**
 * Base class for entities that function as handlers.
 *
 * Part of this could be abstracted -- see the controller class.
 */
class ClientsHandlerEntity extends Entity {

  /**
   * Constructor method.
   *
   * Overrides Entity::__construct().
   */
  function __construct(array $values = array(), $entityType = NULL) {
    // todo: refactor; use parent::()?
    $this->entityType = $entityType;
    $this->setUp();

    // Lump all data unto the object...
    foreach ($values as $field => $value) {
      $this->$field = $value;
    }

    // Connections defined in code are already unserialized.
    // TODO: check if still needed???
    if (isset($object->configuration) && !is_array($object->configuration)) {
      $this->configuration = unserialize($object->configuration);
    }

    return $this;
  }

}

/**
 * Base class for client connections.
 */
abstract class clients_connection_base extends ClientsHandlerEntity {

  /**
   * The machine name of the connection.
   */
  public $name;

  /**
   * The connection id. Only set if this is stored in the database.
   */
  public $cid;

  /**
   * The URL this connection connects to.
   */
  public $endpoint;

  /**
   * An array of further configuration options.
   */
  public $configuration;

  // ============================================ Connection UI

  /**
   * Format the connection's endpoint as a link.
   *
   * @param $url
   *  The connection's endpoint.
   *
   * @return
   *  The string to display in the admin UI. Subclasses may format this as a
   *  link to the remote site.
   */
  function formatEndpoint($url) {
    return $url;
  }

  /**
   * Alter the connection settings form.
   *
   * Subclasses should add extra configuration form elements here.
   *
   * @param $form
   *  The main form array.
   * @param $form_state
   *  The form state from the main form, which you probably don't need anyway.
   *
   * @see clients_connection_form()
   * @see clients_connection_form_submit()
   *
   * TODO: define an interface for the admin UI
   */
  function connectionSettingsFormAlter(&$form, &$form_state) {
  }

  /**
   * Submit handler for saving/updating connections of this class.
   *
   * This base class method handles preserving the password when the connection
   * form is submitted on edit. Connection classes with a password configuration
   * option may make use of it.
   *
   * @see clients_connection_form_submit().
   */
  function connectionSettingsForm_submit($form, &$form_state) {
    // Check there is a password form element.
    if (isset($form['configuration']['password'])) {
      $old_connection = $form['#connection'];

      // Check whether we're editing or adding a new connection.
      if (isset($old_connection->is_new)) {
        // Encryption disabled for now. TODO.
        //$form_state['values']['configuration']['password'] = clients_drupal_encrypt($form_state['values']['configuration']['password']);
      }
      else {
        // Set password to original if blank.
        if (empty($form_state['values']['configuration']['password'])) {
          $this->configuration['password'] = $old_connection->configuration['password'];
        }
        // Encryption disabled for now. TODO
        //$form_state['values']['configuration']['password'] = clients_drupal_encrypt($form_state['values']['configuration']['password']);
      }
    }

    // Base class doesn't save: saving of the connection is handled by the
    // 'real' FormAPI submit handler.
  }

  // ============================================ Connection API.
  // TODO: move these to an XMLRPC client interface.

  /**
   * Call a remote method.
   *
   * This is a wrapper around callMethodArray that gives the convenience of
   * being able to pass method name and parameters as one flat list, and hence
   * is the main API for connection objects.
   *
   * @param $method
   *  The name of the remote method to call.
   * @param ...
   *  All other parameters are passed to the remote method.
   *
   * @return
   *  Whatever is returned from the remote site.
   *
   * @throws Exception on error from the remote site.
   */
  function callMethod($method) {
    // Get all the arguments this function has been passed.
    $function_args  = func_get_args();
    // Slice out the ones that are arguments to the method call: everything past
    // the 1st argument.
    $method_params  = array_slice($function_args, 1);

    return $this->callMethodArray($method, $method_params);
  }

  /**
   * Call a remote method with an array of parameters.
   *
   * This is intended for internal use from callMethod() and
   * clients_connection_call().
   * If you need to call a method on given connection object, use callMethod
   * which has a nicer form.
   *
   * Subclasses do not necessarily have to override this method if their
   * connection type does not make sense with this.
   *
   * @param $method
   *  The name of the remote method to call.
   * @param $method_params
   *  An array of parameters to passed to the remote method.
   *
   * @return
   *  Whatever is returned from the remote site.
   *
   * @throws Exception on error from the remote site.
   *  It's up to subclasses to implement this, as the test for an error and
   *  the way to get information about it varies according to service type.
   */
  function callMethodArray($method, $method_params = array()) {
    // Up to subclasses to override this to do something.
  }

}

/**
 * Base class for resource handlers.
 */
abstract class clients_resource_base extends ClientsHandlerEntity {

  /**
   * The name of the component the resource provides, eg block delta.
   */
  public $component;

  /**
   * The name of the connection the resource uses.
   */
  public $connection;

  /**
   * The connection handler the resource uses, lazy-loaded.
   */
  private $connection_handler;

  /**
   * Extra form elements specific to a class's edit form.
   *
   * This is the same pattern as node_form() -- just ignore the object behind
   * the curtain ;)
   *
   * This (so far) is common to all versions of Drupal Services.
   *
   * @param $form
   *  The main form array.
   * @param $form_state
   *  The form state from the main form, which you probably don't need anyway.
   *
   * @return
   *  A FormAPI form array. This will be merged in with basic data and the
   *  submit button added.
   *
   * @see clients_connection_form()
   * @see clients_connection_form()
   * @see clients_connection_form_submit()
   *
   * TODO: define an interface for the admin UI
   */
  function resourceSettingsFormAlter(&$form, &$form_state) {
  }

  /**
   * Get this resource's connection
   */
  function getConnection() {
    if (!isset($this->connection_handler)) {
      $this->connection_handler = clients_connection_load($this->connection);
    }
    return $this->connection_handler;
  }

}

/**
 * Common class for broken handlers.
 *
 * This does double duty for both connections and resources.
 */
class ClientsHandlerEntityBroken extends ClientsHandlerEntity {

  function connectionSettingsFormAlter(&$form, &$form_state) {
    drupal_set_message(t('This handler is broken.'), 'warning');
  }

  function resourceSettingsFormAlter(&$form, &$form_state) {
    drupal_set_message(t('This handler is broken.'), 'warning');
  }

}
