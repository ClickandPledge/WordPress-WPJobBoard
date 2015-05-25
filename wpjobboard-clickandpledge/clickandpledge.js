jQuery(document).ready(function($) {
	$('#payment-form').submit(function(event) {
        $("#payment-form").validate();
		if( $('#payment-form').valid() ){			
			//$form.find('button').prop('disabled', true);
			clickandpledgeTransaction();
		}		
        return false;
    });		
	
	function clickandpledgeTransaction() {
		var $form = $('#payment-form');
		var data = {
                action: "clickandpledge_transaction",
                engine: "clickandpledge_payment",
                id: WPJB_PAYMENT_ID,
				clickandpledge_selectedPaymentMethod:$("input[name=cnp_payment_method_selection]:checked").val(),
				
				clickandpledge_FirstName_CreditCard:$('#clickandpledge_FirstName_CreditCard').val(),
				clickandpledge_LastName_CreditCard:$('#clickandpledge_LastName_CreditCard').val(),
				
				clickandpledge_FirstName_eCheck:$('#clickandpledge_FirstName_eCheck').val(),
				clickandpledge_LastName_eCheck:$('#clickandpledge_LastName_eCheck').val(),
				
				clickandpledge_FirstName_Invoice:$('#clickandpledge_FirstName_Invoice').val(),
				clickandpledge_LastName_Invoice:$('#clickandpledge_LastName_Invoice').val(),
				
				clickandpledge_FirstName_PO:$('#clickandpledge_FirstName_PO').val(),
				clickandpledge_LastName_PO:$('#clickandpledge_LastName_PO').val(),
				
				clickandpledge_nameOnCard:$('#clickandpledge_nameOnCard').val(),
				clickandpledge_cardNumber:$('#clickandpledge_cardNumber').val(),
				clickandpledge_cvc:$('#clickandpledge_cvc').val(),
				
				clickandpledge_echeck_RoutingNumber:$('#clickandpledge_echeck_RoutingNumber').val(),
				clickandpledge_echeck_CheckNumber:$('#clickandpledge_echeck_CheckNumber').val(),				
				clickandpledge_echeck_AccountNumber:$('#clickandpledge_echeck_AccountNumber').val(),				
				clickandpledge_echeck_AccountType:$('#clickandpledge_echeck_AccountType').val(),
				clickandpledge_echeck_CheckType:$('#clickandpledge_echeck_CheckType').val(),
				clickandpledge_echeck_NameOnAccount:$('#clickandpledge_echeck_NameOnAccount').val(),
				clickandpledge_echeck_IdType:$('#clickandpledge_echeck_IdType').val(),
				
				InvoiceCheckNumber:$('#clickandpledge_Invoice_InvoiceNumber').val(),
				
				PurchaseOrderNumber:$('#clickandpledge_PurchaseOrder_OrderNumber').val(),				
				
				clickandpledge_Amount:$('#clickandpledge_Amount').val(),
				clickandpledge_Discount:$('#clickandpledge_Discount').val(),
				clickandpledge_cardExpMonth:$('#clickandpledge_cardExpMonth').val(),
				clickandpledge_cardExpYear:$('#clickandpledge_cardExpYear').val(),
				clickandpledge_AccountID:$('#clickandpledge_AccountID').val(),
				clickandpledge_AccountGuid:$('#clickandpledge_AccountGuid').val(),
				clickandpledge_OrderMode:$('#clickandpledge_OrderMode').val(),
				clickandpledge_OrganizationInformation:$('#clickandpledge_OrganizationInformation').val(),
				clickandpledge_ThankYouMessage:$('#clickandpledge_ThankYouMessage').val(),
				clickandpledge_TermsCondition:$('#clickandpledge_TermsCondition').val(),
				clickandpledge_email_customer:$('#clickandpledge_email_customer').val(),
				
				clickandpledge_isRecurring:$('#clickandpledge_isRecurring').val(),
				clickandpledge_RecurringMethod:$('#clickandpledge_RecurringMethod').val(),
				clickandpledge_indefinite:$('#clickandpledge_indefinite').val(),
				clickandpledge_Periodicity:$('#clickandpledge_Periodicity').val(),
				clickandpledge_Installment:$('#clickandpledge_Installment').val()
            };
		var request = $.ajax({
                url: ajaxurl,
                cache: false,
                type: "POST",
                data: data
            });
		
		request.done(function( msg ) {
		  var result = jQuery.parseJSON(msg);
		 //console.log(result);
		 if(result.ResultCode == 0) {
			$form.find(".payment-errors").removeClass("wpjb-flash-info");
			clickandpledgeResponse(msg);
		 } else {			
			$form.find(".payment-errors").removeClass("wpjb-flash-info").addClass("wpjb-flash-error").text(result.error);			
		 }		  
		});		
		request.fail(function( jqXHR, textStatus ) {
		  alert( "Request failed: " + textStatus );
		});
		return '2015';
	}
	function clickandpledgeResponse(msg) {
		var $form = $('#payment-form');
		var resultjson = jQuery.parseJSON(msg);
		 var data = {
                action: "wpjb_payment_accept",
                engine: "clickandpledge_payment",
                id: WPJB_PAYMENT_ID,
				clickandpledge_Amount:$('#clickandpledge_Amount').val(),
				clickandpledge_Discount:$('#clickandpledge_Discount').val(),
				token: resultjson.TransactionNumber
            };
		 var request = $.ajax({
                url: ajaxurl,
                cache: false,
                type: "POST",
                data: data
            });
		request.done(function( msg ) {
			$form.find('.payment-errors').removeClass("wpjb-flash-error").addClass("wpjb-flash-info").text('Payment done successfully');
			$form.find('div.htmlholder').hide();
			//$form.find('button').hide();
		});
	}

//Recurring Display
	if(jQuery('#clickandpledge_isRecurring').length > 0) {
		recurring_display('first');
		
		jQuery('#clickandpledge_isRecurring').click(function(){
			recurring_display('no');
		});
	}
	function recurring_display(cas) {	
		if(jQuery('#clickandpledge_isRecurring').is(':checked')) {
			jQuery('#clickandpledge_Periodicity_div').show();
			jQuery('#clickandpledge_RecurringMethod_div').show();
			if(jQuery('#clickandpledge_indefinite_div').length)
					jQuery('#clickandpledge_indefinite_div').show();
		} else {
			jQuery('#clickandpledge_Periodicity_div').hide();
			jQuery('#clickandpledge_RecurringMethod_div').hide();
			if(jQuery('#clickandpledge_indefinite_div').length) {
					jQuery('#clickandpledge_indefinite_div').hide();
			}
		}
		if(cas == 'first')
		isIndefinite(cas);
	}
	
	if(jQuery('#clickandpledge_RecurringMethod').length > 0) {
		jQuery('#clickandpledge_RecurringMethod').change(function(){
			isIndefinite();
		});
	}
		
	function isIndefinite(cas) {		
			if(cas != 'first') {
				if(jQuery('#clickandpledge_RecurringMethod').val() == 'Subscription') {
					if(jQuery('#clickandpledge_indefinite_div').length)
							jQuery('#clickandpledge_indefinite_div').show();
				}
				if(jQuery('#clickandpledge_RecurringMethod').val() == 'Installment') {
					if(jQuery('#clickandpledge_indefinite_div').length)
							jQuery('#clickandpledge_indefinite_div').hide();
				}
			}			
	}
	
	if(jQuery('#clickandpledge_indefinite').length > 0) {
		jQuery('#clickandpledge_indefinite').click(function(){
			if(jQuery('#clickandpledge_indefinite').is(':checked')) {
				jQuery('#clickandpledge_Installment_div').hide();
			} else {
				jQuery('#clickandpledge_Installment_div').show();
			}
		});
	}
	function isRecurring_echeck() {
		if(jQuery('#clickandpledge_isRecurring').is(':checked')) {					
				jQuery('#clickandpledge_Periodicity_p_echeck').show();
				jQuery('#clickandpledge_RecurringMethod_p_echeck').show();
				if(jQuery('#clickandpledge_indefinite_echeck').length) {
					if(jQuery('#clickandpledge_RecurringMethod_echeck').val() == 'Installment') {
					jQuery('#clickandpledge_indefinite_echeck').attr('checked', false);
					jQuery('#clickandpledge_indefinite_p_echeck').hide();
					jQuery('#clickandpledge_Installment_req_echeck').show();
					} else {
						jQuery('#clickandpledge_indefinite_p_echeck').show();
						jQuery('#clickandpledge_Installment_req_echeck').show();
					}
					//jQuery('#clickandpledge_indefinite_p').show();
				}
			} else {
				jQuery('#clickandpledge_Periodicity_p_echeck').hide();
				jQuery('#clickandpledge_RecurringMethod_p_echeck').hide();
				if(jQuery('#clickandpledge_indefinite_echeck').length)
					jQuery('#clickandpledge_indefinite_echeck').attr('checked', false);
					jQuery('#clickandpledge_indefinite_p_echeck').hide();
			}
	}	
});