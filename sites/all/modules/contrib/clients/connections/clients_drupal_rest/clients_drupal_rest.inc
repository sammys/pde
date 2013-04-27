<?php
/**
 * @file
 * Contains classes for Client connections handlers.
 */

/**
 * Class for Drupal client connections, REST D7.
 */
class clients_connection_drupal_services_rest_7 extends clients_connection_base {

  // ============================================ Connection form methods.

  /**
   * Extra form elements specific to a class's edit form.
   *
   * @param $form_state
   *  The form state from the main form, which you probably don't need anyway.
   *
   * @see clients_connection_form()
   * @see clients_connection_form_submit()
   */
  function connectionSettingsFormAlter(&$form, &$form_state) {
    $form['endpoint']['#description'] = t('Remote service URL e.g. http://mysite.com/service-endpoint');

    $form['configuration'] += array(
      '#type' => 'fieldset',
      '#title' => t('Configuration'),
      '#collapsible' => FALSE,
      '#tree' => TRUE,
    );

    $form['configuration']['username'] = array(
      '#type' => 'textfield',
      '#title' => t('Service username'),
      '#size' => 30,
      '#maxlength' => 60,
      '#attributes' => array('autocomplete' => 'off'),
      '#description' => t('This should be same as the username on the server you are connecting to.'),
      '#required' => TRUE,
    );

    $password_exists = isset($this->configuration['password']);
    $password_description = $password_exists
      ? t('This should be same as the password on the server you are connecting to. Leave blank unless you need to change this.')
      : t('This should be same as the password on the server you are connecting to.');
    $form['configuration']['password'] = array(
      '#type' => 'password',
      '#title' => t('Service password'),
      '#size' => 30,
      '#maxlength' => 60,
      '#attributes' => array('autocomplete' => 'off'),
      '#description' => $password_description,
      '#required' => !$password_exists,
    );
  }

  /**
   * Submit handler for saving/updating connections of this class.
   *
   * @see clients_connection_form_submit()
   */
  function connectionSettingsForm_submit($form, &$form_state) {
    // This is here to show an example of how this method works.
    parent::connectionSettingsForm_submit($form, $form_state);
  }

  // ============================================ Resource retrieval.

  /**
   * Common helper for reacting to an error from a REST call.
   *
   * Gets the error from the response, logs the error message,
   * and throws an exception, which should be caught by the module making use
   * of the Clients connection API.
   *
   * @param $response
   *  The REST response data, decoded.
   *
   * @throws Exception
   */
  function handleRestError($response) {
    if ($response->code != 200) {
      watchdog('clients', 'Error with REST request. Error was code @code with error "@error" and message "@message".', array(
        '@code'     => $response->code,
        '@error'    => $response->error,
        '@message'  => $response->status_message,
      ));

      throw new Exception(t("Clients connection error, got message '@message'.", array(
        '@message' => $response->status_message,
      )), $response->code);
    }
  }

  /**
   * Log in as the configured user.
   *
   * This requires the server type 'application/x-www-form-urlencoded' to be
   * enabled.
   *
   * @param $session_id
   *  A session ID obtained from calling system.connect.
   *
   * @return
   *  The full data returned from the remote call.
   */
  function userLogin() {
    // Based on example code at http://drupal.org/node/910598.
    $data = array(
      'username' => $this->configuration['username'],
      'password' => $this->configuration['password'],
    );
    $data = http_build_query($data, '', '&');
    $headers = array();
    $options = array(
      'headers' => array(
        'Accept' => 'application/json',
      ),
      'method' => 'POST',
      'data' => $data,
    );

    $response = drupal_http_request($this->endpoint . '/user/login', $options);
    $data = json_decode($response->data);

    // Check if login was successful.
    $this->handleRestError($response);

    // Set our cookie for subsequent requests.
    $this->cookie = $data->session_name . '=' . $data->sessid;

    return $data;
  }

  /**
   * API function to request a remote resource.
   *
   * (This function has a rubbish name and parameters for historical reasons:
   * XMLRPC connection classes were developed first.)
   *
   * @param $method
   *  The path of the remote resource to retrieve.
   * @param $method_params
   *  An array of further parameters for the request. This should contain:
   *  - The HTTP method.
   *  - (optional) An array of data for the request, such as POST data.
   *
   * @return
   *  Whatever is returned from the remote site.
   */
  function callMethodArray($method, $method_params = array()) {
    $resource_path = $method;
    $http_method = array_shift($method_params);
    // The data array doesn't have to be present, so we have to fiddle about
    // to make sure we don't pass a NULL for it to makeRequest().
    if (count($method_params)) {
      $data = array_shift($method_params);
    }
    else {
      $data = array();
    }

    return $this->makeRequest($resource_path, $http_method, $data);
  }

  /**
   * Make a REST request.
   *
   * Examples:
   * Retrieve a node:
   *  makeRequest('node/NID', 'GET');
   * Update a node:
   *  makeRequest('node/NID', 'POST', $data);
   *
   * @param $resource_path
   *  The path of the resource. Eg, 'node', 'node/1', etc.
   * @param $http_method
   *  The HTTP method. One of 'GET', 'POST', 'PUT', 'DELETE'. For an explanation
   *  of how the HTTP method affects the resource request, see the Services
   *  documentation at http://drupal.org/node/783254.
   * @param $data = array()
   *  (Optional) An array of data to pass to the request.
   *
   * @return
   *  The data from the request response.
   */
  function makeRequest($resource_path, $http_method, $data = array()) {
    if (!isset($this->cookie)) {
      $this->userLogin();
    }

    $options = array();
    // Pass in the login cookie we received previously.
    $options['headers']['Cookie'] = $this->cookie;
    $options['data'] = http_build_query($data, '', '&');
    $options['method'] = $http_method;
    $response = drupal_http_request($this->endpoint . '/' . $resource_path, $options);

    $this->handleRestError($response);

    $result = json_decode($response->data);
    return $result;
  }

}
