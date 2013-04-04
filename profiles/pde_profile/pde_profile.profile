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

  $slogan = variable_get('site_slogan', FALSE);
  drupal_set_title(!empty($slogan) ? $slogan : t('Master branch'));
}

function pde_profile_after_build($element, $form_state) {
  $element['pass1']['#attributes']['value'] = $element['pass2']['#attributes']['value'] = 'admin';
  return $element;
}