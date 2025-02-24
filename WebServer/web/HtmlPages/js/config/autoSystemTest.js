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
//File         : autoSystemTest.js
//Description  : This file contains javascript functions for system_test.html 
// 				 file.
//
///////////////////////////////////////////////////////////////////////////// 

var Common_Xml 		= null;
var sysTest_xml 	= null;
var WifiTest_xml	= null;
var deviceCode;
var maxTimerSec			= 10;
var popUpVisibleF	= false;
var timer_id;
var VegaHwVersion;

var ftpSettings		=
{
	"ftpIPAddress"	:"",
	"ftpUserName"	:"",
	"ftpPassword"	:"",
};

var userData		=
{
    "userPswd"	:"",
};

var doorTestInfo	=
{
	"waitStatus"			:"",
	"doorType"				:"",
	"deviceSerialNumber"	:"",
	"employeeID"			:"",
	"startTest"		:"",
	"doorRelay"		:"",
	"doorSense"		:"",
	"exitSwitch"	:"",
	"USB"		:"",
	"memoryCard"	:"",
	"RTC"			:"",
	"wifi"			:"",
	"bluetooth"		:"",
	"sysTamp"		:"",
	"userEnroll"	:"",
	"buzzerLED"		:"",
	"intCardLF"		: "",
	"intCardHF"		: "",
	"extUReader"	: "",
	"extWReader"	: "",
	"functionalTestTime":"",
	"autoTestTime":"",
};

function getSystemTest()
{
	var query = "GetAutoTest?";
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, doorTestInfo);
	setSystemTestInfo();

	return true;
}

function setPwdcoockieForSystemtest()
{
    var query = "GetUserPswd?";
    var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, userData);
    if(responseCode == 0)
    {
        setCookie("User_Level", getCookie('MxUserType'));
        setCookie("User_Pswd",  userData.userPswd);
    }
    return true;
}

function startSystemTest()
{
	var query = "SetAutoTest?";
	query += "doorType"+ valFsp + doorTestInfo.doorType;
	query += apendWebFsp()+"deviceSerialNumber"+ valFsp + doorTestInfo.deviceSerialNumber;
	query += apendWebFsp()+"employeeID"+ valFsp + doorTestInfo.employeeID;
	query += apendWebFsp()+"startTest"+ valFsp ;

	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, doorTestInfo);
	if(responseCode == 0)
	{
		disableElements(["#startTest", "#viewReport", "#sysDefault"]);
        if(true == IsGuiAccessThroughDefaultIP())
        {
            setTimeout(setPwdcoockieForSystemtest, 10000);
        }
		setSystemTestInfo();
	}
	return true;
}

function getThreadSystemTest()
{
	var query = "GetThreadAutoTest?";
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, doorTestInfo);
	setSystemTestInfo();

	return true;
}

function setSystemTestInfo()
{
	setElemValue("#doorType",			doorTestInfo.doorType);
	setElemValue("#deviceSerialNumber",	doorTestInfo.deviceSerialNumber);

    if( (true == IsGuiAccessThroughDefaultIP()) && (null != getCookie("Emp_Id")) )
    {
        doorTestInfo.employeeID = getCookie("Emp_Id");
    }

	setElemValue("#employeeID",			doorTestInfo.employeeID);
	applyClassTestStatus("sysTamp", 	doorTestInfo.sysTamp);
	applyClassTestStatus("userEnroll", 	doorTestInfo.userEnroll);
	applyClassTestStatus("buzzerLED", 	doorTestInfo.buzzerLED);
	applyClassTestStatus("intCardLF", 	doorTestInfo.intCardLF);
	applyClassTestStatus("intCardHF", 	doorTestInfo.intCardHF);
	applyClassTestStatus("extUReader", 	doorTestInfo.extUReader);
	applyClassTestStatus("extWReader", 	doorTestInfo.extWReader);

	setCardReaderTestResult();
	// applyClassTestStatus("auxRelay",	doorTestInfo.auxRelay);
	// applyClassTestStatus("auxInput", 	doorTestInfo.auxInput);
	// applyClassTestStatus("lockTamper", 	doorTestInfo.lockTamper);
	applyClassTestStatus("doorRelay", 	doorTestInfo.doorRelay);
	applyClassTestStatus("doorSense", 	doorTestInfo.doorSense);
	applyClassTestStatus("exitSwitch", 	doorTestInfo.exitSwitch);
	// applyClassTestStatus("exitRdrTamp", doorTestInfo.exitRdrTamp);
	applyClassTestStatus("USB", 	doorTestInfo.USB);
	applyClassTestStatus("memoryCard", 	doorTestInfo.memoryCard);
	applyClassTestStatus("RTC", 		doorTestInfo.RTC);
	applyClassTestStatus("wifi", 		doorTestInfo.wifi);
	applyClassTestStatus("bluetooth", 	doorTestInfo.bluetooth);

	if(doorTestInfo.waitStatus == "1")
	{
		//Auto Test Running
		setTimeout(getThreadSystemTest, 2);
	}
	else if(doorTestInfo.waitStatus == "0")
	{
		//Auto Test Completed
		enableElements(["#generateReport", "#failBuzLEDTest"]);
		setTimerToTakeAction();
		return;
	}
	else if(doorTestInfo.waitStatus == "2")
	{
		//Auto Test Cancelled or ESC button pressed from DMD display
		enableElements(["#startTest", "#viewReport", "#sysDefault"]);
		disableElements(["#generateReport", "#failBuzLEDTest"]);
	}
}

function setTimerToTakeAction()
{
	setLabel("#validationMsgLabel","Take action within "+maxTimerSec+"s");
	if(maxTimerSec > 0)
	{
		maxTimerSec = maxTimerSec - 1;
		timer_id = setTimeout(setTimerToTakeAction, 1000);
	}
	else if(maxTimerSec == 0)
	{
		//10 sec Timer timeout take action automatic(generate report, display report, system deafult)
		//setLabel("#validationMsgLabel","Functional Test Time: "+doorTestInfo.functionalTestTime+"(Sec)  Automated Test Time: "+doorTestInfo.autoTestTime+"(Sec)");
		takeActionAutomatically();
	}
}

function takeActionAutomatically()
{
	setBlockState(translateNode(sysTest_xml,"sysDefaultPlzWaitScreen"));
	setTimeout(sendReportToFTPAndSysDefaultAction, 3000);
	//generate report
	var query = "SetAutoTest?";
	query += "generateReport";
	ajaxrequest("post", "submit", query, updateDataToObjList);

	//display report
	var newWindow = window.open('/MxTestReport.pdf');
	newWindow.focus();
}

function sendReportToFTPAndSysDefaultAction()
{
	//Send Report to FTP
	var query = "AutoSysTestSendReportToFtp?";
	ajaxrequest("post", "submit", query, updateDataToObjList);

	//System default
	var query = "SetSystemDefault?";
	ajaxrequest("post", "submit", query, updateDataToObjList);
}

function setCardReaderTestResult()
{
	//Internal Card Reader
	if(doorTestInfo.doorType == "0" || doorTestInfo.doorType == "3")//FOE,CAE
	{
		if(doorTestInfo.intCardLF == "1")
		{
			$("#internalCardTestRes").attr("src","Images/DefaultState_Bullet.png");	
		}
		else if(doorTestInfo.intCardLF == "2")
		{
			$("#internalCardTestRes").attr("src","Images/Fail_Bullet.png");
		}
		else if(doorTestInfo.intCardLF == "3")
		{
			$("#internalCardTestRes").attr("src","Images/Pass_Bullet.png");
		}	
	}
	else if(doorTestInfo.doorType == "1" || doorTestInfo.doorType == "4")//FOM,CAM
	{
		if(doorTestInfo.intCardHF == "1")
		{
			$("#internalCardTestRes").attr("src","Images/DefaultState_Bullet.png");	
		}
		else if(doorTestInfo.intCardHF == "2")
		{
			$("#internalCardTestRes").attr("src","Images/Fail_Bullet.png");
		}
		else if(doorTestInfo.intCardHF == "3")
		{
			$("#internalCardTestRes").attr("src","Images/Pass_Bullet.png");
		}	
	}
	else if(doorTestInfo.doorType == "2" || doorTestInfo.doorType == "5")//FOI,CAI
	{
		if( (doorTestInfo.intCardLF == "1") || (doorTestInfo.intCardHF == "1") )
		{
			$("#internalCardTestRes").attr("src","Images/DefaultState_Bullet.png");
		}
		else if( (doorTestInfo.intCardLF == "2") || (doorTestInfo.intCardHF == "2") )
		{
			$("#internalCardTestRes").attr("src","Images/Fail_Bullet.png");
		}
		else if( (doorTestInfo.intCardLF == "3") && (doorTestInfo.intCardHF == "3") )
		{
			$("#internalCardTestRes").attr("src","Images/Pass_Bullet.png");
		}
	}
	
	//External Card Reader
	if( (doorTestInfo.extUReader != "1" || doorTestInfo.extWReader != "1") && ((doorTestInfo.extUReader == "2" || doorTestInfo.extWReader == "2")))
	{
		$("#externalCardTestRes").attr("src","Images/Fail_Bullet.png");
	}
	else if( (doorTestInfo.extUReader != "1" || doorTestInfo.extWReader != "1") && ((doorTestInfo.extUReader == "3" && doorTestInfo.extWReader == "3")))
	{
		$("#externalCardTestRes").attr("src","Images/Pass_Bullet.png");
	}
	else if(doorTestInfo.extUReader == "1" || doorTestInfo.extWReader == "1")
	{
		$("#externalCardTestRes").attr("src","Images/DefaultState_Bullet.png");
	}
}

function setBuzzerLEDStatus()
{
	var query = "SetAutoTest?";
	query += "buzzerLED"+ valFsp + doorTestInfo.buzzerLED;

	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, doorTestInfo);
	if(responseCode == 0)
	{
		// $("#buzzerLEDTestRes").attr("src","Images/Fail_Bullet.png");
		applyClassTestStatus("buzzerLED", 	doorTestInfo.buzzerLED);
	}
}

function getFtpSettings()
{
	var query = "GetAutoTest?";
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, ftpSettings);
	if(responseCode == "0")
	{
		setFtpSettings();
	}
	return true;
}

function setFtpSettings()
{
		setElemValue("#ftpIPAddress", 	ftpSettings.ftpIPAddress);
		setElemValue("#ftpUserName", 	ftpSettings.ftpUserName);
		setElemValue("#ftpPassword", 	ftpSettings.ftpPassword);
}

function saveFtpSettings()
{
	var query = "SetAutoTest?";
	query += "ftpIPAddress"+ valFsp + ftpSettings.ftpIPAddress;
	query += apendWebFsp()+"ftpUserName"+ valFsp + ftpSettings.ftpUserName;
	query += apendWebFsp()+"ftpPassword"+ valFsp + ftpSettings.ftpPassword;

	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, ftpSettings);
	if(responseCode == "0")
	{
		setFtpSettings();
	}
	return responseCode;
}

function updateValidationMessage(id)
{
	switch(id)
	{
		default: break;
		case "sysTamp" 		:
		{ 
			setLabel("#validationMsgLabel","Perform System Tamper Test");
		}break;

		// case "exitRdrTamp" 	:
		// {
			// setLabel("#validationMsgLabel","Perform Reader Tamper Test");
		// }break;

		case "userEnroll" 	:
		{
			setLabel("#validationMsgLabel","Place Your Finger on Finger Sensor");
		} break;

		case "buzzerLED" 	:
		{
			setLabel("#validationMsgLabel","Check Buzzer");
		}break;

		case "intCardLF" 	:
		{
			setLabel("#validationMsgLabel","Show EM Prox or HID Prox Card on Internal Reader");
		}break;

		case "intCardHF" 	:
		{
			setLabel("#validationMsgLabel","Show Mifare or HID iClass Card on Internal Reader");
		}break;

		case "extUReader" 	:
		{
			setLabel("#validationMsgLabel","Show any Card on External Reader");
		}break;

		case "extWReader" 	:
		{
			setLabel("#validationMsgLabel","Show any Card on External Reader");
		}break;
	}
	return true;
}

function applyClassTestStatus(id, val)
{
	
	switch(val)
	{
		default:break;

		case 0:
		{
			$("#"+id).addClass("testLabel");
			$("#"+id+"TestRes").attr("src","Images/DefaultState_Bullet.png");
		}break;

		case 1:
		{
//			updateValidationMessage(id);
			$("#"+id).removeClass("testLabel");
			$("#"+id+"TestRes").attr("src","Images/TestStart_Bullet.png");
		}break;

		case 2:
		{
			$("#"+id).addClass("testLabel");
			if( (id == "intCardLF") || (id == "intCardHF") || (id == "extUReader") || (id == "extWReader") )
			{
				$("#"+id+"TestRes").attr("src","Images/SubTest_Cross.png");
			}
			else
			{
				$("#"+id+"TestRes").attr("src","Images/Fail_Bullet.png");
			}
		}break;

		case 3:
		{
			$("#"+id).addClass("testLabel");
			if( (id == "intCardLF") || (id == "intCardHF") || (id == "extUReader") || (id == "extWReader") )
			{
				$("#"+id+"TestRes").attr("src","Images/SubTest_Tick.png");
			}
			else
			{
				$("#"+id+"TestRes").attr("src","Images/Pass_Bullet.png");
			}
		}break;
	}
}

function disableAllTestLabels()
{
	// $("#auxRelay").addClass("testLabel");
	// $("#auxInput").addClass("testLabel");
	// $("#lockTamper").addClass("testLabel");
	$("#doorRelay").addClass("testLabel");
	$("#doorSense").addClass("testLabel");
	$("#exitSwitch").addClass("testLabel");
	// $("#exitRdrTamp").addClass("testLabel");
	$("#USB").addClass("testLabel");
	$("#memoryCard").addClass("testLabel");
	$("#RTC").addClass("testLabel");
	$("#wifi").addClass("testLabel");
	$("#bluetooth").addClass("testLabel");

	$("#sysTamp").addClass("testLabel");
	$("#userEnroll").addClass("testLabel");
	$("#buzzerLED").addClass("testLabel");
	$("#internalCard").addClass("testLabel");
	$("#intCardLF").addClass("testLabel");
	$("#intCardHF").addClass("testLabel");
	$("#externalCard").addClass("testLabel");
	$("#extUReader").addClass("testLabel");
	$("#extWReader").addClass("testLabel");
}

function disableButtonEle()
{
	disableElements(["#generateReport", "#failBuzLEDTest"]);
}

function hideFtpPopUp()
{
	popUpVisibleF = false;
	$("#ftpPopUp").css({"visibility" : "hidden"});
}

function showFtpPopUp()
{
	popUpVisibleF = true;
	$("#ftpPopUp").css({"visibility" : "visible"});
}

function hideShowTestsBasedOnDeviceType()
{
	if(doorTestInfo.doorType == "0")  // FOE-212
	{
		//Automated Test
		showElements(["#intCardLFRow"]);
		showElements(["#userEnrollRow"]);
		//Functional Test
		hideElements(["#intCardHFRow"]);
        enableElements(["#startTest"]);
	}
	else if(doorTestInfo.doorType == "1") // FOM-212
	{
		//Automated Test
		showElements(["#intCardHFRow"]);
		showElements(["#userEnrollRow"]);
		//Functional Test
		hideElements(["#intCardLFRow"]);
        enableElements(["#startTest"]);
	}
	else if(doorTestInfo.doorType == "2") // FOI-212
	{
		//Automated Test
		showElements(["#intCardLFRow","#intCardHFRow"]);
		showElements(["#userEnrollRow"]);
        enableElements(["#startTest"]);
	}
	else if(doorTestInfo.doorType == "3") // CAE
	{
		//Functional Test
		showElements(["#intCardLFRow"]);
		hideElements(["#intCardHFRow", "#userEnrollRow"]);
        enableElements(["#startTest"]);
	}
	else if(doorTestInfo.doorType == "4") // CAM
	{
		//Functional Test
		showElements(["#intCardHFRow"]);
		hideElements(["#intCardLFRow", "#userEnrollRow"]);
        enableElements(["#startTest"]);
	}
	else if(doorTestInfo.doorType == "5") // CAI
	{
		//Functional Test
		showElements(["#intCardLFRow","#intCardHFRow"]);
		hideElements(["#userEnrollRow"]);
        enableElements(["#startTest"]);
	}
    else
    {
        disableElements(["#startTest"]);
    }

}

function handleSystemTestEvent(e)
{
	var id = $(this).attr("id");
	var element = GetElementFormID(id);
	var index = GetIndexFromID(id);

	if(e.type === "change")
	{
		switch(element)
		{
			default:
					break;

			case "doorType":
			{
				doorTestInfo.doorType = getElemValue(this);
				if(doorTestInfo.doorType == "1")
				{
					$("#functionalTest").css("border","none");
					$("#automatedTest").css("border-left","1px solid #CCCCCC");
				}
				else
				{
					$("#automatedTest").css("border","none");
					$("#functionalTest").css("border-right","1px solid #CCCCCC");
				}
				hideShowTestsBasedOnDeviceType();
			}break;

			case "deviceSerialNumber":
			{
				doorTestInfo.deviceSerialNumber = getElemValue(this);
			}break;

			case "employeeID":
			{
				doorTestInfo.employeeID = getElemValue(this);
			}break;

			case "ftpIPAddress":
			{
				ftpSettings.ftpIPAddress = getElemValue(this);
			}break;

			case "ftpUserName":
			{
				ftpSettings.ftpUserName = getElemValue(this);
			}break;

			case "ftpPassword":
			{
				ftpSettings.ftpPassword = getElemValue(this);
			}break;
		}
	}
	if(e.type === "click")
	{
		switch(element)
		{
			default:
			{
				if(	popUpVisibleF == false)
				{
					break;
				}

				if(! ((e.pageX > 851) && (e.pageX < 1152) && (e.pageY > 45) && (e.pageY < 223)) )
				{
					hideFtpPopUp();
				}
			}break;

			case "ftpSettingButton":
			{
				if(popUpVisibleF == false)
				{
					showFtpPopUp();
					getFtpSettings();
					$("#ftpIPAddress").focus();
				}
				else
				{
					hideFtpPopUp();
				}
			}break;

			case "save":
			{
				saveFtpSettings();
				hideFtpPopUp();
			}break;

			case "cancel":
			{
				getFtpSettings();
				hideFtpPopUp();
			}break;

			case "failBuzLEDTest":
			{
				doorTestInfo.buzzerLED = 0;
				setBuzzerLEDStatus();
			}break;
			/************************************************************************
			 	  System Test -> Start Test / Generate / Display / Send Report
			 ***********************************************************************/
			case "startTest":
			{
                if(true == IsGuiAccessThroughDefaultIP())
                {
                    setCookie("Emp_Id",doorTestInfo.employeeID);
                }
				startSystemTest();
			}break;

			case "generateReport":
			{
				if(timer_id != null)
				{
					clearTimeout(timer_id);
				}
				takeActionAutomatically();
			}break;

			case "viewReport":
			{
				var newWindow = window.open('/MxTestReport.pdf');
				newWindow.focus();
			}break;

			case "sysDefault":
			{
				var query = "SetSystemDefault?";
				ajaxrequest("post", "submit", query, updateDataToObjList);
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
	var container = $("#AutoSystemTest");
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

	$("#employeeID").keypress(filterSmartCardFields);
	$("#deviceSerialNumber").keypress(filterSmartCardFields);
	$("#ftpIPAddress").keypress(AutoFormatIpAddr);
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

	deviceCode = Base64.decode(getCookie('MxCosecDeviceCode'));
	VegaHwVersion = Base64.decode(getCookie('MxVegaHwVersion'));
	SystemTestEventBind();

	disableButtonEle();
	disableAllTestLabels();
	getSystemTest();
	$("#doorType").focus();
	hideShowTestsBasedOnDeviceType();

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
