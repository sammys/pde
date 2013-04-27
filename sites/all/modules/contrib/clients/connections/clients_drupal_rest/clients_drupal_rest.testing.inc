<?php
/**
 * @file
 * Contains classes for Client connection testing.
 */

/**
 * Test basic connection to a Drupal Services connection.
 */
class ClientsConnectionDrupalRESTTestLogin implements ClientsConnectionTestingInterface {

  /**
   * The labels for the test.
   *
   * (This is because it would seem you can't define class variables using
   * expressions.)
   */
  function testLabels() {
    return array(
      'label' => t('Test login'),
      'description' => t('Test the basic connection to the site by logging in the user.'),
      'button' => t('Connect'),
    );
  }

  /**
   * Execute the test.
   *
   * Connection test handlers should return the raw data they got back from the
   * connection for display to the user.
   *
   * @param $connection
   *  The connection handler.
   * @param $button_form_values
   *  The form values for the test form element. The values for elements added
   *  to the form are in $button_form_values['params'].
   *
   * @return
   *  Data from the remote connection. This is output to the form as raw data.
   */
  function test($connection, &$button_form_values) {
    try {
      // Call the login method.
      $login = $connection->userLogin();
    }
    catch (Exception $e) {
      drupal_set_message(t('Could not log in to the remote site, got error message "@message".', array(
        '@message' => $e->getMessage(),
      )), 'warning');
      //dsm($e);

      return;
    }

    if (is_object($login) && isset($login->user)) {
      drupal_set_message(t('Sucessfully logged in to the remote site; got back details for user %user (uid @uid).', array(
        '%user' => $login->user->name,
        '@uid'  => $login->user->uid,
        )));
    }
    else {
      drupal_set_message(t('Could not log in to the remote site.'), 'warning');
    }

    return $login;
  }

}

/**
 * Test node load from a Drupal Services REST connection.
 */
class ClientsConnectionDrupalRESTTestNodeRetrieve implements ClientsConnectionTestingInterface {

  /**
   * The labels for the test.
   */
  function testLabels() {
    return array(
      'label' => t('Test node retrieval'),
      'description' => t('Load a node from the connection.'),
      'button' => t('Load node'),
    );
  }

  /**
   * Creates the form element for the test.
   *
   * This gets a form element with the basics in place. If your test needs input
   * parameters, add form elements here.
   *
   * @param $element
   *  A form element for the test's settings and button.
   *
   * @return
   *  The form element with the test's additions.
   */
  function testForm($element) {
    $element['params']['nid'] = array(
      '#type' => 'textfield',
      '#title' => t('Node ID'),
      '#size' => 6,
      '#required' => TRUE,
    );

    return $element;
  }

  /**
   * Validate test form input.
   */
  function formValidate(&$button_form_values) {
    if (!is_numeric($button_form_values['params']['nid'])) {
      form_set_error('buttons][node_load][nid', t('Node id must be numeric.'));
    }
  }

  /**
   * Execute the test.
   *
   * Connection test handlers should return the raw data they got back from the
   * connection for display to the user.
   *
   * @param $connection
   *  The connection handler.
   * @param $button_form_values
   *  The form values for the test form element. The values for elements added
   *  to the form are in $button_form_values['params'].
   *
   * @return
   *  Data from the remote connection. This is output to the form as raw data.
   */
  function test($connection, &$button_form_values) {
    // Must be cast to integer for faffiness of XMLRPC and Services.
    $nid = (int) $button_form_values['params']['nid'];

    try {
      $node = $connection->makeRequest('node/' . $nid, 'GET');
    }
    catch (Exception $e) {
      drupal_set_message(t('Could not retrieve a node from the remote site, got error message "@message".', array(
        '@message' => $e->getMessage(),
      )), 'warning');
      //dsm($e);

      return;
    }

    if (is_object($node) && isset($node->nid)) {
      drupal_set_message(t('Sucessfully retrieved node %title (nid @nid).', array(
        '%title' => $node->title,
        '@nid'  => $node->nid,
        )));
    }

    return $node;
  }

}
