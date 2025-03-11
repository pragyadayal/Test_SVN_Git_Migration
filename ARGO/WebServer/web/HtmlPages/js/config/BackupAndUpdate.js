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
//Created By   : Manish chandra
//File         : BackupAndUpgrade.js
//Description  : This file contains javascript functions for BackupAndUpdate.html 
// 				 file.
//
///////////////////////////////////////////////////////////////////////////// 

var MxCommonXml 			= null;
var BackupAndUpgradeXml 	= null;
var directDoorFwNameArgo 	= "COSEC_ARGO_DOOR";
var directDoorArgoSDK		= "COSEC_ARGO_SDK";
var cerFwName 				= "COSEC_CER";
var cbrFwName 				= "COSEC_READER_CB";
var atomFwName 				= "COSEC_READER_ATOM";
var	browseBtnPressed 		= false;
var restoreBtnPressed 		= false;
var deviceType;
var panelDoor 	= 	1;
var mmcStatusF	=	true;

var NO_RDR_PRESENT	= 0;
var CB_PRESENT		= 13;
var CER_PRESENT		= 9;
var ATOM_RD300_READER	= 15;
var ATOM_RD200_READER	= 16;
var ATOM_RD100_READER	= 17;
/*
    Applicable Reader: ATOM RD200 /ATOM RD300 (TELIT Board only)
    Threshold Firmware Version: COSEC_READER_ATOM_V01R04.00
 */
var RdrType =
{
	"rdrType":"",
	"rdrHwType":"",
	"atomFwVerDgCtrl":"",
};
var backupAndUpgradeList =
{
	"status":"",
	"buType":"",
	"FileName":"",
	"BenchmarkFwVer":"",
};

var mmcDetail	=
{
	"status"	:"",
	"freeSpace"	:"",
};

function CheckConfig(strName)
{
	var subStr;
	getZipName();
    if(backupAndUpgradeList.FileName == strName)
    {
	    subStr=strName.substr(strName.length-4);    	
	    if(subStr!=".zip")
		    return false;
	    return true;
   }
   else
   {
		clearBlockState();
	    return false;
   }
}

function ValidateUpdate(id)
{
	var deviceCode = Base64.decode(getCookie('MxCosecDeviceCode'));
	
	var F;
	var F_FILE;
	if(browseBtnPressed == true) 			//firmware
	{
		F = document.forms[0]; //$("#firmwareForm");
		F_FILE = F.upgradeFirmwareFile;
	}
	else if(restoreBtnPressed == true)	//Config
	{
		F = document.forms[1];
		F_FILE = F.rstoreCnfgFile;
	}

	if(browseBtnPressed == true)
	{
		GetRdrType();
		if(F_FILE.value.indexOf(directDoorArgoSDK) != -1)
		{
			if(confirm("SDK Upgrade may take few minutes. Do you want to upgrade the SDK?") == 0)
			{
				return false;	
			}
		}
		else
		{
			if(getCookie('MxUpgradeFor') == 0)
			{
				if(confirm("Do you want to upgrade firmware?") == 0)
				{
					return false;	
				}
			}
			else
			{//For Reader 	BLE_MODULE_TYPE_TI = 0,BLE_MODULE_TYPE_TELIT=1
				var FwName = F_FILE.value.substring(F_FILE.value.lastIndexOf('/')+1)
				console.log(FwName);
				FwName = FwName.substr(12);
				console.log(FwName);
				GetRdrType();
				if(((RdrType.rdrType == ATOM_RD300_READER) || (RdrType.rdrType == ATOM_RD200_READER)) && (RdrType.rdrHwType == 1) && (CheckAtomFirmwareZip(FwName) == true))
				{
					if((FwName.indexOf(atomFwName) != -1) && (FwName < RdrType.atomFwVerDgCtrl))   
					{ 
						console.log(FwName);
						if(confirm("Bluetooth will not be supported for the selected firmware version. Do you want to continue?") == 0)
						{
							return false;	
						}
					}
					else if(confirm("Do you want to upgrade firmware?") == 0)
					{
						return false;
					}
				}
				else
				{
					if(confirm("Do you want to upgrade firmware?") == 0)
					{
						return false;	
					}
				}
			  
			}
		}

		//check valid firmware file
		if(CheckFirmwareName(F_FILE.value.substring(F_FILE.value.lastIndexOf('/')+1)) == false)
		{	
			if(CheckFirmwareName(F_FILE.value.substring(F_FILE.value.lastIndexOf('\\')+1)) == false) 
			{
				if(mmcStatusF == true)
				{
					if((getCookie('MxUpgradeFor')!=0) && ((RdrType.rdrType ==0 ) || (RdrType.rdrType =="" )))
			    			displayValidationMessage(error_img , translateNode(BackupAndUpgradeXml, "connectReader")  , false);
			    	else
							displayValidationMessage(error_img , translateNode(BackupAndUpgradeXml, "slctAprprtateFirmware")  , false);
					return false;
				}
				return false;
			}
		}
    }
    else if(restoreBtnPressed == true)
    {
		if(confirm("Do you want to restore configurations?"))
		{ //check valid configuration file
		    if(CheckConfig(F_FILE.value.substring(F_FILE.value.lastIndexOf('/')+1)) == false)
		    {	
			    if(CheckConfig(F_FILE.value.substring(F_FILE.value.lastIndexOf('\\')+1)) == false) 
			    {
			         displayValidationMessage(error_img , translateNode(BackupAndUpgradeXml, "slctAprprtateCnfg")  , false);
				    return false;	
			    }
		    }
		}
		else
		{
			return false;
		}
	}
}

function startUploadBackupAndUploadFile()
{	
	var F;
	var F_FILE;
	if(browseBtnPressed == true) 			//firmware
	{
		F = document.forms[0]; //$("#firmwareForm");
		F_FILE = F.upgradeFirmwareFile;
	}
	else if(restoreBtnPressed == true)	//Config
	{
		F = document.forms[1];
		F_FILE = F.rstoreCnfgFile;
	}
	
	//Now send file
    F.method = "post";
    var pageName = "/BackupAndUpdate.html?";
    F.action = "../../goform/upldForm?"+pageName;
    F.submit();
}

function startEventBackup()
{
	var query = "GetEventBackup?"; 
	var responce=ajaxrequest("post", "submit", query,updateDataToObjList, backupAndUpgradeList);
	if(responce == 0)
	{
		setBlockState(translateNode(Common_XML,"tackingBackup"));
		// setTimeout(startUploadBackupAndUploadFile, 500);
		var F=document.forms[0];
	    F.method="get";
	    F.action="/"+backupAndUpgradeList.FileName;
	    F.submit();
	    displayValidationMessage(success_img ,translateNode(BackupAndUpgradeXml,"BackupSuccessful"), true);
	}
}

function getZipName()
{
	var query = "GetEventBackup?"; 
	ajaxrequest("post", "submit", query,updateDataToObjList, backupAndUpgradeList);
}

function backupAndUpdateSettingsControls(e)
{
	var id = $(this).attr("id");
	var element = GetElementFormID(id);
	var index = GetIndexFromID(id);

	if(e.type === "change")
	{
		switch(element)
		{	 
			case "upgradeFirmwareFile":
			{
				var pathStr = getElemValue("#upgradeFirmwareFile").split('\\');
				setElemValue("#image-upgradeFirmwarefilepath", pathStr[pathStr.length -1]);
				if(pathStr.length > 0)
				{
					if(isEnabledImage("#upgradeFirmwareUploadBtn") == false)
					{
						enableAndBindImgBtn([ "#upgradeFirmwareUploadBtn"], backupAndUpdateSettingsControls); 
					}
					browseBtnPressed = true;
					restoreBtnPressed = false;
					clearBlockState();
				}
			}
			break;
			
			case "rstoreCnfgFile":
			{
				var pathStr = getElemValue("#rstoreCnfgFile").split('\\');
				setElemValue("#image-rstoreCnfgfilepath", pathStr[pathStr.length -1]);
				if(pathStr.length > 0)
				{
					if(isEnabledImage("#rstoreCnfgRestoreBtn") == false)
					{
						enableAndBindImgBtn([ "#rstoreCnfgRestoreBtn"], backupAndUpdateSettingsControls);
					}
					restoreBtnPressed = true;
					browseBtnPressed = false;
					clearBlockState(); 
				}
			}
			break;
			case "upgradeFor":
			{
				setCookie('MxUpgradeFor', getElemValue("#upgradeFor"), 	30);
			//need to set cookie as this value is used after form submit (page reloads) too.
			}
			
		}
	}

	if(e.type === "click")
	{
		switch(element)
		{
			case "autoFirmawreUpgde":
			{
				window.location.href ="AutoFirmwareUpgrade.html";
			}break;

			case "upgradeFirmwareUploadBtn":
			{
				browseBtnPressed = true;
				restoreBtnPressed = false;
				if(ValidateUpdate("upgradeFirmwareFile") == false)
				{
					return false;
				}
				else
				{
					// Here Very small time out require.
					// if time out removed than overlay is not create.
					setBlockState(translateNode(Common_XML,"upgrading"));
					setTimeout(startUploadBackupAndUploadFile, 500);	
				}
			}
			break;
			
			case "rstoreCnfgRestoreBtn":
			{
				restoreBtnPressed = true;
				browseBtnPressed = false;
				if(ValidateUpdate("rstoreCnfgFile") == false)
				{
					return false;
				}
				else
				{
					// Here Very small time out require.
					// if time out removed than overlay is not create.
					setBlockState(translateNode(Common_XML,"restoring"));
					setTimeout(startUploadBackupAndUploadFile, 500);
				}
				
				
			}
			break;

			case "backupEvntBtn":
			{
				//phale get ki request karni h
				startEventBackup();
			    clearBlockState(); 
			
			}
			break;

		}
	}
}


function processBackupUpdatestatus()
{
	if(backupAndUpgradeList.buType  == "1")
	{
		if((backupAndUpgradeList.status == "1") )
		{
			//firmware upgrade successfully
			displayValidationMessage(success_img ,translateNode(BackupAndUpgradeXml,"MXCsubmitSuc"), true);
		}
		else if(backupAndUpgradeList.status == "3")
		{
			//downgrade not allowed
			displayValidationMessage(error_img ,translateNode(BackupAndUpgradeXml,"MxcmsgDNA"), false);
		}
		else if(backupAndUpgradeList.status == "2")
		{
			//firmware checksum fail
			displayValidationMessage(error_img ,translateNode(BackupAndUpgradeXml,"MxcmsgFCF"), false);
		}
		else if(backupAndUpgradeList.status == "4")
		{
			//failed to upgrade firmware
			var str1 = translateNode(BackupAndUpgradeXml,"requiredFwNotAvailLabel1");
			var str2 = translateNode(BackupAndUpgradeXml,"requiredFwNotAvailLabel2");
			var message = str1 + backupAndUpgradeList.BenchmarkFwVer + str2;
			displayValidationMessage(error_img , message, false);			
		}
	}
	else if(backupAndUpgradeList.buType == "2")
	{
		if(backupAndUpgradeList.status == "1")
		{
			//config restore successfully
			displayValidationMessage(success_img ,translateNode(BackupAndUpgradeXml,"configRestoreSucssfly"), true);
		}
		else if(backupAndUpgradeList.status == "0")
		{
			/*Backup and Update Failed*/
			displayValidationMessage(error_img ,translateNode(BackupAndUpgradeXml,"failedToRstoreCnfg"), false);	
		}
	}
	else 
	{
		if(backupAndUpgradeList.status == "1")
		{
			//backup successfully
			displayValidationMessage(success_img ,translateNode(BackupAndUpgradeXml,"BackupSuccessful"), true);
		}
		else if(backupAndUpgradeList.status === "0")
		{
			//backup failed
			displayValidationMessage(error_img ,translateNode(BackupAndUpgradeXml,"BackupFail"), false);	
		}
	}

}

//Check valid firmware name
function CheckFirmwareName(strName)
{
    var subStr = strName.substring(strName.length-4);
    var deviceCode = Base64.decode(getCookie('MxCosecDeviceCode'));

    if(getCookie('MxUpgradeFor') == 0)
	{
		if(strName.indexOf(directDoorFwNameArgo) == -1)
		{
			if(strName.indexOf(directDoorArgoSDK) != -1)
	    	{
	    		var query = "GetMmcStatus?"; 
				var responce=ajaxrequest("post", "submit", query,updateDataToObjList, mmcDetail);
				if(responce == 0)
				{
					if(mmcDetail.status == 0)
					{
						mmcStatusF	=	false;
						displayValidationMessage(error_img ,translateNode(BackupAndUpgradeXml,"mmcNotConnected"), false);
						return false;
					}
					else
					{
						if(mmcDetail.freeSpace == 1)
						{
							return true;
						}
						else
						{
							displayValidationMessage(error_img ,translateNode(BackupAndUpgradeXml,"mmcNotEnoughSpace"), false);
							return false;
						}
					}
				}
	    	}
	    	return false;
		}
		if( (subStr!=".zip") )
		{
	    	return false;
		}
	}
	else if(getCookie('MxUpgradeFor') == 1)	//if anyone of the reader is cer/cb and active accept fw.
	{
		if(strName.indexOf(cerFwName) != -1)
    	{
    		if(RdrType.rdrType != CER_PRESENT )
    			return false;
    	}
    	else if(strName.indexOf(cbrFwName) != -1)
    	{
    		if(RdrType.rdrType != CB_PRESENT )
    			return false;
    	}
    	else if(strName.indexOf(atomFwName) != -1)
    	{
    		if((RdrType.rdrType != ATOM_RD100_READER ) && (RdrType.rdrType != ATOM_RD200_READER )
			&& (RdrType.rdrType != ATOM_RD300_READER ))
    			return false;
    	}
    	else
    		return false;
		
		if((RdrType.rdrType == ATOM_RD300_READER) || (RdrType.rdrType == ATOM_RD200_READER))
		{
			if(subStr != ".zip")
				return false;
		}
		else 
		{
			if((subStr!=".bin"))
			return false;
		}		
	}

    return true;
}

function UnzipNow()
{
    if (window.self.location.search.indexOf("filename=") >= 1)
    {
	    var restoreVal;
	    restoreVal = window.self.location.search.substring(window.self.location.search.indexOf("filename=",0)+9);

		if (CheckFirmwareName(restoreVal) == true)
		{
			var query = "SetBackupAndUpgrade?"; 
			query += "file" + valFsp + restoreVal ;
		}
		else if(CheckConfig(restoreVal) == true)
		{
			var query = "SetBackupAndUpgrade?"; 
			query += "file" + valFsp + restoreVal ;
		}
		else
		{
			// alert("wrong file selected");	
			clearBlockState(); 
			return false;
		}

		do
		{
			var responce=ajaxrequest("post", "submit", query,updateDataToObjList, backupAndUpgradeList);
			if(responce == 0)
			{
				processBackupUpdatestatus();
				clearBlockState();
				browseBtnPressed = false;
				restoreBtnPressed = false;
				break;
			}
		}while(responce == undefined);
    }
}


function backupAndUpdateEventBind()
{
	/*********************************************/
	NOTE:	/*   Always Register this event first For trime 
			         All Input field.*/	
	StopPastFronInputTextField();
	$("input:text").change(treamAllInputTextField);
	/*******************************************/
	var container 			= $("#BackupAndUpdateSettings");
	var sel_elements 		= $("select", $(container));
	var input_elements 		= $("input", $(container));
	var img_elements 		= $("img", $(container));
	var td_elements 		= $("td", $(container));
	var li_elements 		= $("li", $(container));

	for(var index = 0; index < sel_elements.length; index++)
	{
		registerHandlers($(sel_elements[index]), backupAndUpdateSettingsControls);
	}

	for(var index = 0; index < img_elements.length; index++)
	{
		registerHandlers($(img_elements[index]),backupAndUpdateSettingsControls);
	}

	for(var index = 0; index < input_elements.length; index++)
	{
		registerHandlers($(input_elements[index]),backupAndUpdateSettingsControls);
	}

	for(var index = 0; index < td_elements.length; index++)
	{
		registerHandlers($(td_elements[index]),backupAndUpdateSettingsControls);
	}
	for(var index = 0; index < li_elements.length; index++)
	{
		registerHandlers($(li_elements[index]), backupAndUpdateSettingsControls);
	}
	
	registerHandlersArray(["#upgradeFirmwareBrowseBtn" , "upgradeFirmwareUploadBtn" , "rstoreCnfgBrowseBtn" ,"rstoreCnfgRestoreBtn","backupEvntBtn"],backupAndUpdateSettingsControls);	

}

function DisplayBackupAndUpgradePage()
{
	$(".tab li").removeClass("active");	
	$("#backupAndUpgrade").addClass("active");
	//hideElements(["#WiFiAccsPntstg-div", "#wifiAccPoints"]);
	showElem("#backupAndUpgrade-div");
	$(".td-param-label").css("width", 115+"px");
	clearValidationMessage();
	if(timer_id != null)
	{
		clearTimeout(timer_id);
	}
}


function initBackupAndUpdate()
{
	MxCommonXml = getLanguageXmlDoc("configuration", "MxCommon");
	translatePage(MxCommonXml,document,"name");

	BackupAndUpgradeXml = getLanguageXmlDoc("configuration", "BackupAndUpgrd");
	translatePage(BackupAndUpgradeXml, document, "name");
	
	translateElements(BackupAndUpgradeXml, document, "img", "title", "name");
	$('.tooltip').tooltipster();
	deviceType	= Base64.decode(getCookie('MxDeviceType'));
	
	backupAndUpdateEventBind();
	disableAndUnbindImgBtn(["#upgradeFirmwareUploadBtn","#rstoreCnfgRestoreBtn"]);
	DisplayBackupAndUpgradePage();
	if(deviceType == panelDoor)
	{
		setLabel("#mainHeader","Firmware Upgrade");
		hideElements(["#subHeaderRow","#backupEvntRow", "#restoreCnfgRow"]);
	}
	else
	{
		setLabel("#mainHeader","Backup and Upgrade");
		showElements(["#subHeaderRow","#backupEvntRow", "#restoreCnfgRow"]);
	}

}

window.onresize = function onresize(){
	mainPageDivSet();
};

function GetRdrType()
{
	var query = "GetExtReaderType?";
	var responce=ajaxrequest("post", "submit", query,updateDataToObjList, RdrType);
}

//Check valid firmware name
function CheckAtomFirmwareZip(strName)
{
	var subStr=strName.substring(strName.length-4);
	var deviceCode = Base64.decode(getCookie('MxCosecDeviceCode'));
	if (subStr == ".zip") 
	{
	    return true;
	}
	else
	  return false;
}

$(document).ready(function()
{
	mainPageDivSet();
	initBackupAndUpdate();
	GetRdrType();
	UnzipNow();
	setCookie('MxUpgradeFor', getElemValue("#upgradeFor"), 	30);	//on form submit page will reload and we lost the value of element.
});
