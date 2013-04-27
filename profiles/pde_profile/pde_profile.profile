<?php
/**
 * @file
 * Enables modules and site configuration for a minimal site installation.
 */

/**
 * Implements hook_form_FORM_ID_alter() for install_configure_form().
 *
 * Allows the profile to alter the site configuration form.
 */
function pde_profile_form_install_configure_form_alter(&$form, $form_state) {
  // Prevent users from changing the site_name and site_slogan
  $path = drupal_get_path('module', 'pde_profile');
  $form['site_information']['site_name']['#access'] = FALSE;
  $form['site_information']['site_mail']['#attached']['js'] = array(array('data' => $path.'/pde_profile.js'));
  $form['admin_account']['account']['name']['#default_value'] = 'admin';
  $form['server_settings']['site_default_country']['#default_value'] = 'TH';
  $form['admin_account']['account']['pass']['#after_build'][] = 'pde_profile_after_build';
  $form['admin_account']['account']['pass']['#description'] = t('DEFAULT: admin');

  $args = array('!link' => '<a target="_blank" href="http://usda.sammyspets.com/user/register">USDA Nutritional Database Service</a>');
  $form['usda'] = array(
    '#type' => 'fieldset',
    '#title' => t('Credentials for USDA Remote Data'),
    '#description' => t('Before submitting this form go to the !link user registration page to register for an account.', $args),
    '#tree' => TRUE,
    '#weight' => -1,
  );
  $form['usda']['username'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#required' => TRUE,
  );
  $form['usda']['password'] = array(
    '#type' => 'password',
    '#title' => t('Password'),
    '#required' => TRUE,
  );
  $form['#submit'][] = 'pde_profile_usda_data_retrieve';

  $slogan = variable_get('site_slogan', FALSE);
  drupal_set_title(!empty($slogan) ? $slogan : t('Master branch'));
}

function pde_profile_after_build($element, $form_state) {
  $element['pass1']['#attributes']['value'] = $element['pass2']['#attributes']['value'] = 'admin';
  return $element;
}

function pde_profile_usda_data_retrieve(&$form, &$form_state) {
  $entity = entity_load_single('clients_connection', 'usda_ndbs');
  $entity->configuration = $form_state['values']['usda'];
  $entity->save();
  $form_state['redirect'] = 'admin/content/usda/import';
}
