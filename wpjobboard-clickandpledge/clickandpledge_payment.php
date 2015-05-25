<?php
/*
Plugin Name: Click & Pledge WPJobBoard
Plugin URI: http://manual.clickandpledge.com/
Description: This plugin is to integrate WP Job Board Click & Pledge Payment system. Please contact Click & Pledge for assistance, support@clickpledge.com
Author: Kamran Razvan
Version: 1.0.0 (Beta)
Author URI: http://manual.clickandpledge.com/
*/

function wpjb_payment_clickandpledge($list) {
  global $wpjobboard;

  include_once dirname(__FILE__)."/clickandpledge_payment.class.php";
  $cnp = new Payment_ClickandPledge();
  // registers new payment method
  $list[$cnp->getEngine()] = get_class($cnp);
  return $list;
}
add_filter('wpjb_payments_list', 'wpjb_payment_clickandpledge');


