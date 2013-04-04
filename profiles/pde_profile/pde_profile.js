(function ($) {
  function pdeProfileCopy(e) {
    $('#edit-site-mail').val($('#edit-account-mail').val());
  }
  
  Drupal.behaviors.pdeProfile = {
    attach: function(context, settings) {
      $('#edit-site-information').hide();
      $('#edit-account-mail').focus();
      $('#edit-submit:not(.pdeProfile-processed)').addClass('pdeProfile-processed').bind('click', pdeProfileCopy);
    },
    detach: function(context, settings) {
      $('#edit-submit.pdeProfile-processed').removeClass('pdeProfile-processed').unbind('click', pdeProfileCopy);
    }
  };
})(jQuery);