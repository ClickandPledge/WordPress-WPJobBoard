<?php
/**
 * payfast_common.inc
 *
 * Copyright (c) 2009-2012 Click & Pledge
 * 
 * LICENSE:
 * 
 * This payment module is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This payment module is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 * 
 * @author     Kamran Razvan
 * @copyright  2013 Click & Pledge
 * @license    http://www.opensource.org/licenses/lgpl-license.php LGPL
 */

//// Create user agent string
// User agent constituents (for cURL)
define( 'CNP_SOFTWARE_NAME', 'WP Job Board' );
define( 'CNP_SOFTWARE_VER', "3.6.0" );
define( 'CNP_MODULE_NAME', 'PayFast-WP-Job-Board' );
define( 'CNP_MODULE_VER', '1.0.0' );

// Features
// - PHP
$clickandpledgeFeatures = 'PHP '. phpversion() .';';

// - cURL
if( in_array( 'curl', get_loaded_extensions() ) )
{
    define( 'CNP_CURL', '' );
    $clickandpledgeVersion = curl_version();
    $clickandpledgeFeatures .= ' curl '. $clickandpledgeVersion['version'] .';';
}
else
    $clickandpledgeFeatures .= ' nocurl;';

// Create user agrent
define( 'CNP_USER_AGENT', CNP_SOFTWARE_NAME .'/'. CNP_SOFTWARE_VER .' ('. trim( $clickandpledgeFeatures ) .') '. CNP_MODULE_NAME .'/'. CNP_MODULE_VER );


// General Defines
define( 'CNP_TIMEOUT', 15 );
define( 'CNP_EPSILON', 0.01 );

// Messages
    // Error
define( 'CNP_ERR_AMOUNT_MISMATCH', 'Amount mismatch' );
define( 'CNP_ERR_BAD_ACCESS', 'Bad access of page' );
define( 'CNP_ERR_BAD_SOURCE_IP', 'Bad source IP address' );
define( 'CNP_ERR_CONNECT_FAILED', 'Failed to connect to PayFast' );
define( 'CNP_ERR_INVALID_SIGNATURE', 'Security signature mismatch' );
define( 'CNP_ERR_MERCHANT_ID_MISMATCH', 'Merchant ID mismatch' );
define( 'CNP_ERR_NO_SESSION', 'No saved session found for ITN transaction' );
define( 'CNP_ERR_ORDER_ID_MISSING_URL', 'Order ID not present in URL' );
define( 'CNP_ERR_ORDER_ID_MISMATCH', 'Order ID mismatch' );
define( 'CNP_ERR_ORDER_INVALID', 'This order ID is invalid' );
define( 'CNP_ERR_ORDER_PROCESSED', 'This order has already been processed' );
define( 'CNP_ERR_PDT_FAIL', 'PDT query failed' );
define( 'CNP_ERR_PDT_TOKEN_MISSING', 'PDT token not present in URL' );
define( 'CNP_ERR_SESSIONID_MISMATCH', 'Session ID mismatch' );
define( 'CNP_ERR_UNKNOWN', 'Unkown error occurred' );

    // General
define( 'CNP_MSG_OK', 'Payment was successful' );
define( 'CNP_MSG_FAILED', 'Payment has failed' );
define( 'CNP_MSG_PENDING',
    'The payment is pending. Please note, you will receive another Instant'.
    ' Transaction Notification when the payment status changes to'.
    ' "Completed", or "Failed"' );
    



/**
 * clickandpledgelog
 *
 * Log function for logging output.
 *
 * @author Jonathan Smit
 * @param $msg String Message to log
 * @param $close Boolean Whether to close the log file or not
 */
function clickandpledgelog( $msg = '', $close = false )
{
    static $fh = 0;
    global $module;

    // Only log if debugging is enabled
    if( CNP_DEBUG )
    {
        if( $close )
        {
            fclose( $fh );
        }
        else
        {
            // If file doesn't exist, create it
            if( !$fh )
            {
                $pathinfo = pathinfo( __FILE__ );
                $fh = fopen( $pathinfo['dirname'] .'/payfast.log', 'a+' );
            }

            // If file was successfully created
            if( $fh )
            {
                $line = date( 'Y-m-d H:i:s' ) .' : '. $msg ."\n";

                fwrite( $fh, $line );
            }
        }
    }
}

/**
 * clickandpledgeGetData
 *  
 * @author Jonathan Smit
 */
function clickandpledgeGetData()
{
    // Posted variables from ITN
    $clickandpledgeData = $_POST;

    // Strip any slashes in data
    foreach( $clickandpledgeData as $key => $val )
        $clickandpledgeData[$key] = stripslashes( $val );

    // Return "false" if no data was received
    if( sizeof( $clickandpledgeData ) == 0 )
        return( false );
    else
        return( $clickandpledgeData );
}

/**
 * clickandpledgeValidSignature
 * 
 * @author Jonathan Smit
 */
function clickandpledgeValidSignature( $clickandpledgeData = null, &$clickandpledgeParamString = null )
{
    // Dump the submitted variables and calculate security signature
    foreach( $clickandpledgeData as $key => $val )
    {
        if( $key != 'signature' )
        {
            $clickandpledgeParamString .= $key .'='. urlencode( $val ) .'&';
        }
        else
        {
            break;
        }
    }

    // Remove the last '&' from the parameter string
    $clickandpledgeParamString = substr( $clickandpledgeParamString, 0, -1 );
    $signature = md5( $clickandpledgeParamString );
    
    $result = ( $clickandpledgeData['signature'] == $signature );

    clickandpledgelog( 'Signature = '. ( $result ? 'valid' : 'invalid' ) );
    
    
    return( $result );
}

/**
 * clickandpledgeValidData
 *
 * @author Jonathan Smit
 * @param $clickandpledgeHost String Hostname to use 
 * @param $clickandpledgeParamString String
 */
function clickandpledgeValidData( $clickandpledgeHost = 'www.payfast.co.za', $clickandpledgeParamString = '' )
{

    clickandpledgelog( 'Host = '. $clickandpledgeHost );
    

    // Use cURL (if available)
    if( defined( 'CNP_CURL' ) )
    {
        // Variable initialization
        $url = 'https://'. $clickandpledgeHost .'/eng/query/validate';       
        // Create default cURL object
        $ch = curl_init();
        // Set cURL options - Use curl_setopt for freater PHP compatibility
        // Base settings
        curl_setopt( $ch, CURLOPT_USERAGENT, CNP_USER_AGENT );  // Set user agent
        curl_setopt( $ch, CURLOPT_RETURNTRANSFER, true );      // Return output as string rather than outputting it
        curl_setopt( $ch, CURLOPT_HEADER, false );             // Don't include header in output
        curl_setopt( $ch, CURLOPT_SSL_VERIFYHOST, 2);
        curl_setopt( $ch, CURLOPT_SSL_VERIFYPEER, false );
        
        // Standard settings
        curl_setopt( $ch, CURLOPT_URL, $url );
        curl_setopt( $ch, CURLOPT_POST, true );
        curl_setopt( $ch, CURLOPT_POSTFIELDS, $clickandpledgeParamString );
        curl_setopt( $ch, CURLOPT_TIMEOUT, CNP_TIMEOUT );
    
        // Execute CURL
        $response = curl_exec( $ch );
        curl_close( $ch );
    }
    // Use fsockopen
    else
    {
        // Variable initialization
        $header = '';
        $res = '';
        $headerDone = false;
         
        // Construct Header
        $header = "POST /eng/query/validate HTTP/1.0\r\n";
        $header .= "Host: ". $clickandpledgeHost ."\r\n";
        $header .= "User-Agent: ". CNP_USER_AGENT ."\r\n";
        $header .= "Content-Type: application/x-www-form-urlencoded\r\n";
        $header .= "Content-Length: " . strlen( $clickandpledgeParamString ) . "\r\n\r\n";
 
        // Connect to server
        $socket = fsockopen( 'ssl://'. $clickandpledgeHost, 443, $errno, $errstr, CNP_TIMEOUT );
 
        // Send command to server
        fputs( $socket, $header . $clickandpledgeParamString );
 
        // Read the response from the server
        while( !feof( $socket ) )
        {
            $line = fgets( $socket, 1024 );
 
            // Check if we are finished reading the header yet
            if( strcmp( $line, "\n" ) == 0 )
            {
                // read the header
                $headerDone = true;
            }
            // If header has been processed
            else if( $headerDone )
            {
                // Read the main response
                $response .= $line;
            }
        }
        
    }

    clickandpledgelog( "Response:\n". print_r( $response, true ) );

    // Interpret Response
    $lines = explode( "\n", $response );
    $verifyResult = trim( $lines[0] );
    
    if( strcasecmp( $verifyResult, 'VALID' ) == 0 )
        return( true );
    else
        return( false );
}

/**
 * clickandpledgeValidIP
 *
 * @author Jonathan Smit
 * @param $sourceIP String Source IP address 
 */
function clickandpledgeValidIP( $sourceIP )
{
    // Variable initialization
    $validHosts = array(
        'www.payfast.co.za',
        'sandbox.payfast.co.za',
        'w1w.payfast.co.za',
        'w2w.payfast.co.za',
        );

    $validIps = array();

    foreach( $validHosts as $clickandpledgeHostname )
    {
        $ips = gethostbynamel( $clickandpledgeHostname );

        if( $ips !== false )
            $validIps = array_merge( $validIps, $ips );
    }

    // Remove duplicates
    $validIps = array_unique( $validIps );

    clickandpledgelog( "Valid IPs:\n". print_r( $validIps, true ) );

    if( in_array( $sourceIP, $validIps ) )
        return( true );
    else
        return( false );
}

/**
 * clickandpledgeAmountsEqual
 * 
 * Checks to see whether the given amounts are equal using a proper floating
 * point comparison with an Epsilon which ensures that insignificant decimal
 * places are ignored in the comparison.
 * 
 * eg. 100.00 is equal to 100.0001
 *
 * @author Jonathan Smit
 * @param $amount1 Float 1st amount for comparison 
 * @param $amount2 Float 2nd amount for comparison
 */
function clickandpledgeAmountsEqual( $amount1, $amount2 )
{
    if( abs( floatval( $amount1 ) - floatval( $amount2 ) ) > CNP_EPSILON )
        return( false );
    else
        return( true );
}

?>
