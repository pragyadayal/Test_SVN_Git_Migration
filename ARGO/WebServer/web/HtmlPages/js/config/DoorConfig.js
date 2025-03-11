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
//File         : DoorConfig.js
//Description  : This file contains javascript functions for main navigation panel.
//
/////////////////////////////////////////////////////////////////////////////
var DoorProfile_xml	=	null;
var Common_XML		=	null;
var DoorType;
var SdkVerOne = 1;
var SdkVerTwo = 2;
var doorProfile	=
{	
	"exitSW":"",			"applMode":"", 			"doorAccMode":"",		"extrnlRdrMode":"",	
	"activeDays0":"",		"activeDays1":"",		"activeDays2":"",		"activeDays3":"",
	"activeDays4":"",		"activeDays5":"",		"activeDays6":"",
	"workingHours_HH1":"",	"workingHours_MM1":"",	"workingHours_HH2":"",	"workingHours_MM2":"",
	"templatePerFinger":"",	"maxFinger":"",		"isVariantWithInternalFpRdr":"",
};

function getDoorProfile()
{
	var query = "GetDoorProfile?";
	
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, doorProfile);
	
	if(responseCode == "0")
	{
		setDoorProfile();
	}
	return true;
}

function setDoorProfile()
{
	//////////////  Door Access Setting   ///////////////////////
	doorProfile.exitSW === true?checkElem("#exitSW"):uncheckElem("#exitSW");

	setElemValue("#applMode",				translateNode(DoorProfile_xml, "applMode"+doorProfile.applMode) );
	setElemValue("#doorAccessMode",			translateNode(DoorProfile_xml, "accessMode"+doorProfile.doorAccMode) );
	setElemValue("#extnlReaderMode", 		translateNode(DoorProfile_xml, "accessMode"+doorProfile.extrnlRdrMode) );
	
	for(var idx=0; idx<7; idx++)
	{
		(doorProfile["activeDays"+idx])?checkElem("#activeDays"+idx):uncheckElem("#activeDays"+idx);
	}
	setElemValue("#workingHours_HH1", 	doorProfile.workingHours_HH1);
	setElemValue("#workingHours_MM1", 	doorProfile.workingHours_MM1);
	setElemValue("#workingHours_HH2",	doorProfile.workingHours_HH2);
	setElemValue("#workingHours_MM2", 	doorProfile.workingHours_MM2);
	
	PadAllTimeFields();
	
	//////////////    Enrollment         ///////////////////////// 
	setElemValue("#templatePerFinger", 		translateNode(DoorProfile_xml, "templatePerFinger"+doorProfile.templatePerFinger) );
	setElemValue("#maxFinger", 				translateNode(DoorProfile_xml, "maxFinger"+ (doorProfile.maxFinger-1)) );
	
	var deviceCode = Base64.decode(getCookie('MxCosecDeviceCode'));
	
	if(doorProfile.isVariantWithInternalFpRdr == "0" && deviceCode == 20) // ARGO, If Internal FP not then hide
	{
		hideElements(["#templatePerFingerTable", "#Enrollment"]);
	}

	//////////////    General         ///////////////////////// 
	// setElemValue("#motherBoardHwVer", 		doorProfile.motherBoardHwVer);
// 	
	// if(doorProfile.sdkVersion == "1")
	// {
		// setElemValue("#sdkVersion", SdkVerOne);
	// }
	// else if(doorProfile.sdkVersion == "2")
	// {
		// setElemValue("#sdkVersion",SdkVerTwo);
	// }
	
	return false;
}

function PadAllTimeFields()
{
	AutoZeroPadTimeField("#workingHours_HH1");
	AutoZeroPadTimeField("#workingHours_MM1");
	AutoZeroPadTimeField("#workingHours_HH2");
	AutoZeroPadTimeField("#workingHours_MM2");
}

function setPageelementWidth()
{
	$("#workingHours_HH1").css("width", smallInputWidth);						// smallInputWidth = 50px
	$("#workingHours_MM1").css("width", smallInputWidth);
	$("#workingHours_HH2").css("width", smallInputWidth);
	$("#workingHours_MM2").css("width", smallInputWidth);
	$("#templatePerFinger").css("width", normalInputWidth);						// mediumInputWidth = 250px
	$("#maxFinger").css("width", normalInputWidth);	
	// $("#motherBoardHwVer").css("width", smallInputWidth);
	// $("#sdkVersion").css("width", smallInputWidth);
	
}

function initDoorProfile()
{
	setPageelementWidth();
	DoorType = Base64.decode(getCookie('MxDeviceType'));
	if(DoorType == 1)
	{
		$(".param-table").css("margin-top", 5+"px");
		$(".td-param-label-1").css("width", 85+"px");
		hideElements(["#applicationModeTr","#doorAccessModeTr","#extnlReaderAccessModeTr","#activeDaysTr","#workingHoursTr","#Enrollment","#templatePerFingerTable","#AccessSetting"]);
	}
	
	Common_XML = getLanguageXmlDoc("configuration", "MxCommon");
	translatePage(Common_XML, document, "name");

	DoorProfile_xml = getLanguageXmlDoc("configuration", "DoorProfile");
	translatePage(DoorProfile_xml, document, "name");

	getDoorProfile();
	return false;
}

window.onresize = function onresize(){
	mainPageDivSet();
};

$(document).ready(function()
{	
	mainPageDivSet();
	initDoorProfile();
});
