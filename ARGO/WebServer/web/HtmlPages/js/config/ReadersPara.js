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
//Created By   : Kuldip Kanzariya
//File         : ReadersPara.js
//Description  : This file contains javascript functions for Reader Parametets functionality
//
/////////////////////////////////////////////////////////////////////////////

var Common_XML 		= null;
var ReadersPara_xml = null;

var CER_APPL = 1;
var ATOM_RD200_APPL = 7;
var ATOM_RD300_APPL = 8;

var PanelDoor	= 1;
var DirectDoor	= 0;
var DoorType;

var ReaderParaVar =
{
  "SecLevel":"",
  "sensLevel":"",
  "ImgQuality":"",
  "LightConditn":"",
  "IdentiSpeed":"",
  "IntrnlRdrFeq":"",
  "ExtnlRdrFeq":"",
  "TempParam":"",
  "IntnlFprBuildDate":"",
  "ExtnlFprBuildDate":"",  
  "isVariantWithInternalFpRdr":"",  
  "ExtnlRdrDetect":"",
  "tempLogEnable":"",
};
var FpRdrVar =
{
  "GetParaVal":"",
  "TmpltIntrnlFpr":"",
  "TmpltExtnlFpr":"",  
};
var FwUg =
{
		"UpgradeFprFrmware":"",
		"webCustmFile":"",
		"upldStrFileName":"",
		"uploadCustomStingFile":"",		
};
var FpImage =
{
		"ShowLastFpFpRdr":"",
};


function DisplayReaderParaTab()
{
	$(".tab li").removeClass("active");	
	$("#ReaderParaTab").addClass("active");
	hideElements("#FingerPara-div");
	showElem("#ReaderPara-div");
	$(".td-param-label").css("width", 115+"px");
	$("#SecLevel").focus();
	clearValidationMessage();
	if(timer_id != null)
	{
		clearTimeout(timer_id);
	}	
}
function DisplayFpRdrParaTab()
{
	$(".tab li").removeClass("active");	
	$("#FingerParaTab").addClass("active");
	hideElements("#ReaderPara-div");
	showElem("#FingerPara-div");
	$(".td-param-label").css("width", 170+"px");
	$("#GetParaVal").focus();
	clearValidationMessage();
	if(timer_id != null)
	{
		clearTimeout(timer_id);
	}	
}

function saveRdrpara()
{
	var query = "setRdrParamtr?";
	query += "SecLevel" + valFsp + ReaderParaVar.SecLevel;
	query += apendWebFsp()+"sensLevel"		+valFsp+ ReaderParaVar.sensLevel;
	query += apendWebFsp()+"ImgQuality"		+valFsp+ ReaderParaVar.ImgQuality;
	query += apendWebFsp()+"LightConditn"	+valFsp+ ReaderParaVar.LightConditn;
	query += apendWebFsp()+"IdentiSpeed"	+valFsp+ ReaderParaVar.IdentiSpeed;
	query += apendWebFsp()+"IntrnlRdrFeq"	+valFsp+ ReaderParaVar.IntrnlRdrFeq;
	query += apendWebFsp()+"ExtnlRdrFeq"	+valFsp+ ReaderParaVar.ExtnlRdrFeq;
	query += apendWebFsp()+"TempParam"	+valFsp+ ReaderParaVar.TempParam;
	query += apendWebFsp()+"IntnlFprBuildDate"	+valFsp+ ReaderParaVar.IntnlFprBuildDate;
	query += apendWebFsp()+"ExtnlFprBuildDate"	+valFsp+ ReaderParaVar.ExtnlFprBuildDate;

	var response = ajaxrequest("post", "submit" , query, updateDataToObjList, ReaderParaVar);
	if (response == "0") 
	{
		displayValidationMessage(error_img, translateNode(Common_XML,"saveSuccess"), true);
		setRdrPara();		
	}
	else
	{
		displayValidationMessage(error_img, "Saved Failed", false);
	}
	return true;
}
function setRdrPara()
{
	setElemValue("#SecLevel",		ReaderParaVar.SecLevel);
	setElemValue("#sensLevel",		ReaderParaVar.sensLevel);
	setElemValue("#ImgQuality",		ReaderParaVar.ImgQuality);
	setElemValue("#LightConditn",	ReaderParaVar.LightConditn);
	setElemValue("#IdentiSpeed",	ReaderParaVar.IdentiSpeed);
	setElemValue("#IntrnlRdrFeq",	ReaderParaVar.IntrnlRdrFeq);
	setElemValue("#ExtnlRdrFeq",	ReaderParaVar.ExtnlRdrFeq);
	setElemValue("#TempParam",	ReaderParaVar.TempParam);
	setElemValue("#IntnlFprBuildDate",	ReaderParaVar.IntnlFprBuildDate);
	setElemValue("#ExtnlFprBuildDate",	ReaderParaVar.ExtnlFprBuildDate);
	
	setCookie("ExternalDetectedReader", ReaderParaVar.ExtnlRdrDetect);
	var deviceCode = Base64.decode(getCookie('MxCosecDeviceCode'));
	
	if(ReaderParaVar.isVariantWithInternalFpRdr == "0" && deviceCode == 20) // ARGO, If Internal FP not then hide
	{
		hideElements(["#TmpltIntrnlFprRow","#IntnlFprBuildDateRow","#FwIntrnlFpr"]);
		document.getElementById("ShowLastFpFpRdr").style.display = "none";
	}
    hideReaderPara();

	return false;
}

function ValidateReaderTypesupport()
{
	if(FwUg.UpgradeFprFrmware == 1) 
	{
		return false;
	}
	else if((FwUg.UpgradeFprFrmware == 2) && (getCookie("ExternalDetectedReader") != CER_APPL))
	{
		return false;
	}
	else
	{
		return true;
	}
}

function GetRdrPara()
{
	var query = "GetReaderPara?";
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, ReaderParaVar);
	setRdrPara();
	return true;
}
function GetDefaultRdrPara()
{
	var query = "GetDefaultReaderPara?";
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, ReaderParaVar);
	setRdrPara();
}
function SaveFpRdr()
{
	var query = "setFpRdrParam?";
	query += "GetParaVal" + valFsp + FpRdrVar.GetParaVal;
	query += apendWebFsp()+"TmpltIntrnlFpr"		+valFsp+ FpRdrVar.TmpltIntrnlFpr;
	query += apendWebFsp()+"TmpltExtnlFpr"		+valFsp+ FpRdrVar.TmpltExtnlFpr;
	
	var response = ajaxrequest("post", "submit" , query, updateDataToObjList, FpRdrVar);
	if (response == "0") 
	{		
			displayValidationMessage(error_img, translateNode(Common_XML,"saveSuccess"), true);
			setFpRdr();
	}
	else
	{
		displayValidationMessage(error_img, "Saved Failed", false);
	}
	return true;	
}
function GetFpRdr()
{
	var query = "GetFpReader?";
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, FpRdrVar);
	setFpRdr();
	return true;	

}
function setFpRdr()
{
	setElemValue("#GetParaVal",			FpRdrVar.GetParaVal);
	setElemValue("#TmpltIntrnlFpr",		FpRdrVar.TmpltIntrnlFpr);
	setElemValue("#TmpltExtnlFpr",		FpRdrVar.TmpltExtnlFpr);	
	return false;
}
function ValidateFwUpf(id)
{
	if(confirm("Do you want to upgrade firmware?") == true)
	{
	
		if(getElemValue("#"+id) === "")
		{
			// File not found 
			return false;;
		}
		var webStrUpldFileName = getElemValue("#"+id);
		var webStrUpldFileType = webStrUpldFileName.substr(webStrUpldFileName.length-3);
	
		if( (webStrUpldFileName.indexOf("SFM3030_") == -1) && (webStrUpldFileName.indexOf("SFM3050_") == -1))
		{
			displayValidationMessage(error_img, translateNode(ReadersPara_xml,"InvalidFw"), false);
			return false;
		}
	
		if(webStrUpldFileType != "bin")
		{
			displayValidationMessage(error_img, translateNode(ReadersPara_xml,"InvalidFw"), false);
			return false;
		}
	
		if(webStrUpldFileName.lastIndexOf("/") == -1)
		{
			if(webStrUpldFileName.lastIndexOf("\\") == -1)
			{
				webStrUpldFileName = webStrUpldFileName.slice(0 ,-4);
			}
			else
			{
				webStrUpldFileName = webStrUpldFileName.slice(webStrUpldFileName.lastIndexOf("\\")+1, -4);
			}
		}
		else
		{
			webStrUpldFileName = webStrUpldFileName.slice(webStrUpldFileName.lastIndexOf("/")+1, -4);
		}		
	
		if(webStrUpldFileName.indexOf('&') >= 0)
		{
			return false;
		}
		webStrUpldFileName = webStrUpldFileName+"."+webStrUpldFileType;
		setCookie("uploadCustomStingFileCookie", webStrUpldFileName);
		setCookie("UpgradeType", FwUg.UpgradeFprFrmware);
		return true;
	}
	else
	{
		return false;
	}
}
function startUploadWebLabelFile()
{	
	var F;
	var F_FILE;
	F = document.forms[0]; 
	F_FILE = F.webCustmFile;
	//Now send file
    F.method = "post";
    var pageName = "/ReadersPara.html?";
    F.action = "../../goform/upldForm?"+pageName;
    F.submit();
}
function GetShowLastFpFpRdr()
{
	var query = "GetShowLastFpRdr?";
	query += "ShowLastFpFpRdr" + valFsp + FpImage.ShowLastFpFpRdr;
	var response = ajaxrequest("post", "submit" , query);
	if(response == "0")
	{
		DisplayFingrImage();
	}
	else
	{
		displayValidationMessage(error_img, "Saved Failed", false);
	}
}
function DisplayFingrImage()
{
	var newWindow = window.open('','','status=0,height=400,width=300,left=150,top=150');
	var SrcFp= "/Images/fp.bmp?" + Math.random();
	newWindow.document.write("<img src=" + SrcFp + " width='256' height='360'/>");
	newWindow.focus();		
}
function EnableElements()
{
	enableElements(["#webCustmFile"]);
	enableAndBindImgBtn([ "#custmFileBrowseBtn","uploadCustomStingFileBtn"], handleReadersParaEvent); 		
}

function DisableElements()
{
	disableElements(["#webCustmFile"]);					
	disableAndUnbindImgBtn(["#uploadCustomStingFileBtn","#custmFileBrowseBtn"]);		
}
function handleReadersParaEvent(e)
{
	
	if(e.type === "change")
	{
		switch($(this).attr("id"))
		{
			default: break;

			/***************************************************************
			 * 		Reader Parameter Tab
			 ***************************************************************/
			case "SecLevel":
			{
				ReaderParaVar.SecLevel = getElemValue(this);
			}
			break;
			case "sensLevel":
			{
				ReaderParaVar.sensLevel = getElemValue(this);
			}
			break;
			case "ImgQuality":
			{
				ReaderParaVar.ImgQuality = getElemValue(this);
			}
			break;
			case "LightConditn":
			{
				ReaderParaVar.LightConditn = getElemValue(this);
			}
			break;
			case "IdentiSpeed":
			{
				ReaderParaVar.IdentiSpeed = getElemValue(this);
			}
			break;
			case "IntrnlRdrFeq":
			{
				ReaderParaVar.IntrnlRdrFeq = getElemValue(this);
			}
			break;
			case "ExtnlRdrFeq":
			{
				ReaderParaVar.ExtnlRdrFeq = getElemValue(this);
			}
			break;					
			case "TempParam":
			{
				ReaderParaVar.TempParam = getElemValue(this);
			}
			break;
			case "IntnlFprBuildDate":
			{
				ReaderParaVar.IntnlFprBuildDate = getElemValue(this);
			}
			break;
			case "ExtnlFprBuildDate":
			{
				ReaderParaVar.ExtnlFprBuildDate = getElemValue(this);
			}
			break;							
			/***************************************************************
			 * 		Finger Print Reader Tab
			 ***************************************************************/
			case "GetParaVal":
			{
				FpRdrVar.GetParaVal = getElemValue(this);
			}
			break;
			case "TmpltIntrnlFpr":
			{
				FpRdrVar.TmpltIntrnlFpr = getElemValue(this);
			}
			break;
			case "TmpltExtnlFpr":
			{
				FpRdrVar.TmpltExtnlFpr = getElemValue(this);
			}
			break;			
									
			case "UpgradeFprFrmware":
			{
				FwUg.UpgradeFprFrmware = getElemValue(this);
				if(FwUg.UpgradeFprFrmware == "1" || FwUg.UpgradeFprFrmware == "2" )
				{	
					EnableElements();
				}
				else
				{
					DisableElements();
				}
			}
			break;
			case "webCustmFile":
			{
				FwUg.webCustmFile = getElemValue(this);
				var pathStr = getElemValue("#webCustmFile").split('\\');
				setElemValue("#image-filepath", pathStr[pathStr.length -1]);
				if(pathStr.length > 0)
				{
					enableAndBindImgBtn([ "#uploadCustomStingFileBtn"], handleReadersParaEvent); 
				}
			}
			break;
		}
	}
	if(e.type === "click")
	{
		switch($(this).attr("id"))
		{
			default: break;

			/***************************************************************
			 * 		Reader Parameter Tab
			 ***************************************************************/
			case "ReaderParaTab":
			{
			  DisplayReaderParaTab();
			}
			break;
			case "FingerParaTab":
			{
			  DisplayFpRdrParaTab();
			}
			break;
			case "submitRdrPara":
			{
				saveRdrpara();
			}
			break;
			case "cancelRdrPara":
			{
				if(GetRdrPara() == true)
					{
						setRdrPara();
					}
			}
			break;	
			case "defaultRdrPara":
			{
				GetDefaultRdrPara();
			}
			break;	
			/***************************************************************
			 * 		Finger Print Reader Tab
			 ***************************************************************/			 						
			case "submitFpRdr":
			{
				SaveFpRdr();
			}
			break;
			case "cancelFpRdr":
			{
				if(GetFpRdr() == true)
				{
						setFpRdr();
						setElemValue("#UpgradeFprFrmware",	"0");
						setElemValue("#image-filepath","");
						DisableElements();
				}
			}
			break;		
			case "ShowLastFpFpRdr":
			{
				GetShowLastFpFpRdr();
			}
			break;
			case "uploadCustomStingFileBtn":
			{
				if(ValidateReaderTypesupport() == false)
				{
					displayValidationMessage(error_img, translateNode(ReadersPara_xml,"InvalidFw"), false);
					return false;
				}
				
				if(ValidateFwUpf("webCustmFile") == false)
				{
					return false;
				}
				setBlockState(translateNode(ReadersPara_xml,"processingMsg"));
				setTimeout(startUploadWebLabelFile, 500);			
			}
			break;		
			
		}
	}
  
  
}
function ReadersParaEventBind()
{
	/*********************************************/
		NOTE:	/*   Always Register this event first For trime 
			         All Input field.*/		
			    
		$("input:text").change(treamAllInputTextField);
		StopPastFronInputTextField();
	/*******************************************/
	var container = $("#ReadersPara");
	var sel_elements = $("select", $(container));
	var input_elements = $("input", $(container));
	var img_elements = $("img", $(container));
	var li_elements = $("li", $(container));
	var td_elements = $("td", $(container));
	
	for(var index = 0; index < sel_elements.length; index++)
	{
		registerHandlers($(sel_elements[index]),handleReadersParaEvent);
	}
	
	for(var index = 0; index < img_elements.length; index++)
	{
		registerHandlers($(img_elements[index]),handleReadersParaEvent);
	}
	
	for(var index = 0; index < input_elements.length; index++)
	{
		registerHandlers($(input_elements[index]),handleReadersParaEvent);
	}

	for(var index = 0; index < li_elements.length; index++)
	{
		registerHandlers($(li_elements[index]), handleReadersParaEvent);
	}
	
	for(var index = 0; index < td_elements.length; index++)
	{
		registerHandlers($(td_elements[index]), handleReadersParaEvent);
	}
	
	registerHandlersArray(["#custmFileBrowseBtn" , "uploadCustomStingFileBtn", "webCustmFile", ],handleReadersParaEvent);
}
  
function InitReadersPara()
{
	Common_XML = getLanguageXmlDoc("configuration", "MxCommon");
	translatePage(Common_XML, document, "name");

	ReadersPara_xml = getLanguageXmlDoc("configuration", "ReadersPara");
	translatePage(ReadersPara_xml, document, "name");

    DoorType = Base64.decode(getCookie('MxDeviceType'));
	
	ReadersParaEventBind();
	GetRdrPara();
	GetFpRdr();	
	DisableElements();
	if((getCookie("uploadCustomStingFileCookie"))  != null)
	{
		DisplayFpRdrParaTab();
		var query = "setFprFirmwareUpgrade?";
		query += "upldStrFileName" + valFsp + getCookie("uploadCustomStingFileCookie");
		query += apendWebFsp()+"UpgradeFprFrmware"+valFsp+ getCookie("UpgradeType");
		var response = ajaxrequest("post", "submit", query);
		
		setElemValue("#UpgradeFprFrmware",	getCookie("UpgradeType"));
		clearCookie("uploadCustomStingFileCookie");
		clearBlockState();
		clearCookie("UpgradeType");
		if(response == "0")
		{
			displayValidationMessage(success_img , translateNode(ReadersPara_xml, "UpgradeSuccess")  , true);
			
		}
		else
		{
			displayValidationMessage(error_img , translateNode(ReadersPara_xml,"UpgradeFail")  , false);
		}		
	}
	else
	{
		DisplayReaderParaTab();	
	}
    hideReaderPara();
}

function hideReaderPara()
{
    if( (DoorType == PanelDoor) || ((DoorType == DirectDoor) && ( ReaderParaVar.tempLogEnable === 0)) )
    {
        hideElements("#TmpParaField");
    }
    else
    {
        showElem("#TmpParaField");
    }
}

window.onresize = function onresize(){
	mainPageDivSet();
};
	
$(document).ready(function()
{
	// This hide show is for smooth browsing experience
	hideElem("#ReadersPara");
	mainPageDivSet();
	InitReadersPara();
	showElem("#ReadersPara");
});
