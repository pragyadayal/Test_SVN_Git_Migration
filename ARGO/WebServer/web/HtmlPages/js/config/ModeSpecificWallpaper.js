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
//Created By   : MAnish chandra	
//File         : ModeSpecificWallpaper.js
//Description  : This file contains javascript functions for main navigation panel.
//
/////////////////////////////////////////////////////////////////////////////
var ModeSpecificWallpaper_xml	=	null;
var WallPaper_XML 				= null;
var Common_XML					=	null;
var WALLPAPER_IDX 				= 0;
var FILE_PATH 					= 1;
var prevMsgRowId;
var SetExitAsWallpaper			= 1;
var SetEntryAsWallpaper			= 0;
var selectedFileName			= "";

var ModeSpecificWallpaper	=
{	
	"dataType":"",
	"index":"",
	"file":"",
	"WP_1":"",
	"WP_2":"",
	"currentWallPaper":"",
	"WP_REMOVE_1":"",
	"WP_REMOVE_2":"",
	"WP_ENTRY_SET_WP":"",
	"WP_EXIT_SET_WP":"",
};

function getModeSpecificWallpaper()
{
	var query = "GetModeSpecificWallpaper?";
	var responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, ModeSpecificWallpaper);
	if(responseCode == "0")
	{
		setModeSpecificWallpaper();
		fillData();
	}
	return true;
}

function validateFileName(imageFile)
{
	var imageName;
	
	imageName = getElemValue("#" +imageFile);
	if(imageFile == "entryModewallPaper")
	{
		msgRowId = 1;
	}
	else
	{
		msgRowId = 2;
	}
	
	if(imageName === "")
	{
		displayValidateMsg(msgRowId, translateNode(WallPaper_XML, "slectFileStr"));;
		return false;
	}

	var subStr = imageName.substring(imageName.lastIndexOf("."));
	
	if( (subStr!=".png") && (subStr!=".PNG") && (subStr!=".jpg") && (subStr!=".JPG") && (subStr!=".jpeg") && (subStr!=".JPEG") && (subStr!=".bmp") && (subStr!=".BMP") )
	{
		displayValidateMsg(msgRowId, translateNode(WallPaper_XML, "supportImgFileStr"));
		return false;
	}
	var mainStr;
	mainStr=imageName.slice(0,imageName.lastIndexOf("."));
	mainStr = mainStr.split(/[\\\/]/).pop();
	    
	if(ValidateImageName(mainStr) == false)
	{
		displayValidateMsg(msgRowId, translateNode(WallPaper_XML, "supportCharacterStr"));
		return false;
	} 
	
	var fileSize;
	try
	{
		fileSize=$("#"+ imageFile)[0].files[0].size;
	}
	catch(e)
	{
		try
		{
			fileSize=$("#"+imageFile)[0].files.size;
		}
		catch(e)
		{
			fileSize=0;
		}
	}
	
	if(fileSize)
	{
		if((fileSize/1024) >512)
		{
			displayValidateMsg(msgRowId, translateNode(WallPaper_XML, "sizeValidationStr"));
			selectedFileName = "";
			return false;
		}
		selectedFileName = mainStr + subStr;
		return true;
	}
	else
	{
		displayValidateMsg(msgRowId, translateNode(WallPaper_XML, "browsererrorMsg"));
		return false;
	}
}	
	
function clearMsg()
{
	setLabel("#invalidMsg_"+prevMsgRowId, "");
}

function displayValidateMsg(msgRowId, validationMsgStr)
{
	clearMsg();
	prevMsgRowId=msgRowId;
	setLabel("#invalidMsg_"+ msgRowId, validationMsgStr);
	$("#invalidMsg_"+ msgRowId).attr({"class":"msg_str_fail"});
	$("#invalidMsg_"+ msgRowId).css("margin-left", "0px");
	setTimeout(clearMsg, 5000);
}

function setModeSpecificWallpaper()
{
	setElemValue("#WP_1", 				ModeSpecificWallpaper.WP_1);
	setElemValue("#WP_2", 				ModeSpecificWallpaper.WP_2);
	if(ModeSpecificWallpaper.WP_1 == "IN_Wallpaper.png")  
	{
		disableAndUnbindImgBtn(["#WP_REMOVE_1"]);
	}
	else
	{
		enableAndBindImgBtn(["#WP_REMOVE_1"]);
	}
	if(ModeSpecificWallpaper.WP_2 == "OUT_Wallpaper.png")
	{
		disableAndUnbindImgBtn(["#WP_REMOVE_2"]);
	}
	else
	{
		enableAndBindImgBtn(["#WP_REMOVE_2"]);
	}
}

function startUploadingImage(imageId)
{
	var imageFile;
	if(imageId == 1)
	{
		imageFile = "entryModewallPaper";
	}
	else
	{
		imageFile = "exitModewallPaper";
	}
	var webStrUpldFileName= selectedFileName;
	setCookie("uploadModeSpecificImageFileCookie", webStrUpldFileName);
	var dataType;

	dataType = WALLPAPER_IDX;
	var query, restoreVal, fileStr, responseCode;
	query = "SetModeSpecificWallpaperImage?";
	query += "dataType"+ valFsp + WALLPAPER_IDX;
	query += apendWebFsp()+"index"+ valFsp + (imageId-1);
	query += apendWebFsp()+"WP_ENTRY_SET_WP"+ valFsp + ModeSpecificWallpaper.WP_ENTRY_SET_WP;
	query += apendWebFsp()+"WP_EXIT_SET_WP"+ valFsp + ModeSpecificWallpaper.WP_EXIT_SET_WP;	
	
	responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, ModeSpecificWallpaper);
	
	//Now send file
	var F;
	var F_FILE;
	F = document.forms[imageId-1]; 
	F.method = "post";
	var pageName = "/ModeSpecificWallpaper.html?";
	F.action = "../../goform/upldForm?"+pageName;
	F.submit();
}

function RemoveModeSpecificWallpaper(idx)
{
	var	query = "RemoveModeSpecificWallpaper?";
	query += "index"+ valFsp + (idx-1);
	responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, ModeSpecificWallpaper);
	
	getModeSpecificWallpaper();
	FillData();
}

function handleModeSpecificWalpaper(e)
{
	var id = $(this).attr("id");
	var element = GetElementFormID(id);
	var index = GetIndexFromID(id);
	var pathStr;
	
	if(e.type === "change")
	{
		switch(id)
		{
			case "entryModewallPaper":
			{
				pathStr = getElemValue("#entryModewallPaper").split('\\');
				setElemValue("#entryModewallPaperimage-filepath", pathStr[pathStr.length -1]);
			}
			break;
			
			case "exitModewallPaper":
			{
				var pathStr = getElemValue("#exitModewallPaper").split('\\');
				setElemValue("#exitModeWappaeruploadImage-filepath", pathStr[pathStr.length -1]);
			}
			break;
	
			default:
			 break;	
			
		}
	}
	/*****************************************************
					Btn Action
	 ****************************************************/
	if(e.type === "click")
	{
		switch(element)
		{
			case "entryModeWallPaperuploadImage":
			{
				if(validateFileName("entryModewallPaper") == true)
				{
					setBlockState(translateNode(Common_XML,"uploadingMsg"));
					startUploadingImage(1);
				}
			}break;
			
			case "exitModeWallpaperuploadImage":
			{
				if(validateFileName("exitModewallPaper") == true)
				{
					setBlockState(translateNode(Common_XML,"uploadingMsg"));
					startUploadingImage(2);
				}
			}break;
			case "WP_REMOVE_":
			{
				RemoveModeSpecificWallpaper(index);
			}
			break;
					
			case "WP_ENTRY_SET_WP":
			{
				if(ModeSpecificWallpaper.WP_ENTRY_SET_WP != isChecked(this)? 1: 0)
				{
					if( ( (ModeSpecificWallpaper.WP_ENTRY_SET_WP == 0) && (confirm("Shown image will be set as device wallpaper") == true))
						|| ((ModeSpecificWallpaper.WP_ENTRY_SET_WP == 1) && (confirm("Shows Mode instead of Mode specific wallpaper") == true)) )
					{
						ModeSpecificWallpaper.WP_ENTRY_SET_WP =  isChecked(this)? 1: 0;
						setWallpaperQuery(ModeSpecificWallpaper.WP_ENTRY_SET_WP,SetEntryAsWallpaper);
					}
					else
					{
						ModeSpecificWallpaper.WP_ENTRY_SET_WP == 1 ? checkElem("#WP_ENTRY_SET_WP") :uncheckElem("#WP_ENTRY_SET_WP");
					}
				}
			} break;
			
			case "WP_EXIT_SET_WP":
			{
				if(ModeSpecificWallpaper.WP_EXIT_SET_WP != isChecked(this)? 1: 0)
				{
					if( ( (ModeSpecificWallpaper.WP_EXIT_SET_WP == 0) && (confirm("Shown image will be set as device wallpaper") == true))
						|| ((ModeSpecificWallpaper.WP_EXIT_SET_WP == 1) && (confirm("Shows Mode instead of Mode specific wallpaper") == true)) )
					{
						ModeSpecificWallpaper.WP_EXIT_SET_WP =  isChecked(this)? 1: 0;
						setWallpaperQuery(ModeSpecificWallpaper.WP_EXIT_SET_WP,SetExitAsWallpaper);
					}
					else
					{
					  	ModeSpecificWallpaper.WP_EXIT_SET_WP == 1 ? checkElem("#WP_EXIT_SET_WP") :uncheckElem("#WP_EXIT_SET_WP");
					}
				}
			} break;
			
			default:
			 break;	
		}
	}
}

function setWallpaperQuery(value, wallpaperSet)
{
	var query,responseCode;	
	query = "SetModeSpecificWallpaperImage?";
	if(wallpaperSet == 0)
	{
		query += "WP_ENTRY_SET_WP"+ valFsp + value;	
	}
	else
	{
		query += "WP_EXIT_SET_WP"+ valFsp + value;
	}
	responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, ModeSpecificWallpaper);
	if(responseCode == 0)
	{
		getModeSpecificWallpaper();
	}
}

function fillData()
{
	var idx;
	for(idx=1; idx<=2; idx++)
	{
		$("#img"+idx).attr("src", "/Images/WallpaperLink/DoorModeWallpapers/"+ ModeSpecificWallpaper["WP_"+ idx] + "?" + new Date().getTime());
		
		if(ModeSpecificWallpaper["WP_REMOVE_"+ idx] == 1)
		{
			enableElem("WP_REMOVE_"+idx);
		}
		else
		{
			disableElem("WP_REMOVE_"+idx);	
		}
	}
	ModeSpecificWallpaper.WP_ENTRY_SET_WP == "1"?checkElem("#WP_ENTRY_SET_WP"):uncheckElem("#WP_ENTRY_SET_WP");
	ModeSpecificWallpaper.WP_EXIT_SET_WP == "1"?checkElem("#WP_EXIT_SET_WP"):uncheckElem("#WP_EXIT_SET_WP");
}

function modeSpecificWallpaperEventBind()
{
	/*********************************************/
	NOTE:	/*   Always Register this event first For trime 
			         All Input field.*/		
	StopPastFronInputTextField();    
	$("input:text").change(treamAllInputTextField);
	/*******************************************/
	var container 			= $("#ModeSpecificWalpaper");
	var sel_elements 		= $("select", $(container));
	var input_elements 		= $("input", $(container));
	var img_elements 		= $("img", $(container));
	var td_elements 		= $("td", $(container));
	var li_elements 		= $("li", $(container));
	
	for(var index = 0; index < sel_elements.length; index++)
	{
		registerHandlers($(sel_elements[index]), handleWallPaperControls);
	}
	
	for(var index = 0; index < img_elements.length; index++)
	{
		registerHandlers($(img_elements[index]), handleModeSpecificWalpaper);
	}
	
	for(var index = 0; index < input_elements.length; index++)
	{
		registerHandlers($(input_elements[index]), handleModeSpecificWalpaper);
	}
	
	for(var index = 0; index < td_elements.length; index++)
	{
		registerHandlers($(td_elements[index]), handleModeSpecificWalpaper);
	}
	
	for(var index = 0; index < li_elements.length; index++)
	{
		registerHandlers($(li_elements[index]), handleModeSpecificWalpaper);
	}
}


function initModeSpecificWallpaper()
{
	Common_XML = getLanguageXmlDoc("configuration", "MxCommon");
	translatePage(Common_XML, document, "name");

	ModeSpecificWallpaper_xml = getLanguageXmlDoc("configuration", "ModeSpecificWal");
	translatePage(ModeSpecificWallpaper_xml, document, "name");
	translateElements(ModeSpecificWallpaper_xml, document, "img", "title", "name");
	
	WallPaper_XML = getLanguageXmlDoc("configuration", "WallPaper");
	translatePage(WallPaper_XML, document, "name");

	modeSpecificWallpaperEventBind();
	if((getCookie("uploadModeSpecificImageFileCookie"))  != null)
	{
		var query,responseCode;	
		query = "SetModeSpecificWallpaperImage?";
		query += "dataType"+ valFsp + FILE_PATH;
		query += apendWebFsp()+"file"+ valFsp + getCookie("uploadModeSpecificImageFileCookie");
		responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, ModeSpecificWallpaper);
		if(responseCode == 0)
		{
			clearCookie("uploadModeSpecificImageFileCookie");
			clearBlockState();	
		}
		
	}
	getModeSpecificWallpaper();
	$('.tooltip').tooltipster();
	
	$("#WP_ENTRY_SET_WP").focus();
	return false;
}

window.onresize = function onresize(){
	mainPageDivSet();
};

$(document).ready(function()
{	
	mainPageDivSet();
	initModeSpecificWallpaper();
});
