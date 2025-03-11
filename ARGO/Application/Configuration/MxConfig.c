///////////////////////////////////////////////////////////////////////////
//
//   MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//   MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//   MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
//   MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
//   MM       MM   AA       AA       TT      RR  RR       II      XX XX
//   MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//   Company      : Matrix Comsec Pvt. Ltd., Baroda, India.
//   Project      : Next Generation Terminal(COSEC_NGT)
//   Created By   : Viral Chavda
//   File         : MxConfig.c
//   Description  : This file declares configuration variables
//
/////////////////////////////////////////////////////////////////////////////

//******** Include files *******
#include <stdio.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include "MxDeviceSpecific.h"
#include "MxTypedef.h"
#include "MxConfig.h"
#include "MxConfigFileIo.h"
#include "MxLogs.h"
#include "MxSysCommonDef.h"
#include "MxDcTask.h"
#include "MxEnrollment.h"
#include "MxSysUtils.h"
#include "MxCerTask.h"
#include "MxSysCommonUtils.h"
#include "MxEthernetPort.h"
#include "MxAttendanceDetailConfig.h"
#include "MxBluetoothConfig.h"
#include "MxReaderBluetoothConfig.h"
#include "MxReaderTask.h"
#include "MxGpioDrv.h"
#include "MxUserConfig.h"
#include "MxConfigUpdateHandler.h"
#include "HttpReqFSM.h"

//******** Extern Variables **********
extern const BLOCKED_USER_GROUP_t 	DfltBlockedUser;
extern const USER_CONFIG_t 			DfltTestUserConfig;
extern const READERS_PARAMETER_t 	DfltReadersParaConfig;
extern struct tm 					CurrTimeDate;
extern DOOR_ACTIVITY_INFO_t 		DcInfo;
extern time_t						TimeInSec;
extern const WIFI_PARA_t			DfltWifiPara;
extern const ETHERNET_PARA_t		DfltEthPara;
extern const VAM_CONFIG_t			DfltVamConfig;
extern const USER_CONFIG_t 			DfltUserConfig;
extern BOOL 						SamayModeEnable;
extern pthread_mutex_t 				MutexLockAutoSwitchWiFi;
extern BLUETOOTH_CONFIG_t			BluetoothPara;
extern READER_BLUETOOTH_CONFIG_t	ReaderBluetoothPara;
extern READER_STATUS_e 				readerStatus[MAX_READERS];

//******** Defines and Data Types ****

//******** Function Prototypes *******


//******** Global Variables **********
SYS_VAR_t					SysVar;
TWO_PERSON_GROUP_t			TwoPersonGroup[MAX_TWO_PERSON_GROUP];
BLOCKED_USER_GROUP_t		BlockedUser[MAX_BLOCKED_USER];
FIRST_IN_LIST_t				FirstInUserGroup[MAX_FIRST_IN_LISTS];
FIRST_IN_INFO_t				FirstInInfo;
HOLIDAY_SCHEDULE_t          HolidaySchedule[MAX_HOLIDAY_SCHEDULE];
PASSWORD_t			  		Password;
ETHERNET_PARA_t		  		EthernetParameters;
V11R3_SERVER_PARA_t			V11R3ServerParameters;
SERVER_PARA_t		  		ServerParameters;
WIFI_PARA_t					WifiParameters;
MODEM_CONFIG_t				ModemProfile[MAX_MODEM_PROFILE];
COMMAND_CTRL_CONFIG_t		CmdCtrlConfig;
DST_CONFIG_t				DstConfig;
SYS_ADVANCE_PARA_t	  		SysAdvPara;
SYS_BASIC_PARA_t	  		SysBasicPara;
VISITOR_MANAGEMENT_t  		Visitor[MAX_VISITORS];
SPECIAL_FUNCTION_t			SpecialFunctions[MAX_SPECIAL_FUNC];
TIME_TRIG_FUNC_t			TimeTrigFunc[MAX_TIME_TRIG_FUNC];
FPR_CONFIG_t				FprConfig;
READERS_PARAMETER_t			ReadersParaConfig;
IO_LINK_CONFIG_t			IoLinkConfig[MAX_IO_LINK];
IO_LINK_EXT_DEV_CONFIG_t	IoLinkExtDevConfig[MAX_IO_LINK_EXT_DEVICE];
IO_LINK_EXT_DEV_MAPPING_t	IoLinkExtDevMapping[MAX_IO_LINK];
CAFE_ITEM_CONFIG_t			CafeItemConfig[MAX_CAFE_ITEM_CODE];
CAFE_MENU_CONFIG_t			CafeMenuConfig[MAX_CAFE_MENU_CODE];
CAFE_MENU_SCHEDULE_t		CafeMenuSchConfig[MAX_CAFE_MENU_SCHEDULE];
PRINTER_CONFIG_t			PrinterConfig;
DVR_EVENT_ACTION_CONFIG_t	DvrEvtActionConfig[MAX_DVR_EVT_ACTN_CONFIG];
SMART_ROUTE_ACCESS_CONFIG_t	SmartRouteAccessConfig[MAX_SMART_ROUTE_ACCESS];
DOOR_APPL_TYPE_e			DoorApplType = DIRECT_DOOR_APPL;
FILE_LOG_PARA_CONFIG_t		FileLogPara;
WEB_API_EVENTS_CONFIG_t 	WebAPIEventsConfig;
WEB_API_EVENTS_ACTIVITY_t	WebApiEventActivity;
CARD_FORMAT_CONFIG_t		CardFormatConf[MAX_CARD_FORMAT_PER_DOOR];
VAM_CONFIG_t				VamConfig[MAX_VAM_CONFIG];
JCM_CONFIG_t				JcmConfig[MAX_JCM_CONFIG+3]; //+3 for 'None', 'Default' and 'Continue' Job
DOOR_MODE_SCH_CONFIG_t		DoorModeSchCfg[MAX_DOOR_MODE_SCHEDULE];

BUZZ_LED_CAD_CONFIG_t		BuzzLedCadPara[MAX_LED_BUZ_CADANCE];
SERVER_MODE_CONFIG_t		ServerModeCfg;
VYOM_SERVER_CONFIG_t		VyomServerPara;
BACKUP_AND_UPDATE_t			BackUpAndUpdate;
CONNECT_TO_PANEL_CONFIG_t	ConnectToPanelPara;

UINT8						UserInOutStatus[USER_IN_OUT_STATUS_SIZE];
LICENSE_DONGLE_CONFIG_t		LicenseDongleCfg;
IP_CAMERA_CONFIG_t			IpCameraConfig;
EVENT_PICS_INFO_t			EventPicsInfo;
BRIGHTNESS_DETAIL_t			DisplayBrightness;
ACCESS_SCHEDULE_t			AccessScheduleConfig[MAX_TIME_FRAME];

//******** Static Variables **********

//******** Function Definations ******

//*****************************************************************************
//	IsAlphaNumIdValid()
//	Param: 
//		IN : alphaNumId	- alpha num user id to be searched in users database
//		OUT: userIdx		- user index for which user id matched
//			 INVALID_USER 	- if no match found
//	Returns: 
//			YES	- if user id matched
//			NO	- if user id not found
//	Description:
//			This function checks whether the alpha num user id is there in users database.
//			If updates user index at given pointer.
//*****************************************************************************
BOOL IsAlphaNumIdValid(CHARPTR alphaNumId, UINT16PTR userIdxPtr)
{
	UINT16 userIdx;
	
	// Read user id from user configuration only if RAM copy of user info
	// indicates user id is configured. This allows fast search for matching 
	// user id. Once matching user id is found, we pass the index for which the
	// user id is found.
	// Search only if valid user id is provided
	if((strlen(alphaNumId) != 0) && (strlen(alphaNumId)<ALPHANUM_ID_SIZE))
	{
		// Serach through all users
		for(userIdx = 0; userIdx < MAX_USERS; userIdx++)
		{
			//	Compare the User ID with the existing user ID in configuration
			// comparison is case insensitive a-z is same as A-Z
			if(strcasecmp(alphaNumId, GetAlphaNumId(userIdx)) == 0)
			{
    			// If user id found and userIdxPtr is not null 
				// copy the user Id in to the userIdxPtr
				if(userIdxPtr != NULL)
					*userIdxPtr = userIdx;
				return YES;
    		}
		}
	}
	// No match found
	if(userIdxPtr != NULL)
		*userIdxPtr = INVALID_USER;

	return NO;
}

//*****************************************************************************
//	IsCardValid( )	
//	Param: 
//		IN : cardId		- card id to be matched	in users database
//		OUT: userIdx	- user index for which card matched
//			 INVALID_USER - if no match found
//	Returns: 
//			YES	- if card matched
//			NO	- if card not matched
//	Description:
//			This function checks whether the card id is there in users databese.
//			If updates user index at given pointer.
//*****************************************************************************
BOOL IsCardValid(UINT64 cardId, UINT16PTR userIdxPtr, UINT32 *credSubType)
{
	UINT16 userIdx;
	UINT8 cardIdx;

	// Search only if valid card id is provided
	if(cardId != INVALID_CARD_ID)
	{
		// Search through user configuration
		for(userIdx = 0 ; userIdx < MAX_USERS ; userIdx++)
		{
			if(GetUserId(userIdx) != INVALID_USER_ID)
			{
				for(cardIdx = 0; cardIdx < MAX_USER_CARD; cardIdx++)
				{
					// Each User can have upto two cards. So check both
					if(cardId == GetUserCardId(userIdx, cardIdx))
					{
						// card is matched so return index
						if(userIdxPtr != NULL)
							*userIdxPtr = userIdx;

						if (credSubType != NULL)
						{
							if(cardIdx == 0)
								*credSubType = ACCESS_CARD1;
							else
								*credSubType = ACCESS_CARD2;
						}
						return YES;
					}
				}
			}
		}
	}

	// No match found
	if(userIdxPtr != NULL)
		*userIdxPtr = INVALID_USER;
	if (credSubType != NULL)
	{
		*credSubType = ACCESS_CARD1;
	}
	return NO;
}

//*****************************************************************************
//	IsSplFnCard( )	
//	Param: 
//		IN : cardId			- card id to be serached in special function database
//		OUT: splFnIdx		- special function index if card matched
//			 INVALID_SPL_FN	- if card not matched
//			 cardIdx		- special function card index if card matched
//			 SPL_FN_CARD_NONE- if card not matched
//	Returns: 
//			YES	- if special function card 
//			NO	- if not special function card
//	Description:
//			This function checks whether the card id is there in 
//			special function databese.
//*****************************************************************************
BOOL IsSplFnCard(UINT64 cardId, SPECIAL_FUNCTIONS_e * splFnIdxPtr, CARD_INDEX_e * cardIdxPtr)
{
	UINT8 cardIdx;
	SPECIAL_FUNCTIONS_e splFnIdx;

	// Search only if valid card id is provided
	if(cardId != INVALID_CARD_ID)
	{
		// Search through special function configuration
		for(splFnIdx = 0 ; splFnIdx < MAX_SPECIAL_FUNC; splFnIdx++)
		{
			// Each special function can have upto 4 cards. So check all
			for(cardIdx = 0 ; cardIdx < MAX_SP_FUNC_CARD; cardIdx++)
			{
				if(cardId == SpecialFunctions[splFnIdx].cardId[cardIdx])
				{	
					if(splFnIdxPtr != NULL)
						*splFnIdxPtr = splFnIdx;
					if(cardIdxPtr != NULL)
						*cardIdxPtr = cardIdx;
					return YES;
				}
			}
		}
	}

	// No match found
	if(splFnIdxPtr != NULL)
		*splFnIdxPtr = INVALID_SPL_FN;
	if(cardIdxPtr != NULL)
		*cardIdxPtr = CARD_NONE;
	return NO;
}

//*****************************************************************************
//	IsPinValid( )	
//	Param: 
//		IN : userPin	- 	PIN to be matched in users database
//		OUT: userIdx	-	user index for which PIN matched
//			 INVALID_USER - if no match found
//	Returns: 
//			YES	- if PIN matched
//			NO	- if PIN not matched
//	Description:
//			This function checks whether the PIN is there in users databese.
//			If updates user index at given pointer.
//*****************************************************************************
BOOL IsPinValid(CHARPTR userPin, UINT16PTR userIdxPtr)
{
	UINT16	userIdx;

	// Only search for user pin if input is valid	
	if(strlen(userPin) > 0)
	{
		// Search through user configuration
		for(userIdx = 0; userIdx < MAX_USERS; userIdx++)
		{
			if(strcmp(userPin, GetUserPinCode(userIdx)) == 0)
			{
				// PIN is matched, if valid user id is assigned then return index
				if(GetUserId(userIdx) != INVALID_USER_ID)
				{
					if(userIdxPtr != NULL)
						*userIdxPtr = userIdx;
					return YES;
				}
			}
		}
	}

	// No match found
	if(userIdxPtr != NULL)
		*userIdxPtr = INVALID_USER;
	return NO;
}

//*****************************************************************************
//	IsUserBlocked( )	
//	Param: 
//		IN : userIdx	- user index for which block list is to check			
//		OUT: blockIdx	- If user is blocked
//	Returns: 
//			YES	- if user blocked
//			NO	- if user not blocked
//	Description:
//			This function checks whether specifed user is blocked or not.
//*****************************************************************************
BOOL IsUserBlocked(UINT16 userIdx, UINT8PTR blockIdxPtr)
{
	UINT8	blockIdx;

	// search only if valid user index is provided
	if(userIdx < MAX_USERS)
	{
		// search through blocked list
		for(blockIdx = 0 ; blockIdx < MAX_BLOCKED_USER; blockIdx++)
		{
			if(userIdx == BlockedUser[blockIdx].userIdx)
			{
				if(blockIdxPtr != NULL)
					*blockIdxPtr = blockIdx;
				return YES;
			}
		}
	}

	if(blockIdxPtr != NULL)
		*blockIdxPtr = NOT_IN_BLOCKED_LIST;

	return NO;
}

//*****************************************************************************
//	AddBlockedUser( )	
//	Param: 
//		IN : userIdx	
//			 reason   -  reason for which user is blocke			
//		OUT: NONE
//	Returns: Success - if user added to blocked list
//			 Fail - if user already in list or no free entry
//	Description:
//			It adds specified user in blocked user list for reason mentioned.
//			If user already exist in blocked list,we do not create another entry
//*****************************************************************************

BOOL AddBlockedUser(UINT16 userIdx, BLOCK_REASON_e reason)
{
	UINT8	blockIdx;

	// If user index is invalid, we can't identify the user
	if(userIdx >= MAX_USERS)
	{
		return FAIL;
	}

	// If user is already blocked, we don't create another entry
	if(IsUserBlocked(userIdx, &blockIdx) == YES)
	{
		return FAIL;
	}

	// Loop up to max blocked user supported in system to find blank entry
	for(blockIdx=0; blockIdx< MAX_BLOCKED_USER; blockIdx++)
	{
		// Check if entry is blank
		if(BlockedUser[blockIdx].userIdx == INVALID_USER)
			break; 
	}

	// if blank entry found, add user
	if(blockIdx < MAX_BLOCKED_USER)
	{
		BlockedUser[blockIdx].userIdx = userIdx;
		BlockedUser[blockIdx].reason = reason;
		BlockedUser[blockIdx].date = TimeInSec;
		// If failed to read time/date, reset it
		if(BlockedUser[blockIdx].date == (time_t)-1)
			BlockedUser[blockIdx].date = (time_t)0;

		// write to config file
		WriteSingleBlockedUserConfig(blockIdx);
		// Log event
		WriteEventLog(BLOCK_RESTORE_USER, GetUserId(userIdx), reason, USER_BLOCKED,UNUSED_FIELD,UNUSED_FIELD);

		return SUCCESS;
	}

	// No free entry found
	EPRINT(DC_LOG, "No entry found to add blocked user");
	return FAIL;

}

//*****************************************************************************
//	RestoreBlockedUser( )	
//	Param: 
//		IN : userIdx	
//		OUT: NONE
//	Returns: None
//
//	Description:
//			This function will remove specified blocked user from blocked user list.
//*****************************************************************************

void RestoreBlockedUser(UINT16 userIdx)
{
	UINT8	blockIdx, startIdx;
	UINT8	pg;

	// If user index is invalid, we can't identify the user
	if(userIdx >= MAX_USERS)
	{
		DPRINT(DC_LOG, "UserIdx %d is invalid user", userIdx);
		return;
	}

	// If user is not in blocked user list, no need to do anything
	if(IsUserBlocked(userIdx, &blockIdx) == NO)
	{
		DPRINT(DC_LOG, "User [%s] is not present in blocked user list", GetAlphaNumId(userIdx));
		return;
	}

	// Remember the entry we are removing from list
	startIdx = blockIdx;
	// Log event before clearing entry
	WriteEventLog(BLOCK_RESTORE_USER, GetUserId(BlockedUser[blockIdx].userIdx), UNUSED_FIELD, USER_RESTORED,UNUSED_FIELD,UNUSED_FIELD);
	// clear entry to restore user
	BlockedUser[blockIdx] = DfltBlockedUser;
	
	// Move remaining used entries up by one
	while( ++blockIdx < MAX_BLOCKED_USER)
	{
		// Once we find one blank entry, we are done
		if(BlockedUser[blockIdx].userIdx == INVALID_USER)
			break;
		else
		{
			// Move current entry to previous
			BlockedUser[blockIdx-1] = BlockedUser[blockIdx];
			BlockedUser[blockIdx] = DfltBlockedUser;
		}
	}

	// Now figure out how many files to write starting from entry that we removed
	while ( startIdx < blockIdx)
	{
		pg = startIdx / BLOCKED_USER_PER_PAGE;
		WriteBlockedUserConfig(pg);
		// Since we moved remaining entries up, we need to check if other files to be written
		// move index to start of next file
		startIdx = (pg+1) * BLOCKED_USER_PER_PAGE;
	}
}

//*****************************************************************************
//	IsVisitorValid( )	
//	Param: 
//		IN : visitorIdx				
//		OUT: NONE
//	Returns:
//			YES - 	If Visitor is within valid time
//			NO- 	If Visitor is not within valid time
//	Description:
//			It checks whether specified visitor is within valid time.
//*****************************************************************************
BOOL IsVisitorValid(UINT16 visitorIdx)
{
	BOOL status = NO;

	status = CheckVisitorPolicy(Visitor[visitorIdx],&CurrTimeDate);
	return status;
}

//*****************************************************************************
//	IsUserValidityExpired()
//	Param: 
//		IN :	terminationDate	- termination Date in format DD/MM/YY
//		OUT:	None
//	Returns:	None
//	Description:
//				This function checks if User's validity period is over.
//
//*****************************************************************************
BOOL IsUserValidityExpired(DATE_DDMMYY_t terminationDate)
{
	UINT16	terminationDateCnt;
	UINT16	currentDateCnt;
	BOOL	status = NO;

	// Convert termination and current date into day count
	terminationDateCnt = ConvertDateInDayCnt(terminationDate.day,
								  terminationDate.month, terminationDate.year);

	// Year from the system will be given from 1900. To calculate day count 
	// for currrent date, subtract 100 from the year. 
	// For example, for the year 2009  linux will give 109. 
	currentDateCnt = ConvertDateInDayCnt(CurrTimeDate.tm_mday, CurrTimeDate.tm_mon, 
										(CurrTimeDate.tm_year - ADJUST_TM_YEAR) );

	// Check if user validity expired
	if(currentDateCnt > terminationDateCnt)
	{
		DPRINT(DC_LOG, "User's validity expired");
		status = YES;
	}

	return status;
}

//*****************************************************************************
//	IsUserIn2PersonGrp( )	
//	Param: 
//		IN : userIdx		- user index to be searched in 2 person group
//		OUT: twoPersonGrpDtl	- 2 person rule index in which user is found
//	Returns: 
//			YES	- if user found in any 2 person group
//			NO		- if user not found in any 2 person group
//	Description:
//			This function checks if given user is a member of any 2-person
//			group assigned to given zone.
//*****************************************************************************
BOOL IsUserIn2PersonGrp(UINT16 userIdx, UINT8PTR twoPersonGrpDtl)
{
	UINT32 userId;
	UINT8  twoPrsnGrpIdx;
	UINT8  index;
	BOOL   status = NO;

	userId = GetUserId(userIdx);
	*twoPersonGrpDtl = MAX_TWO_PERSON_USER_GRP_DTL;
	// For first user sequence is (1) Primary group (2) Secondary grup
	// For second user sequece is  (1)group other then first user (2) group of first user
	twoPrsnGrpIdx = SysAdvPara.twoPerson.grpId1;

	if(twoPrsnGrpIdx != TWO_PERSON_GRP_NONE)
	{
		for(index = 0 ; index < MAX_TWO_PERSON_MEMBERS ; index++)
		{
			if(userId == TwoPersonGroup[twoPrsnGrpIdx].userId[index])
			{
				DPRINT(DC_LOG, "User [%s] is in two person group Idx %d", GetAlphaNumId(userIdx), twoPrsnGrpIdx);
				if(twoPersonGrpDtl != NULL)
				{
					*twoPersonGrpDtl = PRIMARY_GRP;
					status = YES;
				}
			}
		}
	}

	twoPrsnGrpIdx = SysAdvPara.twoPerson.grpId2;

	// If second group not programmed, no need to check the table
	if(twoPrsnGrpIdx != TWO_PERSON_GRP_NONE)
	{
		// Check if user is in secondary 2 person group
		for(index = 0 ; index < MAX_TWO_PERSON_MEMBERS ; index++)
		{
			if(userId == TwoPersonGroup[twoPrsnGrpIdx].userId[index])
			{
				DPRINT(DC_LOG, "User [%s] is in two person group Idx %d", GetAlphaNumId(userIdx), twoPrsnGrpIdx);
				if(twoPersonGrpDtl != NULL)
				{
					if(*twoPersonGrpDtl == PRIMARY_GRP)
					{
						*twoPersonGrpDtl = PRIMARY_AND_SECONADAY_BOTH_GRP;
					}
					else
					{
						*twoPersonGrpDtl = SECONDARY_GRP;
					}
				}
				status = YES;
			}
		}
	}

	if(status == YES)
	{
		return YES;
	}
	else
	{
		if((SysAdvPara.twoPersonMode == PRIMARY_AND_SECONDARY_MUST) && (SysAdvPara.twoPerson.grpId2 == TWO_PERSON_GRP_NONE))
		{
			// Not found in either group
			DPRINT(DC_LOG, "User [%s] not in either two person group", GetAlphaNumId(userIdx));
			if(twoPersonGrpDtl != NULL)
				*twoPersonGrpDtl = MAX_TWO_PERSON_USER_GRP_DTL;
			return YES;
		}
		else
		{
			// Not found in either group
			DPRINT(DC_LOG, "User [%s] not in either two person group but it is valid user", GetAlphaNumId(userIdx));
			if(twoPersonGrpDtl != NULL)
				*twoPersonGrpDtl = MAX_TWO_PERSON_USER_GRP_DTL;
			return NO;
		}
	}
}

//*****************************************************************************
//	IsFirstInUser( )	
//	Param: 
//		IN : userId		- user id to be searched in first in users group
//		OUT: firstInIdx	- index where user is stored in first in users list
//			 NOT_IN_FIRST_IN_LIST - if not found in first in users group
//	Returns: 
//			YES	- if user is from first in group
//			NO	- if user is not from first in group
//	Description:
//			This function checks if given user is from first in group or not.
//*****************************************************************************
BOOL IsFirstInUser(UINT32 userId, UINT8PTR firstInIdxPtr)
{
    UINT32 firstInIdx;
	UINT8 listId;

	listId = SysAdvPara.firstIn.listId;
	// Search through first in user group configuration
	for(firstInIdx = 0; firstInIdx < MAX_FIRST_IN_MEMBERS; firstInIdx++)
	{
		if(userId == FirstInUserGroup[listId].members[firstInIdx])
		{
			DPRINT(DC_LOG, "UserId [%u] in FirstIn list Idx %d", userId, firstInIdx);
			if(firstInIdxPtr != NULL)
				 *firstInIdxPtr = firstInIdx;
			return YES;
		}
	}

	DPRINT(DC_LOG, "UserId [%u] not in FirstIn list", userId);
	if(firstInIdxPtr != NULL)
		 *firstInIdxPtr = NOT_IN_FIRST_IN_LIST;
	return NO;
}

//*****************************************************************************
//	IsReaderPresent()
//	Param: 
//		IN:	 category	- card/smartcard/fingerprint reader
//			 respPort	- port (internal/external/any)
//			 mode		- mode (entry/exit/any)
//		OUT: reader index		- if reader found
//			 INAVLID_READER_ID	- if reader not found
//	Returns:
//			YES	- if reader as per given input is present
//			NO	- if reader as per given input is not present
//	Description:
//			It searches for the reader as per given inputs and return reader id.
//*****************************************************************************
BOOL IsReaderPresent(READER_CATEGORY_e category, RESPONSE_PORT_e port, READER_MODE_e mode, READER_ID_e * readerIdPtr)
{
	READER_ID_e readerId;

	// search through all readers
	for(readerId=0; readerId < MAX_READERS; readerId++)
	{
		// If reader port matched
		if( (port == ANY_RESP_PORT) || (port == GetReaderRespPort(readerId)) )
		{
			// if reader mode matched
			if( (mode == ANY_READER_MODE)||(GetReaderEntryExitMode(readerId) == mode) )
			{
				// Based on given reader category, check if reader is present
				switch(category)
				{
					case CARD_READER:
					{
						if( (SysBasicPara.reader[readerId].type == SM_PROX_READER) ||
							(SysBasicPara.reader[readerId].type == HID_PROX_READER) ||
							(SysBasicPara.reader[readerId].type == MIFARE_U_READER) ||
							(SysBasicPara.reader[readerId].type == HID_ICLASS_W_READER) ||
							(SysBasicPara.reader[readerId].type == HID_ICLASS_U_READER) ||
							(SysBasicPara.reader[readerId].type == MIFARE_W_READER)	)
						{
							if(readerIdPtr != NULL)
								*readerIdPtr = readerId;
							return YES;
						}
						break;
					}

					case SMARTCARD_READER:
					{
						if( (SysBasicPara.reader[readerId].type == MIFARE_U_READER)
							|| (SysBasicPara.reader[readerId].type == HID_ICLASS_U_READER) )
						{
							if(readerIdPtr != NULL)
								*readerIdPtr = readerId;
							return YES;
						}
						break;
					}

					case FP_READER:
					{
						if(SysBasicPara.reader[readerId].type == SUP_FP_3030_READER)
						{
							if(readerIdPtr != NULL)
								*readerIdPtr = readerId;
							return YES;
						}
						break;
					}

					case ANY_READER:
					{
						if(SysBasicPara.reader[readerId].type != NO_READER_PRESENT)
						{
							if(readerIdPtr != NULL)
								*readerIdPtr = readerId;
							return YES;
						}					
						break;
					}

					case EXTERNAL_PIN_READER:
					{
						if(SysBasicPara.reader[readerId].type == PIN_W_READER)
						{
							if(readerIdPtr != NULL)
								*readerIdPtr = readerId;
							return YES;
						}
						break;
					}

					default:
						break;
				} // end of SWITCH: reader category
			} // if mode matched
		}
	} // loop until MAX_READERS

	if(readerIdPtr != NULL)
		*readerIdPtr = INVALID_READER;
	return NO;
}

//*****************************************************************************
//	DeleteUserCards
//	Param: 
//		IN :	source		- source- ACMS/WEB JEEVES/SPL FN
//				userIdx		- user index for which cards to be deleted
//		OUT:	none
//	Returns:	none
//	Description:
//			Deletes user cards for given users and saves configuration
//
//*****************************************************************************
void DeleteUserCards(UINT16 userIdx)
{
	UINT8	idx;

	// default card id for given user
	for(idx = 0; idx < MAX_USER_CARD; idx++)	
	{
		SetUserCardId(userIdx, idx, 0);
	}

	WriteSingleUserConfig(userIdx);
}

//*****************************************************************************
//	GetAvailUserFp
//	Param: 
//		IN :	userIdx	- index of user in user configuartion database
//		OUT:	none
//	Returns:	no. of available finger print for enrollment 
//	Description:
//			It gives no. of finger print available for enrollment of given user.
//				
//*****************************************************************************
UINT8 GetAvailUserFp(UINT16 userIdx)
{
	UINT8 	fpIdx;
	UINT8 	fpCnt = 0;

	for(fpIdx = 0; fpIdx < SysAdvPara.maxFingerPerUser; fpIdx++)
	{
		if(IsUserFpPresent(userIdx, fpIdx) == NO)
			fpCnt++;			
	}
	return fpCnt;	
}
//*****************************************************************************
//	GetCurrentUserFp
//	Param:
//		IN :	userIdx	- index of user in user configuartion database
//		OUT:	none
//	Returns:	no. of available finger print for enrollment
//	Description:
//			It gives no. of finger print available for enrollment of given user.
//
//*****************************************************************************
UINT8 GetCurrentUserFp(UINT16 userIdx)
{
	UINT8 	fpIdx;
	UINT8 	fpCnt = 0;

	for(fpIdx = 0; fpIdx < SysAdvPara.maxFingerPerUser; fpIdx++)
	{
		if(IsUserFpPresent(userIdx, fpIdx) == YES)
			fpCnt++;
	}
	return fpCnt;
}

//*****************************************************************************
//	GetAvailUserCard
//	Param: 
//		IN :	userIdx	- index of user in user configuartion database
//		OUT:	none
//	Returns:	no. of available cards for enrollment
//	Description:
//			It gives no. of cards available for enrollment of given user.
//				
//*****************************************************************************
UINT8 GetAvailUserCard(UINT16 userIdx)
{
	UINT8 	cardIdx;
	UINT8 	cardCnt = 0;;

	for(cardIdx = 0; cardIdx < MAX_USER_CARD; cardIdx++)
	{
		if(IsUserCardPresent(userIdx, cardIdx) == NO)
			cardCnt++;			
	}
	return cardCnt;	
}

//*****************************************************************************
//	GetAvailSplFnCard
//	Param: 
//		IN :	splFnIdx	- index of special function
//		OUT:	none
//	Returns:	no. of available cards for enrollment
//	Description:
//			It gives no. of cards available for special function enrollment.
//				
//*****************************************************************************
UINT8 GetAvailSplFnCard(UINT8 splFnIdx)
{
	UINT8 	cardIdx;
	UINT8 	cardCnt = 0;;

	for(cardIdx = 0; cardIdx < MAX_SP_FUNC_CARD; cardIdx++)
	{
		if(IsSplFnCardPresent(splFnIdx, cardIdx) == NO)
			cardCnt++;			
	}
	return cardCnt;	
}

//*****************************************************************************
//	IsUserCardPresent
//	Param: 
//		IN :	userIdx	- user index
//				cardIdx	- card index
//		OUT:	none
//	Returns:	YES if present, else NO
//	Description:
//			It checks if card for given user is present.
//				
//*****************************************************************************
BOOL IsUserCardPresent(UINT16 userIdx, UINT8 cardIdx)
{
	if(GetUserCardId(userIdx, cardIdx) == 0)
		return NO;
	return YES;		
}

//*****************************************************************************
//	IsSplFnCardPresent
//	Param: 
//		IN :	splFnIdx	- special function index
//				cardIdx		- card index
//		OUT:	none
//	Returns:	YES if present, else NO
//	Description:
//			It checks if card for given special function is present.
//				
//*****************************************************************************
BOOL IsSplFnCardPresent(UINT8 splFnIdx, UINT8 cardIdx)
{
	if(SpecialFunctions[splFnIdx].cardId[cardIdx] == 0)
		return NO;
	return YES;
}

//*****************************************************************************
//	RemoveUserFromSystem
//	Param: 
//		IN :	userIdx	- user index
//		OUT:	none
//	Returns:	none
//	Description:
//			It removes user configuration from system and restores policies 
//			applied on it.
//
//*****************************************************************************
void RemoveUserFromSystem(UINT16 userIdx)
{
	DeleteUserFp(userIdx);
	RestoreBlockedUser(userIdx);
	DefltSingleAbsenteeRecord(userIdx);
	RemoveUserFromApbList(userIdx, USER_REMOVED_FROM_SYS);
	DeleteSingleUserPhoto(GetUserId(userIdx));
	SetProfilePhotoSyncFlag(DISABLED, userIdx);
	
	//Intimate HTTP FTP FSM to stop
	RemoveHttpFSMReq(userIdx,MAX_REQ_TYPE);
	BreakOperationEventData_t* evtPtr = calloc(1, sizeof(BreakOperationEventData_t));
	if(evtPtr != NULL)
	{
	    evtPtr->userIdx = userIdx;
	    evtPtr->reqType = MAX_REQ_TYPE;
	    InsertHttpReqEvent(UF_BreakOperationEvent, evtPtr);
	}
	// Clear use count and attempt count if last user processed by DC task is the same 
	if(userIdx == DcInfo.userIdx)
	{
		DcInfo.useCount = 0;
	}
	
	SetUserInOutStatus(userIdx, USER_IS_OUT);
	LoadDfltSingleUserConfig(userIdx);
	LoadDfltSingleUserAttndnsDetailConfig(USER_ATNDNC_MNTH_CRRNT, userIdx);
	LoadDfltSingleUserAttndnsDetailConfig(USER_ATNDNC_MNTH_PRVS, userIdx);
	LoadDfltSingleUserLeavveDetailConfig(userIdx);
}

//******************************************************************************
//	CreateTestUser()
//	Param:
//		IN :	index - User Index
//		OUT:    None
//	Returns:
//				None
//	Description:
//				It create a test User for on given index
//******************************************************************************
void CreateTestUser(UINT16 index)
{
	SetUserConfig(index, DfltTestUserConfig);
	if(WriteSingleUserConfig(index) == FAIL)
	{
		EPRINT(GENERAL_LOG, "Failed to create test User");
	}
}

//*****************************************************************************
// GetSM125DefltRadfValue()
// Param:       None
// Returns:     SUCCESS/FAIL
// Description: It writes Finger Print Reader related configuration from
//				non-volatile memory.
//*****************************************************************************
void GetSM125DefltRadfValue(void)
{
	ReadersParaConfig = DfltReadersParaConfig;
	// For Combo Exit reader EM prox value
	if(SysBasicPara.cerPresentF == TRUE)
	{
		ReadersParaConfig.sm125Radf[READER3] = DEFLT_SM125_RADF_VALUE_CER_READER;
	}
}

//*****************************************************************************
// GetDoorApplType()
// Param:       NINE
// Returns:     DOOR_APPL_TYPE_e
// Description:
//			This function return door application type, panel door or direct door.
//*****************************************************************************
DOOR_APPL_TYPE_e GetDoorApplType(void)
{
	return DoorApplType;
}

//******************************************************************************
//	ReadDoorApplType()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				NONE
//	Description:
//				This function get door application type from signature and upadate
//			into ram copy
//******************************************************************************
void ReadDoorApplType(void)
{
	FILE 	*fp;

	fp = fopen(DOOR_APPL_TYPE_SIGNATURE_FILE, "r");
	if(fp != NULL)
	{
		DoorApplType = PANEL_DOOR_APPL;
		fclose(fp);
	}
	else
	{
		DoorApplType = DIRECT_DOOR_APPL;
	}
}

//******************************************************************************
//	SetDoorApplType()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				NONE
//	Description:
//				This function set door application type into signature
//******************************************************************************
void SetDoorApplType(DOOR_APPL_TYPE_e doorApplType)
{
	char	sysBuff[100];

	if(DoorApplType != doorApplType)
	{
		if(doorApplType == PANEL_DOOR_APPL)
		{
			sprintf(sysBuff, "touch %s", DOOR_APPL_TYPE_SIGNATURE_FILE);

			// If Wifi IP assignment is dynamic then change it to static.
			// Load Default IP address
			// Write Wifi Config
			if(WifiParameters.nwType == IP_DYNAMIC)
			{
				pthread_mutex_lock(&MutexLockAutoSwitchWiFi);

				WifiParameters.nwType = IP_STATIC;
				strcpy(WifiParameters.ipAddress, DfltWifiPara.ipAddress);
				strcpy(WifiParameters.subnet, DfltWifiPara.subnet);
				strcpy(WifiParameters.gateway, DfltWifiPara.gateway);
				strcpy(WifiParameters.dns1, DfltWifiPara.dns1);
				strcpy(WifiParameters.dns2, DfltWifiPara.dns2);

				pthread_mutex_unlock(&MutexLockAutoSwitchWiFi);
				WriteWifiParaConfig();
			}

			if( EthernetParameters.nwType == LAN_IP_DYNAMIC )
			{
				EthernetParameters.nwType = LAN_IP_STATIC;
				WriteEthParaConfig();
			}

		}
		else
		{
			sprintf(sysBuff, "rm -f %s", DOOR_APPL_TYPE_SIGNATURE_FILE);
		}
		system(sysBuff);

		// Write system DefaultF
		if(WriteDfltSysFlag(SYS_DFLT_BASIC_WITH_DOOR_MODE_CHNG) == FAIL)
		{
			EPRINT(SYS_LOG, "Failed to default System while changing door mode");
		}


		SetSysRebootTimer(SYS_REBOOT_TMR_CNT);
	}
}

//*****************************************************************************
//	GetFreeUserFpIdx()
//	Param:
//		IN : NONE
//		OUT: userIdx		- user index for which user id matched
//			 INVALID_USER 	- if no match found
//	Returns:
//			YES	- if free user index is found
//			NO	- if free user index is not found
//	Description:
//			This function checks whether if any user config is available or not.
//			If updates free user index at given pointer.
//*****************************************************************************
BOOL GetFreeUserFpIdx(UINT16 userIdx, UINT8PTR fpIdxPtr)
{
	UINT8 idx;

	for(idx = 0; idx < SysAdvPara.maxFingerPerUser; idx++)
	{
		if(IsUserFpPresent(userIdx, idx) == NO)
		{
			*fpIdxPtr = idx;
			return YES;
		}
	}
	return NO;
}

//*****************************************************************************
//	GetSysBasicConfigWorkingCopy()
//	Param:
//		IN :	None
//		OUT: 	None
//	Returns:
//		None
//	Description:
//		This function will get local copy
//*****************************************************************************
void GetSysBasicConfigWorkingCopy(SYS_BASIC_PARA_t * tempSysBasicPara)
{
	if(tempSysBasicPara == NULL)
	{
		return;
	}

	(*tempSysBasicPara) = SysBasicPara;
	SetComboRdrCfgType(READER3,&SysBasicPara,tempSysBasicPara);
}

//******************************************************************************
//	SetComboRdrCfgType()
//	Param:
//		IN :	readerId - Reader Id
//		IN :	srcSysBasicPara - Source pointer to SYS_BASIC_PARA_t
//		IN :	destSysBasicPara - Destination pointer to SYS_BASIC_PARA_t
//		OUT:    destSysBasicPara - Destination pointer to SYS_BASIC_PARA_t
//	Returns:
//				FAIL / SUCCESS
//	Description:
//				This function will update reader type based on combo reader flag
//
//******************************************************************************
BOOL SetComboRdrCfgType(const READER_ID_e readerId, const SYS_BASIC_PARA_t * const srcSysBasicPara ,SYS_BASIC_PARA_t * destSysBasicPara)
{
	//Validate Input parameters
	if((NULL == srcSysBasicPara) || (NULL == destSysBasicPara))
	{
		EPRINT(CONFIG_LOG,"SourceSysBasicPara/DestSysBasicPara is NULL");
		return FAIL;
	}

	if(readerId >= MAX_READERS)
	{
		EPRINT(CONFIG_LOG,"Invalid Parameters received readerId[%d]",readerId);
		return FAIL;
	}

	//Update Required Data
	if(srcSysBasicPara->cerPresentF == TRUE)
	{
		destSysBasicPara->reader[readerId].type = COMBO_EXIT_READER;
	}
	else if(srcSysBasicPara->cbrPresentF == TRUE)
	{
		destSysBasicPara->reader[readerId].type = CB_U_READER;
	}
	else if(srcSysBasicPara->atomRD100PresentF == TRUE)
	{
		destSysBasicPara->reader[readerId].type = ATOM_RD100_READER;
	}
	else if(srcSysBasicPara->atomRD200PresentF == TRUE)
	{
		destSysBasicPara->reader[readerId].type = ATOM_RD200_READER;
	}
	else if(srcSysBasicPara->atomRD300PresentF == TRUE)
	{
		destSysBasicPara->reader[readerId].type = ATOM_RD300_READER;
	}
	return SUCCESS;
}

//******************************************************************************
//	GetCosecDeviceCode()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				NONE
//	Description:
//				This function returns device code used in ACMS or other 
//				communication and can be used for device identification.
//******************************************************************************
UINT8 GetCosecDeviceCode(void)
{
	if(SamayModeEnable == TRUE)
	{
		return ACCESS_CONTROL_SAMAY_DEVICE_CODE;
	}
	else
	{
		return ACCESS_CONTROL_DEVICE_CODE;
	}
}

//******************************************************************************
//	GetVamConfigIndex()
//	Param:
//		IN :	pass Id
//		OUT:    index- where passId matched or to be stored
//	Returns:
//				SUCCESS/FAIL
//	Description:
//				This function checks if pass id matches with any config if yes
//				assign config index if no find empty index and assign it. If
//				empty index is not available return fail
//******************************************************************************
BOOL GetVamConfigIndex(UINT32 passId, UINT16PTR configIdx)
{
	BOOL	blankIndexFoundF = FALSE;
	UINT16	index, blankIndex;

	//check if pass id matched and find blankIndex
	for(index = 0; index < MAX_VAM_CONFIG; index++)
	{
		//if pass id match assign to configIdx
		if(passId == VamConfig[index].passId)
		{
			*configIdx = index;
			break;
		}

		//assign first blank index to blankindex
		if( (blankIndexFoundF == FALSE) && (VamConfig[index].passId == DfltVamConfig.passId) )
		{
			blankIndex = index;
			blankIndexFoundF = TRUE;
		}
	}

	//if pass id don't match assign blankIndex to configIdx
	if(index >= MAX_VAM_CONFIG)
	{
		//if blankIndex and passId don't found return FAIL
		if(blankIndexFoundF == FALSE)
		{
			*configIdx = MAX_VAM_CONFIG;
			return FAIL;
		}
		else
		{
			*configIdx = blankIndex;
		}
	}

	DPRINT(CONFIG_LOG, "VAM config index[%u]", *configIdx);
	return SUCCESS;
}

//******************************************************************************
//	IsVamCard()
//	Param:
//		IN :	UINT64 cardId
//		OUT:    BOOL
//	Returns:
//				SUCCESS/FAIL
//	Description:
//				This function checks if cardId matches with vehicleCard or
//				driverCard or helperCard of vam configuration if yes it
//				return YES and assigns index of config to DcInfo.vamConfigIndex
//				if no it returns NO
//******************************************************************************
BOOL IsVamCardValid(UINT64 cardId)
{
	UINT16		passIdx;
	UINT8		cardIdx;

	if(GetVamCardCount() == 0)
	{
		for(passIdx = 0; passIdx < MAX_VAM_CONFIG; passIdx++)
		{
			for(cardIdx = 0; cardIdx < MAX_VAM_CARD_TYPE; cardIdx++)
			{
				if(VamConfig[passIdx].vamCardId[cardIdx] == cardId)
				{
					DcInfo.vamConfigIndex = passIdx;
					DcInfo.detectedVamCardType = cardIdx;
					DcInfo.detectedVamCards[cardIdx] = TRUE;
					return YES;
				}
			}
		}
	}
	else
	{
		for(cardIdx = 0; cardIdx < MAX_VAM_CARD_TYPE; cardIdx++)
		{
			if(VamConfig[DcInfo.vamConfigIndex].vamCardId[cardIdx] == cardId)
			{
				if(DcInfo.detectedVamCards[cardIdx] == FALSE)
				{
					DcInfo.detectedVamCardType = cardIdx;
					DcInfo.detectedVamCards[cardIdx] = TRUE;
					return YES;
				}
			}
		}
	}

	return NO;
}

//******************************************************************************
//	StringReverse()
//	Param:
//		IN :
//		OUT:
//	Returns:
//
//	Description:
//
//******************************************************************************
void StringReverse(CHARPTR string)
{
	UINT8	idx1, idx2;
	CHAR	ch;
	
	idx2 = strlen(string);
	if(idx2 > 0)
	{
		idx2 -= 1;

		for(idx1 = 0; idx1 < idx2; idx1++,idx2--)
		{
			ch = string[idx1];
			string[idx1] = string[idx2];
			string[idx2] = ch;
		}
	}
}
//******************************************************************************
//	GetComboRdrPresentCfg()
//	Param:
//		IN : combo readerId
//		OUT:
//	Returns: status
//
//	Description:
//
//******************************************************************************
BOOL GetComboRdrPresentCfg(const EXT_COMBO_READER_e readerId)
{
	BOOL status = UNKNOWN;

	if(readerId >= COMBO_READER_MAX)
		return status;

	switch(readerId)
	{
		case COMBO_READER_CER:
			status = SysBasicPara.cerPresentF;
			break;

		case COMBO_READER_CB_U:
			status = SysBasicPara.cbrPresentF;
			break;

#ifdef ATOM_READER_SUPPORTED
		case COMBO_READER_ATOM_RD100:
			status = SysBasicPara.atomRD100PresentF;
			break;

		case COMBO_READER_ATOM_RD200:
			status = SysBasicPara.atomRD200PresentF;
			break;

		case COMBO_READER_ATOM_RD300:
			status = SysBasicPara.atomRD300PresentF;
			break;
#endif //#ifdef ATOM_READER_SUPPORTED

		default:
			status = UNKNOWN;
	}
	return status;
}

BOOL GetBuzMuteCfg(void)
{
	return (SysBasicPara.buzMuteF);
}

BOOL GetSmartRouteAccessEnblCfg(void)
{
	return (SysAdvPara.smartRouteAccessEnbl);
}

READER_TYPE_e GetReaderTypeCfg(READER_ID_e readerId)
{
	return (SysBasicPara.reader[readerId].type);
}

void SetReaderTypeCfg(READER_ID_e readerId, READER_TYPE_e readerType)
{
	SysBasicPara.reader[readerId].type = readerType;
}

BOOL GetAddlSecCodeEnblCfg(void)
{
	return (SysAdvPara.addlSecurity.enable);
}

BOOL GetFacilityCodeEnblCfg(void)
{
	return (SysAdvPara.facilityCodeCheckEnableF);
}

UINT16PTR GetFacilityCodeCfgPtr(void)
{
	return &(SysAdvPara.facilityCode);
}

UINT16PTR GetAddlSecCodeCfgPtr(void)
{
	return &(SysAdvPara.addlSecurity.code);
}

BOOL GetHidIclassCustomKeyEnblCfg(void)
{
	return (SysAdvPara.hidIclasCustomKeyEnbl);
}

UINT8PTR GetHidIclassCustomKeyCfgPtr(void)
{
	return (&SysAdvPara.hidIclasCustomKey[0]);
}

BOOL GetMifareCustomKeyEnblCfg(void)
{
	return (SysAdvPara.mifareCustomKeyEnbl);
}

UINT8PTR GetMifareCustomKeyCfgPtr(void)
{
	return (&SysAdvPara.mifareCustomKey[0]);
}

BOOL GetDesfireCustomKeyEnblCfg(void)
{
	return (SysAdvPara.desfireCustomKeyEnbl);
}

UINT8PTR GetDesfireCustomKeyCfgPtr(void)
{
	return (&SysAdvPara.desfireCustomKey[0]);
}

UINT16 GetCustomKeyAutoUpdateEnblCfg(void)
{
	return (SysAdvPara.customKeyAutoUpdateF);
}

UINT16 GetUHFTagRedetectDelayCfg(void)
{
	return (SysBasicPara.tagRedetectDelay);
}

//******************************************************************************
//	GetJcmConfigIdxFromRefId()
//	Param:
//		IN :	ref Id
//		OUT:    index- where refId matched or to be stored
//	Returns:
//				SUCCESS/FAIL
//	Description:
//				This function checks if ref id matches with any config if yes
//				assign config index if no find empty index and assign it. If
//				empty index is not available return fail
//******************************************************************************
BOOL GetJcmConfigIdxFromRefId(UINT32 refId, UINT16PTR configIdxPtr)
{
	UINT16		idx, blankConfigIdx = INVALID_JCM_CONFIG_INDEX;

	// to get config index we need to search through config list and match reference Id or find free index
	for(idx=0; idx < MAX_JCM_CONFIG; idx++)
	{
		if( JcmConfig[idx].refId == refId )
		{
			//if ref Id matches
			*configIdxPtr = idx;
			break;
		}
		else if( (blankConfigIdx == INVALID_JCM_CONFIG_INDEX) && (JcmConfig[idx].refId == INVALID_JOB_REF_ID) )
		{
			//remember first free config Idx in case ref ID not matched with any job
			blankConfigIdx = idx;
		}
	}

	//if ref ID not matched with any job but there's free index where job can be stored
	if((idx >= MAX_JCM_CONFIG))
	{
		if(blankConfigIdx != INVALID_JCM_CONFIG_INDEX)
		{
			//update configIdx with free index
			*configIdxPtr = blankConfigIdx;
		}
		else
		{
			return FAIL;
		}
	}

	return SUCCESS;
}

//*****************************************************************************
//	GetGreetingMsg()
//	Param:
//		IN : NONE
//		OUT: NONE.
//	Returns:
//			 pointer to the message string if all condition satisfied otherwise NULL.
//	Description:
//			 This function wiil check the eligibility of greeting message and it will
//		return pointer to the greeting message string or NULL.
//*****************************************************************************
CHARPTR GetGreetingMsg(UINT16 userIdx)
{
	UINT8	greetingIdx;
	DATE_DDMMYY_t	tempDate;

	if(SysAdvPara.enblGreetingsF == TRUE)
	{
		if((userIdx < (MAX_USERS - 1)) && (IsBirthdayMsgEnbl(userIdx) == TRUE))
		{
			DATE_DDMMYY_t tDateOfBirth = GetDateOfBirth(userIdx);
			//check dob of user for greeting msg
			tempDate.day	= tDateOfBirth.day;
			tempDate.month	= tDateOfBirth.month;
			tempDate.year	= CurrTimeDate.tm_year - ADJUST_TM_YEAR;

			if(IsDateInGivenWindow(&CurrTimeDate, tempDate, tempDate) == YES)
			{
				return SysAdvPara.birthdayMessage;
			}
		}

		for(greetingIdx = 0; greetingIdx < MAX_GREETING_CONFIG; greetingIdx++)
		{
			if(IsCurrTimeInGivenWindow(SysAdvPara.greetingConfig[greetingIdx].startTime,
									SysAdvPara.greetingConfig[greetingIdx].endTime) == YES)
			{
				if(strlen(SysAdvPara.greetingConfig[greetingIdx].message) > 0)
				{
					return SysAdvPara.greetingConfig[greetingIdx].message;
				}
			}
		}
	}

	return NULL;
}
//*****************************************************************************
//	SetCadenceDuration()
//	Param:
//		IN	: cadDuration
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API sets cadence duration as per configuration
//*****************************************************************************
BOOL SetCadenceDuration(CAD_DURATION_e accessAllowCadDuration, CAD_DURATION_e accessaDenyCadDuration)
{

	//{.led = ENABLED,	.buzzer = ENABLED,	.onOff = {1200, 0}, .cycles = 1}, // LED_AND_BUZ_CAD_SUCCESS
	//{.led = ENABLED,	.buzzer = ENABLED,	.onOff = {200, 200}, .cycles = 3}, // LED_AND_BUZ_CAD_DENY

	if(accessAllowCadDuration == CADENCE_SHORT)
	{
		BuzzLedCadPara[LED_AND_BUZ_CAD_SUCCESS].cadBits = SUCCESS_CAD_ON_SHORT;
	}
	else if(accessAllowCadDuration == CADENCE_MEDIUM)
	{
		BuzzLedCadPara[LED_AND_BUZ_CAD_SUCCESS].cadBits = SUCCESS_CAD_ON_MEDIUM;
	}
	else if(accessAllowCadDuration == CADENCE_SHORTEST)
	{
		BuzzLedCadPara[LED_AND_BUZ_CAD_SUCCESS].cadBits = SUCCESS_CAD_ON_SHORTEST;
	}
	else
	{
		BuzzLedCadPara[LED_AND_BUZ_CAD_SUCCESS].cadBits = SUCCESS_CAD_ON_LONG;
	}

	if(accessaDenyCadDuration == CADENCE_SHORT)
	{
		BuzzLedCadPara[LED_AND_BUZ_CAD_DENIED].cycles =	FAIL_CAD_CYCLES_SHORT;
	}
	else if(accessaDenyCadDuration == CADENCE_MEDIUM)
	{
		BuzzLedCadPara[LED_AND_BUZ_CAD_DENIED].cycles =	FAIL_CAD_CYCLES_MEDIUM;
	}
	else if(accessaDenyCadDuration == CADENCE_SHORTEST)
	{
		BuzzLedCadPara[LED_AND_BUZ_CAD_DENIED].cycles =	FAIL_CAD_CYCLES_SHORT;
	}
	else
	{
		BuzzLedCadPara[LED_AND_BUZ_CAD_DENIED].cycles =	FAIL_CAD_CYCLES_LONG;
	}

	//default data
	BuzzLedCadPara[LED_AND_BUZ_CAD_DENIED].cadBits = FAIL_CAD_DURATION;
	BuzzLedCadPara[LED_AND_BUZ_CAD_SUCCESS].cycles = SUCCESS_CAD_CYCLES;

	if(accessaDenyCadDuration == CADENCE_SHORTEST)
	{
		BuzzLedCadPara[LED_AND_BUZ_CAD_DENIED].cadBits = FAIL_CAD_DURATION_SHORTEST;
	}
	SetBuzzerLedCadDuration(LED_AND_BUZ_CAD_SUCCESS);
	SetBuzzerLedCadDuration(LED_AND_BUZ_CAD_DENIED);

	//send cad duration info to CER
	SendCerCadDuration();
	return SUCCESS;
}

//*****************************************************************************
//	GetAccesAllowCadDuration()
//	Param:
//		IN	: NONE
//		OUT	: NONE
//
//	Returns:
//		CAD DURATION
//	Description:
//		This API gets cadence duration  value as per configuration
//*****************************************************************************
CAD_DURATION_e GetAccesAllowCadDuration(void)
{
	return (SysBasicPara.accessAllowCadDuration);
}

//*****************************************************************************
//	GetAccesDenyCadDuration()
//	Param:
//		IN	: NONE
//		OUT	: NONE
//
//	Returns:
//		CAD DURATION
//	Description:
//		This API gets  accessDeny cadence duration of value as per configuration
//*****************************************************************************
CAD_DURATION_e GetAccesDenyCadDuration(void)
{
	return (SysBasicPara.accessDenyCadDuration);
}



//******************************************************************************
//	GetReaderRespPortQr()
//	Param:
//		IN :	doorId - Door Id
//				readerId - reader Id
//		OUT:    None
//	Returns:
//				Response port
//	Description:
//				It returns Device type from panel type
//******************************************************************************
RESPONSE_PORT_e	GetReaderRespPortQr(READER_MODE_e readerMode)
{
	switch(readerMode)
		{
			case ENTRY_READER:
				if(GetReaderEntryExitMode(READER1) == ENTRY_READER)
				{
					return INTERNAL_PORT;
				}
				else if(GetReaderEntryExitMode(READER3) == ENTRY_READER)
				{
					return EXTERNAL_PORT;
				}
				else
				{
					return MAX_RESP_PORT;
				}
				break;

			case EXIT_READER:
				if(GetReaderEntryExitMode(READER1) == EXIT_READER)
				{
					return INTERNAL_PORT;
				}
				else if(GetReaderEntryExitMode(READER3) == EXIT_READER)
				{
					return EXTERNAL_PORT;
				}
				else
				{
					return MAX_RESP_PORT;
				}
				break;

			default:
				return MAX_RESP_PORT;
				break;
		}

}

//*****************************************************************************
//	GetAlphaNumIdForSIUser()
//	Param:
//		IN : none
//		OUT: 	alphaNumId	- user Alphanumeric User Id
//	Returns:
//		YES 	- If SSA user is in process and matching user found
//		NO	 	- If no matching user found
//	Description:
//		This function will find Alphanumeric User ID for SI user
//*****************************************************************************
BOOL GetAlphaNumIdForSIUser(CHARPTR alphaNumId)
{
	BOOL status = FAIL;

	if((DcInfo.procSSAF == TRUE) || (DcInfo.sendAphanumIdInEvent == TRUE))
	{
		strncpy(alphaNumId, GetAlphaNumId(SSA_USER_IDX), EVT_ALPHANUM_ID_SIZE);
		status = SUCCESS;
	}
	return status;
}

//******************************************************************************
//	IsServerModeVyom()
//	Param:
//		IN :	NONE
//		OUT:    YES/NO
//	Returns:
//				NONE
//	Description:
//				This function returns YES if device server mode is VYOM
//				else it returns NO
//******************************************************************************
BOOL IsServerModeVyom(void)
{
	if(ServerModeCfg.serverMode == COSEC_VYOM)
	{
		return YES;
	}

	return NO;
}

//******************************************************************************
//	GetServerMode()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				NONE
//	Description:
//				This function returns device code used in ACMS or other
//				communication and can be used for device identification.
//******************************************************************************
UINT8 GetServerMode(void)
{
	return ServerModeCfg.serverMode;
}

//******************************************************************************
//	IsServerModeThirdParty()
//	Param:
//		IN :	NONE
//		OUT:    YES/NO
//	Returns:
//				NONE
//	Description:
//				This function returns YES if device server mode is THIRD PARTY
//				else it returns NO
//******************************************************************************
BOOL IsServerModeThirdParty(void)
{
#ifdef PUSH_API_SUPPORTED
	if(ServerModeCfg.serverMode == COSEC_THIRD_PARTY)
	{
		return YES;
	}
#endif

	return NO;
}

//******************************************************************************
//	GetUserInOutStatus()
//	Param:
//		IN :	NONE
//		OUT:    YES/NO
//	Returns:
//				NONE
//	Description:
//				This function returns YES if device server mode is VYOM
//				else it returns NO
//******************************************************************************
USER_IN_OUT_STATUS_e GetUserInOutStatus(UINT32 userIdx)
{
	if(userIdx <= MAX_USERS)
	{
		if(UserInOutStatus[(userIdx/8)] & (1 << ((userIdx%8))))
		{
			DPRINT(GENERAL_LOG,"userIs out useridx %u", userIdx);
			return USER_IS_OUT;
		}
		else
		{
			DPRINT(GENERAL_LOG,"userIs in useridx %u", userIdx);
			return USER_IS_IN;
		}
	}
	else
	{
		return USER_IS_OUT;
	}
}

//******************************************************************************
//	SetUserInOutStatus()
//	Param:
//		IN :	NONE
//		OUT:    YES/NO
//	Returns:
//				NONE
//	Description:
//				This function returns YES if device server mode is VYOM
//				else it returns NO
//******************************************************************************
BOOL SetUserInOutStatus(UINT32 userIdx, USER_IN_OUT_STATUS_e status)
{
	if(userIdx >= MAX_USERS)
	{
		return FAIL;
	}

	if(status == USER_IS_IN)
	{
		DPRINT(GENERAL_LOG,"set user status in useridx %u", userIdx);
		UserInOutStatus[(userIdx/8)] = ( (UserInOutStatus[(userIdx/8)]) & (~(1 << (userIdx%8))) );
	}
	else if(status == USER_IS_OUT)
	{
		DPRINT(GENERAL_LOG,"set user status out useridx %u", userIdx);
		UserInOutStatus[(userIdx/8)] = ( (UserInOutStatus[(userIdx/8)]) | (1 << (userIdx%8)) );
	}
	else
	{
		return FAIL;
	}

	return SUCCESS;
}

//******************************************************************************
//	ChngReqConfigForVariantWoIntFPReader()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				NONE
//	Description:
//				This function will change configurations and other required changes
//				for ARGO CAM or CAI Variant without internal FP reader
//******************************************************************************
BOOL ChngReqConfigForVariantWoIntFPReader(VOID)
{
	if(YES == IsVariant_With_Internal_FP_Rdr()) // Returning  in case other variant than CAM or CAI or CAE
	{
		return SUCCESS;
	}

	SYS_BASIC_PARA_t	tempSysBasicPara;
	SYS_ADVANCE_PARA_t	tempSysAdvPara;
	UINT16 				configIdx = 0;
	BOOL				changeBasicF = FALSE;
	BOOL				changeAdvF = FALSE;
	BOOL				status = SUCCESS;
	
	// Get current configuration
	GetSysBasicConfigWorkingCopy(&tempSysBasicPara);
	memcpy((VOIDPTR)&tempSysAdvPara,(VOIDPTR)&SysAdvPara,sizeof(tempSysAdvPara));

	if(SUP_FP_3030_READER == tempSysBasicPara.reader[READER2].type)
	{
		tempSysBasicPara.reader[READER2].type = NO_READER_PRESENT;
		changeBasicF = TRUE;
	}

	if((ENROLL_BIOMETRIC_ONLY == tempSysAdvPara.enrollmentType) ||
	   (ENROLL_BIOMETRIC_THEN_CARD == tempSysAdvPara.enrollmentType))
	{
		tempSysAdvPara.enrollmentType = ENROLL_READ_ONLY_CARD;
		changeAdvF = TRUE;
	}

	if(TRUE == changeBasicF)
	{
		if(ConfigUpdateHandler(GEN_SYS_BASIC_TABLE_ID, configIdx, (VOIDPTR)(&tempSysBasicPara)) == FAIL)
		{
			status = FAIL;
		}
	}

	if(TRUE == changeAdvF)
	{
		if(ConfigUpdateHandler(GEN_SYS_ADV_TABLE_ID, configIdx, (VOIDPTR)(&tempSysAdvPara)) == FAIL)
		{
			status = FAIL;
		}
	}

	return status;
}

BOOL IsStringNull(char *pInStr, UINT8 strLen)
{
	UINT8 tCnt = 0;
	while (strLen--)
	{
		if ('\0' != pInStr[tCnt++])
		{
			return (FALSE);
		}
	}
	return (TRUE);
}


BOOL DecryptPswdFromWebGUI(char *pInStr, char *pOutStr, unsigned long encslength, int EncryptPswdId)
{
	AES_KEY dec_key;
	char tBuff[100];

	if (TRUE == IsStringNull(pInStr, encslength))
	{
		return (FALSE);
	}

	/* Get string to be decrypted with its complete block size */
	memset(tBuff, 0, 100);
	memcpy(tBuff, pInStr, encslength);

	AES_set_decrypt_key((const unsigned char *)gWebEncryptPswdInfo[EncryptPswdId].EncryptPswd, 128, &dec_key);
	AES_cbc_encrypt((const unsigned char *)tBuff, (unsigned char *)pOutStr, encslength, &dec_key, (unsigned char *)gWebEncryptPswdInfo[EncryptPswdId].EncryptInitVector, AES_DECRYPT);

	memset(gWebEncryptPswdInfo[EncryptPswdId].EncryptPswd, '\0', sizeof(gWebEncryptPswdInfo[EncryptPswdId].EncryptPswd));
	memset(gWebEncryptPswdInfo[EncryptPswdId].EncryptInitVector, '\0', sizeof(gWebEncryptPswdInfo[EncryptPswdId].EncryptInitVector));

	return (TRUE);
}
