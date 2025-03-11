/////////////////////////////////////////////////////////////////////////////
//
//   MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//   MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//   MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
//   MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
//   MM       MM   AA       AA       TT      RR  RR       II      XX XX
//   MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//   Company      : Matrix Telecom Pvt. Ltd., Baroda, India.
//   Project      : Access control System
//   Created By   : Sourabh Kahalekar
//   File         : MxJobCosting.c
//   Description  : This file handles all activity related Job Costing feature.
//					Displaying job options as per configuration and performs
//					navigation, search and select actions as per selected options.
//
/////////////////////////////////////////////////////////////////////////////
#define _GNU_SOURCE

//******** Place all include files ***
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>

#include "MxTypedef.h"
#include "MxSysCommonDef.h"
#include "MxSysTimer.h"
#include "MxConfig.h"
#include "MxDcTask.h"
#include "MxLogs.h"
#include "MxSysUtils.h"
#include "MxEventActionHandler.h"
#include "MxJobCosting.h"
#include "MxGuiHandler.h"
#include "MxDcSpecialFunc.h"
#include "MxUserConfig.h"

//******** Extern Variables **********
extern DOOR_ACTIVITY_INFO_t		DcInfo;
extern SYS_ADVANCE_PARA_t		SysAdvPara;
extern SYS_BASIC_PARA_t			SysBasicPara;
extern JCM_CONFIG_t				JcmConfig[MAX_JCM_CONFIG+3]; //+3 for 'None', 'Default' and 'Continue' Job
extern struct tm 				CurrTimeDate;
extern DOOR_MONITOR_t			DcMonitorControl;
extern SPECIAL_FUNCTION_t		SpecialFunctions[MAX_SPECIAL_FUNC];

//******** Defines and Data Types ****
#define CURSOR_POSITION				1
#define BULLET_POSITION				0
#define JOB_CODE_POSITION			2

#define CURSOR_CHAR					0x7F
#define BULLET_CHAR					0x1F
#define BLANK_CHAR					0x20

#define SEARCH_JOB_MSG			"   ENTER JOB CODE :  "
#define PAGE_STR				"Page    "

#define	MAX_JOB_IN_SUB_PAGE			10
#define MAX_RECENT_JOBS				5

typedef enum
{
	JOB_FIRST_PAGE,
	JOB_NEXT_PAGE,
	JOB_PREV_PAGE,
	MAX_JOB_CODE_NAVIGATION,
}JOB_CODE_NAVIGATION_e;

typedef struct
{
	UINT16	jobIndex;
	UINT8	searchOffset;				//location offset in jobcode where search keyword was found
}JOB_SEARCH_LIST_t;

//******** Function Prototypes *******

//******** Global Variables **********
UINT32 				RetainJobCode = DEFAULT_RETAIN_JOB_CODE;
//******** Static Variables **********
static UINT16					lastDispJobIdx;		//index from where next jobs to be loaded
static UINT16					searchedJobCnt;		//total no.of valid jobs loaded in RAM
static UINT16					RecentUserJobsList[MAX_USERS][MAX_RECENT_JOBS];
static JOB_SEARCH_LIST_t		SearchedJobList[MAX_JCM_CONFIG + 3]; //list of valid jobs searched or loaded initially

//******** Function Definitions ******
static BOOL IsJobValid(UINT16 index);
static void UpdateRecentUserJobs(UINT16 userIdx, UINT16 jobIdx);
static void LoadDisplayJobList(JOB_CODE_NAVIGATION_e jobNavigation, GUI_MSG_ID_e msgId);
static void SearchValidJobs(CHARPTR query);
static BOOL SetJobDisplayWaitTimer(UINT8 timer);
static void JobDisplayWaitTimeout(UINT32 dummy);
static void DisplayJobCode(UINT16 firstSubPageNo, UINT16 startIdx, UINT8 jobDispCnt, GUI_MSG_ID_e msgId);
static void GetPageButtonInfo(JOB_CODE_NAVIGATION_e jobNavigation, UINT8PTR firstSubPageNoPtr, UINT8PTR subPageCntPtr);

//******** Function Definitions ******

//*****************************************************************************
//	InitJobCost()
//	Param:
//		IN :	none
//		OUT:	None
//	Returns:	None
//	Description: Initialises recent job list for all users with invalid index value
//*****************************************************************************
void InitJobCost(void)
{
	memset(&RecentUserJobsList[0][0], INVALID_JCM_CONFIG_INDEX, sizeof(RecentUserJobsList) );
}

//*****************************************************************************
//	SwitchDcToJobDisplayState()
//	Param:
//		IN :	doorId
//		OUT:	None
//	Returns:	None
//	Description:
//				This function switches DCTask FSM to DC_JOB_CODE_DISPLAY_STATE
//				and initializes required parameters.
//*****************************************************************************
void SwitchDcToJobDisplayState(void)
{
	//Pass empty search query at first page
	SearchValidJobs("");
	LoadDisplayJobList(JOB_FIRST_PAGE, GUI_MSG_JOB_CODE_DISPLAY_SCREEN);

	// start timer for user input

	if(SetLcdIdleTimer(JOB_INPUT_WAIT_TIMER) == FAIL)
	{
		EPRINT(DC_LOG,"Unable to start LCD IDLE Timer for job code input for Door");
	}

	if(SetJobDisplayWaitTimer(JOB_INPUT_WAIT_TIMER) == FAIL)
	{
		EPRINT(DC_LOG,"Unable to start job code Wait Timer for Door");
	}

	CleanUpTempUserFP();
	DcInfo.curState = DC_JOB_CODE_DISPLAY_STATE;
}

//*****************************************************************************
//	DcJobCodeDisplayStateFn()
//	Param:
//		IN : msgId - ID of the received message
//			 dataPtr - Pointer to the data
//			 length - length of the data
//		OUT: NONE
//	Returns:
//			 NONE
//	Description:
//			This Function will be called by the dc task FSM. It passes events
//		to appropriate job costing related function and handles
//		in default.
//
//*****************************************************************************
void DcJobCodeDisplayStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	BOOL	startJobCodeTimerF = FALSE;

	switch(msgId)
	{
		case MSG_SEARCH_JOB_CODE_LIST_EVENT:
		{
			startJobCodeTimerF = TRUE;
			SearchValidJobs((CHARPTR)dataPtr);
			LoadDisplayJobList(JOB_FIRST_PAGE, GUI_MSG_JOB_CODE_LIST);
			break;
		}

		case GUI_BUTTON_NEXT_JOB_CODE_LIST:
		{
			//DisplayJobList next page
			startJobCodeTimerF = TRUE;
			LoadDisplayJobList(JOB_NEXT_PAGE, GUI_MSG_JOB_CODE_LIST);
			break;
		}

		case GUI_BUTTON_PREVIOUS_JOB_CODE_LIST:
		{
			//DisplayJobList Prev Page
			startJobCodeTimerF = TRUE;
			LoadDisplayJobList(JOB_PREV_PAGE, GUI_MSG_JOB_CODE_LIST);
			break;
		}

		case MSB_JOB_CODE_SELECTED_EVENT:
		{
			UINT8	jobcodeInPage, selectedRowIdx = *(UINT8PTR)dataPtr;

			if(searchedJobCnt > 0) //If no job found Enter should not work
			{
				jobcodeInPage = lastDispJobIdx % MAX_JOB_IN_PAGE;
				if(jobcodeInPage == 0)
				{
					jobcodeInPage = MAX_JOB_IN_PAGE;
				}
				DcInfo.selectedJobIdx = SearchedJobList[ (lastDispJobIdx + selectedRowIdx) - jobcodeInPage].jobIndex;

				if(SysAdvPara.retainJobSelectionF == ENABLED)
				{
					if(DcInfo.selectedJobIdx == NONE_JOB_CODE_CONFIG_INDEX)
					{
						ClearRetainJobCode();
					}
					else
					{
						RetainJobCode = DcInfo.selectedJobIdx;
					}
				}
				UpdateRecentUserJobs(DcInfo.userIdx, DcInfo.selectedJobIdx);
				CheckFinalPolicyAndAllow();
			}
			break;
		}

		case GUI_BUTTON_KEEP_ALIVE:
		{
			startJobCodeTimerF = TRUE;
			break;
		}

		case GUI_BUTTON_HOME:
		{
			SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);

			if( (DcInfo.singleUserSplCode != USER_SPECIAL_CODE_NONE) && (DcInfo.singleUserSplCode <= MAX_INOUT_SPL_FUNC_LIST) )
			{
				DcInfo.singleUserSplCode = USER_SPECIAL_CODE_NONE;
			}
			SwitchDcToIdleState();
			break;
		}

		default:
		{
			DcDefaultMsgHandler(msgId, dataPtr, length);
			break;
		}
	}
	if(startJobCodeTimerF == TRUE)
	{
		//ReloadJobDisplayTimer :verify
		if(SetLcdIdleTimer(JOB_INPUT_WAIT_TIMER) == FAIL)
		{
			EPRINT(DC_LOG,"Unable to start LCD IDLE Timer for job code input for Door");
		}

		if(SetJobDisplayWaitTimer(JOB_INPUT_WAIT_TIMER) == FAIL)
		{
			EPRINT(DC_LOG,"Unable to start job code Wait Timer for Door");
		}

	}
}

//*****************************************************************************
//	LoadDisplayJobList()
//	Param:
//		IN :PAGE_NAVIGATION_e dispPage
//		   :GUI_MSG_ID_e msgId
//		OUT: NONE
//	Returns:
//			 NONE
//	Description:
//			This Function will be called when previous or next arrow button is
//			pressed to update current job code list and at first job code
//			display to load job code load list.
//
//*****************************************************************************
static void LoadDisplayJobList(JOB_CODE_NAVIGATION_e jobNavigation, GUI_MSG_ID_e msgId)
{
	UINT8			jobcodeInPage;
	UINT16			firstSubPageNo;

	switch(jobNavigation)
	{
		case JOB_FIRST_PAGE:
		{
			lastDispJobIdx = 0;
			break;
		}

		case JOB_PREV_PAGE:
		{
			INT32		tmpJobIdx;
			//get job count in current page
			jobcodeInPage = lastDispJobIdx % MAX_JOB_IN_PAGE;
			if(jobcodeInPage == 0)
			{
				jobcodeInPage = MAX_JOB_IN_PAGE;
			}

			tmpJobIdx = lastDispJobIdx - jobcodeInPage - MAX_JOB_IN_PAGE;
			//if currently on 1st page then navigate to last page
			if(tmpJobIdx < 0)
			{
				if(searchedJobCnt % MAX_JOB_IN_PAGE == 0)	// last page also has MAX jobs
				{
					lastDispJobIdx = searchedJobCnt - MAX_JOB_IN_PAGE;
				}
				else
				{
					lastDispJobIdx = searchedJobCnt - (searchedJobCnt % MAX_JOB_IN_PAGE);
				}

			}
			else
			{
				lastDispJobIdx = tmpJobIdx;
			}
			break;
		}

		case JOB_NEXT_PAGE:
		{
			if( (searchedJobCnt - lastDispJobIdx) == 0)
			{
				lastDispJobIdx = 0;
			}
			break;
		}

		default:
			break;
	}

	//extra check to avoid unknown navigation issues
	if(lastDispJobIdx >= searchedJobCnt)
	{
		//reset every navig.related info
		lastDispJobIdx = 0;
	}

	//derive job code to be displayed in page
	jobcodeInPage = (searchedJobCnt - lastDispJobIdx);
	if(jobcodeInPage > MAX_JOB_IN_PAGE)
	{
		jobcodeInPage = MAX_JOB_IN_PAGE;
	}

	//derive page number
	//Set first sub page no to display
	//We have 5 sub pages in first page
	firstSubPageNo = (lastDispJobIdx / MAX_JOB_IN_SUB_PAGE) + 1;
	if(lastDispJobIdx % MAX_JOB_IN_PAGE > 0)
	{
		firstSubPageNo += 1;
	}

	DisplayJobCode(firstSubPageNo, lastDispJobIdx, jobcodeInPage, msgId);
}

//*****************************************************************************
//	SearchValidJobs()
//	Param:
//		IN : searchCode
//		OUT: NONE
//	Returns:
//			 NONE
//	Description:
//			This Function will be called when one of numeric key is pressed or when state is changed to job costing
//			this function will fill searchJobList with all valid jobs (if search string is blank) or valid jobs matching search string
//			It will also display first page of result
//*****************************************************************************
static void SearchValidJobs(CHARPTR queryStr)
{
	UINT16 				index, sortIndex;
	CHARPTR 			substrPtr;
	JOB_SEARCH_LIST_t	tempJob;

	searchedJobCnt = 0;
	if(strlen(queryStr) > 0) //SEARCH ACTION
	{
		for(index = 0; index < (MAX_JCM_CONFIG + 3); index++)    //+1 for None job
		{
			substrPtr = strcasestr(JcmConfig[index].jobCode, queryStr);

			if( (substrPtr != NULL) && (IsJobValid(index) == TRUE) )
			{
				SearchedJobList[searchedJobCnt].jobIndex = index;
				SearchedJobList[searchedJobCnt].searchOffset = (substrPtr - &JcmConfig[index].jobCode[0]);
				searchedJobCnt++;
			}
		}
		//sort ascending wrt offset
		//insertion sort used
		for(index = 0; index < (searchedJobCnt - 1); index++) //+1 for None job
		{
			for(sortIndex = (index + 1); sortIndex < searchedJobCnt; sortIndex++)
			{
				if(SearchedJobList[index].searchOffset > SearchedJobList[sortIndex].searchOffset)
				{
					tempJob = SearchedJobList[index];
					SearchedJobList[index] = SearchedJobList[sortIndex];
					SearchedJobList[sortIndex] = tempJob;
					break;
				}
			}
		}
	}
	//if search query is empty : LOAD ALL JOBS ACTION
	else
	{
		SearchedJobList[searchedJobCnt++].jobIndex = MAX_JCM_CONFIG + 1;// Default Job
		SearchedJobList[searchedJobCnt++].jobIndex = MAX_JCM_CONFIG + 2;// Continue Job
		// load valid recent jobs
		for(index = 0; index < MAX_RECENT_JOBS; index++)
		{
			if(RecentUserJobsList[DcInfo.userIdx][index] < MAX_JCM_CONFIG)
			{
				if (IsJobValid(RecentUserJobsList[DcInfo.userIdx][index]) == TRUE)
				{
					SearchedJobList[searchedJobCnt].jobIndex = RecentUserJobsList[DcInfo.userIdx][index];
					searchedJobCnt++;
				}
			}
		}
		for(index = 0; index <= MAX_JCM_CONFIG; index++) // +1 for none Job
		{
			if(IsJobValid(index) == TRUE)
			{
				for(sortIndex = 0; sortIndex < MAX_RECENT_JOBS; sortIndex++)
				{
					if(RecentUserJobsList[DcInfo.userIdx][sortIndex] == index)
					{
						break;
					}
				}
				if(sortIndex < MAX_RECENT_JOBS)
				{
					continue;
				}
				SearchedJobList[searchedJobCnt].jobIndex = index;
				searchedJobCnt++;
			}
		}
	}
}

//*****************************************************************************
//	IsJobValid()
//	Param:
//		IN : index - index of Job in JcmConfig
//		OUT: NONE
//	Returns:
//			 TRUE/FALSE
//	Description:
//			This Function checks job validity for given index
//*****************************************************************************
static BOOL IsJobValid(UINT16 index)
{
	BOOL	retVal = FALSE;
	UINT16	curDateInDayCnt, jobStartDateInDayCnt, jobEndDateInDayCnt;

	if(JcmConfig[index].refId != INVALID_JOB_REF_ID)
	{
		curDateInDayCnt = ConvertDateInDayCnt(CurrTimeDate.tm_mday, CurrTimeDate.tm_mon, (CurrTimeDate.tm_year - ADJUST_TM_YEAR) );
		jobStartDateInDayCnt = ConvertDateInDayCnt(JcmConfig[index].startDate.day, JcmConfig[index].startDate.month, JcmConfig[index].startDate.year);
		jobEndDateInDayCnt = ConvertDateInDayCnt(JcmConfig[index].endDate.day, JcmConfig[index].endDate.month, JcmConfig[index].endDate.year);

		if( (curDateInDayCnt >= jobStartDateInDayCnt) && (curDateInDayCnt <= jobEndDateInDayCnt) )
		{
			//job valid
			retVal = TRUE;
		}
	}

	return retVal;
}
//*****************************************************************************
//	UpdateRecentUserJobs()
//	Param:
//		IN :	userIdx & jobIdx
//		OUT: 	NONE
//	Returns: NONE
//	Description: updates the recent job list such that current job will be at the start of recent job list
//
//*****************************************************************************
static void UpdateRecentUserJobs(UINT16 userIdx, UINT16 jobIdx)
{
	UINT8	recentJobIndex;

	if(jobIdx >= MAX_JCM_CONFIG)
	{
		return;//donot add None or any invalid job in recent list
	}
	//find position of current job in recent job list
	for(recentJobIndex = 0; recentJobIndex < MAX_RECENT_JOBS; recentJobIndex++)
	{
		if(RecentUserJobsList[userIdx][recentJobIndex] == jobIdx)
		{
			break;
		}
	}

	if(recentJobIndex == MAX_JOB_IN_PAGE) //current job not found in recent
	{
		recentJobIndex--;
	}

	// shift other job to right
	while(recentJobIndex > 0)
	{
		RecentUserJobsList[userIdx][recentJobIndex] = RecentUserJobsList[userIdx][recentJobIndex -1];
		recentJobIndex--;
	}

	RecentUserJobsList[userIdx][recentJobIndex] = jobIdx; // new job added at the start
}

//*****************************************************************************
//	SetJobDisplayWaitTimer()
//	Param:
//		IN :	timer value
//		OUT: 	NONE
//	Returns: SUCCESS/FAIL
//	Description: start/reload job code display wait timer
//
//*****************************************************************************
static BOOL SetJobDisplayWaitTimer(UINT8 timer)
{
	if(DcInfo.jobInputWaitTimer == INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t tInfo;

		tInfo.count = (timer * 10);
		tInfo.data = 0;
		tInfo.funcPtr = &JobDisplayWaitTimeout;
		return (StartTimer(tInfo, &DcInfo.jobInputWaitTimer, __FUNCTION__) );
	}
	else //reload timer
	{
		return (ReloadTimer(DcInfo.jobInputWaitTimer, (timer * 10)) );
	}
}
//*****************************************************************************
//	JobDisplayWaitTimeout
//	Param:
//		IN :	dummy
//		OUT:	none
//	Returns:	none
//	Description:
//				It switches DC to idle state and sends audio visual response to
//				DC for timeout. If special code for single user is active,
//				it deactivates special code.
//
//*****************************************************************************
static void JobDisplayWaitTimeout(UNUSED_PARA UINT32 dummy)
{
	DeleteTimer(&DcInfo.jobInputWaitTimer);

	if( (DcInfo.singleUserSplCode != USER_SPECIAL_CODE_NONE) && (DcInfo.singleUserSplCode <= MAX_INOUT_SPL_FUNC_LIST) )
	{
		DcInfo.singleUserSplCode = USER_SPECIAL_CODE_NONE;
	}
	DcInfo.authReason = AUTH_TIMEOUT;
	DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
}

//*****************************************************************************
//	DisplayJobStringsFn
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description: displays job list on LCD display
//				
//*****************************************************************************
static void DisplayJobCode(UINT16 firstSubPageNo, UINT16 startIdx, UINT8 jobDispCnt, GUI_MSG_ID_e msgId)
{
	UINT8 				lineIndex;
	JOB_CODE_MSG_QT_t	jobCodeMsg;

	jobCodeMsg.jobCodeCnt = jobDispCnt;
	for(lineIndex = 0; lineIndex < jobDispCnt; lineIndex++)
	{
		strcpy(&jobCodeMsg.jobCode[lineIndex][0], JcmConfig[SearchedJobList[ (lineIndex + startIdx) ].jobIndex].jobCode);
		//update last displayed job index
		lastDispJobIdx++;
	}

	//Derive page button info for CURRENT page
	jobCodeMsg.pageBtnInfo.currFirstSubPageNo = firstSubPageNo;
	jobCodeMsg.pageBtnInfo.currSubPageCnt = (jobDispCnt / MAX_JOB_IN_SUB_PAGE);
	if( (jobDispCnt % MAX_JOB_IN_SUB_PAGE) > 0)
		jobCodeMsg.pageBtnInfo.currSubPageCnt += 1;

	//Derive page button info for PREVIOUS page
	GetPageButtonInfo(JOB_PREV_PAGE, &jobCodeMsg.pageBtnInfo.prevFirstSubPageNo, &jobCodeMsg.pageBtnInfo.prevSubPageCnt);

	//Derive page button info for NEXT page
	GetPageButtonInfo(JOB_NEXT_PAGE, &jobCodeMsg.pageBtnInfo.nextFirstSubPageNo, &jobCodeMsg.pageBtnInfo.nextSubPageCnt);

	SendMessageToQt((VOIDPTR)&jobCodeMsg, msgId);
}

//*****************************************************************************
//	GetPageButtonInfo()
//	Param:
//		IN :PAGE_NAVIGATION_e dispPage
//	    OUT:firstSubPageNo
//		   :subPageCnt
//	Returns:
//			 NONE
//	Description:
//			This Function will derive and fill the page button info for given page
//
//*****************************************************************************
static void GetPageButtonInfo(JOB_CODE_NAVIGATION_e jobNavigation, UINT8PTR firstSubPageNoPtr, UINT8PTR subPageCntPtr)
{
	UINT8			jobcodeInPage;
	UINT16			temDispLastJobIdx;

	temDispLastJobIdx = lastDispJobIdx;

	switch(jobNavigation)
	{
		//Derive page button info for previous page
		case JOB_PREV_PAGE:
		{
			INT32		tmpJobIdx;

			//get job count in current page
			jobcodeInPage = temDispLastJobIdx % MAX_JOB_IN_PAGE;
			if(jobcodeInPage == 0)
			{
				jobcodeInPage = MAX_JOB_IN_PAGE;
			}

			tmpJobIdx = temDispLastJobIdx - jobcodeInPage - MAX_JOB_IN_PAGE;
			//if currently on 1st page then navigate to last page
			if(tmpJobIdx < 0)
			{
				if(searchedJobCnt % MAX_JOB_IN_PAGE == 0)	// last page also has MAX jobs
				{
					temDispLastJobIdx = searchedJobCnt - MAX_JOB_IN_PAGE;
				}
				else
				{
					temDispLastJobIdx = searchedJobCnt - (searchedJobCnt % MAX_JOB_IN_PAGE);
				}

			}
			else
			{
				temDispLastJobIdx = tmpJobIdx;
			}
			break;
		}

		//Derive page button info for next page
		case JOB_NEXT_PAGE:
		{
			if( (searchedJobCnt - temDispLastJobIdx) == 0)
			{
				temDispLastJobIdx = 0;
			}
			break;
		}

		default:
			break;
	}

	//extra check to avoid unknown navigation issues
	if(temDispLastJobIdx >= searchedJobCnt)
	{
		//reset every navig.related info
		temDispLastJobIdx = 0;
	}

	//derive first sub page number
	*firstSubPageNoPtr = (temDispLastJobIdx / MAX_JOB_IN_SUB_PAGE) + 1;
	if(temDispLastJobIdx % MAX_JOB_IN_PAGE > 0)
	{
		*firstSubPageNoPtr += 1;
	}

	//derive sub page count
	jobcodeInPage = (searchedJobCnt - temDispLastJobIdx);
	if(jobcodeInPage > MAX_JOB_IN_PAGE)
	{
		jobcodeInPage = MAX_JOB_IN_PAGE;
	}
	*subPageCntPtr = (jobcodeInPage / MAX_JOB_IN_SUB_PAGE);
	if( (jobcodeInPage % MAX_JOB_IN_SUB_PAGE) > 0)
		*subPageCntPtr += 1;

}

//*****************************************************************************
//	IsJobCodeInputReq
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//
//*****************************************************************************
BOOL IsJobCodeInputReq(void)
{
	BOOL	spFnCheckF = TRUE;
	UINT8	spFnCode = GetCurrSplCode();

	if( (spFnCode != USER_SPECIAL_CODE_NONE) && (spFnCode <= MAX_INOUT_SPL_FUNC_LIST) )
	{
		spFnCode--;
		if(SpecialFunctions[spFnCode].promptJobCodeF == FALSE)
		{
			spFnCheckF = FALSE;
		}
	}
	if( (DcMonitorControl.doorLock == DOOR_LOCK_NORMAL) && (DcInfo.userType == NORMAL_USER) &&
			(SysAdvPara.jcmEnblF == ENABLED) && (GetJobType(DcInfo.userIdx) == JCM_DYNAMIC) &&
			(SysBasicPara.application != CAFETERIA) &&  (DcInfo.readerMode == ENTRY_READER) &&
			(DcInfo.respPort == INTERNAL_PORT) && (DcInfo.apiUser == CLEAR) && (spFnCheckF == TRUE) && (DcInfo.cbReaderBleAccess == CLEAR))
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

//*****************************************************************************
//	IsJobAssignmentTypeStatic
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//
//*****************************************************************************
BOOL IsJobAssignmentTypeStatic(void)
{
	BOOL	spFnCheckF = TRUE;
	UINT8	spFnCode = GetCurrSplCode();

	if( (spFnCode != USER_SPECIAL_CODE_NONE) && (spFnCode <= MAX_INOUT_SPL_FUNC_LIST) )
	{
		spFnCode--;
		if(SpecialFunctions[spFnCode].promptJobCodeF == FALSE)
		{
			spFnCheckF = FALSE;
		}
	}
	if( (DcMonitorControl.doorLock == DOOR_LOCK_NORMAL) && (DcInfo.userType == NORMAL_USER) &&
			(SysAdvPara.jcmEnblF == ENABLED) && (GetJobType(DcInfo.userIdx) == JCM_STATIC) &&
			(SysBasicPara.application != CAFETERIA) &&  (DcInfo.readerMode == ENTRY_READER) &&
			(DcInfo.respPort == INTERNAL_PORT) && (DcInfo.apiUser == CLEAR) && (spFnCheckF == TRUE) && (DcInfo.cbReaderBleAccess == CLEAR))
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

//*****************************************************************************
//	IsRetainJobCodeAvailable
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//
//*****************************************************************************
BOOL IsRetainJobCodeAvailable(void)
{
	if(RetainJobCode == DEFAULT_RETAIN_JOB_CODE)
	{
		return NO;
	}
	else
	{
		return YES;
	}
}

//*****************************************************************************
//	ClearRetainJobCode
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//
//*****************************************************************************
VOID ClearRetainJobCode(void)
{
	RetainJobCode = DEFAULT_RETAIN_JOB_CODE;
}
