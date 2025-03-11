///////////////////////////////////////////////////////////////////////////
//
//MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//MM MM MM MM     AA   AA         TT      RR    RR     II       xxx
//MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       xxx
//MM       MM   AA       AA       TT      RR  RR       II      XX XX
//MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//Company      : Matrix Comsec Pvt. Ltd., Baroda, India.
//Project      : Cosec Door
//Created By   : Jaydeep Faldu
//File         : CafeteriaSettings.js
//Description  : This file contains javascript functions for main navigation panel.
//
/////////////////////////////////////////////////////////////////////////////
var Common_XML 		= null;
var CafeSetting_xml = null;
var mediumWidth		= 50;
var ref_cafeSetting = null;

var cafeSetting 	=
{	
	"numberOfTxReceipts":"", 
};


function getCafeSetting()
{	
	var query = "GetCafeSetting?";	
	//query += apendWebFsp()+"numberOfTxReceipts";
  	
	var responseCode = ajaxrequest("post", "submit" , query, updateDataToObjList, cafeSetting);
	if(responseCode == "0")
	{
		setCafeSetting();
	}
	return true;
}

function setCafeSetting()
{
	setElemValue("#PRINT_CNT", cafeSetting["numberOfTxReceipts"]);
	return false;
}

function saveCafePara()
{
	var query = "SetCafeSetting?";
	query += apendWebFsp()+"numberOfTxReceipts"+ valFsp +cafeSetting["numberOfTxReceipts"];

	ajaxrequest("post", "submit", query);
	displayValidationMessage(error_img ,"Saved Successfully", true);
	return response;
}

function handleCafeSettingEvent(e)
{
	var id = $(this).attr("id");
	var element = GetElementFormID(id);
	var index = GetIndexFromID(id);
	
	if(e.type === "change")
	{
		switch(element)
		{
			default: break;
			
			case "PRINT_CNT":
			{
				cafeSetting["numberOfTxReceipts"] = getElemValue(this);
				
			}break;
		}
	}
	
		/*****************************************************
						Btn Action
		 ****************************************************/
	if(e.type === "click")
	{
		switch(element)
		{
			default: break;
			
			case "submit":
			{
				saveCafePara();
			}break;
			
			case "cancel":
			{
				if(getCafeSetting() == true)
				{
					setCafeSetting();
				}
			}break;			
		}
	}
}

function cafeSettingEventBind()
{
	/*********************************************/
		NOTE:	/*   Always Register this event first For trime 
			         All Input field.*/		
			    
		$("input:text").change(treamAllInputTextField);
		StopPastFronInputTextField();
	/*******************************************/
	var container = $("#CafeSetting");
	var sel_elements = $("select", $(container));
	var input_elements = $("input", $(container));
	
	for(var index = 0; index < sel_elements.length; index++)
	{
		registerHandlers($(sel_elements[index]), handleCafeSettingEvent);
	}
	
	for(var index = 0; index < input_elements.length; index++)
	{
		registerHandlers($(input_elements[index]), handleCafeSettingEvent);
	}
}

function InitCafeSettings()
{
	$("#PRINT_CNT").css("width",mediumWidth);
	Common_XML = getLanguageXmlDoc("configuration", "MxCommon");
	translatePage(Common_XML, document, "name");
	
	CafeSetting_xml = getLanguageXmlDoc("configuration", "CafeSetting");
	translatePage(CafeSetting_xml, document, "name");
	cafeSettingEventBind();
	
	if(getCafeSetting() == true)
	{
		setCafeSetting();
	}

	$("#PRINT_CNT").focus();
	
}

window.onresize = function onresize(){
	mainPageDivSet();
};

$(document).ready(function()
{	
	mainPageDivSet();
	InitCafeSettings();
}); 

