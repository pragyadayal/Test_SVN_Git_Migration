////////////////////////////////////////////////////////////////////////
//
//   MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//   MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//   MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
//   MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
//   MM       MM   AA       AA       TT      RR  RR       II      XX XX
//   MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//   Company      : Matrix ComSec Pvt. Ltd., Baroda, India.
//   Project      : Access Control System
//   Created By   : Kaival Shah
//   File         : MxSysTest.c
//   Description  : This file implements logic related to system testing.
/////////////////////////////////////////////////////////////////////////////

//*** Place all include files here ***
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "MxTypedef.h"
#include "MxSysCommonDef.h"
#include "MxSysTimer.h"
#include "MxConfig.h"
#include "MxSlaveCommonMsgList.h"
#include "MxSlaveMsg.h"
#include "MxSysTest.h"
#include "MxDcTask.h"
#include "MxLogs.h"
#include "MxEnrollment.h"
#include "MxSysUtils.h"
#include "MxGpio.h"
#include "Text2Pdf.h"
#include "MxUserConfig.h"
#include "MxWebJvsAutoSystemTest.h"

//******** Extern Variables **********
extern CHARPTR 					IoStatus[];
extern DOOR_ACTIVITY_INFO_t		DcInfo;
extern SYS_BASIC_PARA_t			SysBasicPara;
extern SYS_ADVANCE_PARA_t		SysAdvPara;
extern INPUT_STATUS_t			SysInputStatus;
extern ETHERNET_PARA_t			EthernetParameters;
extern MMC_INFO_t				MmcInfo;
extern UINT32					SfprSensorSerialNumber;
extern AUTO_SYSTEM_TEST_PARA_t		AutoSystemTestPara;
extern const CHARPTR DeviceVariantType[MAX_ARGO_VARIANT_TYPE+1];
//******** Defines and Data Types ****

#define SYS_TEST_FAIL_TIMEOUT			2		//in second
#define TWO_STATE_OUTPUT_CHANGE_TIME	3		//in second
#define SYSTEM_REBOOT_TEST_TIMER_CNT	1800	//3*60*10 count in 100 mili sec resolution


#define FOR_STAT_IN_TEST_SEQ_STAT	6
#define TWO_STAT_IN_TEST_SEQ_STAT	4

#define KERNEL_FS_FILE_NAME_INITIAL		"COSEC_GATE_DD_kernel_fs"
#define MAX_KERNEL_FS_FILE_LIST			5
#define	KERNEL_FS_FILE_NAME_SIZE		40

//******** Function Prototypes *******

static void MonitorFourStateInput(IO_STATUS_e *input, SYSTEM_TEST_STATES_e testState);
static void MonitorTwoStateInput(IO_STATUS_e *input, SYSTEM_TEST_STATES_e testState);
static BOOL StartTwoStateOutputTimer(UINT8 outputCnt);
static void TwoStateOutputTimeOut(UINT32 outputCnt);
static void SwitchToIdle(void);
static BOOL ReadSysTestPara(void);
static BOOL LoadDfltSysTestPara(void);
static void SystemTestTimeOut(UINT32 dummy);
static BOOL SystemTestTimer(void);
static void UpdateTestResult(BOOL result);
static BOOL WriteSysRebootCnt(void);

//******** Global Variables **********
SYSTEM_TEST_PARA_t			SystemTestPara;
SYSTEM_REBOOT_TEST_PARA_t	SystemRebootTestPata;
SYSTEM_TEST_STATUS_e		TestStatusF = SYSTEM_TEST_STATUS_IDLE;

//******** Static Variables **********
static SYSTEM_TEST_STATES_e		systemTeststate = SYSTEM_TEST_IDLE;
static UINT8					expectedInput;
static UINT8					wrongInputCnt;
static TIMER_HANDLE				twoStateOutTmrHandle = INVALID_TIMER_HANDLE;
static TIMER_HANDLE				systemTestTmrHandle = INVALID_TIMER_HANDLE;

static const SYSTEM_TEST_PARA_t  dfltSysTestPara =
{
	.empName = "",
	.empId = "",
	.devSrNo = "",
	.mainPcbDesign = "",
	.connPcbDesign = "",
	.testDate = "",
	.ftpIpaddr = "",
	.ftpUsrName = "",
	.ftpPwd = "",
	.macAddrResult = FAIL,
	.voltResult = {FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL},
	.rtcFreqResult = FAIL,
	.cardRdrRange = {"", "", "", "", "", ""},
	.cardRdrResult = {FAIL, FAIL, FAIL, FAIL, FAIL, FAIL},
	.intFingerRdrResult = FAIL,
	.extFingerRdrResult = FAIL,
	.genericIoResult = {FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL, FAIL},
	.usrEnrlResult = FAIL,
	.sysDfltResult = FAIL,
	.wifiResult = FAIL,
	.asmblerName = "",
	.asmblerId = "",
	.detectedCardId = {0, 0, 0, 0, 0, 0},
	.sysUpgrdFileInMmcRes = FAIL,
	.upgrdFileDetail = "",
	.bluetoothResult = FAIL,
};

static const IO_STATUS_e testSeqFourStateInput[FOR_STAT_IN_TEST_SEQ_STAT] =
{
	IO_INACTIVE,
	IO_OPEN,
	IO_INACTIVE,
	IO_SHORT,
	IO_INACTIVE,
	IO_ACTIVE,
};

static const IO_STATUS_e testSeqTwoStateInput[TWO_STAT_IN_TEST_SEQ_STAT] =
{
	INPUT_INACTIVE,
	INPUT_ACTIVE,
	INPUT_INACTIVE,
	INPUT_ACTIVE,
};

static const CHARPTR resultStr[2] =
{
	"FAIL",
	"SUCCESS",
};

//******** Function Definitions ******

//*****************************************************************************
//	DcSystemTestStateFn()
//	Param:
//		IN	:	SLAVE_MSG_ID_e msgId -> message Id
//				VOIDPTR dataPtr
//				UINT16 length
//		OUT	:	NONE
//	Returns	:	NONE
//	Description:
//		This function is a state function of ststem test related activity it
//		handles system test related activiy and sub-state.
//*****************************************************************************
void DcSystemTestStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UNUSED_PARA UINT16 length)
{
	switch(systemTeststate)
	{
		case SYSTEM_TEST_IDLE:
		default:
			break;

		case SYSTEM_TEST_TOUCH_PANEL:
		{
			if(msgId == GUI_BUTTON_KEEP_ALIVE)
			{
				SystemTestTimer();
				SetLcdIdleTimer(SYS_TEST_PROG_TIMEOUT);
			}
			else if(msgId == MSG_TOUCH_SCREEN_TEST_SUCCESS)
			{
				UpdateTestResult(SUCCESS);
				SetAudioVisual(USER_RESP_TEST_SUCCESS, INTERNAL_PORT, NULL);
				SwitchToIdle();
			}
			break;
		}

		case SYSTEM_TEST_DOOR_SENSE:
			if(msgId == MSG_DOOR_SENSE_CHANGE_EVENT)
			{
				MonitorTwoStateInput( (IO_STATUS_e *)dataPtr, SYSTEM_TEST_DOOR_SENSE);
			}
			break;

		case SYSTEM_TEST_AUX_IN:
			if(msgId == MSG_AUX_IP_CHANGE_EVENT)
			{
				MonitorFourStateInput( (IO_STATUS_e *)dataPtr, SYSTEM_TEST_AUX_IN);
			}
			break;

		case SYSTEM_TEST_DOOR_TMPR:
			if(msgId == MSG_TAMPER_IP_CHANGE_EVENT)
			{
				MonitorTwoStateInput( (IO_STATUS_e *)dataPtr, SYSTEM_TEST_DOOR_TMPR);
			}
			break;

		case SYSTEM_TEST_EXT_TMPR:
			if(msgId == MSG_READER_TAMPER_CHANGE_EVENT)
			{
				MonitorTwoStateInput( (IO_STATUS_e *)dataPtr, SYSTEM_TEST_EXT_TMPR);
			}
			break;

		case SYSTEM_TEST_LOCK_TMPR:
			if(msgId == MSG_LOCK_TAMPER_CHANGE_EVENT)
			{
				MonitorTwoStateInput( (IO_STATUS_e *)dataPtr, SYSTEM_TEST_LOCK_TMPR);
			}
			break;

		case SYSTEM_TEST_EXT_SWITCH:
			if(msgId == MSG_REX_IP_CHANGE_EVENT)
			{
				MonitorTwoStateInput( (IO_STATUS_e *)dataPtr, SYSTEM_TEST_EXT_SWITCH);
			}
			break;
	}
}

//*****************************************************************************
//	MonitorFourStateInput()
//	Param:
//		IN	:	SLAVE_MSG_ID_e msgId -> message Id
//				VOIDPTR dataPtr
//				UINT16 length
//		OUT	:	NONE
//	Returns	:	NONE
//	Description:
//		This function is a state function of ststem test related activity it
//		handles system test related activiy and sub-state.
//*****************************************************************************
static void MonitorFourStateInput(IO_STATUS_e *input, SYSTEM_TEST_STATES_e testState)
{
	SYS_TEST_INPUT_DATA_t	fourStateData;

	SystemTestTimer();
	fourStateData.testState = testState;
	if(testSeqFourStateInput[expectedInput] == *input)
	{
		expectedInput++;
		if(expectedInput < FOR_STAT_IN_TEST_SEQ_STAT)
		{
			wrongInputCnt = 0;
			fourStateData.expectedInput = testSeqFourStateInput[expectedInput];
			fourStateData.dispTimeOut = SYS_TEST_PROG_TIMEOUT;

			SetAudioVisual(USER_RESP_APPLY_FOUR_STATE_INPUT, INTERNAL_PORT, (VOIDPTR)(&fourStateData) );
		}
		else
		{
			UpdateTestResult(SUCCESS);
			SetAudioVisual(USER_RESP_TEST_SUCCESS, INTERNAL_PORT, NULL);
			SwitchToIdle();
		}

	}
	else
	{
		fourStateData.detectedInput = *input;
		fourStateData.expectedInput = testSeqFourStateInput[expectedInput];
		if(wrongInputCnt > 0)
			fourStateData.dispTimeOut = SYS_TEST_FAIL_TIMEOUT;
		else
			fourStateData.dispTimeOut = SYS_TEST_PROG_TIMEOUT;

		SetAudioVisual(USER_RESP_WRONG_FOUR_STATE_INPUT, INTERNAL_PORT, (VOIDPTR)(&fourStateData) );

		if(wrongInputCnt > 0)
		{
			UpdateTestResult(FAIL);
			SwitchToIdle();
		}

		wrongInputCnt++;
	}
}

//*****************************************************************************
//	MonitorTwoStateInput()
//	Param:
//		IN	:	BOOL *input,
//				SYSTEM_TEST_STATES_e testState
//		OUT	:	NONE
//	Returns	:	NONE
//	Description:
//		This function monitor two state inputs and also guide user using audio
//		visual responses.
//*****************************************************************************
static void MonitorTwoStateInput(UNUSED_PARA IO_STATUS_e *input, SYSTEM_TEST_STATES_e testState)
{
	SYS_TEST_INPUT_DATA_t	twoStateData;

	SystemTestTimer();
	expectedInput++;
	twoStateData.testState = testState;
	if(expectedInput < TWO_STAT_IN_TEST_SEQ_STAT)
	{
		twoStateData.expectedInput = testSeqTwoStateInput[expectedInput];
		twoStateData.dispTimeOut = SYS_TEST_PROG_TIMEOUT;
		SetAudioVisual(USER_RESP_APPLY_TWO_STATE_INPUT, INTERNAL_PORT, (VOIDPTR)(&twoStateData) );
	}
	else
	{
		UpdateTestResult(SUCCESS);
		SetAudioVisual(USER_RESP_TEST_SUCCESS, INTERNAL_PORT, NULL);
		SwitchToIdle();
	}
}

//*****************************************************************************
//	UpdateTestResult()
//	Param:
//		IN	:	result
//		OUT	:	NONE
//	Returns	:	NONE
//	Description:
//				This function update the result of current test into the ram copy
//
//*****************************************************************************
static void UpdateTestResult(BOOL result)
{
	switch(systemTeststate)
	{
		case SYSTEM_TEST_IDLE:
		default:
			break;

		case SYSTEM_TEST_TOUCH_PANEL:
			SystemTestPara.genericIoResult[GENERIC_TOUCH_PANEL] = result;
			break;

		case SYSTEM_TEST_AUX_RELAY:
			SystemTestPara.genericIoResult[GENERIC_AUX_RELAY] = result;
			break;

		case SYSTEM_TEST_DOOR_RELAY:
			SystemTestPara.genericIoResult[GENERIC_DOOR_RELAY] = result;
			break;

		case SYSTEM_TEST_DOOR_SENSE:
			SystemTestPara.genericIoResult[GENERIC_DOOR_SENSE] = result;
			break;

		case SYSTEM_TEST_AUX_IN:
			SystemTestPara.genericIoResult[GENERIC_AUX_IN] = result;
			break;

		case SYSTEM_TEST_DOOR_TMPR:
			SystemTestPara.genericIoResult[GENERIC_DOOR_TMPR] = result;
			break;

		case SYSTEM_TEST_EXT_TMPR:
			SystemTestPara.genericIoResult[GENERIC_EXT_TMPR] = result;
			break;

		case SYSTEM_TEST_LOCK_TMPR:
			SystemTestPara.genericIoResult[GENERIC_LOCK_TMPR] = result;
			break;

		case SYSTEM_TEST_EXT_SWITCH:
			SystemTestPara.genericIoResult[GENERIC_EXT_SWITCH] = result;
			break;
	}
}

//*****************************************************************************
//	StartTwoStateOutputTimer()
//	Param:
//		IN	:	UINT8 outputCnt
//		OUT	:	NONE
//	Returns	:	NONE
//	Description:
//		This function start output timer to toggle output state for testing the
//		output
//*****************************************************************************
static BOOL StartTwoStateOutputTimer(UINT8 outputCnt)
{
	TIMER_INFO_t	   twoStateOutPutTimer;

	if(twoStateOutTmrHandle == INVALID_TIMER_HANDLE)
	{
		twoStateOutPutTimer.funcPtr = &TwoStateOutputTimeOut;
		twoStateOutPutTimer.count = (TWO_STATE_OUTPUT_CHANGE_TIME*10);
		twoStateOutPutTimer.data = outputCnt;

		return (StartTimer(twoStateOutPutTimer, &twoStateOutTmrHandle, __FUNCTION__) );
	}
	else
	{
		return (ReloadTimer(twoStateOutTmrHandle, (TWO_STATE_OUTPUT_CHANGE_TIME*10) ) );
	}
}

//*****************************************************************************
//	TwoStateOutputTimeOut()
//	Param:
//		IN	:	UINT32 outputCnt
//		OUT	:	NONE
//	Returns	:	NONE
//	Description:
//		This function is timeout function of twoStateOutTimer.
//*****************************************************************************
static void TwoStateOutputTimeOut(UINT32 outputCnt)
{
	SYS_TEST_OUTPUT_DATA_t	outputData;

	DeleteTimer(&twoStateOutTmrHandle);
	SystemTestTimer();
	if(outputCnt < 4)
	{
		if( (outputCnt % 2) == 0)
		{
			if(systemTeststate == SYSTEM_TEST_AUX_RELAY)
			{
				ActivateAuxRelay(0);
			}
			else
			{
				ActivateDoorRelay(0);
			}
			outputData.output = IO_ACTIVE;
		}
		else
		{
			if(systemTeststate == SYSTEM_TEST_AUX_RELAY)
			{
				DeactivateAuxRelay();
			}
			else
			{
				DeactivateDoorRelay();
			}
			outputData.output = IO_INACTIVE;
		}
		outputData.testState = systemTeststate;
		outputData.dispTimeOut = TWO_STATE_OUTPUT_CHANGE_TIME;
		SetAudioVisual(USER_RESP_TWO_STATE_OUPUT, INTERNAL_PORT, (VOIDPTR)(&outputData) );
		outputCnt++;
		StartTwoStateOutputTimer( (UINT8)outputCnt);
	}
	else
	{
		SwitchToIdle();
	}
}

//*****************************************************************************
//	SwitchToIdle()
//	Param:
//		IN	:	NONE
//		OUT	:	NONE
//	Returns	:	NONE
//	Description:
//				This function switch into idle state.
//*****************************************************************************
static void SwitchToIdle(void)
{
	DeleteTimer(&systemTestTmrHandle);
	TestStatusF = SYSTEM_TEST_STATUS_COMPLETE;
	SwitchDcToIdleState();
}

//*****************************************************************************
//	SystemTestTimer()
//	Param:
//		IN	:	NONE
//		OUT	:	NONE
//	Returns	:	NONE
//	Description:
//			This function start system test timer.
//*****************************************************************************
static BOOL SystemTestTimer(void)
{
	TIMER_INFO_t	   systemTestTimer;

	if(systemTestTmrHandle == INVALID_TIMER_HANDLE)
	{
		systemTestTimer.funcPtr = &SystemTestTimeOut;
		systemTestTimer.count = (SYS_TEST_PROG_TIMEOUT*10);

		return (StartTimer(systemTestTimer, &systemTestTmrHandle, __FUNCTION__) );
	}
	else
	{
		return (ReloadTimer(systemTestTmrHandle, (SYS_TEST_PROG_TIMEOUT*10)) );
	}
}

//*****************************************************************************
//	SystemTestTimeOut()
//	Param:
//		IN	:	NONE
//		OUT	:	NONE
//	Returns	:	NONE
//	Description:
//			This function is timeout function of system test timer.
//*****************************************************************************
static void SystemTestTimeOut(UNUSED_PARA UINT32 dummy)
{
	DeleteTimer(&systemTestTmrHandle);
	UpdateTestResult(FAIL);
	TestStatusF = SYSTEM_TEST_STATUS_COMPLETE;
	SetAudioVisual(USER_RESP_TIMEOUT, INTERNAL_PORT, NULL);
	SwitchDcToIdleState();
}

//*****************************************************************************
//	SwitchDcToSysTestState()
//	Param:
//		IN	:	SYSTEM_TEST_STATES_e state
//		OUT	:	NONE
//	Returns	:	NONE
//	Description:
//				This function switch system to appropriate system test state and
//				also it initialize parameter and audio visual response related to
//				that state.
//*****************************************************************************
void SwitchDcToSysTestState(SYSTEM_TEST_STATES_e state)
{
	CleanUpTempUserFP();
	DcInfo.curState = DC_SYSTEM_TEST_STATE;
	systemTeststate = state;
	wrongInputCnt = 0;
	TestStatusF = SYSTEM_TEST_STATUS_PROGRESS;
	SystemTestTimer();

	switch(systemTeststate)
	{
	case SYSTEM_TEST_IDLE:
		break;

	case SYSTEM_TEST_TOUCH_PANEL:
	{
		SetAudioVisual(USER_RESP_TOUCH_SCREEN_TEST_START, INTERNAL_PORT, NULL );
		break;
	}

	case SYSTEM_TEST_AUX_RELAY:
	case SYSTEM_TEST_DOOR_RELAY:
	{
		SYS_TEST_OUTPUT_DATA_t	outputData;

		outputData.output = IO_INACTIVE;
		outputData.testState = systemTeststate;
		outputData.dispTimeOut = TWO_STATE_OUTPUT_CHANGE_TIME;
		SetAudioVisual(USER_RESP_TWO_STATE_OUPUT, INTERNAL_PORT, (VOIDPTR)(&outputData) );

		if(systemTeststate == SYSTEM_TEST_AUX_RELAY)
		{
			DeactivateAuxRelay();
		}
		else
		{
			DeactivateDoorRelay();
		}
		StartTwoStateOutputTimer(0);
		break;
	}

	case SYSTEM_TEST_DOOR_SENSE:
	{
		SYS_TEST_INPUT_DATA_t twoStateData;

		SysBasicPara.doorSense.enable = ENABLED;
		SysBasicPara.doorSense.type = NORMAL_CLOSE;
		SysBasicPara.doorSense.supervise = ENABLED;
		ClearDoorSenseStatus();

		expectedInput = INPUT_INACTIVE;
		twoStateData.expectedInput = expectedInput;
		twoStateData.dispTimeOut = SYS_TEST_PROG_TIMEOUT;
		twoStateData.testState = systemTeststate;
		SetAudioVisual(USER_RESP_APPLY_TWO_STATE_INPUT, INTERNAL_PORT, (VOIDPTR)(&twoStateData) );
		break;
	}

	case SYSTEM_TEST_DOOR_TMPR:
	case SYSTEM_TEST_EXT_TMPR:
	case SYSTEM_TEST_LOCK_TMPR:
	case SYSTEM_TEST_EXT_SWITCH:
	{
		SYS_TEST_INPUT_DATA_t twoStateData;

		expectedInput = INPUT_INACTIVE;
		twoStateData.expectedInput = expectedInput;
		twoStateData.dispTimeOut = SYS_TEST_PROG_TIMEOUT;
		twoStateData.testState = systemTeststate;
		SetAudioVisual(USER_RESP_APPLY_TWO_STATE_INPUT, INTERNAL_PORT, (VOIDPTR)(&twoStateData) );

		SysInputStatus.sysTamper = INPUT_ACTIVE;
		SysInputStatus.rdrTamper = INPUT_ACTIVE;
		SysInputStatus.lockTamper = INPUT_ACTIVE;
		SysInputStatus.exitSw = INPUT_ACTIVE;
		break;
	}
	default:
		break;
	}
}

//*****************************************************************************
//	InitSysTest()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system test related parameter
//*****************************************************************************
void InitSysTest(void)
{
	//check system reboot test is going on or not.
	InitSystemRebootTest();

	if(SUCCESS != ReadSysTestPara() )
	{
		if(SUCCESS != LoadDfltSysTestPara() )
		{
			EPRINT(CONFIG_LOG,"Failed to intialise system test para");
		}
	}
}

//*****************************************************************************
//	ReadSysTestPara()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads system test parameter from non volatile memory.
//*****************************************************************************
static BOOL ReadSysTestPara(void)
{
	INT32 		fd;
	UINT8		ver;
	BOOL		status = FAIL;

	//	Open System Advance Parameter File
	if( (fd = open(SYS_TEST_CONFIG_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO) ) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open system test parameter");
		return FAIL;
	}

	// Read version number from the file
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read system test version");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != SYS_TEST_PARA_VER)
	{
		EPRINT(CONFIG_LOG,"system test para version doesn't match");
		close(fd);
		return status;
	}

	if(sizeof(SystemTestPara) != read(fd,&SystemTestPara,sizeof(SystemTestPara) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to read system test para");
		close(fd);
		return FAIL;
	}
	close(fd);
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltSysTestPara()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default system test parameters.
//*****************************************************************************
static BOOL LoadDfltSysTestPara(void)
{
	SystemTestPara = dfltSysTestPara;
	return WriteSysTestPara();
}

//******************************************************************************
//	WriteSysTestPara()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes system test parameter into the non volatile memory.
//******************************************************************************
BOOL WriteSysTestPara(void)
{
	INT32	fd;
	UINT8	ver;

	if( (fd = open(SYS_TEST_CONFIG_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO) ) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open system test parameter");
		return FAIL;
	}

	// Write version number
	ver = SYS_TEST_PARA_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write system test para version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if(sizeof(SystemTestPara) != write(fd,&SystemTestPara,sizeof(SystemTestPara) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write system test para");
		close(fd);
		return FAIL;
	}
	close(fd);
	return SUCCESS;
}

//*****************************************************************************
//	EnrollTestUser()
//	Param:
//		IN: UINT16 userIdx
//		OUT:NONE.
//	Returns:
//		NONE
//	Description:
//		This function enroll fingerprint of test user.
//*****************************************************************************
void EnrollTestUser(UINT16 userIdx)
{
	ENROLLMENT_INPUTS_t enrollmentInput;

	sprintf(enrollmentInput.alphaNumId, "%s", GetAlphaNumId(userIdx));
	enrollmentInput.category = ENROLL_CATEGORY_USER;
	enrollmentInput.enrollmentType = ENROLL_BIOMETRIC_ONLY;
	enrollmentInput.fpCnt = 1;
	enrollmentInput.userId = GetUserId(userIdx);
	enrollmentInput.source = ENROLL_SOURCE_WEBJVS;
	RequestEnrollment(&enrollmentInput);
}

//*****************************************************************************
//	SaveCardIdInTestConfig()
//	Param:
//		IN: READER_ID_e rdrId
//			UINT64      cardId
//		OUT:NONE.
//	Returns:
//		NONE
//	Description:
//		This function enroll fingerprint of test user.
//*****************************************************************************
void SetCardIdInTestConfig(READER_ID_e rdrId, UINT64 cardId)
{
	switch(rdrId)
	{
	case READER1:
		switch(SysBasicPara.reader[rdrId].type)
		{
		case SM_PROX_READER:
			SystemTestPara.detectedCardId[SYS_TEST_INT_EM_PROX] = cardId;
			break;
		case HID_PROX_READER:
			SystemTestPara.detectedCardId[SYS_TEST_INT_HID_PROX] = cardId;
			break;
		case MIFARE_U_READER:
			SystemTestPara.detectedCardId[SYS_TEST_INT_MIFARE] = cardId;
			break;
		case HID_ICLASS_U_READER:
			SystemTestPara.detectedCardId[SYS_TEST_INT_HID_ICLASS] = cardId;
			break;
		default:
			break;
		}
		break;

	case READER3:
		switch(SysBasicPara.reader[rdrId].type)
		{
		case HID_ICLASS_U_READER:
			SystemTestPara.detectedCardId[SYS_TEST_EXT_HID_ICLASS_U] = cardId;
			break;
		case HID_ICLASS_W_READER:
			SystemTestPara.detectedCardId[SYS_TEST_EXT_HID_ICLASS_W] = cardId;
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}
}

//*****************************************************************************
//	GenerateTestReport()
//	Param:
//		IN: NONE
//		OUT:NONE
//	Returns:
//		NONE
//	Description:
//		This function Generate test report.
//*****************************************************************************
void GenerateTestReport(void)
{
	INT32		fd = -1;
	UINT16		tmpStrLen = 0;
	char		tmpBuff[2048];

	fd = open(SYS_TEST_REPORT_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd > 0)
	{
		tmpStrLen = sprintf(tmpBuff, "########################### System Test Report################################\r\n");

		tmpStrLen += PrintSubHeaderInFile(&tmpBuff[tmpStrLen], "Basic Detail");
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Employee Name: %s\r\n", SystemTestPara.empName);
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Employee Id  : %s\r\n", SystemTestPara.empId);
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Device Serial No.: %s\r\n", SystemTestPara.devSrNo);
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += snprintf(&tmpBuff[tmpStrLen], (sizeof(tmpBuff)-tmpStrLen), "Main PCB       =>  HWD Design: %s   |   Software Version: V%dR%d.%d\r\n",SystemTestPara.mainPcbDesign, SW_VER, SW_REV, SW_MINOR_REV);
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Connector PCB  =>  HWD Design: %s\r\n", SystemTestPara.connPcbDesign);
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Mac Address     : %s   |   Verification: %s\r\n", EthernetParameters.macAddress, resultStr[SystemTestPara.macAddrResult] );
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Date(dd/mm/yyyy):	%s\r\n", SystemTestPara.testDate);
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);

		write(fd, (VOIDPTR)tmpBuff, tmpStrLen);

		tmpStrLen = PrintSubHeaderInFile(tmpBuff, "Hardware Detail");

		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Assembler Name : %s\r\n", SystemTestPara.asmblerName);
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Assembler Id   : %s\r\n", SystemTestPara.asmblerId);
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "32Khz Frequency: %s\r\n", resultStr[SystemTestPara.rtcFreqResult] );
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "1.1VDC    : %s\r\n", resultStr[SystemTestPara.voltResult[VOLTAGE_1_1V]] );
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "1.26VDC   : %s\r\n", resultStr[SystemTestPara.voltResult[VOLTAGE_1_26V]] );
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "1.5VDC    : %s\r\n", resultStr[SystemTestPara.voltResult[VOLTAGE_1_5V]] );
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "1.8VDC    : %s\r\n", resultStr[SystemTestPara.voltResult[VOLTAGE_1_8V]] );
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "3.3VDC    : %s\r\n", resultStr[SystemTestPara.voltResult[VOLTAGE_3_3V]] );
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "5.0VDC    : %s\r\n", resultStr[SystemTestPara.voltResult[VOLTAGE_5_0V]] );
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "12VDC     : %s\r\n", resultStr[SystemTestPara.voltResult[VOLTAGE_12_0V]] );
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);

		write(fd, (VOIDPTR)tmpBuff, tmpStrLen);

		tmpStrLen = PrintSubHeaderInFile(tmpBuff, "Reader");
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);

		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Internal EM-Prox Reader     => %s | CardId:%llu\r\n",
			resultStr[SystemTestPara.cardRdrResult[SYS_TEST_INT_EM_PROX]], SystemTestPara.detectedCardId[SYS_TEST_INT_EM_PROX]);
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Internal HID Prox Reader    => %s | CardId:%llu\r\n",
			resultStr[SystemTestPara.cardRdrResult[SYS_TEST_INT_HID_PROX]], SystemTestPara.detectedCardId[SYS_TEST_INT_HID_PROX]);
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);

		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Internal HID iClass-U Reader=> %s | CardId:%llu\r\n",
			resultStr[SystemTestPara.cardRdrResult[SYS_TEST_INT_HID_ICLASS]], SystemTestPara.detectedCardId[SYS_TEST_INT_HID_ICLASS]);
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Internal MiFare Reader      => %s | CardId:%llu\r\n",
			resultStr[SystemTestPara.cardRdrResult[SYS_TEST_INT_MIFARE]], SystemTestPara.detectedCardId[SYS_TEST_INT_MIFARE]);
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "External U Reader           => %s | CardId:%llu\r\n",
				resultStr[SystemTestPara.cardRdrResult[SYS_TEST_EXT_HID_ICLASS_U]], SystemTestPara.detectedCardId[SYS_TEST_EXT_HID_ICLASS_U]);
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "External W Reader           => %s | CardId:%llu\r\n",
				resultStr[SystemTestPara.cardRdrResult[SYS_TEST_EXT_HID_ICLASS_W]], SystemTestPara.detectedCardId[SYS_TEST_EXT_HID_ICLASS_W]);
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);

		if(YES == IsVariant_With_Internal_FP_Rdr()) //Not applicable for ARGO CAE/CAM/CAI Variant
		{
			tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "FP Sensor Serial No.	    => 0x%08x - [%010u]\r\n",SfprSensorSerialNumber, SfprSensorSerialNumber);
			tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		}

		write(fd, (VOIDPTR)tmpBuff, tmpStrLen);

		tmpStrLen = PrintSubHeaderInFile(tmpBuff, "Generic Input-Ouput");

		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Buzzer	         : %s\r\n", resultStr[SystemTestPara.genericIoResult[GENERIC_BUZ_LED]] );
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Touch Screen      : %s\r\n", resultStr[SystemTestPara.genericIoResult[GENERIC_TOUCH_PANEL]] );
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Door Relay        : %s\r\n", resultStr[SystemTestPara.genericIoResult[GENERIC_DOOR_RELAY]] );
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Door Sense        : %s\r\n", resultStr[SystemTestPara.genericIoResult[GENERIC_DOOR_SENSE]] );
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Door Tamper       : %s\r\n", resultStr[SystemTestPara.genericIoResult[GENERIC_DOOR_TMPR]] );
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Exit Switch       : %s\r\n", resultStr[SystemTestPara.genericIoResult[GENERIC_EXT_SWITCH]] );
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);

		write(fd, (VOIDPTR)tmpBuff, tmpStrLen);

		tmpStrLen = sprintf(&tmpBuff[0], "USB          : %s\r\n", resultStr[SystemTestPara.genericIoResult[GENERIC_USB]] );
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Memory Card       : %s\r\n", resultStr[SystemTestPara.genericIoResult[GENERIC_MEMORY_CARD]] );
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "RTC Test          : %s\r\n", resultStr[SystemTestPara.genericIoResult[GENERIC_RTC_TEST]] );
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "LCD Test          : %s\r\n", resultStr[SystemTestPara.genericIoResult[GENERIC_LCD_TEST]] );
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);

		write(fd, (VOIDPTR)tmpBuff, tmpStrLen);

		tmpStrLen = PrintSubHeaderInFile(tmpBuff, "Others");
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Wifi Test              : %s\r\n", resultStr[SystemTestPara.wifiResult] );
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);

		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Bluetooth Test         : %s\r\n", resultStr[SystemTestPara.bluetoothResult] );
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);

		if(YES == IsVariant_With_Internal_FP_Rdr()) //Not applicable for ARGO CAE/CAM/CAI Variant
		{
			tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Create And Enroll User : %s\r\n", resultStr[SystemTestPara.usrEnrlResult] );
			tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		}

		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "System Default         : %s\r\n", resultStr[SystemTestPara.sysDfltResult] );
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);

		write(fd, (VOIDPTR)tmpBuff, tmpStrLen);

		close(fd);
		ConvertTxtFileToPdf();
	}
}

//*****************************************************************************
//	SendTestReport()
//	Param:
//		IN: NONE
//		OUT:NONE
//	Returns:
//		NONE
//	Description:
//		This function Generate test report.
//*****************************************************************************
void SendTestReport(void)
{
	char		tmpBuff[200];

    sprintf(tmpBuff,"curl -T %s ftp://%s:%s@%s/%s_%s.pdf",SYS_TEST_REPORT_FILE_PDF,SystemTestPara.ftpUsrName,SystemTestPara.ftpPwd,SystemTestPara.ftpIpaddr,DeviceVariantType[AutoSystemTestPara.devType],SystemTestPara.devSrNo);
	system(tmpBuff);
}

//*****************************************************************************
//	PrintSubHeaderInFile()
//	Param:
//		IN	:	FILE *fp
//				CHARPTR str
//		OUT	:	NONE
//	Returns	:	NONE
//	Description:
//			This function print sub header into the file.
//*****************************************************************************
UINT16 PrintSubHeaderInFile(CHARPTR charPtr, CHARPTR strPtr)
{
	UINT16		tmpStrLen;

	tmpStrLen = sprintf(charPtr, "\r\n******************************************************************************\r\n");
	tmpStrLen += sprintf( (charPtr+tmpStrLen), "\t\t\t%s\r\n", strPtr);
	tmpStrLen += sprintf( (charPtr+tmpStrLen), "******************************************************************************\r\n");
	return tmpStrLen;
}

//*****************************************************************************
//	GenerateDesPattern()
//	Param:
//		IN	:	FILE *fp
//				CHARPTR str
//		OUT	:	NONE
//	Returns	:	NONE
//	Description:
//			This function print des pattern into the file.
//*****************************************************************************
UINT16 GenerateDesPattern(CHARPTR charPtr)
{
	UINT16		tmpStrLen;

	tmpStrLen = sprintf(charPtr, "-----------------------------------------------------------------------------\r\n");
	return tmpStrLen;
}

//*****************************************************************************
//	StartRebootTest()
//	Param:
//		IN: NONE
//		OUT:NONE.
//	Returns:
//		NONE
//	Description:
//		This function start reboot test.
//*****************************************************************************
void InitSystemRebootTest(void)
{
	INT32	fd;
	UINT32	systemRebootFileOpenTick;

	systemRebootFileOpenTick = GetSysTick();
	while(1)
	{
		if(ElapsedTick(systemRebootFileOpenTick) >= (TIMER_1MS * 300))	//300 ms
		{
			SystemRebootTestPata.sysRebootCnt = 0;
			break;
		}

		fd = open(SYS_REBOOT_TEST_CONFIG_FILE, O_RDWR | O_SYNC , S_IRWXU | S_IRWXG | S_IRWXO);
		if(fd > 0)
		{
			if(sizeof(SYSTEM_REBOOT_TEST_PARA_t) != read(fd, &SystemRebootTestPata, sizeof(SYSTEM_REBOOT_TEST_PARA_t) ) )
			{
				EPRINT(CONFIG_LOG,"Failed to read system test reboot count file");
			}
			close(fd);
		}
		else
		{
			if( (errno == EINTR) || (errno == ENOENT) )
			{
//				EPRINT(CONFIG_LOG,"Failed to Open system test reboot count file : [%u] : %m",errno);
				continue;
			}
			SystemRebootTestPata.sysRebootCnt = 0;
		}
		break;
	}

	EPRINT(CONFIG_LOG, "systemRebootFileOpenTick = %u, SystemRebootTestPata.sysRebootCnt = %u", ElapsedTick(systemRebootFileOpenTick), SystemRebootTestPata.sysRebootCnt);

	fd = open(SYS_REBOOT_FILE, O_RDWR | O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd > 0)
	{
		close(fd);
		if( (SystemRebootTestPata.sysRebootCnt <= 200) && (SystemRebootTestPata.sysRebootCnt > 1) )
		{
			SystemRebootTestPata.sysRebootCnt--;
			WriteSysRebootCnt();
			SetSysRebootTimer(SYSTEM_REBOOT_TEST_TIMER_CNT);
		}
		else
		{
			SystemRebootTestPata.sysRebootCnt = 0;
			WriteSysRebootCnt();
			unlink(SYS_REBOOT_FILE);
		}
	}
}

//*****************************************************************************
//	WriteSysRebootCnt()
//	Param:
//		IN: NONE
//		OUT:NONE.
//	Returns:
//		SUCCESS/FAIL
//	Description:
//		This function write reboot count in file.
//*****************************************************************************
static BOOL WriteSysRebootCnt(void)
{
	INT32	fd;

	fd = open(SYS_REBOOT_TEST_CONFIG_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open reboot test config file");
		return FAIL;
	}
	if(sizeof(SystemRebootTestPata) != write(fd, &SystemRebootTestPata, sizeof(SystemRebootTestPata) ) )
	{
		EPRINT(CONFIG_LOG,"fail to write reboot test config file");
		close(fd);
		return FAIL;
	}
	close(fd);
	return SUCCESS;
}

//*****************************************************************************
//	StartRebootTest()
//	Param:
//		IN: NONE
//		OUT:NONE.
//	Returns:
//		NONE
//	Description:
//		This function start reboot test.
//*****************************************************************************
void StartRebootTest(void)
{
	INT32	fd;

	fd = open(SYS_REBOOT_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd > 0)
	{
		WriteSysRebootCnt();
		SetSysRebootTimer(SYS_REBOOT_TMR_CNT);
		close(fd);
	}
}
