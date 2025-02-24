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
//File         : BluetoothSetting.js
//Description  : This file contains javascript functions for main navigation panel.
//
/////////////////////////////////////////////////////////////////////////////
var BluetoothSetting_xml	=	null;
var Common_XML		=	null;
var DoorType;
var RangeStrengthMax = 10;
var RangeStrengthMin = -40;

var BluetoothSetting	=
{	
	"AdvertiseBluetooth":"",
	"MACAddress":"",
	"name":"",
	"UUID":"",
	"btRange":"",
	"CustomBtRange":"",
};

var BluetoothRange =
{
	"Short" : 0,
	"Medium" : 1,
	"Long" : 2,
	"Custom" : 3,
}

function getBluetoothSetting()
{
	var query = "GetBluetoothSetting?";
	
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, BluetoothSetting);
	
	if(responseCode == "0")
	{
		setBluetoothSetting();
	}
	return true;
}

function saveBluetoothSetting()
{
	var query = "SetBluetoothSetting?";
	
	query += apendWebFsp()+"AdvertiseBluetooth"+valFsp+ BluetoothSetting.AdvertiseBluetooth;
	if(BluetoothSetting.AdvertiseBluetooth == "1")
	{
		query += apendWebFsp()+"name"+valFsp+ BluetoothSetting.name;
		query += apendWebFsp()+"btRange"+valFsp+ BluetoothSetting.btRange;
		query += apendWebFsp()+"CustomBtRange"+valFsp+ BluetoothSetting.CustomBtRange;
	}

	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, BluetoothSetting);
	
	if(responseCode == "0")
	{
		setBluetoothSetting();
		displayValidationMessage(success_img ,translateNode(Common_XML,"saveSuccess"), true);
	}
	return true;
}

function validateBluetoothSetting()
{
	if(BluetoothSetting.AdvertiseBluetooth == "1")
	{
		if(isFieldBlank(BluetoothSetting.name, translateNode(Common_XML,"nameStr"), BluetoothSetting, "name") == false)
		{
			return false;
		}
		if(BluetoothSetting["btRange"] == BluetoothRange.Custom)
		{
			if(isFieldNaNOrNotInRange(BluetoothSetting.CustomBtRange, translateNode(Common_XML,"CustomBluetooth"), BluetoothSetting, "CustomBtRange",RangeStrengthMin, RangeStrengthMax) == false)
			{
				return false;
			}
		}
	}
	return true;
}

function setDefaultBluetoothSetting()
{
	var query = "SetDfltBluetoothSetting?";
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, BluetoothSetting);
	
	if(responseCode == "0")
	{
		setBluetoothSetting();
		displayValidationMessage(success_img ,translateNode(Common_XML,"saveSuccess"), true);
	}
}

function setBluetoothSetting()
{
	(BluetoothSetting.AdvertiseBluetooth == "1")?checkElem("#AdvertiseBluetooth"):uncheckElem("#AdvertiseBluetooth");
	setElemValue("#MACAddress", 	BluetoothSetting.MACAddress);
	setElemValue("#name", 	BluetoothSetting.name);
	setElemValue("#UUID", 	BluetoothSetting.UUID);
	if(BluetoothSetting.AdvertiseBluetooth == "1")
	{
		enableElements(["#name","#rangeShort","#rangeMedium","#rangeLong","#rangeCustom","#CustomBluetoothRange","#CustomBluetooth"]);
	}
	else
	{
		disableElements(["#name","#rangeShort","#rangeMedium","#rangeLong","#rangeCustom","#CustomBluetoothRange","#CustomBluetooth"]);
	}
	
	if(BluetoothSetting["btRange"] == BluetoothRange.Short)
	{
		checkElem("#rangeShort");
		hideElem("#CustomBluetoothRow");
	}
	else if(BluetoothSetting["btRange"] == BluetoothRange.Medium)
	{
		checkElem("#rangeMedium");
		hideElem("#CustomBluetoothRow");
	}
	else if(BluetoothSetting["btRange"] == BluetoothRange.Long)
	{
		checkElem("#rangeLong");
		hideElem("#CustomBluetoothRow");
	}
	
	else if(BluetoothSetting["btRange"] == BluetoothRange.Custom)
	{
		checkElem("#rangeCustom");
		if(BluetoothSetting.AdvertiseBluetooth == "1")
		{
			showElem("#CustomBluetoothRow");
			enableElements(["#CustomBluetoothRange","#CustomBluetooth"]);
		}
	}

	if(!isNaN(BluetoothSetting.CustomBtRange))
	{
		setElemValue("#CustomBluetooth", BluetoothSetting.CustomBtRange);
	}
	if((BluetoothSetting.CustomBtRange >= RangeStrengthMin) && (BluetoothSetting.CustomBtRange <= RangeStrengthMax) && (BluetoothSetting.CustomBtRange.trime != ""))
	{
		setElemValue("#CustomBluetoothRange", BluetoothSetting.CustomBtRange);
		$("#CustomBluetoothRange").tooltipster('content', BluetoothSetting.CustomBtRange);
	}


	return false;
}

function setPageelementWidth()
{
	$("#name").css("width", normalInputWidth);						// normalInputWidth	= "137px";
	$("#UUID").css("width", largeInputWidth);						// largeInputWidth 	= "280px";
}

function handleBluetoothSettingEvent(e)
{
	var id = $(this).attr("id");
	
	if(e.type === "change")
	{
		switch(id)
		{
			default: break;
			
			case "AdvertiseBluetooth":
				BluetoothSetting.AdvertiseBluetooth = isChecked(this)?"1":"0";
				if(BluetoothSetting.AdvertiseBluetooth == "1")
				{
					enableElements(["#name","#rangeShort","#rangeMedium","#rangeLong","#rangeCustom","#CustomBluetoothRange","#CustomBluetooth"]);
				}
				else
				{
					disableElements(["#name","#rangeShort","#rangeMedium","#rangeLong","#rangeCustom","#CustomBluetoothRange","#CustomBluetooth"]);
				}
				break;

			case "name":
			{
				BluetoothSetting.name = getElemValue(this);
			}break;
				
			case "rangeShort":
			{
				BluetoothSetting["btRange"] = "0";
				hideElem("#CustomBluetoothRow");
			}break;

			case "rangeMedium":
			{
				BluetoothSetting["btRange"] = "1";
				hideElem("#CustomBluetoothRow");
			}break;
				
			case "rangeLong":
			{
				BluetoothSetting["btRange"] = "2";
				hideElem("#CustomBluetoothRow");
			}break;

			case "rangeCustom":
			{
				BluetoothSetting["btRange"] = "3";
				showElem("#CustomBluetoothRow");
				enableElements(["#CustomBluetoothRange","#CustomBluetooth"]);
			}break;		
			
			case "CustomBluetooth":
			{
				BluetoothSetting.CustomBtRange = parseInt(getElemValue("#CustomBluetooth"));
				if(!isNaN(BluetoothSetting.CustomBtRange))
				{
					setElemValue("#CustomBluetooth", BluetoothSetting.CustomBtRange);
				}
				
				if((BluetoothSetting.CustomBtRange >= RangeStrengthMin) && (BluetoothSetting.CustomBtRange <= RangeStrengthMax) &&(BluetoothSetting.CustomBtRange.trime != "") )
				{
					setElemValue("#CustomBluetoothRange", BluetoothSetting.CustomBtRange);
					$("#CustomBluetoothRange").tooltipster('content', BluetoothSetting.CustomBtRange);
				}

			}break;	
			
			case "CustomBluetoothRange":
			{
				var CorrectionRatioSlider = document.getElementById("CustomBluetoothRange");
				var CorrectionRatioRangeOutput = document.getElementById("CustomBluetooth");

				CorrectionRatioRangeOutput.value = CorrectionRatioSlider.value;
				BluetoothSetting.CustomBtRange = CorrectionRatioRangeOutput.value;
			}break;
		}
	}
	else if (e.type === "click")
	{
		switch(id)
		{
			default: break;

			case "submit":
			{
				if(validateBluetoothSetting() == true)
				{
					saveBluetoothSetting();
				}
			}break;
			
			case "cancel":
			{
				getBluetoothSetting();
			}break;
			
			case "default":
			{
				hideElem("#CustomBluetoothRow");
				disableElements(["#CustomBluetoothRange","#CustomBluetooth"]);
				setDefaultBluetoothSetting();
			}break;
		}
	}
}

function BluetoothSettingEventBind()
{
	/*********************************************/
		NOTE:	/*   Always Register this event first For trime 
			         All Input field.*/		
			    
		$("input:text").change(treamAllInputTextField);
		StopPastFronInputTextField();
	/*******************************************/
	var container = $("#BluetoothSetting");
	var sel_elements = $("select", $(container));
	var input_elements = $("input", $(container));
	var img_elements = $("img", $(container));
	var td_elements = $("td", $(container));
	var li_elements = $("li", $(container));
	
	for(var index = 0; index < sel_elements.length; index++)
	{
		registerHandlers($(sel_elements[index]), handleBluetoothSettingEvent);
	}
	
	for(var index = 0; index < img_elements.length; index++)
	{
		registerHandlers($(img_elements[index]),handleBluetoothSettingEvent);
	}
	
	for(var index = 0; index < input_elements.length; index++)
	{
		registerHandlers($(input_elements[index]),handleBluetoothSettingEvent);
	}
	
	for(var index = 0; index < td_elements.length; index++)
	{
		registerHandlers($(td_elements[index]),handleBluetoothSettingEvent);
	}
	for(var index = 0; index < li_elements.length; index++)
	{
		registerHandlers($(li_elements[index]), handleBluetoothSettingEvent);
	}
	
	$("#name").keypress(filterAsciiCharWithSpace);
	$("#CustomBluetooth").keypress(filterPositiveNegativeNumKey);

}

function initBluetoothSetting()
{
	setPageelementWidth();
	clearValidationMessage();

	Common_XML = getLanguageXmlDoc("configuration", "MxCommon");
	translatePage(Common_XML, document, "name");

	BluetoothSetting_xml = getLanguageXmlDoc("configuration", "BluetoothSetting");
	translatePage(BluetoothSetting_xml, document, "name");

	BluetoothSettingEventBind();

	$('.tooltip').tooltipster();
	$("#rangeInfo").tooltipster('option', "contentAsHTML", "true");
	$("#rangeInfo").tooltipster('content', translateElemFrom("configuration","BluetoothSetting", "rangeInfoMsg"));
	$("#CustomBluetoothRange").tooltipster('option', "contentAsHTML", "true");

	$("#AdvertiseBluetooth").focus();
	getBluetoothSetting();

	var CorrectionRatioSlider = document.getElementById("CustomBluetoothRange");
	var CorrectionRatioRangeOutput = document.getElementById("CustomBluetooth");

	CorrectionRatioSlider.oninput = function()
	{
		CorrectionRatioRangeOutput.value = this.value
		BluetoothSetting.CustomBtRange = CorrectionRatioRangeOutput.value;
		var message = BluetoothSetting.CustomBtRange;
		$("#CustomBluetoothRange").tooltipster('content', message);
	}

	return false;
}

window.onresize = function onresize(){
	mainPageDivSet();
};

$(document).ready(function()
{	
	mainPageDivSet();
	initBluetoothSetting();
});
