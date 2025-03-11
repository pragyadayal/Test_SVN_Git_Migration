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
//Created By   : Meetkumar Bariya
//File         : system_test.js
//Description  : This file contains javascript functions for system_test.html 
// 				 file.
//
///////////////////////////////////////////////////////////////////////////// 

var Common_Xml 		= null;
var sysTest_xml 	= null;
var WifiTest_xml	= null;
var volt_Index		=	0;
const MASKED_FTP_PASSWORD = "...............";

/***************************************************************
			 		System Test -> Basic Information (Divison)
****************************************************************/
var sysTestBasicInfo =
{
	"employeeName":"",		"employeeID":"",		"deviceSerialNo":"",	"ftpPassword":"",
	"mainPCBHWDesine":"",	"connectorHWDesign":"",	"macAddress":"",		"result":"",
	"date":"",				"ftpIPAddress":"",		"ftpUserName":"",		"mainPCBSWVersion":"",		"isVariantWithInternalFpRdr":"",
};

function getSysTestBasicInfo()
{	
	var query = "GetSysTestBasicInfo?";
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, sysTestBasicInfo);
	if(responseCode == "0")
	{
		setSysTestBasicInfo();
	}
	return true;
}

function setSysTestBasicInfo()
{
	setElemValue("#employeeName", 		sysTestBasicInfo.employeeName);
	setElemValue("#employeeID", 		sysTestBasicInfo.employeeID);
	setElemValue("#deviceSerialNo", 	sysTestBasicInfo.deviceSerialNo);
	setElemValue("#mainPCBHWDesine", 	sysTestBasicInfo.mainPCBHWDesine);
	setElemValue("#connectorHWDesign", 	sysTestBasicInfo.connectorHWDesign);
	setElemValue("#macAddress", 		sysTestBasicInfo.macAddress);
	setElemValue("#date", 				sysTestBasicInfo.date);
	setElemValue("#ftpIPAddress", 		sysTestBasicInfo.ftpIPAddress);
	setElemValue("#ftpUserName", 		sysTestBasicInfo.ftpUserName);
	setElemValue("#mainPCBSWVersion", 	sysTestBasicInfo.mainPCBSWVersion);
	if(sysTestBasicInfo.ftpPassword != "")
	{
		sysTestBasicInfo.ftpPassword = MASKED_FTP_PASSWORD;
	}
	setElemValue("#ftpPassword", 		sysTestBasicInfo.ftpPassword);

	sysTestBasicInfo.result === 1 ?checkElem("#result"):uncheckElem("#result");
	
	var deviceCode = Base64.decode(getCookie('MxCosecDeviceCode'));
	
	if(sysTestBasicInfo.isVariantWithInternalFpRdr == "0" && deviceCode == 20) // ARGO, If Internal FP not then hide
	{
		hideElements(["#internalFPReader", "#internalFPReaderStatus", "#userFpEnrollment"]);
	}
		
	return false;
}

function saveBasicInfo()
{
	var query = "";
	query = "SetSysTestBasicInfo?";
	query += apendWebFsp()+"employeeName"+ valFsp + sysTestBasicInfo.employeeName;
	query += apendWebFsp()+"employeeID"+ valFsp + sysTestBasicInfo.employeeID;
	query += apendWebFsp()+"deviceSerialNo"+ valFsp + sysTestBasicInfo.deviceSerialNo;
	query += apendWebFsp()+"mainPCBHWDesine"+ valFsp + sysTestBasicInfo.mainPCBHWDesine;
	query += apendWebFsp()+"connectorHWDesign"+ valFsp + sysTestBasicInfo.connectorHWDesign;
	query += apendWebFsp()+"macAddress"+ valFsp + sysTestBasicInfo.macAddress;
	query += apendWebFsp()+"date"+ valFsp + sysTestBasicInfo.date;
	query += apendWebFsp()+"ftpIPAddress"+ valFsp + sysTestBasicInfo.ftpIPAddress;
	query += apendWebFsp()+"ftpUserName"+ valFsp + sysTestBasicInfo.ftpUserName;
	query += apendWebFsp()+"mainPCBSWVersion"+ valFsp + sysTestBasicInfo.mainPCBSWVersion;
	query += apendWebFsp()+"result"+ valFsp + sysTestBasicInfo.result;
	query += apendWebFsp()+"ftpPassword"+ valFsp + sysTestBasicInfo.ftpPassword;
	
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, sysTestBasicInfo);
	if(responseCode == "0")
	{
		getSysTestBasicInfo();
	}
	return responseCode;
}

function validateBasicInfo()
{
	var messageBlank = "";
	var message		 = "";
	
	// Invalid FTP IP Address
	message = translateNode(Common_XML,"inValid");
	message = message + " " + translateNode(sysTest_xml,"ftpIPAddress");		
	if(ValidateAddr($("#ftpIPAddress"), message) == false)
	{
		return false;
	}

	return true;	
}

function DisplayBasicInformationTab()
{
	$(".tab li").removeClass("active");	
	$("#basicInfo").addClass("active");
	hideElements(["#hwTest-div","#readerTest-div","#genIOTest-div","#others-div"]);
	showElem("#BasicInfo-div");
	clearValidationMessage();
	$("#employeeName").focus();
}

/***************************************************************
			 		System Test -> Hardware Test (Divison)
****************************************************************/
var sysTestHWTest =
{
	"assemblerName":"",	"assemblerID":"",	"vegaVoltage0":"",	"vegaVoltage1":"",
	"vegaVoltage2":"",	"vegaVoltage3":"",	"vegaVoltage4":"",	"vegaVoltage5":"",
	"frequency":"",		"vegaVoltage6":"",
};

function getSysTestHWTest()
{	
	var query = "GetSysTestHWTest?";
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, sysTestHWTest);
	if(responseCode == "0")
	{
		setSysTestHWTest();
	}
	return true;
}

function setSysTestHWTest()
{
	DisplayHWTestTab();
	setElemValue("#assemblerName", 		sysTestHWTest.assemblerName);
	setElemValue("#assemblerID", 		sysTestHWTest.assemblerID);

	sysTestHWTest.vegaVoltage0 === 1?checkElem("#vegaVoltage0"):uncheckElem("#vegaVoltage0");
	sysTestHWTest.vegaVoltage1 === 1?checkElem("#vegaVoltage1"):uncheckElem("#vegaVoltage1");
	sysTestHWTest.vegaVoltage2 === 1?checkElem("#vegaVoltage2"):uncheckElem("#vegaVoltage2");
	sysTestHWTest.vegaVoltage3 === 1?checkElem("#vegaVoltage3"):uncheckElem("#vegaVoltage3");
	sysTestHWTest.vegaVoltage4 === 1?checkElem("#vegaVoltage4"):uncheckElem("#vegaVoltage4");
	sysTestHWTest.vegaVoltage5 === 1?checkElem("#vegaVoltage5"):uncheckElem("#vegaVoltage5");
	sysTestHWTest.vegaVoltage6 === 1?checkElem("#vegaVoltage6"):uncheckElem("#vegaVoltage6");
	sysTestHWTest.frequency === 1?checkElem("#frequency"):uncheckElem("#frequency");
	
	return false;
}

function saveHWTestResult()
{
	var query = "";
	query = "SetSysTestHWTest?";
	query += "assemblerName"+ valFsp + sysTestHWTest.assemblerName;
	query += apendWebFsp()+"assemblerID"+ valFsp + sysTestHWTest.assemblerID;
	
	query += apendWebFsp()+"vegaVoltage0"+ valFsp + sysTestHWTest.vegaVoltage0;
	query += apendWebFsp()+"vegaVoltage1"+ valFsp + sysTestHWTest.vegaVoltage1;
	query += apendWebFsp()+"vegaVoltage2"+ valFsp + sysTestHWTest.vegaVoltage2;
	query += apendWebFsp()+"vegaVoltage3"+ valFsp + sysTestHWTest.vegaVoltage3;
	query += apendWebFsp()+"vegaVoltage4"+ valFsp + sysTestHWTest.vegaVoltage4;
	query += apendWebFsp()+"vegaVoltage5"+ valFsp + sysTestHWTest.vegaVoltage5;
	query += apendWebFsp()+"vegaVoltage6"+ valFsp + sysTestHWTest.vegaVoltage6;
	query += apendWebFsp()+"frequency"	 + valFsp + sysTestHWTest.frequency;

	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, sysTestHWTest);
	if(responseCode == "0")
	{
		setSysTestHWTest();
	}
	return responseCode;
}

function DisplayHWTestTab()
{
	$(".tab li").removeClass("active");	
	$("#hwTest").addClass("active");
	hideElements(["#BasicInfo-div","#readerTest-div","#genIOTest-div","#others-div"]);
	showElem("#hwTest-div");
	clearValidationMessage();
	$("#assemblerName").focus();
}

/***************************************************************
			 		System Test -> Reader Test (Divison)
****************************************************************/
var sysTestReaderTest =
{
	"intCardReader":"",				"intFingerReader":"",			"extReader":"",	
	"intCardReaderStatus":"",		"intFingerReaderStatus":"",		"extReaderStatus1":"",
	"extReaderStatus2":"",			"lastDetectedCardReader":"",	"cardID":"",
	"intEMProxRdrResult":"",		"intHIDProxRdrResult":"",		"intMiFareRdrResult":"",
	"intHIDiClassURdrResult":"",	"vegaExternelURdrResult":"",	"vegaExternelWRdrResult":"",
	"fpReaderSrNo":"",
};

var readerStatus=[	"INACTIVE", "ACTIVE", "UNKNOWN", "NONE", "PROGRESS"];
var lastDetectedReader = ["INTERNAL CARD READER", "FINGER READER", "EXTERNAL CARD READER", "NONE","undefined"];

function getSysTestReaderTest()
{	
	var query = "GetSysTestReaderTest?";
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, sysTestReaderTest);
	if(responseCode == "0")
	{
		setSysTestReaderTest();
	}
	return true;
}

function setSysTestReaderTest()
{
	// DisplayReaderTestTab();

	setElemValue("#lastDetectedCardReader",lastDetectedReader[parseDecimal(sysTestReaderTest.lastDetectedCardReader)]);
	setElemValue("#cardID",sysTestReaderTest.cardID);
	
	sysTestReaderTest.intEMProxRdrResult 		=== 1?checkElem("#intEMProxRdrResult")	:uncheckElem("#intEMProxRdrResult");
	sysTestReaderTest.intHIDProxRdrResult		=== 1?checkElem("#intHIDProxRdrResult")	:uncheckElem("#intHIDProxRdrResult");
	sysTestReaderTest.intMiFareRdrResult		=== 1?checkElem("#intMiFareRdrResult")	:uncheckElem("#intMiFareRdrResult");
	sysTestReaderTest.intHIDiClassURdrResult	=== 1?checkElem("#intHIDiClassURdrResult")	:uncheckElem("#intHIDiClassURdrResult");
	sysTestReaderTest.vegaExternelURdrResult 	=== 1?checkElem("#vegaExternelURdrResult")	:uncheckElem("#vegaExternelURdrResult");
	sysTestReaderTest.vegaExternelWRdrResult 	=== 1?checkElem("#vegaExternelWRdrResult")	:uncheckElem("#vegaExternelWRdrResult");

	return false;
}

function SetReaderStatusValue () 
{
	setElemValue("#intCardReader",parseDecimal(sysTestReaderTest.intCardReader));
	setElemValue("#intFingerReader",parseDecimal(sysTestReaderTest.intFingerReader));
	if(sysTestReaderTest.intFingerReader != "0")
	{
		setElemValue("#fpReaderSrNo",	sysTestReaderTest.fpReaderSrNo);
	}
	else
	{
		sysTestReaderTest.fpReaderSrNo = "";
		setElemValue("#fpReaderSrNo",	sysTestReaderTest.fpReaderSrNo);
	}
	setElemValue("#extReader",parseDecimal(sysTestReaderTest.extReader));
	setElemValue("#intCardReaderStatus",readerStatus[parseDecimal(sysTestReaderTest.intCardReaderStatus)]);
	setElemValue("#intFingerReaderStatus",readerStatus[parseDecimal(sysTestReaderTest.intFingerReaderStatus)]);
	setElemValue("#lastDetectedCardReader",lastDetectedReader[parseDecimal(sysTestReaderTest.lastDetectedCardReader)]);
	setElemValue("#cardID",sysTestReaderTest.cardID);
	if((parseDecimal(sysTestReaderTest.extReader)) === 9)
	{
		setElemValue("#extReaderStatus","Card: " + readerStatus[parseDecimal(sysTestReaderTest.extReaderStatus1)] + "    FP: " + readerStatus[parseDecimal(sysTestReaderTest.extReaderStatus2)]);
	}
	else
	{
		setElemValue("#extReaderStatus",readerStatus[parseDecimal(sysTestReaderTest.extReaderStatus1)]);
	}
}
function saveReaderResult()
{
	var query = "";
	query = "SetSysTestReaderTest?";
	query += "intEMProxRdrResult"+ valFsp + sysTestReaderTest.intEMProxRdrResult;
	query += apendWebFsp()+"intHIDProxRdrResult"	+ valFsp + sysTestReaderTest.intHIDProxRdrResult;
	query += apendWebFsp()+"intMiFareRdrResult"		+ valFsp + sysTestReaderTest.intMiFareRdrResult;
	query += apendWebFsp()+"intHIDiClassURdrResult"	+ valFsp + sysTestReaderTest.intHIDiClassURdrResult;
	query += apendWebFsp()+"vegaExternelURdrResult"	+ valFsp + sysTestReaderTest.vegaExternelURdrResult;
	query += apendWebFsp()+"vegaExternelWRdrResult"	+ valFsp + sysTestReaderTest.vegaExternelWRdrResult;
	
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, sysTestReaderTest);
	if(responseCode == "0")
	{
		setSysTestReaderTest();		
	}
	return responseCode;
}

function refreshReaderStatus()
{
	var query = "GetSysTestReaderTest?";
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, sysTestReaderTest);
	if(responseCode == "0")
	{
		SetReaderStatusValue ();
	}
	return true;
}

function saveSetReaderInfo()
{
	var query = "";
	query = "SetSysTestReaderTest?";
	query += "intCardReader"+ valFsp + sysTestReaderTest.intCardReader;
	query += apendWebFsp()+"intFingerReader"+ valFsp + sysTestReaderTest.intFingerReader;
	query += apendWebFsp()+"extReader"		+ valFsp + sysTestReaderTest.extReader;
	
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, sysTestReaderTest);
	if(responseCode == "0")
	{
		SetReaderStatusValue ();
	}
	return responseCode;
}

function DisplayReaderTestTab()
{
	$(".tab li").removeClass("active");	
	$("#readerTest").addClass("active");
	hideElements(["#hwTest-div","#BasicInfo-div","#genIOTest-div","#others-div"]);
	showElem("#readerTest-div");
	clearValidationMessage();
	
	$("#intCardReader").focus();
}

/***************************************************************
			 		System Test -> Generic IO Test (Divison)
****************************************************************/
var sysTestGenericIO =
{
	"buzzerTestRes":"",		"RTCTestRes":"",		"auxRelayTestRes":"",	"doorRelayTestRes":"",
	"auxInTestRes":"",		"doorSenseTestRes":"",	"sysTampTestRes":"",	"exitRdrTampTestRes":"",
	"lookTampTestRes":"",	"exitSWTestRes":"",		"usbTestRes":"",		"touchPanelTestRes":"",
	"mmcCardTestRes":"", 	"lcdTestRes":"",			
};

var testResult = ["FAIL","SUCCESS"];

function getSysTestGenericIO()
{
	var query = "GetSysTestGenericIO?";
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, sysTestGenericIO);
	if(responseCode == "0")
	{
		setSysTestGenericIO();
	}
	return true;
}

function setSysTestGenericIO()
{
	sysTestGenericIO.buzzerTestRes 		=== 1 ? checkElem("#buzzerTestRes") 	: uncheckElem("#buzzerTestRes") ;
	sysTestGenericIO.auxRelayTestRes 	=== 1 ? checkElem("#auxRelayTestRes")	: uncheckElem("#auxRelayTestRes") ;
	sysTestGenericIO.doorRelayTestRes	=== 1 ? checkElem("#doorRelayTestRes")	: uncheckElem("#doorRelayTestRes") ;
	sysTestGenericIO.lcdTestRes			=== 1 ? checkElem("#lcdTestRes") 		: uncheckElem("#lcdTestRes") ;

	setElemValue("#touchPanelTestRes",testResult[sysTestGenericIO.touchPanelTestRes]);
	setElemValue("#auxInTestRes",testResult[sysTestGenericIO.auxInTestRes]);
	setElemValue("#doorSenseTestRes",testResult[sysTestGenericIO.doorSenseTestRes]);
	setElemValue("#sysTampTestRes",testResult[sysTestGenericIO.sysTampTestRes]);
	setElemValue("#exitRdrTampTestRes",testResult[sysTestGenericIO.exitRdrTampTestRes]);
	setElemValue("#lookTampTestRes",testResult[sysTestGenericIO.lookTampTestRes]);
	setElemValue("#exitSWTestRes",testResult[sysTestGenericIO.exitSWTestRes]);
	setElemValue("#usbTestRes",testResult[sysTestGenericIO.usbTestRes]);
	setElemValue("#mmcCardTestRes",testResult[sysTestGenericIO.mmcCardTestRes]);
	setElemValue("#RTCTestRes",testResult[sysTestGenericIO.RTCTestRes]);
	
	return false;
}

function saveSysTestGenericIO()
{
	var query = "";
	query = "SetSysTestGenericIO?";
	query += "buzzerTestRes"+ valFsp + sysTestGenericIO.buzzerTestRes;
	query += apendWebFsp()+"auxRelayTestRes"	+ valFsp + sysTestGenericIO.auxRelayTestRes;
	query += apendWebFsp()+"doorRelayTestRes" 	+ valFsp + sysTestGenericIO.doorRelayTestRes;
	query += apendWebFsp()+"lcdTestRes"			+ valFsp + sysTestGenericIO.lcdTestRes;
	
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, sysTestGenericIO);
	if(responseCode == "0")
	{
		setSysTestGenericIO();
	}
	return responseCode;
}

function DisplayGenericIOTestTab()
{
	$(".tab li").removeClass("active");	
	$("#genIOTest").addClass("active");
	hideElements(["#hwTest-div","#readerTest-div","#BasicInfo-div","#others-div",]);
	showElem("#genIOTest-div");
	clearValidationMessage();
	$("#buzzerTestRes").focus();
}

/***************************************************************
			 		System Test -> Other Info (Divison)
****************************************************************/
var sysTestOtherInfo =
{
	"userEnrollTestRes":"",		"sysDefaultTestRes":"",		"sysRebootCount":"",	"wifiTestRes":"",
	"bluetoothTestRes":"",	
};

function getSysTestOtherInfo()
{
	var query = "GetSysTestOtherInfo?";
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, sysTestOtherInfo);
	if(responseCode == "0")
	{
		setSysTestOtherInfo();
	}
	return true;
}

function setSysTestOtherInfo()
{
	sysTestOtherInfo.userEnrollTestRes === 1 ? checkElem("#userEnrollTestRes") : uncheckElem("#userEnrollTestRes") ;
	sysTestOtherInfo.sysDefaultTestRes === 1 ? checkElem("#sysDefaultTestRes") : uncheckElem("#sysDefaultTestRes") ;
	sysTestOtherInfo.wifiTestRes	   === 1 ? checkElem("#wifiTestRes") 	   : uncheckElem("#wifiTestRes") ;
	
	setElemValue("#sysRebootCount", sysTestOtherInfo.sysRebootCount);
	setElemValue("#bluetoothTestRes",testResult[sysTestOtherInfo.bluetoothTestRes]);
	return false;
}

function saveSysTestOtherInfo()
{
	var query = "";
	query = "SetSysTestOtherInfo?";
	query += "userEnrollTestRes"+ valFsp + sysTestOtherInfo.userEnrollTestRes;
	query += apendWebFsp()+"sysDefaultTestRes"	+ valFsp + sysTestOtherInfo.sysDefaultTestRes;
	query += apendWebFsp()+"sysRebootCount"		+ valFsp + sysTestOtherInfo.sysRebootCount;
	query += apendWebFsp()+"wifiTestRes"		+ valFsp + sysTestOtherInfo.wifiTestRes;
	
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, sysTestOtherInfo);
	if(responseCode == "0")
	{
		setSysTestOtherInfo();
	}
	return responseCode;
}

var WiFiAcsPntsList = 
{
	"accsPntSts":"",
	"accsPntCnt":"",
	ssid	 	: new Array(),
	nwSecureF 	: new Array(),
	sigQuality	: new Array(),
	secType	 	: new Array(),
	keyMgmt	 	: new Array(),
};

var secTypeStr = ["open", "secure"];
var ConnectionStrength = ["", "Images/Weak.png", "Images/Moderate.png", "Images/Strong.png", "Images/Strong.png"];

function getWiFiAccessPoint()
{
	query = "GetWiFiAcsPntLst?";
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, WiFiAcsPntsList);
	clearBlockState();
	if (responseCode == "0") 
	{
		if(WiFiAcsPntsList.accsPntSts == "0")
		{
			displayValidationMessage(error_img, translateNode(WifiTest_xml,"wifiModuleNotPrsnt"), false);
			hideElem("#wifiTable");
		}
		else
		{
			setWifiAccsPntList();		
		}
	}
}

function setWifiAccsPntList()
{
	clearValidationMessage();
	if(parseDecimal(WiFiAcsPntsList.accsPntCnt) == 0)
	{
		hideElem("#wifiTable");
		displayValidationMessage(error_img, translateNode(WifiTest_xml,"noNwFound"), false);
	}
	else
	{	
		hideElem("#btDeviceTable");
		showElem("#wifiTable");
		for(var index=1; index<= parseDecimal(WiFiAcsPntsList.accsPntCnt);index++)
		{
			showElem("#availableNwRow"+index);
			setLabel("#nwName"+index, 	WiFiAcsPntsList.ssid[index-1]);
			setLabel("#nwType"+index, 	secTypeStr[parseDecimal(WiFiAcsPntsList.nwSecureF[index-1])]);
			$("#wifiIcons"+index).attr("src", ConnectionStrength[parseDecimal(WiFiAcsPntsList.sigQuality[index-1])]);
		}
		for(; index<= 10;index++)
		{
			hideElem("#availableNwRow"+index);
		}
	}
}

var BtDiscoveryList = 
{
	"btDisAddFrameReqF":"",
	"btDiscoverySts":"",
	"btDiscoverCnt":"",
	btDeviceName: new Array(),
	btStrength 	: new Array(),
};

function getBtDeviceList()
{
	hideElem("#btDeviceTable");
	
	do
	{
		query = "GetBtDeviceList?";
		var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, BtDiscoveryList);
		clearBlockState();
		if (responseCode == "0") 
		{
			if(BtDiscoveryList.btDiscoverySts == "0")
			{
				displayValidationMessage(error_img, translateNode(sysTest_xml,"failToStartBtDiscovery"), false);
				hideElem("#btDeviceTable");
				break;
			}
			else
			{
				setBtDeviceList();		
			}
		}
		else
		{
			break;	
		}
	}while(BtDiscoveryList.btDisAddFrameReqF == "1");
	
	// Update Bluetooth test result on webpage
	getSysTestOtherInfo();
}

function setBtDeviceList()
{
	clearValidationMessage();
	if(parseDecimal(BtDiscoveryList.btDiscoverCnt) == 0)
	{
		hideElem("#btDeviceTable");
		if(BtDiscoveryList.btDisAddFrameReqF == "0")
		{
			displayValidationMessage(error_img, translateNode(sysTest_xml,"noBtDeviceFound"), false);
		}
	}
	else
	{	
		hideElem("#wifiTable");
		showElem("#btDeviceTable");
		for(var index=1; index<= parseDecimal(BtDiscoveryList.btDiscoverCnt);index++)
		{
			showElem("#availableBtDevRow"+index);
			setLabel("#btDeviceName"+index, 	BtDiscoveryList.btDeviceName[index-1]);
			setLabel("#btSignalStrength"+index, BtDiscoveryList.btStrength[index-1]);
		}
		for(; index<= 10;index++)
		{
			hideElem("#availableBtDevRow"+index);
		}
	}
}

function DisplayOtherInfoTab()
{
	$(".tab li").removeClass("active");	
	$("#others").addClass("active");
	hideElements(["#hwTest-div","#readerTest-div","#genIOTest-div","#BasicInfo-div",]);
	showElem("#others-div");
	clearValidationMessage();
	$("#userEnrollTestRes").focus();
}

///////////////////////////////////

function handleSystemTestEvent(e)
{
	var id = $(this).attr("id");
	var element = GetElementFormID(id);
	var index = GetIndexFromID(id);
	
	if(e.type === "change")
	{
		switch(element)
		{
			default: break;
			/***************************************************************
			 		System Test -> Basic Information
			 ***************************************************************/
			case "employeeName":
			case "employeeID":
			case "deviceSerialNo":
			case "mainPCBHWDesine":
			case "connectorHWDesign":
			case "macAddress":
			case "date":
			case "ftpIPAddress":
			case "ftpUserName":
			case "mainPCBSWVersion":
			case "ftpPassword":
			{
			 	sysTestBasicInfo[id]	=	getElemValue(this);
			}break;
			
			case "result":
			{
				sysTestBasicInfo["result"] = isChecked("#result")?"1":"0";
			}break;
			
			/***************************************************************
			 		System Test -> Hardware Test
			 ***************************************************************/		
			case "assemblerName":
			{
				sysTestHWTest.assemblerName = getElemValue(this);
			}break;
			
			case "assemblerID":
			{
				sysTestHWTest.assemblerID = getElemValue(this);
			}break;
			
			case "vegaVoltage":
			{
				sysTestHWTest["vegaVoltage"+index] = isChecked("#vegaVoltage"+index)?"1":"0";
			}break;
			
			
			case "frequency":
			{
				sysTestHWTest["frequency"] = isChecked("#frequency")?"1":"0";

			}break;

			/***************************************************************
			 		System Test -> Readers Test
			 ***************************************************************/	
			 case "intCardReader":
			 case "intFingerReader":
			 case "extReader":
			 {
			 	sysTestReaderTest[id]	=	getElemValue(this);
			 }
			 break;
			 
			 case "intEMProxRdrResult":
			 case "intHIDProxRdrResult":
			 case "intMiFareRdrResult":
			 case "intHIDiClassURdrResult":			 
			 case "vegaExternelURdrResult":
			 case "vegaExternelWRdrResult":
			 {
				sysTestReaderTest[id] = isChecked(this)?"1":"0";
			 }
			 break;
			 			 
			 
			 /***************************************************************
			 		System Test -> Generic Input Output Test
			 ***************************************************************/		
			 case "buzzerTestRes":
			 case "auxRelayTestRes":
			 case "doorRelayTestRes":
			 case "lcdTestRes":
			 {
				sysTestGenericIO[id] = isChecked(this)?"1":"0";
			 }break;
			 
			 /***************************************************************
			 		System Test -> Other Test
			 ***************************************************************/
			
			case "userEnrollTestRes":
			case "sysDefaultTestRes":
			case "wifiTestRes":
			{
				sysTestOtherInfo[id] = isChecked(this)?"1":"0";
			}break;
			
			case "sysRebootCount":
			{
				sysTestOtherInfo.sysRebootCount = getElemValue(this);
			}break;
		}
	}
	if(e.type === "click")
	{
		switch(element)
		{
			default: break;

			/***************************************************************
			 		System Test -> Basic Information
			 ***************************************************************/
			case "basicInfo":
			{
				DisplayBasicInformationTab();
			}break;
			
			case "submitBasicInfo":
			{
				if(validateBasicInfo() == true)
				{
					saveBasicInfo();
					displayValidationMessage(error_img, translateNode(Common_Xml, "saveSuccess"), true);
				}
			}break;
			/***************************************************************
			 		System Test -> Hardware Test
			 ***************************************************************/			
			case "hwTest":
			{
				getSysTestHWTest();
				DisplayHWTestTab();
			}break;
			
			case "submitHWTest":
			{
				saveHWTestResult();
				displayValidationMessage(error_img, translateNode(Common_Xml, "saveSuccess"), true);
			
			}break;
			/***************************************************************
			 		System Test -> Readers Test
			 ***************************************************************/			
			case "readerTest":
			{
				getSysTestReaderTest();
				saveReaderResult();
				refreshReaderStatus();
				DisplayReaderTestTab();

			}break;

			case "setReader":
			{
				saveSetReaderInfo();
			}break;
			
			case "refresh":
			{
				refreshReaderStatus();
			}break;
			
			case "submitReaderResult":
			{
				saveReaderResult();
				displayValidationMessage(error_img, translateNode(Common_Xml, "saveSuccess"), true);
			}break;
			/***************************************************************
			 		System Test -> Generic Input Output Test
			 ***************************************************************/			
			case "genIOTest":
			{
				getSysTestGenericIO();
				DisplayGenericIOTestTab();
			}break;

			case "buzzerTest":
			case "auxRelayTest":
			case "doorRelayTest":
			case "touchPanelTest":
			case "auxInTest":
			case "doorSenseTest":
			case "sysTampTest":
			case "exitRdrTampTest":
			case "lookTampTest":
			case "exitSWTest":
			case "usb":
			case "mmcCardTest":
			case "RTCTest":
			case "lcdTest":
			{
				var query = "";
				query = "SetSysTestGenericIO?";
				query += element;
					
				ajaxrequest("post", "submit", query, updateDataToObjList, sysTestGenericIO);
				switch(element)
				{
					case "touchPanelTest":
					case "auxInTest":
					case "doorSenseTest":
					case "sysTampTest":
					case "exitRdrTampTest":
					case "lookTampTest":
					case "exitSWTest":
					case "usb":
					case "mmcCardTest":
					case "RTCTest":
					{
						setElemValue("#"+element+"Res", testResult[parseDecimal(sysTestGenericIO[element+"Res"])]);
					}break;
					
					default:
						break;
				}
			}break;
			
			case "submitGenericIOInfo":
			{
				saveSysTestGenericIO();
				displayValidationMessage(error_img, translateNode(Common_Xml, "saveSuccess"), true);
			}break;
			/***************************************************************
			 		System Test -> Other Test
			 ***************************************************************/			
			case "others":
			{
				getSysTestOtherInfo();
				DisplayOtherInfoTab();	
			}break;	
			
			case "userEnrollTest":
			case "sysDefaultTest":
			{
				var query = "";
				query = "SetSysTestOtherInfo?";
				query += element;
					
				ajaxrequest("post", "submit", query, updateDataToObjList, sysTestOtherInfo);
			}break;
			
			case "sysRebootTest":
			{
				sysTestOtherInfo.sysRebootCount = getElemValue(sysRebootCount);
				if(isFieldBlankOrNotInRange(sysTestOtherInfo.sysRebootCount,translateNode(sysTest_xml,"sysRebootCount"),sysTestOtherInfo,"sysRebootCount",1,200) === true)
				{
					var query = "";
					query = "SetSysTestOtherInfo?";
					query += "sysRebootTest"+ valFsp + sysTestOtherInfo.sysRebootCount;
					
					ajaxrequest("post", "submit", query, updateDataToObjList, sysTestOtherInfo);
				}
				else
				{
						displayValidationMessage(error_img, translateNode(sysTest_xml, "outOfRange"), false);
						$("#sysRebootCount").focus();
				}
			}break;
			
			case "wifiTest":
			{
				getWiFiAccessPoint();
			}break;
			
			case "bluetoothTest":
			{
				getBtDeviceList();
			}break;
			
			case "submitOtherInfo":
			{
				saveSysTestOtherInfo();
				displayValidationMessage(error_img, translateNode(Common_Xml, "saveSuccess"), true);
			}break;
			
			/***************************************************************
			 		System Test -> Generate / Display / Send Report
			 ***************************************************************/
			
			case "sendReportToFTP":
			{
				var query = "";
				query = "SysTestSendReportToFtp?";

				ajaxrequest("post", "submit", query, updateDataToObjList);	
			}break;
			
			case "generateReport":
			{
				var query = "";
				query = "SysTestGenerateReport?";
				
				ajaxrequest("post", "submit", query, updateDataToObjList);
			}break;
			
			case "displayReport":
			{
				var newWindow = window.open('/MxTestReport.pdf');
				newWindow.focus();
			}break;
		}
	}
}

function SystemTestEventBind()
{
	/*********************************************/
		NOTE:	/*   Always Register this event first For trime 
			         All Input field.*/		
			    
		$("input:text").change(treamAllInputTextField);
	/*******************************************/
	var container = $("#SystemTest");
	var sel_elements = $("select", $(container));
	var input_elements = $("input", $(container));
	var td_elements = $("td", $(container));
	var li_elements = $("li", $(container));
	
	for(var index = 0; index < sel_elements.length; index++)
	{
		registerHandlers($(sel_elements[index]), handleSystemTestEvent);
	}
		
	for(var index = 0; index < input_elements.length; index++)
	{
		registerHandlers($(input_elements[index]),handleSystemTestEvent);
	}
	
	for(var index = 0; index < td_elements.length; index++)
	{
		registerHandlers($(td_elements[index]),handleSystemTestEvent);
	}
	for(var index = 0; index < li_elements.length; index++)
	{
		registerHandlers($(li_elements[index]), handleSystemTestEvent);
	}

	$("#sysRebootCount").keypress(filterNumberKey);
	$("#employeeName").keypress(filterSmartCardFields);
	$("#employeeID").keypress(filterSmartCardFields);
	$("#deviceSerialNo").keypress(filterSmartCardFields);
	$("#mainPCBHWDesine").keypress(filterSmartCardFields);
	$("#connectorHWDesign").keypress(filterSmartCardFields);

	$("#ftpPassword").focus(clearField);
}

function initSystemTest()
{
	Common_Xml = getLanguageXmlDoc("configuration", "MxCommon");
	translatePage(Common_Xml,document,"name");

	sysTest_xml = getLanguageXmlDoc("configuration", "SystemTest");
	translatePage(sysTest_xml , document, "name");
	translateElements(sysTest_xml , document, "name");

	WifiTest_xml = getLanguageXmlDoc("configuration", "WifiSetting");
	translatePage(WifiTest_xml, document, "name");
	translateElements(WifiTest_xml, document, "img", "title", "name");
	
	hideElem("#wifiTable");
	hideElem("#btDeviceTable");
	SystemTestEventBind();
	getSysTestBasicInfo();
	DisplayBasicInformationTab();
	$("#employeeName").focus();
	return false;
}

window.onresize = function onresize(){
	mainPageDivSet();
};

$(document).ready(function()
{		
	mainPageDivSet();
	initSystemTest();
});
