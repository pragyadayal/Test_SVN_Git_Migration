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
//Created By   : Hardik patel
//File         : WallPaper.js
//Description  :
//
/////////////////////////////////////////////////////////////////////////////
var Common_XML 		= null;
var WallPaper_XML = null;
var prevMsgRowId;
var selectedWallIndex;
var selectedFileName = "";

var wallPaperList =
{
	"dataType":"",
	"index":"",
	"file":"",
	"WP_1":"",
	"WP_2":"",
	"WP_3":"",
	"WP_4":"",
	"WP_5":"",
	"WP_6":"",
	"WP_7":"",
	"WP_8":"",
	"WP_9":"",
	"WP_10":"",
	"WP_11":"",
	"WP_12":"",
	"currentWallPaper":""
};

function getImageWallPaper()
{
	var query = "GetWallpaperImage?";	
	ajaxrequest("post", "submit" , query, updateDataToObjList, wallPaperList);
	setImageWallPaper();
}

function setImageWallPaper()
{
	setElemValue("#WP_1", 				wallPaperList.WP_1);
	setElemValue("#WP_2", 				wallPaperList.WP_2);
	setElemValue("#WP_3", 				wallPaperList.WP_3);
	setElemValue("#WP_4", 				wallPaperList.WP_4);
	setElemValue("#WP_5", 				wallPaperList.WP_5);
	setElemValue("#WP_6", 				wallPaperList.WP_6);
	setElemValue("#WP_7", 				wallPaperList.WP_7);
	setElemValue("#WP_8", 				wallPaperList.WP_8);
	setElemValue("#WP_9", 				wallPaperList.WP_9);
	setElemValue("#WP_10", 				wallPaperList.WP_10);
	setElemValue("#WP_11", 				wallPaperList.WP_11);
	setElemValue("#WP_12", 				wallPaperList.WP_12);
	setElemValue("#currentWallPaper", 	wallPaperList.currentWallPaper);
	
	showElem("#setCurrentWallPaper" +(wallPaperList["currentWallPaper"]));
	setLabel("#setCurrentWallPapertxt" +(wallPaperList["currentWallPaper"]), "Current Wallpaper");
	
	fillData();
}

function displayValidateMsg(msgRowId, validationMsgStr)
{
	clearMsg();
	prevMsgRowId=msgRowId;
	setLabel("#invalidMsg"+ msgRowId, validationMsgStr);
	$("#invalidMsg"+ msgRowId).attr({"class":"msg_str_fail"});
	$("#invalidMsg"+ msgRowId).css("margin-left", "0px");
	setTimeout(clearMsg, 5000);
}

function clearMsg()
{
	setLabel("#invalidMsg"+prevMsgRowId, "");
}

function validateFile(FileName,index)
{
	var imageName;
	var msgRowId="_"+index;
	
	imageName = FileName;
	
	var subStr = imageName.substring(imageName.lastIndexOf("."));
	
	if( (subStr!=".png") && (subStr!=".PNG") && (subStr!=".jpg") && (subStr!=".JPG") && (subStr!=".jpeg") && (subStr!=".JPEG") && (subStr!=".bmp") && (subStr!=".BMP") )
	{
		displayValidateMsg(msgRowId, translateNode(WallPaper_XML, "supportImgFileStr"));
		selectedFileName="";
		return false;
	}
	var mainStr;
	mainStr=imageName.slice(0,imageName.lastIndexOf("."));
	mainStr = mainStr.split(/[\\\/]/).pop();
	
	if(ValidateImageName(mainStr) == false)
	{
		displayValidateMsg(msgRowId, translateNode(WallPaper_XML, "supportCharacterStr"));
		selectedFileName="";
		return false;
	} 

	var	fileSize=document.forms[index-1].inputwallPaperFormId.files[0].size;
	
	if(fileSize)
	{
		if((fileSize/1024) > 512)
		{
			displayValidateMsg(msgRowId, translateNode(WallPaper_XML, "sizeValidationStr"));
			selectedFileName="";
			return false;
		}
		
		selectedFileName = mainStr + subStr;
		return true;
	}
	else
	{
		displayValidateMsg(msgRowId, translateNode(WallPaper_XML, "browsererrorMsg"));
		selectedFileName="";
		return false;
	}
}	
	
function startUploadImage(imageId)
{
	var	webStrUpldFileName=selectedFileName;
	
	setCookie("uploadImageFileCookie", webStrUpldFileName);
	setCookie("selectedWallPaperIndexCookie", imageId);
	
	//Now send file
	var F;
	F = document.forms[imageId-1]; 
	F.method = "post";
	var pageName = "/Wallpaper.html?";
	F.action = "../../goform/upldForm?"+pageName;
	F.submit();
}

function fillData()
{
	var idx;
	for(idx=1;idx<=12;idx++)
	{
		$("#img"+idx).attr("src", "/Images/WallpaperLink/"+ wallPaperList["WP_"+ idx] + "?" + new Date().getTime());
	}	
		
}

function handleWallPaperControls(e)
{
	var id = $(this).attr("id");
	var element = GetElementFormID(id);
	var index = GetIndexFromID(id);
	var pathStr;
	
	if(e.type === "change")
	{
		switch(id)
		{
			case "inputwallPaperFormId":
			{
				 var F=document.forms[selectedWallIndex-1];
				 selectedFileName = F.inputwallPaperFormId.value;
				 selectedFileName = selectedFileName.split(/[\\\/]/).pop();
				 setElemValue("#image-filepath"+selectedWallIndex, selectedFileName);
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
		switch(id)
		{
			case "uploadImage"+index:
			{
				var msgRowId="_"+index;
				var F=document.forms[index-1];
				var F_FILE = F.inputwallPaperFormId;
	
				if(F_FILE.value.length == 0)
				{
					displayValidateMsg(msgRowId, translateNode(WallPaper_XML, "slectFileStr"));
					selectedFileName="";
					return false;
				}
				if(validateFile((F_FILE.value.substring(F_FILE.value.lastIndexOf('/')+1)), index)== false)
				{
					if(validateFile((F_FILE.value.substring(F_FILE.value.lastIndexOf('\\')+1)),index) == false)
					{
						return false;
					}
				}
				setBlockState(translateNode(Common_XML,"uploadingMsg"));
				startUploadImage(index);
			}
			break;
			
			case "image-filepath"+index:
			case "wallPaperBrowseBtn"+index:
				{
					selectedWallIndex = index;
					var F=document.forms[index-1];
					F.inputwallPaperFormId.click();
				}break;
			
			default:
			 break;	
		}
	}
}

function wallpaperEventBind()
{
	/*********************************************/
	NOTE:	/*   Always Register this event first For trime 
			         All Input field.*/		
	StopPastFronInputTextField();    
	$("input:text").change(treamAllInputTextField);
	/*******************************************/
	var container 			= $("#WallPaper");
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
		registerHandlers($(img_elements[index]), handleWallPaperControls);
	}
	
	for(var index = 0; index < input_elements.length; index++)
	{
		registerHandlers($(input_elements[index]), handleWallPaperControls);
	}
	
	for(var index = 0; index < td_elements.length; index++)
	{
		registerHandlers($(td_elements[index]), handleWallPaperControls);
	}
	
	for(var index = 0; index < li_elements.length; index++)
	{
		registerHandlers($(li_elements[index]), handleWallPaperControls);
	}
	
	registerHandlersArray(["wallPaperBrowseBtn1" , "wallPaperBrowseBtn2" , 
	"wallPaperBrowseBtn3" ,"wallPaperBrowseBtn4" ,"wallPaperBrowseBtn5" ,
	"wallPaperBrowseBtn6" ,"wallPaperBrowseBtn7" ,"wallPaperBrowseBtn8" ,
	"wallPaperBrowseBtn9" ,"wallPaperBrowseBtn10" ,"wallPaperBrowseBtn11" ,"wallPaperBrowseBtn12"],handleWallPaperControls);	
}

function InitWallPaper()
{
	Common_XML = getLanguageXmlDoc("configuration", "MxCommon");
	translatePage(Common_XML,document,"name");

	WallPaper_XML = getLanguageXmlDoc("configuration", "WallPaper");
	translatePage(WallPaper_XML, document, "name");
	translateElements(WallPaper_XML, document, "img", "title", "name");

	wallpaperEventBind();
	$('.tooltip').tooltipster();
	
	hideElements(["#setCurrentWallPaper1", "#setCurrentWallPaper2", "#setCurrentWallPaper3", "#setCurrentWallPaper4",
	"#setCurrentWallPaper5","#setCurrentWallPaper6","#setCurrentWallPaper7","#setCurrentWallPaper8","#setCurrentWallPaper9","#setCurrentWallPaper10",
	"#setCurrentWallPaper11","#setCurrentWallPaper12"]);

	if((getCookie("uploadImageFileCookie"))  != null)
	{
		var query,responseCode;
			
		clearBlockState();
		query = "SetWallpaperImage?";
		query += "index"+ valFsp + getCookie("selectedWallPaperIndexCookie");
		query += apendWebFsp()+"file"+ valFsp + getCookie("uploadImageFileCookie");
		responseCode = ajaxrequest("post", "submit", query, updateDataToObjList, wallPaperList);
		clearCookie("selectedWallPaperIndexCookie");
		clearCookie("uploadImageFileCookie");
	}
	getImageWallPaper();
}

window.onresize = function onresize(){
	mainPageDivSet();
};

$(document).ready(function()
{	
	mainPageDivSet();
	InitWallPaper();
});
