//////////////////////////////////////////////////////////////////////////
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
//File         : ViewUserConfig.js
//Description  : This file contains javascript functions for Identification Server setting
//
/////////////////////////////////////////////////////////////////////////////
var ViewUserProfile_xml	= null;
var Common_XML		 = null;
var success_img		 = "";
var strtPortRange 	 = 1024;
var endPortRange 	 = 65535;
var portNum;
var activeRange;
var maxRowCnt 		 = 24;
var displayRowNo	 = 1;
var maxUser		     = 25;
var minvalue;
var strtYearRng		  = 2009;
var endYearRng		  = 2037;
var strtUserHoliSchNo = 1;
var endUserHoliSchNo  = 30;
var strtAbDayCount	  = 1;
var endAbDayCount	  = 365;
var strtVisitorRange  = 0;
var endVisitorRange   = 99999;
var strtCardRange     = 1;
var endCardRange      = 18446744073709551615;
var PanelDoor	      = 1;
var DirectDoor	      = 0;

// Web Configuration 
var ViewUserProfileCnfg =
{	
	activeUser 			: new Array(maxUser)   ,
	userId 				: new Array(maxUser)   ,
	refencId 			: new Array(maxUser)   ,
	userName 			: new Array(maxUser)   ,
	vip 				: new Array(maxUser)   ,
	rstrctAcc 			: new Array(maxUser)   ,
	byPassBioMetric 	: new Array(maxUser)   ,
	bioMetricTemp		: new Array(maxUser)   ,
	cardOne 			: new Array(maxUser)   ,
	cardTwo 			: new Array(maxUser)   ,
	userPin 			: new Array(maxUser)   ,
	vistorIdx 			: new Array(maxUser)   ,
	dateActive 			: new Array(maxUser)   ,
	date 				: new Array(maxUser)   ,
	holidaySch 			: new Array(maxUser)   ,
	abstyRuleActive 	: new Array(maxUser)   ,
	dayCount 			: new Array(maxUser)   ,
	templateUpdateTime  : new Array(maxUser)   ,
	faceImages			: new Array(maxUser)   ,
};

// Get Settings Request
function getViewUserProfile()
{	
	var query = "GetViewUserProfile?";
	query += "minvalue"+valFsp+minvalue;
	ajaxrequest("post", "submit" , query, updateDataToObjList, ViewUserProfileCnfg);
	setViewUserProfile();
	$("#activeUser1").focus();
	return true;
}


function ValidateNumber(preNumValue, minValue, maxValue, str ,id, Idx) 
{

    if(isFieldBlank(preNumValue, translateNode(ViewUserProfile_xml,str), ViewUserProfileCnfg, id) == false)
    {
        return false;
    }
    
     if (ValidateNumericRange(preNumValue, minValue, maxValue) == false) 
    {
        // ViewUserProfileCnfg.id[Idx-1] = preNumValue;
        return false;
    }
    return true;
}

function ValidateNumericRange(preNumValue, minValue, maxValue) 
{
    var nonNumeric = /\D/;
    if ((nonNumeric.test(preNumValue)) == true) 
    {
	   displayValidationMessage(error_img ,translateNode(ViewUserProfile_xml,"numricAllowed"), false);
       return false;
    }

    if (preNumValue >= minValue && preNumValue <= maxValue) 
    {
        return true;
    }
	var message = translateNode(ViewUserProfile_xml,"validInputRange") + minValue + " to " + maxValue + "." ;
    displayValidationMessage(error_img ,message, false);
    
    return false;
}




//check valid data during submit
function checkDate(indexVal) 
{
	var testDate;
    var tempstr =  ViewUserProfileCnfg.date[indexVal-1];
    var dateCheck = tempstr.split("-");
	// var dateCheck = document.getElementById(elementId + indexVal).value.split("-");
    //It check for valid forward date.
    //NOTE: We use year 2008 to allow feb 29
    //NOTE: We use year 2007 to not allow feb 29
    //NOTE: Subtract 1 from month because in javascript month argument start from 0 to 11
    if (dateCheck[1] == 2) 
    {
        if ((parseInt(dateCheck[2]) % 4) == 0) 
        {
            testDate = new Date(2008, dateCheck[1] - 1, dateCheck[0]);

        }
        else
            testDate = new Date(2007, dateCheck[1] - 1, dateCheck[0]);
    }
    else
        testDate = new Date(2007, dateCheck[1] - 1, dateCheck[0]);

    if (testDate.getDate() == dateCheck[0]) 
    {
        if (testDate.getMonth() != dateCheck[1] - 1) 
        {
        	displayValidationMessage(error_img ,translateNode(ViewUserProfile_xml,"validDate"), false);
            return false;
        }
    }
    else 
    {
     	displayValidationMessage(error_img ,translateNode(ViewUserProfile_xml,"validDate"), false);
        return false;
    }
    if(validateDate(indexVal, dateCheck[0], dateCheck[1], dateCheck[2] ,strtYearRng, endYearRng) == false)
    {
    	return false;
    }
    return true;
}


function validateDate(indexVal, value1, value2, value3, strtYearRng, endYearRng) 
{
    var arryIndx;
    var tempstr =  ViewUserProfileCnfg.date[indexVal-1];
	var dateArry = tempstr.split("-");
    var numericExp = /[^0-9-]/;
    if(isFieldBlank(ViewUserProfileCnfg.date[indexVal-1], translateNode(ViewUserProfile_xml,"dateStr"), ViewUserProfileCnfg,"date") == false)
	{
		return false;
	}
    if (numericExp.test(tempstr) == true) 
    {
        displayValidationMessage(error_img ,translateNode(ViewUserProfile_xml,"numricAllowed"), false);
        return false;
    }
    for (arryIndx = 0; arryIndx < 3; arryIndx++) 
    {
        if (dateArry[arryIndx] != undefined) 
        {
            if (arryIndx == 0 && (dateArry[arryIndx] > 31 || dateArry[arryIndx] == 0)) 
            {
              	displayValidationMessage(error_img ,translateNode(ViewUserProfile_xml,"validDateRange"), false);
                return false;
            }
            if (arryIndx == 1 && (dateArry[arryIndx] > 12 || dateArry[arryIndx] == 0)) 
            {
               	displayValidationMessage(error_img ,translateNode(ViewUserProfile_xml,"validMonthRange"), false);
                return false;
            }
            if (arryIndx == 2 && (dateArry[arryIndx] > endYearRng || dateArry[arryIndx] < strtYearRng)) 
            {
               	displayValidationMessage(error_img ,translateNode(ViewUserProfile_xml,"invalidYear"), false);
                return false;
            }
        }
    }
}



function checkAllValidateField()
{
    var userId,cnt;
    for (userId = 1; userId <=  (maxUserPerPage); userId++)
    {
		if (checkDate(userId) == false)
        return false;
	    //if checkbox is selected
	    if(ViewUserProfileCnfg.activeUser[userId-1]==true)
	    {
	    	
	    	//check for Access group name
		    if(isFieldBlank(ViewUserProfileCnfg.userId[userId-1], translateNode(ViewUserProfile_xml,"userIdStr"), ViewUserProfileCnfg,"userId") == false)
			{
			  	$("#userId"+userId).focus();
				return false;
			}
	
		    if(isFieldBlank(ViewUserProfileCnfg.refencId[userId-1], translateNode(ViewUserProfile_xml,"refIdStr"), ViewUserProfileCnfg,"refencId") == false)
			{
			    $("#refencId"+userId).focus();
				return false;
			}
		
		    if(isFieldBlank(ViewUserProfileCnfg.userName[userId-1], translateNode(ViewUserProfile_xml,"userName"), ViewUserProfileCnfg,"userName") == false)
			{
			    $("#userName"+userId).focus();
				return false;
			}
				    
		     if((ViewUserProfileCnfg.abstyRuleActive[userId-1]== "1") )
		    {	
		    	if(isFieldBlank(ViewUserProfileCnfg.dayCount[userId-1], translateNode(ViewUserProfile_xml,"dayCountStr"), ViewUserProfileCnfg,"dayCount") == false)
		    	{
		    	    $("#dayCount"+userId).focus();
				    return false;
			    }
			    else
			    {
			    	if((ValidateNumber(ViewUserProfileCnfg.dayCount[userId-1],strtAbDayCount,endAbDayCount, "Day Count", "dayCount", userId))== false)
			    	{
			    		ViewUserProfileCnfg.dayCount[userId-1] = "";
			    		return false;
			    	}
		   	    }
		    }
		    
		    if(ValidateNumber(ViewUserProfileCnfg.holidaySch[userId-1],strtUserHoliSchNo,endUserHoliSchNo, "Holiday Schedule No", "holidaySch", userId) == false)
		    {
		    	return false;
		    }
		    if(ValidateNumber(ViewUserProfileCnfg.vistorIdx[userId-1],strtVisitorRange,endVisitorRange, "Visitor Index", "vistorIdx", userId) == false)
		    {
		    	return false;
		    }
		    
		    if((isLiesBetween(strtCardRange,ViewUserProfileCnfg.cardOne[userId-1], endCardRange) == false) && (ViewUserProfileCnfg.cardOne[userId-1] != ""))
		    {
		    	displayValidationMessage(error_img ,translateNode(ViewUserProfile_xml,"enterValidCardRange"), false);
		    	return false;
		    }
 			if((isLiesBetween(strtCardRange,ViewUserProfileCnfg.cardTwo[userId-1], endCardRange) == false)&& (ViewUserProfileCnfg.cardOne[userId-1] != ""))
 			{
 				displayValidationMessage(error_img ,translateNode(ViewUserProfile_xml,"enterValidCardRange"), false);
 				return false;
 			}
	    }
	}
    return true;		
}


// Set Default Settings Request
function setDefaultUserProfile()
{
	var query = "SetdfltUsrProfile?";
	query += "minvalue"+valFsp+minvalue;
	var responce =ajaxrequest("post", "submit" , query, updateDataToObjList, ViewUserProfileCnfg);
	if(responce == 0)
	{
		displayValidationMessage(success_img ,translateNode(Common_XML,"defaultSucess"), true);
		setViewUserProfile();
	}
	
	return true;
}

// Set Parameters of Web page
function setViewUserProfile()
{
	for(index=1;index<=25;index++)
	{
		setLabel("#Idx"+index+"",	((minvalue-1)+index)	);
		ViewUserProfileCnfg.activeUser[index-1] == "1"?checkElem("#activeUser"+index+""):uncheckElem("#activeUser"+index+"");
		setElemValue("#userId"+index+"",		ViewUserProfileCnfg.userId[index-1]);
		setElemValue("#refencId"+index+"",		ViewUserProfileCnfg.refencId[index-1]);
		setElemValue("#userName"+index+"",		ViewUserProfileCnfg.userName[index-1]);
		ViewUserProfileCnfg.vip[index-1] == "1"?checkElem("#vip"+index+""):uncheckElem("#vip"+index+"");
		ViewUserProfileCnfg.rstrctAcc[index-1] == "1"?checkElem("#rstrctAcc"+index+""):uncheckElem("#rstrctAcc"+index+"");
		ViewUserProfileCnfg.byPassBioMetric[index-1] == "1"?checkElem("#byPassBioMetric"+index+""):uncheckElem("#byPassBioMetric"+index+"");
		setElemValue("#bioMetricTemp"+index+"",		ViewUserProfileCnfg.bioMetricTemp[index-1]);
		if(ViewUserProfileCnfg.cardOne[index-1] == 0)
		{
			ViewUserProfileCnfg.cardOne[index-1] = "";
		}
		setElemValue("#cardOne"+index+"",		ViewUserProfileCnfg.cardOne[index-1]);
		if(ViewUserProfileCnfg.cardTwo[index-1] == 0)
		{
			ViewUserProfileCnfg.cardTwo[index-1] = "";
		}
		setElemValue("#cardTwo"+index+"",		ViewUserProfileCnfg.cardTwo[index-1]);
		setElemValue("#userPin"+index+"",		ViewUserProfileCnfg.userPin[index-1]);
		setElemValue("#vistorIdx"+index+"",		ViewUserProfileCnfg.vistorIdx[index-1]);
		
		ViewUserProfileCnfg.dateActive[index-1] == "1"?checkElem("#dateActive"+index+""):uncheckElem("#dateActive"+index+"");
			(ViewUserProfileCnfg.dateActive[index -1] == 1) ?  enableElem("#date"+(index)): disableElem("#date"+(index));
		setElemValue("#date"+index+"",			ViewUserProfileCnfg.date[index-1]);
		setElemValue("#holidaySch"+index+"",	ViewUserProfileCnfg.holidaySch[index-1]);
		ViewUserProfileCnfg.abstyRuleActive[index-1] == "1"?checkElem("#abstyRuleActive"+index+""):uncheckElem("#abstyRuleActive"+index+"");
			(ViewUserProfileCnfg.abstyRuleActive[index -1] == 1) ?  enableElem("#dayCount"+(index)): disableElem("#dayCount"+(index));
		setElemValue("#dayCount"+index+"",		ViewUserProfileCnfg.dayCount[index-1]);
		
		var tmpltStr = "";//ViewUserProfileCnfg.faceImages[index-1];
		if(ViewUserProfileCnfg.templateUpdateTime[index-1] != "" &&
				ViewUserProfileCnfg.templateUpdateTime[index-1] != undefined)
		{
			//tmpltStr += " ("+ViewUserProfileCnfg.templateUpdateTime[index-1]+")";
			tmpltStr = ViewUserProfileCnfg.templateUpdateTime[index-1];
		}
		setElemValue("#faceImage"+index+"",	tmpltStr);
	}
	UserEnbl();
	return false;
}

// Set/Save Settings Request
function saveViewUserProfile()
{
	
	var activeUserSaveValue			= new Array();
	var userIdSaveValue				= new Array();
	var refencIdSaveValue			= new Array();
	var userNameSaveValue			= new Array();
	var vipSaveValue				= new Array();
	var rstrctAccSaveValue 			= new Array();
	var byPassBioMetricSaveValue 	= new Array();
	var cardOneSaveValue			= new Array();
	var cardTwoSaveValue			= new Array();
	var userPinSaveValue			= new Array();
	var vistorIdxSaveValue			= new Array();
	var dateActiveSaveValue 		= new Array();
	var dateSaveValue 				= new Array();
	var holidaySchSaveValue 		= new Array();
	var abstyRuleActiveSaveValue	= new Array();
	var dayCountSaveValue 			= new Array();
	
	var query = "";
	query = "SetViewUserConfig?";	
	query += "minvalue"+valFsp+minvalue;
		
	//Table Element
	for(idx=0; idx < 25; idx++)
	{
		activeUserSaveValue.push(ViewUserProfileCnfg.activeUser[idx]);
		userIdSaveValue.push(ViewUserProfileCnfg.userId[idx]);
		refencIdSaveValue.push(ViewUserProfileCnfg.refencId[idx]);
		userNameSaveValue.push(ViewUserProfileCnfg.userName[idx]);
		vipSaveValue.push(ViewUserProfileCnfg.vip[idx]);
		rstrctAccSaveValue.push(ViewUserProfileCnfg.rstrctAcc[idx]);
		byPassBioMetricSaveValue.push(ViewUserProfileCnfg.byPassBioMetric[idx]);
		cardOneSaveValue.push(ViewUserProfileCnfg.cardOne[idx]);
		cardTwoSaveValue.push(ViewUserProfileCnfg.cardTwo[idx]);
		userPinSaveValue.push(ViewUserProfileCnfg.userPin[idx]);
		vistorIdxSaveValue.push(ViewUserProfileCnfg.vistorIdx[idx]);
		dateActiveSaveValue.push(ViewUserProfileCnfg.dateActive[idx]);
		dateSaveValue.push(ViewUserProfileCnfg.date[idx]);
		holidaySchSaveValue.push(ViewUserProfileCnfg.holidaySch[idx]);
		abstyRuleActiveSaveValue.push(ViewUserProfileCnfg.abstyRuleActive[idx]);
		dayCountSaveValue.push(ViewUserProfileCnfg.dayCount[idx]);
	
	}
	 
	var sepFsp=String.fromCharCode(webSepertorFsp).toString();
	
	query += apendWebFsp()+"activeUser"+ valFsp + activeUserSaveValue.join(sepFsp)+sepFsp;
	query += apendWebFsp()+"userId"+ valFsp + userIdSaveValue.join(sepFsp)+sepFsp;
	query += apendWebFsp()+"refencId"+ valFsp + refencIdSaveValue.join(sepFsp)+sepFsp;
	query += apendWebFsp()+"userName"+ valFsp + userNameSaveValue.join(sepFsp)+sepFsp;
	query += apendWebFsp()+"vip"+ valFsp + vipSaveValue.join(sepFsp)+sepFsp;
	query += apendWebFsp()+"rstrctAcc"+ valFsp + rstrctAccSaveValue.join(sepFsp)+sepFsp;
	query += apendWebFsp()+"byPassBioMetric"+ valFsp + byPassBioMetricSaveValue.join(sepFsp)+sepFsp;
	query += apendWebFsp()+"cardOne"+ valFsp + cardOneSaveValue.join(sepFsp)+sepFsp;
	query += apendWebFsp()+"cardTwo"+ valFsp + cardTwoSaveValue.join(sepFsp)+sepFsp;
	query += apendWebFsp()+"userPin"+ valFsp + userPinSaveValue.join(sepFsp)+sepFsp;
	query += apendWebFsp()+"vistorIdx"+ valFsp + vistorIdxSaveValue.join(sepFsp)+sepFsp;
	query += apendWebFsp()+"dateActive"+ valFsp + dateActiveSaveValue.join(sepFsp)+sepFsp;
	query += apendWebFsp()+"date"+ valFsp + dateSaveValue.join(sepFsp)+sepFsp;
	query += apendWebFsp()+"holidaySch"+ valFsp + holidaySchSaveValue.join(sepFsp)+sepFsp;
	query += apendWebFsp()+"abstyRuleActive"+ valFsp + abstyRuleActiveSaveValue.join(sepFsp)+sepFsp;
	query += apendWebFsp()+"dayCount"+ valFsp + dayCountSaveValue.join(sepFsp)+sepFsp;

	var responce = ajaxrequest("post", "submit", query);
	if(responce == "0")
	{	
		displayValidationMessage(success_img ,translateNode(Common_XML,"saveSuccess"), true);
		setViewUserProfile();
	}
	return responce;


}

function UserEnbl()
{
 	for(var index=1; index<= 25; index++)
 	{
	    if (ViewUserProfileCnfg.activeUser[index-1] == "1")
	    {
	    	enableElements(["#userId"+index,"#refencId"+index,"#userName"+index,"#vip"+index,"#rstrctAcc"+index,
				 				"#byPassBioMetric"+index,"#cardOne"+index,"#cardTwo"+index, "#userPin"+index, ,"#vistorIdx"+index ,"#dateActive"+index ,"#holidaySch"+index, "#abstyRuleActive"+index]);
		}
		else
		{	
		   disableElements(["#userId"+index,"#refencId"+index,"#userName"+index,"#vip"+index,"#rstrctAcc"+index,
				 				"#byPassBioMetric"+index,"#bioMetricTemp"+index,"#cardOne"+index,"#cardTwo"+index, "#userPin"+index, ,"#vistorIdx"+index ,"#dateActive"+index ,"#holidaySch"+index, "#abstyRuleActive"+index]);
				
	    }
   }
   return true;
}

function nextButtonCalcution()
{
	if(displayRowNo < maxRowCnt-1)
	{
		displayRowNo++;
	}
	else
	{
		displayRowNo =1;
	}
}

function prevButtonCalcution()
{
	if(displayRowNo == 1)
	{
		displayRowNo = maxRowCnt-1 ;
	}
	else
	{
		displayRowNo--;
	}
}


// Event Handling
function handleViewUserProfileEvent(e)
{
	var id = $(this).attr("id");
	var element = GetElementFormID(id);
	var index = GetIndexFromID(id);

	if(e.type === "click")
	{
		switch(element)
		{
			default: break;
			case "range":
			{			
				if(index == 15000)
				{
					activeRange = "range1to5000";				
					displayRowNo = 1;
					activeTab(activeRange);				//active main range
					subDivisionRow(1, 5000);			//create sub range
					minvalue = 1;
					getViewUserProfile();			
					boldSubTab("subRangeRow1");			//make bold sub range 
				}
				else if(index == 500110000)
				{
					activeRange = "range5001to10000";
					displayRowNo = 1;
					activeTab(activeRange);
					subDivisionRow(5001, 10000);
					minvalue = 5001;
					getViewUserProfile();
					boldSubTab("subRangeRow1");
				}
				else if(index == 1000115000)
				{
					activeRange = "range10001to15000";
					displayRowNo = 1;
					activeTab(activeRange);
					subDivisionRow(10001, 15000);
					minvalue = 10001;
					getViewUserProfile();
					boldSubTab("subRangeRow1");
				}
				else if(index == 1500120000)
				{
					activeRange = "range15001to20000";
					displayRowNo = 1;
					activeTab(activeRange);
					subDivisionRow(15001, 20000);
					minvalue = 15001;
					getViewUserProfile();
					boldSubTab("subRangeRow1");
				}
				else if(index == 2000125000)
				{
					activeRange = "range20001to25000";
					displayRowNo = 1;
					activeTab(activeRange);
					subDivisionRow(20001, 25000);
					minvalue = 20001;
					getViewUserProfile();
					boldSubTab("subRangeRow1");
				}
				else if(index == 2500130000)
				{
					activeRange = "range25001to30000";
					displayRowNo = 1;
					activeTab(activeRange);
					subDivisionRow(25001, 30000);
					minvalue = 25001;
					getViewUserProfile();
					boldSubTab("subRangeRow1");
				}
				else if(index == 3000135000)
				{
					activeRange = "range30001to35000";
					displayRowNo = 1;
					activeTab(activeRange);
					subDivisionRow(30001, 35000);
					minvalue = 30001;
					getViewUserProfile();
					boldSubTab("subRangeRow1");
				}
				else if(index == 3500140000)
				{
					activeRange = "range35001to40000";
					displayRowNo = 1;
					activeTab(activeRange);
					subDivisionRow(35001, 40000);
					minvalue = 35001;
					getViewUserProfile();
					boldSubTab("subRangeRow1");
				}
				else if(index == 4000145000)
				{
					activeRange = "range40001to45000";
					displayRowNo = 1;
					activeTab(activeRange);
					subDivisionRow(40001, 45000);
					minvalue = 40001;
					getViewUserProfile();
					boldSubTab("subRangeRow1");
				}
				else if(index == 4500150000)
				{
					activeRange = "range45001to50000";
					displayRowNo = 1;
					activeTab(activeRange);
					subDivisionRow(45001, 50000);
					minvalue = 45001;
					getViewUserProfile();
					boldSubTab("subRangeRow1");
				}

			}
			break;
		
			case "nextbtn":  												//next button
			{
				if(activeRange == "range1to5000")
				{
					nextButtonCalcution();
					subDivisionRow(1, 5000);
					removeBoldSubtab();
				}
				else if(activeRange == "range5001to10000")
				{
					nextButtonCalcution();
					subDivisionRow(5001, 10000);
					removeBoldSubtab();
				}
				else if(activeRange == "range10001to15000")
				{
					nextButtonCalcution();
					subDivisionRow(10001, 15000);
					removeBoldSubtab();
				}
				else if(activeRange == "range15001to20000")
				{
					nextButtonCalcution();
					subDivisionRow(15001, 20000);
					removeBoldSubtab();
				}
				else if(activeRange == "range20001to25000")
				{
					nextButtonCalcution();
					subDivisionRow(20001, 25000);
					removeBoldSubtab();
				}
				else if(activeRange == "range30001to35000")
				{
					nextButtonCalcution();
					subDivisionRow(30001, 35000);
					removeBoldSubtab();
				}
				else if(activeRange == "range25001to30000")
				{
					nextButtonCalcution();
					subDivisionRow(25001,30000);
					removeBoldSubtab();
				}
				else if(activeRange == "range35001to40000")
				{
					nextButtonCalcution();
					subDivisionRow(35001, 40000);
					removeBoldSubtab();
				}
				else if(activeRange == "range40001to45000")
				{
					nextButtonCalcution();
					subDivisionRow(40001, 45000);
					removeBoldSubtab();
				}
				else if(activeRange == "range45001to50000")
				{
					nextButtonCalcution();
					subDivisionRow(45001, 50000);
					removeBoldSubtab();
				}
				
			}break;
		
			case "firstbtn": 								//prev button
			{
				if(activeRange == "range1to5000")
				{
					prevButtonCalcution();
					subDivisionRow(1, 5000);
					removeBoldSubtab();
				}
				else if(activeRange == "range5001to10000")
				{
					prevButtonCalcution();
					subDivisionRow(5001, 10000);
					removeBoldSubtab();
				}
				else if(activeRange == "range10001to15000")
				{
					prevButtonCalcution();
					subDivisionRow(10001, 15000);
					removeBoldSubtab();
				}
				else if(activeRange == "range15001to20000")
				{
					prevButtonCalcution();
					subDivisionRow(15001, 20000);
					removeBoldSubtab();
				}
				else if(activeRange == "range20001to25000")
				{
					prevButtonCalcution();
					subDivisionRow(20001, 25000);
					removeBoldSubtab();
				}
				else if(activeRange == "range30001to35000")
				{
					prevButtonCalcution();
					subDivisionRow(30001, 35000);
					removeBoldSubtab();
				}
				else if(activeRange == "range25001to30000")
				{
					prevButtonCalcution();
					subDivisionRow(25001, 30000);
					removeBoldSubtab();
				}
				else if(activeRange == "range35001to40000")
				{
					prevButtonCalcution();
					subDivisionRow(35001, 40000);
					removeBoldSubtab();
				}
				else if(activeRange == "range40001to45000")
				{
					prevButtonCalcution();
					subDivisionRow(40001, 45000);
					removeBoldSubtab();
				}
				else if(activeRange == "range45001to50000")
				{
					prevButtonCalcution();
					subDivisionRow(45001, 50000);
					removeBoldSubtab();
				}
					
			}break;
			
			
			case "submit":
			{
				
				if(checkAllValidateField() == true)
				{
					saveViewUserProfile();
				}
			}break;
			
			case "default":
			{
				if(confirm("Do you want to set this page parameters to default settings?")==true)
				{
					setDefaultUserProfile();
					getViewUserProfile();	
				}
				
			}break;
			
			case "subRangeRow":
			{
				if(activeRange == "range1to5000")
				{
					var minvalueofRange = 1;
					minvalue = (((displayRowNo -1)* (25*9))+ minvalueofRange)+((index-1)*25 );
					getViewUserProfile();
					boldSubTab("subRangeRow"+index);
				}
				else if(activeRange == "range5001to10000")
				{
					var minvalueofRange = 5001;
					minvalue = (((displayRowNo -1)* (25*9))+ minvalueofRange)+((index-1)*25 );
					getViewUserProfile();
					boldSubTab("subRangeRow"+index);
				}
				else if(activeRange == "range10001to15000")
				{
					var minvalueofRange = 10001;
					minvalue = (((displayRowNo -1)* (25*9))+ minvalueofRange)+((index-1)*25 );
					getViewUserProfile();
					boldSubTab("subRangeRow"+index);
				}
				else if(activeRange == "range15001to20000")
				{
					var minvalueofRange = 15001;
					minvalue = (((displayRowNo -1)* (25*9))+ minvalueofRange)+((index-1)*25 );
					getViewUserProfile();
					boldSubTab("subRangeRow"+index);
				}
				else if(activeRange == "range20001to25000")
				{
					var minvalueofRange = 20001;
					minvalue = (((displayRowNo -1)* (25*9))+ minvalueofRange)+((index-1)*25);
					getViewUserProfile();
					boldSubTab("subRangeRow"+index);
				}
				else if(activeRange == "range30001to35000")
				{
					var minvalueofRange = 30001;
					minvalue = (((displayRowNo -1)* (25*9))+ minvalueofRange)+((index-1)*25 );
					getViewUserProfile();
					boldSubTab("subRangeRow"+index);
				}
				else if(activeRange == "range25001to30000")
				{
					var minvalueofRange = 25001;
					minvalue = (((displayRowNo -1)* (25*9))+ minvalueofRange)+((index-1)*25 );
					getViewUserProfile();
					boldSubTab("subRangeRow"+index);
				}
				else if(activeRange == "range35001to40000")
				{
					var minvalueofRange = 35001;
					minvalue = (((displayRowNo -1)* (25*9))+ minvalueofRange)+((index-1)*25);
					getViewUserProfile();
					boldSubTab("subRangeRow"+index);
				}
				else if(activeRange == "range40001to45000")
				{
					var minvalueofRange = 40001;
					minvalue = (((displayRowNo -1)* (25*9))+ minvalueofRange)+((index-1)*25 );
					getViewUserProfile();
					boldSubTab("subRangeRow"+index);
				}
				else if(activeRange == "range45001to50000")
				{
					var minvalueofRange = 45001;
					minvalue = (((displayRowNo -1)* (25*9))+ minvalueofRange)+((index-1)*25 );
					getViewUserProfile();
					boldSubTab("subRangeRow"+index);
				}
			}break;
		}
	}
	if(e.type === "change")
	{
		switch(element)
		{
			
			default: break;
			
			case "activeUser":
			{
				ViewUserProfileCnfg.activeUser[index -1] = isChecked(this)? 1: 0;	
				UserEnbl();
			}break;
			
			case "userId":
			{
				ViewUserProfileCnfg.userId[index -1] = getElemValue(this);
			}break;
			
			case "refencId":
			{
				ViewUserProfileCnfg.refencId[index -1] = getElemValue(this);	
			}break;
			
			case "userName":
			{
				ViewUserProfileCnfg.userName[index -1] = getElemValue(this);	
			}break;
			
			case "vip":
			{
				ViewUserProfileCnfg.vip[index -1] =  isChecked(this)? 1: 0;
			}break;
			
			case "rstrctAcc":
			{
				ViewUserProfileCnfg.rstrctAcc[index -1] = isChecked(this)? 1: 0;	
			}break;
				
			case "byPassBioMetric":
			{
				ViewUserProfileCnfg.byPassBioMetric[index -1] = isChecked(this)? 1: 0;	
			}break;
			
			case "bioMetricTemp":
			{
				ViewUserProfileCnfg.bioMetricTemp[index -1] = getElemValue(this);	
			}break;
			
			case "cardOne":
			{
				ViewUserProfileCnfg.cardOne[index -1] = getElemValue(this);	
			}break;
			
			case "cardTwo":
			{
				ViewUserProfileCnfg.cardTwo[index -1] = getElemValue(this);	
			}break;
			
			case "userPin":
			{
				ViewUserProfileCnfg.userPin[index -1] = getElemValue(this);	
			}break;
			
			case "vistorIdx":
			{
				ViewUserProfileCnfg.vistorIdx[index -1] = getElemValue(this);	
			}break;
			
			case "dateActive":
			{
				ViewUserProfileCnfg.dateActive[index -1] = isChecked(this)? 1: 0;	
				(ViewUserProfileCnfg.dateActive[index -1] == 1) ?  enableElem("#date"+(index)): disableElem("#date"+(index));
				
			}break;
			
			case "date":
			{
				ViewUserProfileCnfg.date[index -1] = getElemValue(this);	
			}break;
			
			case "holidaySch":
			{
				ViewUserProfileCnfg.holidaySch[index -1] = getElemValue(this);	
			}break;
			
			case "abstyRuleActive":
			{
				ViewUserProfileCnfg.abstyRuleActive[index -1] = isChecked(this)? 1: 0;
				(ViewUserProfileCnfg.abstyRuleActive[index -1] == 1) ?  enableElem("#dayCount"+(index)): disableElem("#dayCount"+(index));
			}break;
			
			case "dayCount":
			{
				ViewUserProfileCnfg.dayCount[index -1] = getElemValue(this);	
			}break;
			
		}
	}
}

// Event Binding
function ViewUserProfileEventBind()
{
	/*********************************************/
		NOTE:	/*   Always Register this event first For trime 
			         All Input field.*/		
			    
		$("input:text").change(treamAllInputTextField);
		StopPastFronInputTextField();
	/*******************************************/
	var container 		= $("#ViewUserProfileSetting");
	var sel_elements 	= $("select", $(container));
	var input_elements 	= $("input", $(container));
	var img_elements 	= $("img", $(container));
	var td_elements 	= $("td", $(container));
	var li_elements 	= $("li", $(container));
	
	for(var index = 0; index < sel_elements.length; index++)
	{
		registerHandlers($(sel_elements[index]), handleViewUserProfileEvent);
	}
	
	for(var index = 0; index < img_elements.length; index++)
	{
		registerHandlers($(img_elements[index]),handleViewUserProfileEvent);
	}
	
	for(var index = 0; index < input_elements.length; index++)
	{
		registerHandlers($(input_elements[index]),handleViewUserProfileEvent);
	}
	
	for(var index = 0; index < td_elements.length; index++)
	{
		registerHandlers($(td_elements[index]),handleViewUserProfileEvent);
	}
	
	for(var index = 0; index < li_elements.length; index++)
	{
		registerHandlers($(li_elements[index]), handleViewUserProfileEvent);
	}
	
}

function removeBoldSubtab()
{
	$(".tab li").css("font-weight","initial");
}

function boldSubTab(id)
{
	$(".tab li").css("font-weight","initial");
	$("#"+id).css("font-weight","unset");
	$("#"+id).css("color","#2C3E50");
}


function activeTab(id)
{
	$(".tab li").removeClass("active");	
	$("#"+id).addClass("active");
}

function DisplayFirstTabPage()
{
	$(".tab li").removeClass("active");	
	$("#range1to5000").addClass("active");
	subDivisionRow(1,5000);
	activeRange = 'range1to5000';
	minvalue = 1;
	$("#activeUser1").focus();
	boldSubTab("subRangeRow1");
}

// Initialisation
function InitViewUserProfile()
{
	Common_XML = getLanguageXmlDoc("configuration", "MxCommon");
	translatePage(Common_XML, document, "name");
	
	ViewUserProfile_xml = getLanguageXmlDoc("configuration", "ViewUsrCnfg");
	translatePage(ViewUserProfile_xml, document, "name");
    translateElements(ViewUserProfile_xml, document, "img", "title", "name");

    ViewUserProfileEventBind();
    var showSubmiButton = Base64.decode(getCookie('MxShowSubmitButton'));
    var DoorType = Base64.decode(getCookie('MxDeviceType'));

    if (DoorType == PanelDoor)
    {
        $("#range25001to30000").css("display","none");
        $("#range30001to35000").css("display","none");
        $("#range35001to40000").css("display","none");
        $("#range40001to45000").css("display","none");
        $("#range45001to50000").css("display","none");
    }

    if(showSubmiButton == "1")
    {
        showElem("#submit");
        showElem("#default");
    }
	else
	{
		hideElem("#submit");
		hideElem("#default");
	}
	
	DisplayFirstTabPage();
	// $(".config-leftpart").css("height", ($(window).height()-5)+"px");
	getViewUserProfile();
	for(var eleCnt = 1; eleCnt<=25 ; eleCnt++)
	{
		
		$("#userName"+eleCnt).keypress(filterSmartCardFields);
		$("#cardOne"+eleCnt).keypress(filterNumKey);
		$("#cardTwo"+eleCnt).keypress(filterNumKey);
		$("#userPin"+eleCnt).keypress(filterNumKey);
		$("#holidaySch"+eleCnt).keypress(filterNumKey);
		$("#dayCount"+eleCnt).keypress(filterNumKey);
	}
}

function subDivisionRow(minValue, maxValue)
{
	var rowRangeMin = minValue;
	if(displayRowNo < maxRowCnt)
	{
		rowRangeMin = ((displayRowNo -1)* (25*9))+ minValue;
	}
	
	for(var idx = 1; idx <= 9; idx++)
	{
		setLabel("#subRangeRow"+idx, (rowRangeMin+"-"+((rowRangeMin-1)+25)));
	
		if(rowRangeMin <= (maxValue - 24) )
		{
			rowRangeMin = rowRangeMin + 25;
		}
		else
		{
			setLabel("#subRangeRow"+idx, "");
		}
	}
}

window.onresize = function onresize(){
	mainPageDivSet();
};

$(document).ready(function()
{	
	mainPageDivSet();
	InitViewUserProfile();
}); 
