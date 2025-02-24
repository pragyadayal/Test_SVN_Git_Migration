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
//File         : WifiSettings.js
//Description  : This file contains javascript functions for main navigation panel.
//
/////////////////////////////////////////////////////////////////////////////
var WifiSetting_xml	= null;
var BasicProfile_xml = null;
var Common_XML		= null;
var success_img		= "";
var WpsEnblF		= "0";
const MASKED_WIFI_SEC_KEY = "................................................................";

var WpsPin = 
{
	"wpsPin":"",
};

var WifiNwSetting =
{	
	"ipAssignment":"",
	"ipAddress":"",
	"subNetMask":"",
	"defaultGateWay":"",
	"preferredDNS":"",
	"alternetDNS":"",
	"MACAddress":"", 
};

var ref_wifiNwSetting;

var WiFiAccsPntstg =
{	
	"connectionStatus":"",
	"SsidNwName":"",
	"SecType":"",
	"encryptionType":"",
	"advncAuth":"",
	"anonymsIdentt":"",
	"username":"",
	"secKey":"", 
	"peapVer":"", 
	"innerAuth":"", 
	"cnctvtOptn":"",
	"pin":"",
	"wifi":"",
    	"bssid":"",
   	"signalStrength":"",
	"bandFreqType":"",
};

function DisplayWiFiNwstgTab()
{
	$(".tab li").removeClass("active");	
	$("#WiFiNwstg").addClass("active");
	hideElements(["#WiFiAccsPntstg-div", "#wifiAccPoints", "#savedWifiList"]);
	showElem("#WiFiNwstg-div");
	$(".td-param-label").css("width", 115+"px");
	getWifiNwSetting();
	$("#ipAssignment").focus();
	clearValidationMessage();
	if(timer_id != null)
	{
		clearTimeout(timer_id);
	}
}

function DisplayWiFiAccsPntstgTab()
{
	$(".tab li").removeClass("active");	
	$("#WiFiAccsPntstg").addClass("active");
	hideElem("#WiFiNwstg-div");
	showElem("#WiFiAccsPntstg-div");
	$(".td-param-label").css("width", 170+"px");
    disableAndUnbindImgBtn("#RefreshImage");
	getWifiAccPtSetting();
	$("#wifi").focus();
	clearValidationMessage();
	if(timer_id != null)
	{
		clearTimeout(timer_id);
	}
}

function HideShowElementsBasedOnSecType () 
{
	showElements(["#connectionStatusTR","#SsidNwNameTR","#encryptionTypeTR", "#advncAuthTR","#anonymsIdenttTR","#usernameTR","#secKeyTR","#peapVerTR","#innerAuthTR","#urlTR"]);
	if(WiFiAccsPntstg.SecType == "0")
	{
		 hideElements(["#encryptionTypeTR", "#advncAuthTR","#anonymsIdenttTR","#usernameTR","#secKeyTR","#peapVerTR","#innerAuthTR"]);
	}
	else if(WiFiAccsPntstg.SecType == "1")
	{
		hideElements(["#encryptionTypeTR", "#advncAuthTR","#anonymsIdenttTR","#usernameTR","#peapVerTR","#innerAuthTR"]);
	}
	else if(WiFiAccsPntstg.SecType == "2")
	{
		hideElements(["#advncAuthTR","#anonymsIdenttTR","#usernameTR","#peapVerTR","#innerAuthTR"]);
	}
	else if(WiFiAccsPntstg.SecType == "3")
	{
		hideElements(["#advncAuthTR","#anonymsIdenttTR","#usernameTR","#peapVerTR","#innerAuthTR"]);
	}
	else
	{
		showElements(["#connectionStatusTR","#SsidNwNameTR","#encryptionTypeTR", "#advncAuthTR","#anonymsIdenttTR","#usernameTR","#secKeyTR","#peapVerTR","#innerAuthTR","#urlTR"]);
	}
}

function HideShowElementsBasedOnAccPntStg () 
{
	showElements(["#connectionStatusTR","#SsidNwNameTR","#encryptionTypeTR", "#advncAuthTR","#anonymsIdenttTR","#usernameTR","#secKeyTR","#peapVerTR","#innerAuthTR","#urlTR"]);
	if(WiFiAccsPntstg.SecType == "0")
	{
		 hideElements(["#encryptionTypeTR", "#advncAuthTR","#anonymsIdenttTR","#usernameTR","#secKeyTR","#peapVerTR","#innerAuthTR"]);
	}
	else if(WiFiAccsPntstg.SecType == "1")
	{
		hideElements(["#encryptionTypeTR", "#advncAuthTR","#anonymsIdenttTR","#usernameTR","#peapVerTR","#innerAuthTR"]);
	}
	else if(WiFiAccsPntstg.SecType == "2")
	{
		hideElements(["#advncAuthTR","#anonymsIdenttTR","#usernameTR","#peapVerTR","#innerAuthTR"]);
	}
	else if(WiFiAccsPntstg.SecType == "3")
	{
		hideElements(["#advncAuthTR","#anonymsIdenttTR","#usernameTR","#peapVerTR","#innerAuthTR"]);
	}
	else
	{
		showElements(["#connectionStatusTR","#SsidNwNameTR","#encryptionTypeTR", "#advncAuthTR","#anonymsIdenttTR","#usernameTR","#secKeyTR","#peapVerTR","#innerAuthTR","#urlTR"]);
	}

	if(WpsEnblF == "0")
	{
		hideElements(["#cnctvtOptn-TR","#pin-TR"]);
	}
	else
	{
		showElem("#cnctvtOptn-TR");
		
		if(WiFiAccsPntstg.cnctvtOptn == "1")
		{
			showElem("#pin-TR");
		}
		else
		{
			hideElem("#pin-TR");			
		}
	}
}
function getWifiNwSetting()
{	
	var query = "GetWifiSetting?";	
	ajaxrequest("post", "submit" , query, updateDataToObjList, WifiNwSetting);
	setWifiNwSetting();
	return true;
}

function getWifiAccPtSetting()
{
	var query = "GetWifiAccPtStg?";	
	ajaxrequest("post", "submit" , query, updateDataToObjList, WiFiAccsPntstg);
	setWifiAccsPntstg();
	return true;
}

function setDefaultWifiNwSetting()
{
	var query = "SetDefaultWifiStg?";
	ajaxrequest("post", "submit" , query, updateDataToObjList, WifiNwSetting);
	setWifiNwSetting();
	
	return true;
}

function setDefaultWifiAccsPntstg()
{
	var query = "SetDefaultWifiAccPtStg?";
	ajaxrequest("post", "submit" , query, updateDataToObjList, WiFiAccsPntstg);
	WiFiAccsPntstg.SecType = "0";
	WiFiAccsPntstg.cnctvtOptn = "0";
	hideElements(["#pin-TR","#cnctvtOptn-TR"]);		
	showElem("#SecType-TR");

	WpsEnblF = "0";
	setWifiAccsPntstg();
	return true;
}

function setWifiNwSetting()
{
	ref_wifiNwSetting = createObjCopy(WifiNwSetting);
	setElemValue("#ipAssignment", 	WifiNwSetting.ipAssignment);
	setElemValue("#ipAddress", 		WifiNwSetting.ipAddress);
	setElemValue("#subNetMask", 	WifiNwSetting.subNetMask);
	setElemValue("#defaultGateWay", WifiNwSetting.defaultGateWay);
	setElemValue("#preferredDNS", 	WifiNwSetting.preferredDNS);
	setElemValue("#alternetDNS", 	WifiNwSetting.alternetDNS);
	setElemValue("#MACAddress", 	WifiNwSetting.MACAddress);
	if(WifiNwSetting.ipAssignment == "0")
	{
		enableElements(["#ipAddress", "#subNetMask","#defaultGateWay","#preferredDNS","#alternetDNS",]);
	}
	else
	{
		disableElements(["#ipAddress", "#subNetMask","#defaultGateWay","#preferredDNS","#alternetDNS",]);
	}
	
	var cosecDeviceCode = Base64.decode(getCookie("MxDeviceType"));
	if(cosecDeviceCode == "1")
	{
		disableElements(["#ipAssignment"]);
		$("#ipAddress").focus();
	}
	else if(cosecDeviceCode == "0")
	{
		enableElements(["#ipAssignment"]);
		$("#ipAssignment").focus();
	}

	return false;
}

function setWifiSignalStrength()
{
    if( (WiFiAccsPntstg.wifi === "1") && (WiFiAccsPntstg.connectionStatus !== "0") )
    {
        showElem("#signalStrength");
    }
    else
    {
        hideElements(["#signalStrength"]);
        return false;
    }

    var strength=WiFiAccsPntstg.signalStrength.toString();

    switch (strength)
    {
        default:    break;
    case "0":
        $("#signalStrength").attr({"src":"Images/WiFiSigLevel0.png"});
        break;
    case "1":
        $("#signalStrength").attr({"src":"Images/Weak.png"});
        break;
    case "2":
        $("#signalStrength").attr({"src":"Images/Moderate.png"});
        break;
    case "3":
        $("#signalStrength").attr({"src":"Images/WiFiSigLevel3.png"});
        break;
    case "4":
        $("#signalStrength").attr({"src":"Images/Strong.png"});
        break;
    }
    return true;
}

function setWifiAccsPntstg()
{
	setElemValue("#connectionStatus", 	translateNode(WifiSetting_xml,"WifiCncsnSts"+parseDecimal(WiFiAccsPntstg.connectionStatus) ) );
	setElemValue("#SsidNwName", 		WiFiAccsPntstg.SsidNwName);
	setElemValue("#SecType", 			WiFiAccsPntstg.SecType);
	setElemValue("#encryptionType", 	WiFiAccsPntstg.encryptionType);
	setElemValue("#advncAuth", 			WiFiAccsPntstg.advncAuth);
	setElemValue("#anonymsIdentt", 		WiFiAccsPntstg.anonymsIdentt);
	setElemValue("#username", 			WiFiAccsPntstg.username);
	if(WiFiAccsPntstg.secKey != "")
	{
		WiFiAccsPntstg.secKey = MASKED_WIFI_SEC_KEY;
	}
	setElemValue("#secKey", 			WiFiAccsPntstg.secKey);
	setElemValue("#peapVer", 			WiFiAccsPntstg.peapVer);
	setElemValue("#innerAuth", 			WiFiAccsPntstg.innerAuth);
	if(WiFiAccsPntstg.cnctvtOptn == "")
	{
    		WiFiAccsPntstg.cnctvtOptn = "0";
   	}
	setElemValue("#cnctvtOptn", 		WiFiAccsPntstg.cnctvtOptn);
	setElemValue("#pin", 				WiFiAccsPntstg.pin);
	WiFiAccsPntstg["wifi"] == "1"?checkElem("#wifi"):uncheckElem("#wifi");

	HideShowElementsBasedOnAccPntStg();
    disableAndUnbindImgBtn("#RefreshImage");
	EnableDisabledParatmeter();
	return false;
}

function saveWifiNwPara()
{
	var query = "SetWifiSetting?";
	query += "ipAssignment"	+valFsp+ WifiNwSetting.ipAssignment;
	if(WifiNwSetting.ipAssignment == "0")
	{
		query += apendWebFsp()+"ipAddress"		+valFsp+ WifiNwSetting.ipAddress;
		query += apendWebFsp()+"subNetMask"		+valFsp+ WifiNwSetting.subNetMask;
		query += apendWebFsp()+"defaultGateWay"	+valFsp+ WifiNwSetting.defaultGateWay;
		query += apendWebFsp()+"preferredDNS"	+valFsp+ WifiNwSetting.preferredDNS;
		query += apendWebFsp()+"alternetDNS"	+valFsp+ WifiNwSetting.alternetDNS;
	}

	if(WifiNwSetting.ipAssignment == "1") // Dynamic IP Address
	{
		var response = ajaxrequest("post", "submit" , query, updateDataToObjList, WifiNwSetting);
	}
	else // Static IP Address
	{
		var response = saveNetparaAndRedirectToNewIP(ref_wifiNwSetting["ipAddress"], WifiNwSetting["ipAddress"], query,WifiNwSetting);	
	}
		
	if (response == "0") 
	{
		displayValidationMessage(error_img, translateNode(Common_XML,"saveSuccess"), true);
		setWifiNwSetting();		
	}

	ref_wifiNwSetting = createObjCopy(WifiNwSetting);
	return true;
}

function WpsTriggerPushBtn()
{
	var query = "wpsTrigrPushBtn?";

	var response = ajaxrequest("post", "submit" , query);
	if (response == "0") 
	{
		alert(translateNode(WifiSetting_xml,"WpsTriggerPushBtnMsg"));
		return true;
	}
}

function WpsTriggerPinBtn()
{
	var query = "GetwpsTrigrPin?";

	var response = ajaxrequest("post", "submit" , query, updateDataToObjList, WpsPin);
	clearBlockState();
	if (response == "0") 
	{
		if(WpsPin.wpsPin != "")
		{
			alert(translateNode(WifiSetting_xml,"WpsTriggerPinBtnMsg1")+WpsPin.wpsPin+translateNode(WifiSetting_xml,"WpsTriggerPinBtnMsg2"));
		}
		else
		{
			alert(translateNode(WifiSetting_xml,"WpsTriggerPinBtnErrorMsg"));
		}		
	}
	return true;
}

function saveWifiAccsPntstg()
{
	var query = "SetWifiAccsPntstg?";
	query += "SsidNwName"+valFsp+ WiFiAccsPntstg.SsidNwName;
	query += apendWebFsp()+"cnctvtOptn"		+valFsp+ WiFiAccsPntstg.cnctvtOptn;
	query += apendWebFsp()+"bssid"		+valFsp+ WiFiAccsPntstg.bssid;
	query += apendWebFsp()+"bandFreqType"		+valFsp+ WiFiAccsPntstg.bandFreqType;
	if(WiFiAccsPntstg.cnctvtOptn == "1")
	{
		query += apendWebFsp()+"pin"		+valFsp+ WiFiAccsPntstg.pin;
	}
	else
	{
		query += apendWebFsp()+"SecType"		+valFsp+ WiFiAccsPntstg.SecType;
		query += apendWebFsp()+"encryptionType"	+valFsp+ WiFiAccsPntstg.encryptionType;
		query += apendWebFsp()+"secKey"			+valFsp+ WiFiAccsPntstg.secKey;
		query += apendWebFsp()+"advncAuth"		+valFsp+ WiFiAccsPntstg.advncAuth;
		query += apendWebFsp()+"anonymsIdentt"	+valFsp+ WiFiAccsPntstg.anonymsIdentt;
		query += apendWebFsp()+"username"		+valFsp+ WiFiAccsPntstg.username;
		query += apendWebFsp()+"peapVer"		+valFsp+ WiFiAccsPntstg.peapVer;
		query += apendWebFsp()+"innerAuth"		+valFsp+ WiFiAccsPntstg.innerAuth;
	}

	var response = ajaxrequest("post", "submit" , query, updateDataToObjList, WiFiAccsPntstg);
	if (response == "0") 
	{
		displayValidationMessage(error_img, translateNode(Common_XML,"saveSuccess"), true);
		setWifiAccsPntstg();		
	}
	else
	{
		displayValidationMessage(error_img, "Saved Failed", false);
	}
	return true;
}

function validationTestConnection()
{
	var messageBlank = "";
	if(getElemValue("#url") === "")
	{
		messageBlank = translateNode(Common_XML,"MXCPleaseEnter")+" "+translateNode(Common_XML,"urlForVAlidation");
		displayValidationSecondMessage(error_img ,messageBlank , false);
		return false;
	}

	if(validateAsciiUrlString(document.getElementById("url")) == false)
	{
		messageBlank = translateNode(Common_XML,"validationAsciiUrl");
		displayValidationMessage(error_img ,messageBlank , false);
		return false;
	}
	return true;	
}

function validateWifiSetting()
{
	var messageBlank = "";
	var message		 = "";
	if(WifiNwSetting.ipAssignment == "0")
	{
		// Please enter IP Address
		messageBlank = translateNode(Common_XML,"MXCPleaseEnter")+" "+translateNode(Common_XML,"ipAddress");
			
		// Invalid IP Address
		message = translateNode(Common_XML,"inValid");
		message = message + " " + translateNode(Common_XML,"ipAddress");		
		if(ValidateCompulsoryAddr($("#ipAddress"), message, messageBlank) == false)
		{
			return false;
		}
	
		// Invalid Subnet mask
		message = translateNode(Common_XML, "inValid");
		message = message + " " + translateNode(Common_XML, "subNetMask");
		messageBlank = translateNode(Common_XML,"MXCPleaseEnter")+" "+translateNode(Common_XML,"subNetMask");		
		if(CheckNetMask($("#subNetMask"), message, messageBlank) == false)
		{
			return false;
		}
		
		// Invalid Gateway Address
		message = translateNode(Common_XML, "inValid");
		message = message + " " + translateNode(Common_XML, "defaultGateWay");		
		if(ValidateAddr($("#defaultGateWay"), message) == false)
		{
			return false;
		}
		
		// Invalid Preferred Address
		message = translateNode(Common_XML, "inValid");
		message = message + " " + translateNode(Common_XML, "preferredDNS");		
		if(ValidateAddr($("#preferredDNS"), message) == false)
		{
			return false;
		}
		
		// Invalid Preferred Address
		message = translateNode(Common_XML, "inValid");
		message = message + " " + translateNode(Common_XML, "alternetDNS");		
		if(ValidateAddr($("#alternetDNS"), message) == false)
		{
			return false;
		}
	}
	return true;
}

function validateWifiAccsPntstg()
{
	var messageBlank = "";
	var message		 = "";

	if(WpsEnblF == "1")
	{
		if(WiFiAccsPntstg.cnctvtOptn == "1")
		{
			if(isFieldBlank(WiFiAccsPntstg["pin"], translateNode(WifiSetting_xml,"pin"),WiFiAccsPntstg,"pin") == false)
			{
				return false;
			}
		}
	}

	if(WiFiAccsPntstg.cnctvtOptn == "0") //password
	{
		if( (WiFiAccsPntstg.SecType == "4") || (WiFiAccsPntstg.SecType == "5") )
		{
			if(isFieldBlank(WiFiAccsPntstg["username"], translateNode(Common_XML,"username"),WiFiAccsPntstg,"username") == false)
			{
				return false;
			}
		}
		if(WiFiAccsPntstg.SecType != "0") 
		{
			if(isFieldBlank(WiFiAccsPntstg["secKey"], translateNode(Common_XML,"secKey"),WiFiAccsPntstg,"secKey") == false)
			{
				return false;
			}
		}
	}	
	return true;
}

function GetWifiCnctnSts()
{
	var WifiCnctnSts =
	{	
		"connectionStatus":"",
        "signalStrength":"",
	};
	var query = "GetWifiCnctnSts?";
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, WifiCnctnSts);
	if (responseCode == "0") 
	{
		WiFiAccsPntstg.connectionStatus = WifiCnctnSts.connectionStatus;
        WiFiAccsPntstg.signalStrength = WifiCnctnSts.signalStrength;
		setWifiAccsPntstg();		
	}
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
	wpsEnblF 	: new Array(),
	bssid 		: new Array(),
	bandFreqType	: new Array(),
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
			displayValidationMessage(error_img, translateNode(WifiSetting_xml,"wifiModuleNotPrsnt"), false);
			hideElem("#wifiAccPoints");
		}
		else
		{
			setWifiAccsPntList();		
		}
	}
}

function setWifiAccsPntList()
{
	if(parseDecimal(WiFiAcsPntsList.accsPntCnt) == 0)
	{
		hideElem("#wifiAccPoints");
		displayValidationMessage(error_img, translateNode(WifiSetting_xml,"noNwFound"), false);
	}
	else
	{	
		showElem("#wifiAccPoints");
		for(var index=1; index<= parseDecimal(WiFiAcsPntsList.accsPntCnt);index++)
		{
			showElem("#availableNwRow"+index);
			if(WiFiAcsPntsList.bandFreqType[index-1] == "1")
			{
				setLabel("#nwName"+index, 	WiFiAcsPntsList.ssid[index-1] + "(5G)");
			}
			else
			{
			setLabel("#nwName"+index, 	WiFiAcsPntsList.ssid[index-1]);
			}
// 			alert("Band freq->"+ WiFiAcsPntsList.bandFreqType[index-1]);
			setLabel("#nwType"+index, 	secTypeStr[parseDecimal(WiFiAcsPntsList.nwSecureF[index-1])]);
			$("#wifiIcons"+index).attr("src", ConnectionStrength[parseDecimal(WiFiAcsPntsList.sigQuality[index-1])]);
			uncheckElem("#select" + index);
		}
		for(; index<= 30;index++)
		{
			hideElem("#availableNwRow"+index);
			uncheckElem("#select" + index);
		}
	}
}

function UpdateWifiEnableFlag()
{
	var query = "SetWifiAccsPntstg?";
	query += "wifi"+valFsp+ WiFiAccsPntstg.wifi;
	var response = ajaxrequest("post", "submit" , query, updateDataToObjList, WiFiAccsPntstg);
}

function EnableDisabledParatmeter()
{
	if(WiFiAccsPntstg.wifi == 0)
	{
		disableElements(["#select2", "#SecType","#srchNw","#submitAcsPtSetting","#cancelAcsPtSetting","#defaultAcsPtSetting"
		,"#wpsPushBtn","#wpsPinfrmDeviceBtn","#SsidNwName","#encryptionType","#secKey","#url","#testConnection","#cnctvtOptn",
		"#pin","#SecType","#advncAuth","#anonymsIdentt","#username","#peapVer","#innerAuth","#savedNw"]);

		setElemValue("#connectionStatus", "");

		disableAndUnbindImgBtn("#RefreshImage");
		
		if(WiFiAcsPntsList.accsPntCnt > 0)
		{
			hideElem("#wifiAccPoints");
        }
        hideElem("#savedWifiList");
        setWifiSignalStrength();
    }
	else
	{
		enableElements(["#select2", "#SecType","#srchNw","#submitAcsPtSetting","#cancelAcsPtSetting","#defaultAcsPtSetting"
		,"#wpsPushBtn","#wpsPinfrmDeviceBtn","#SsidNwName","#encryptionType","#secKey","#url","#testConnection","#cnctvtOptn",
		"#pin","#SecType","#advncAuth","#anonymsIdentt","#username","#peapVer","#innerAuth","#savedNw"]);
		
		enableAndBindImgBtn("#RefreshImage",handleWifiSettingEvent);
		
		if(WiFiAcsPntsList.accsPntCnt > 0)
		{
			showElem("#wifiAccPoints");
		}
        setWifiSignalStrength();
	}
}

/***************************************************************
 *		Saved Wi-Fi Networks
 ***************************************************************/
var savedWiFiList =
{
	"savedWifiListCnt"	:"",
	ssid	 			: new Array(),
	nwType 				: new Array(),
	bandFreqType		: new Array(),
	"deleteNw"			:"",
};

var DeleteNwNum = "";

function getSavedWiFiList()
{
	query = "GetSavedWiFiList?";
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, savedWiFiList);
	if (responseCode == "0")
	{
			setSavedWiFiList();
	}
}

function setSavedWiFiList()
{
	if((parseDecimal(savedWiFiList.savedWifiListCnt)) == 0)
	{
		hideElem("#savedWifiList");
	}
	else
	{
		showElem("#savedWifiList");
		for(var index=1; index<= parseDecimal(savedWiFiList.savedWifiListCnt);index++)
		{
			showElem("#savedWifiListTableRow"+index);
			if(savedWiFiList.bandFreqType[index-1] == "1")
			{
				setLabel("#savedNwName"+index, 	savedWiFiList.ssid[index-1] + "(5G)");
			}
			else
			{
			setLabel("#savedNwName"+index, 	savedWiFiList.ssid[index-1]);
			}
// 			alert("Saved Band freq->"+ savedWiFiList.ssid[index-1]);
			setLabel("#savedNwType"+index, 	secTypeStr[parseDecimal(savedWiFiList.nwType[index-1])]);
		}
		for(; index<= 30;index++)
		{
			hideElem("#savedWifiListTableRow"+index);
		}
	}
}

function deleteSavedWiFi()
{
	query = "DeleteSavedWiFi?";
	query += "deleteNw"+valFsp+ savedWiFiList.ssid[DeleteNwNum -1];

	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, savedWiFiList);
	if (responseCode == "0")
	{
		setSavedWiFiList();
		if((parseDecimal(savedWiFiList.savedWifiListCnt)) == 0)
		{
			displayValidationMessage(error_img, translateNode(WifiSetting_xml,"noSavedNw"), false);
		}
	}
	else
	{
		displayValidationMessage(error_img, translateNode(WifiSetting_xml,"savedNwNotDeleted"), false);
	}
}

function handleWifiSettingEvent(e)
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
			 * 		Wi-Fi Network Settings
			 ***************************************************************/
			case "ipAssignment":
			{
				WifiNwSetting.ipAssignment = getElemValue(this);
				if(WifiNwSetting.ipAssignment == "0")
				{
					enableElements(["#ipAddress", "#subNetMask","#defaultGateWay","#preferredDNS","#alternetDNS",]);
				}
				else
				{
					disableElements(["#ipAddress", "#subNetMask","#defaultGateWay","#preferredDNS","#alternetDNS",]);
				}
			}
			break;

			case "ipAddress":
			{
				WifiNwSetting.ipAddress = getElemValue(this);
				
			}break;
			
			case "subNetMask":
			{
				WifiNwSetting.subNetMask = getElemValue(this);
			}break;
			
			case "defaultGateWay":
			{
				WifiNwSetting.defaultGateWay = getElemValue(this);
			}break;
			
			case "preferredDNS":
			{
				WifiNwSetting.preferredDNS = getElemValue(this);
			}break;
			
			case "alternetDNS":
			{
				WifiNwSetting.alternetDNS = getElemValue(this);
			}break;

			/***************************************************************
			 * 		Wi-Fi Access Point Settings
			 ***************************************************************/
			case "SsidNwName":
			{
				WiFiAccsPntstg.SsidNwName = getElemValue(this);
			}break;
			case "SecType":
			{
				WiFiAccsPntstg.SecType = getElemValue(this);
				
				HideShowElementsBasedOnSecType();
			}break;
			case "encryptionType":
			{
				WiFiAccsPntstg.encryptionType = getElemValue(this);
			}break;
			case "advncAuth":
			{
				WiFiAccsPntstg.advncAuth = getElemValue(this);
			}break;
			case "anonymsIdentt":
			{
				WiFiAccsPntstg.anonymsIdentt = getElemValue(this);
			}break;
			case "username":
			{
				WiFiAccsPntstg.username = getElemValue(this);
			}break;
			case "secKey":
			{
				WiFiAccsPntstg.secKey = getElemValue(this);
			}break;
			case "peapVer":
			{
				WiFiAccsPntstg.peapVer = getElemValue(this);
			}break;
			case "innerAuth":
			{
				WiFiAccsPntstg.innerAuth = getElemValue(this);
			}break;
			case "url":
			{
				testConnection.url = getElemValue(this);
			}break;
			case "cnctvtOptn":
			{
				WiFiAccsPntstg.cnctvtOptn = getElemValue(this);
				if(WiFiAccsPntstg.cnctvtOptn == "0")
				{
					hideElem("#pin-TR");			
					showElem("#SecType-TR");
					HideShowElementsBasedOnSecType();		
				}
				else
				{
					showElem("#pin-TR");
					hideElements(["#SecType-TR","#encryptionTypeTR","#advncAuthTR", "#anonymsIdenttTR","#usernameTR","#secKeyTR","#peapVerTR","#innerAuthTR"]);
				}				
			}break;
			case "pin":
			{
				WiFiAccsPntstg.pin = getElemValue(this);
			}break;
		}
	}
	
	if(e.type === "click")
	{
		switch(element)
		{
			default: break;

			/***************************************************************
			 * 		Wi-Fi Network Settings
			 ***************************************************************/
			
			case "submitNwSetting":
			{
				if(validateWifiSetting() == true)
				{
					saveWifiNwPara();
				}
			
			}break;
			
			case "cancelNwSetting":
			{
				if(getWifiNwSetting() == true)
				{
					setWifiNwSetting();
				}
			}break;
			
			case "defaultNwSetting":
			{
				setDefaultWifiNwSetting();
			}break;

			case "WiFiNwstg":
			{
				DisplayWiFiNwstgTab();
			}break;

			/***************************************************************
			 * 		Wi-Fi Access Point Settings
			 ***************************************************************/

			case "WiFiAccsPntstg":
			{
				DisplayWiFiAccsPntstgTab();
			}break;
			case "submitAcsPtSetting":
			{
				if(validateWifiAccsPntstg() == true)
				{
					saveWifiAccsPntstg();
				}
			}break;
			case "cancelAcsPtSetting":
			{
				if(getWifiAccPtSetting() == true)
				{
					setWifiAccsPntstg();
				}
			}break;
			case "defaultAcsPtSetting":
			{
				setDefaultWifiAccsPntstg();
			}break;
			case "testConnection":
			{
				if(validationTestConnection() == true)
				{
					testConnection["interface"] = "1";
					// Here Very small time out require.
					// if time out removed than overlay is not create.
					setBlockState(translateNode(Common_XML,"connectingMsg"));
					setTimeout(startTestConnection, 500);
				}
			}break;
			case "RefreshImage":
			{
				GetWifiCnctnSts();
			}break;
			case "srchNw":
			{
				// Here Very small time out require.
				// if time out removed than overlay is not create.
				setBlockState(translateNode(Common_XML,"searchingMsg"));
				setTimeout(getWiFiAccessPoint, 500);
				
			}break;
			case "savedNw":
			{
				getSavedWiFiList();
				if((parseDecimal(savedWiFiList.savedWifiListCnt)) == 0)
				{
					displayValidationMessage(error_img, translateNode(WifiSetting_xml,"noSavedNw"), false);
				}
			}break;
			case "availableNwRow":
			{
				WiFiAccsPntstg.SsidNwName = WiFiAcsPntsList.ssid[index-1];
				WiFiAccsPntstg.SecType = WiFiAcsPntsList.secType[index-1];
				WiFiAccsPntstg.bssid = WiFiAcsPntsList.bssid[index-1];
				WiFiAccsPntstg.bandFreqType = WiFiAcsPntsList.bandFreqType[index-1];
				for(var wifiIdx = 1; wifiIdx <= parseDecimal(WiFiAcsPntsList.accsPntCnt); wifiIdx++)
				{
					if(wifiIdx == index)
					{
						checkElem("#select" + index);
						continue;
					}
						
					uncheckElem("#select" + wifiIdx);
				}
				WpsEnblF = WiFiAcsPntsList.wpsEnblF[index-1];
				WiFiAccsPntstg.cnctvtOptn = "0";
				//WiFiAccsPntstg.pin = "";
				setWifiAccsPntstg();
			}break;
			case "wpsPushBtn":
			{
				WpsTriggerPushBtn();				
			}
			break;
			case "wpsPinfrmDeviceBtn":
			{
				setBlockState(translateNode(Common_XML,""));
				setTimeout(WpsTriggerPinBtn, 500);
			}
			break;
			case "wifi":
			{
				WiFiAccsPntstg.wifi=isChecked(this)?"1":"0";
				UpdateWifiEnableFlag();
                disableAndUnbindImgBtn("#RefreshImage");
				EnableDisabledParatmeter();
			}break;
			case "deleteNw":
			{
				DeleteNwNum = index;
				deleteSavedWiFi();
			}break;
		}
	}
}

function WifiSettingEventBind()
{
	/*********************************************/
		NOTE:	/*   Always Register this event first For trime 
			         All Input field.*/		
			    
		$("input:text").change(treamAllInputTextField);
		StopPastFronInputTextField();
	/*******************************************/
	var container = $("#WifiSettings");
	var sel_elements = $("select", $(container));
	var input_elements = $("input", $(container));
	var img_elements = $("img", $(container));
	var tr_elements = $("tr", $(container));
	var td_elements = $("td", $(container));
	var li_elements = $("li", $(container));
	
	for(var index = 0; index < sel_elements.length; index++)
	{
		registerHandlers($(sel_elements[index]), handleWifiSettingEvent);
	}
	
	for(var index = 0; index < img_elements.length; index++)
	{
		registerHandlers($(img_elements[index]),handleWifiSettingEvent);
	}
	
	for(var index = 0; index < input_elements.length; index++)
	{
		registerHandlers($(input_elements[index]),handleWifiSettingEvent);
	}
	
	for(var index = 0; index < tr_elements.length; index++)
	{
		registerHandlers($(tr_elements[index]),handleWifiSettingEvent);
	}

	for(var index = 0; index < td_elements.length; index++)
	{
		registerHandlers($(td_elements[index]),handleWifiSettingEvent);
	}

	for(var index = 0; index < li_elements.length; index++)
	{
		registerHandlers($(li_elements[index]), handleWifiSettingEvent);
	}
}

function setPageelementWidth()
{
	$("#anonymsIdentt").css("width", mediumInputWidth);
	$("#username").css("width", mediumInputWidth);
	$("#secKey").css("width", mediumInputWidth);
}

function setFieldProperty()
{
	$("#url").css("width", mediumInputWidth);
}

function InitWifiSettings()
{
	setPageelementWidth();
	Common_XML = getLanguageXmlDoc("configuration", "MxCommon");
	translatePage(Common_XML, document, "name");
	translateElements(Common_XML, document, "img", "title", "name");

	BasicProfile_xml = getLanguageXmlDoc("configuration", "BasicProfile");
	translatePage(BasicProfile_xml, document, "name");
	
	WifiSetting_xml = getLanguageXmlDoc("configuration", "WifiSetting");
	translatePage(WifiSetting_xml, document, "name");
	translateElements(WifiSetting_xml, document, "img", "title", "name");
	setFieldProperty();
	$('.tooltip').tooltipster();

	WifiSettingEventBind();
	DisplayWiFiNwstgTab();

	$("#ipAddress").keypress(AutoFormatIpAddr);
	$("#subNetMask").keypress(AutoFormatIpAddr);
	$("#defaultGateWay").keypress(AutoFormatIpAddr);
	$("#preferredDNS").keypress(AutoFormatIpAddr);
	$("#alternetDNS").keypress(AutoFormatIpAddr);

	$("#secKey").focus(clearField);
	$("#url").keypress(filterAsciiUrlSecond);
}

window.onresize = function onresize(){
	mainPageDivSet();
};

$(document).ready(function()
{	
	mainPageDivSet();
	InitWifiSettings();
}); 
