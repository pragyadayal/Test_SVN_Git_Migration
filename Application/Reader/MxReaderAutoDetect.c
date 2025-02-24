///////////////////////////////////////////////////////////////////////////
//
//   MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//   MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//   MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
//   MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
//   MM       MM   AA       AA       TT      RR  RR       II      XX XX
//   MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//   Company      : Matrix Telecom Pvt. Ltd., Baroda, India.
//   Project      : Access Control System
//   Created By   : Ronak Tanna
//   File         : MxReaderTask.c
//   Description  : This file implements API layer of reader task to communicate
//					with diffrent reader drivers
//
/////////////////////////////////////////////////////////////////////////////

//******** Include files **********

#include "MxTypedef.h"
#include "MxSysTimer.h"
#include "MxReaderTask.h"
#include "MxReaderRxTx.h"
#include "MxMifareReader.h"
#include "MxSM130Task.h"
#include "MxSM125Task.h"
#include "MxPN532Task.h"
#include "MxCLR663Task.h"
#include "MxCerTask.h"
#include "MxHIDProx.h"
#include "MxSupremaReader.h"
#include "MxOEM75Task.h"
#include "MxLogs.h"
#include "MxConfigFileIo.h"
#include "MxReaderUtils.h"
#include "MxSE32X0Task.h"
#include "MxReaderCommonUtils.h"

//******** Extern Variables **********

//****** Defines and Data Types ******

//******** Function Prototypes *******
static void LoadReaderAutoDetectInfo(UINT8 readerId);
static void AutoReaderDetect(READER_ID_e readerId);

//******** Global Variables **********
MIFARE_U_READER_TYPE_e						MifareReaderType[MAX_READERS];
HID_ICLASS_READER_TYPE_e					HidIclassReaderType[MAX_READERS];
READER_INIT_PROC_INFO_t						ReaderInitInfo;
READER_AUTO_DETECT_PROC_INFO_t				ReaderAutoDetectInfo;

//******** Static Variables **********
// This table is list of reader type as per reader Id
// for auto detectin task
// Group-1 Variants which supports Internal FP reader 
static const READER_TYPE_e autoDetectReaderSeq1[MAX_AUTO_DETECT_READER_TYPE][MAX_READERS] =
{
	{SM_PROX_READER,		SUP_FP_3030_READER,		COMBO_EXIT_READER, 		SUP_FP_3030_READER,	SUP_FP_3030_READER	},
	{MIFARE_U_READER,		NO_READER_PRESENT,		CB_U_READER, 			CB_U_READER	,		NO_READER_PRESENT	},
	{HID_ICLASS_U_READER,	NO_READER_PRESENT,		ATOM_RD100_READER, 		ATOM_RD100_READER,	NO_READER_PRESENT	},
	{NO_READER_PRESENT,		NO_READER_PRESENT,		ATOM_RD200_READER,		ATOM_RD200_READER,	NO_READER_PRESENT	},
	{NO_READER_PRESENT,		NO_READER_PRESENT,		ATOM_RD300_READER,		ATOM_RD300_READER,	NO_READER_PRESENT	},
	{NO_READER_PRESENT,	    NO_READER_PRESENT,		SM_PROX_READER, 		NO_READER_PRESENT,	NO_READER_PRESENT	},
	{NO_READER_PRESENT,		NO_READER_PRESENT,		MIFARE_U_READER,		NO_READER_PRESENT,	NO_READER_PRESENT	},
	{NO_READER_PRESENT,		NO_READER_PRESENT,		HID_ICLASS_U_READER,	NO_READER_PRESENT,	NO_READER_PRESENT	},
	{NO_READER_PRESENT,		NO_READER_PRESENT,		SUP_FP_3030_READER,		NO_READER_PRESENT,	NO_READER_PRESENT	},
	{NO_READER_PRESENT,		NO_READER_PRESENT,		HID_PROX_READER,		NO_READER_PRESENT,	NO_READER_PRESENT	},
	{NO_READER_PRESENT,		NO_READER_PRESENT,		NO_READER_PRESENT,		NO_READER_PRESENT,	NO_READER_PRESENT	}
};

// Group-2 Variants which does not supports Internal FP reader
static const READER_TYPE_e autoDetectReaderSeq2[MAX_AUTO_DETECT_READER_TYPE][MAX_READERS] =
{
	{SM_PROX_READER,		NO_READER_PRESENT,		COMBO_EXIT_READER, 		SUP_FP_3030_READER,	SUP_FP_3030_READER	},
	{MIFARE_U_READER,		NO_READER_PRESENT,		CB_U_READER, 			CB_U_READER		,	NO_READER_PRESENT	},
	{HID_ICLASS_U_READER,	NO_READER_PRESENT,		ATOM_RD100_READER,	 	ATOM_RD100_READER,	NO_READER_PRESENT	},
	{NO_READER_PRESENT,		NO_READER_PRESENT,		ATOM_RD200_READER,		ATOM_RD200_READER,	NO_READER_PRESENT	},
	{NO_READER_PRESENT,		NO_READER_PRESENT,		ATOM_RD300_READER,		ATOM_RD300_READER,	NO_READER_PRESENT	},
	{NO_READER_PRESENT,		NO_READER_PRESENT,		SM_PROX_READER, 		NO_READER_PRESENT,	NO_READER_PRESENT	},
	{NO_READER_PRESENT,		NO_READER_PRESENT,		MIFARE_U_READER,		NO_READER_PRESENT,	NO_READER_PRESENT	},
	{NO_READER_PRESENT,		NO_READER_PRESENT,		HID_ICLASS_U_READER,	NO_READER_PRESENT,	NO_READER_PRESENT	},
	{NO_READER_PRESENT,		NO_READER_PRESENT,		SUP_FP_3030_READER,		NO_READER_PRESENT,	NO_READER_PRESENT	},
	{NO_READER_PRESENT,		NO_READER_PRESENT,		HID_PROX_READER,		NO_READER_PRESENT,	NO_READER_PRESENT	},
	{NO_READER_PRESENT,		NO_READER_PRESENT,		NO_READER_PRESENT,		NO_READER_PRESENT,	NO_READER_PRESENT	}
};

#if defined(MIFARE_READER_INIT_PRIORITY_PN532)
MIFARE_U_READER_TYPE_e mifareReaderInitPrioritySeq[MAX_MIFARE_U_READER_TYPE] =
{
	MIFARE_U_READER_PN532,
	MIFARE_U_READER_SM130
};
#elif defined(COMPACT_DOOR_CONTROLLER)
MIFARE_U_READER_TYPE_e mifareReaderInitPrioritySeq[MAX_MIFARE_U_READER_TYPE] =
{
	MIFARE_U_READER_PN532,
	MAX_MIFARE_U_READER_TYPE
};
#elif defined(MIFARE_READER_INIT_PRIORITY_CLR663)
MIFARE_U_READER_TYPE_e mifareReaderInitPrioritySeq[MAX_MIFARE_U_READER_TYPE] =
{
	MIFARE_U_READER_CLR663,
	MIFARE_U_READER_PN532,
	MIFARE_U_READER_SM130,
	MAX_MIFARE_U_READER_TYPE
};
#else
MIFARE_U_READER_TYPE_e mifareReaderInitPrioritySeq[MAX_MIFARE_U_READER_TYPE] =
{
	MIFARE_U_READER_SM130,
	MIFARE_U_READER_PN532
};
#endif

HID_ICLASS_READER_TYPE_e hidIclassReaderInitPrioritySeq[MAX_HID_READER_TYPE] =
{
	HID_READER_SE_32X0,
	HID_READER_OEM_150,
};
//******** Function Definations ******

READER_TYPE_e GetAutoDetectReader(UINT8 auto_detect_reader_type, READER_ID_e reader)
{
	/* Group-1 Variants which supports Internal FP reader */
	if(TRUE == IsVariant_With_Internal_FP_Rdr())
	{
		return autoDetectReaderSeq1[auto_detect_reader_type][reader];
	}
	/* Group-2 Variants which does not supports Internal FP reader */
	else
	{
		return autoDetectReaderSeq2[auto_detect_reader_type][reader];
	}
}

//*****************************************************************************
//	AutoReaderDetectionTask()
//	Param:
//		IN :	readerId
//		OUT:	none
//	Returns:	YES/NO
//	Description:
//				It will detect reader type automatically in initialization
//
//*****************************************************************************
void AutoReaderDetectionTask(void)
{
	READER_ID_e		readerId = 0;
	BOOL			tryForNextRdrTypeF = FALSE;
	EXT_COMBO_READER_e comboRdrId;

	// Auto detection process starts
	ReaderAutoDetectInfo.autoDetectF = TRUE;
	// Initialize All combo reader card detect flag to False
	for(comboRdrId = COMBO_READER_CER ; comboRdrId < COMBO_READER_MAX ; comboRdrId++)
	{
		ReaderAutoDetectInfo.comboCardDetectF[comboRdrId] = FALSE;
	}

	InitReaderRxTxDriver();

	for(readerId = 0; readerId < MAX_AUTO_DETECT_READER; readerId++)
	{
		InitReaderRelatedInfo(readerId);
		CheckNextHidIclassReader(readerId, TRUE);
		CheckNextMifareReader(readerId, TRUE);
		ReaderAutoDetectInfo.rdrTypeIdx[readerId] = 0;
		LoadReaderAutoDetectInfo(readerId);
	}
	ReaderAutoDetectInfo.readerType[READER4] = NO_READER_PRESENT;
	ReaderInitInfo.state[READER4] = MAX_READER_INIT;
	ReaderAutoDetectInfo.readerType[READER5] = NO_READER_PRESENT;
	ReaderInitInfo.state[READER5] = MAX_READER_INIT;

	while(1)
	{
		for(readerId = 0; readerId < MAX_AUTO_DETECT_READER; readerId++)
		{
			if (ReaderAutoDetectInfo.rdrStatus[readerId] == RDR_DETECT_SUCCESS)
			{
				continue;
			}

			if( (readerId != READER3) && (GetDoorApplType() == PANEL_DOOR_APPL) )
			{
				ReaderAutoDetectInfo.rdrStatus[readerId] = RDR_DETECT_SUCCESS;
				continue;
			}

			if(ReaderAutoDetectInfo.readerType[readerId] == NO_READER_PRESENT)
			{
				ReaderAutoDetectInfo.rdrStatus[readerId] = RDR_DETECT_SUCCESS;
				continue;
			}

			if(ReaderAutoDetectInfo.rdrStatus[readerId] == RDR_DETECT_PROGRESS)
			{
				AutoReaderDetect(readerId);
			}

			if(ReaderAutoDetectInfo.rdrStatus[readerId] == RDR_DETECT_FAIL)
			{
				if( (ReaderAutoDetectInfo.readerType[readerId] == MIFARE_U_READER) &&
					(CheckNextMifareReader(readerId, FALSE) == TRUE) )
				{
					LoadReaderAutoDetectInfo(readerId);
				}
				else if( (ReaderAutoDetectInfo.readerType[readerId] == HID_ICLASS_U_READER) &&
						(CheckNextHidIclassReader(readerId, FALSE) == TRUE)	)
				{
					LoadReaderAutoDetectInfo(readerId);
				}
				else
				{
					tryForNextRdrTypeF = TRUE;
				}
			}

			if(ReaderAutoDetectInfo.rdrStatus[readerId] == RDR_DETECT_SUCCESS)
			{
				if( (ReaderAutoDetectInfo.readerType[readerId] == COMBO_EXIT_READER) && (readerId == READER3) )
				{
					// For Cer, also start detection task for reader4
					tryForNextRdrTypeF = TRUE;
					ReaderAutoDetectInfo.rdrTypeIdx[readerId] += 4; //To skip [CB-U -> ATOM_RD100 -> ATOM_RD200 -> ATOM_RD300] for READER3
					LoadReaderAutoDetectInfo(READER4);
				}

				if( (ReaderAutoDetectInfo.readerType[readerId] == CB_U_READER) && (readerId == READER3) )
				{
					// For Cbr, also start detection task for reader4
					tryForNextRdrTypeF = TRUE;
					++(ReaderAutoDetectInfo.rdrTypeIdx[READER4]);	//To skip SUP_FP_3030_READER.
					ReaderAutoDetectInfo.rdrTypeIdx[readerId] += 3; //To skip [ATOM_RD100 -> ATOM_RD200 -> ATOM_RD300] for READER3
					LoadReaderAutoDetectInfo(READER4);
				}

				if( (ReaderAutoDetectInfo.readerType[readerId] == ATOM_RD100_READER) && (readerId == READER3) )
				{
					// For atom RD100, also start detection task for reader4
					tryForNextRdrTypeF = TRUE;
					ReaderAutoDetectInfo.rdrTypeIdx[readerId] += 2; //To skip [ATOM_RD200 -> ATOM_RD300] for READER3
					ReaderAutoDetectInfo.rdrTypeIdx[READER4] += 2;  //To skip [SUP_FP_3030_READER -> CB_U_READER] for READER4
					LoadReaderAutoDetectInfo(READER4);
				}

				if((ReaderAutoDetectInfo.readerType[readerId] == ATOM_RD200_READER) && (readerId == READER3))
				{
					// For atom RD200, also start detection task for reader4
					tryForNextRdrTypeF = TRUE;
					++(ReaderAutoDetectInfo.rdrTypeIdx[readerId]);	//To skip ATOM_RD300
					ReaderAutoDetectInfo.rdrTypeIdx[READER4] += 3;  //To skip [SUP_FP_3030_READER -> CB_U_READER -> ATOM RD100] for READER4
					LoadReaderAutoDetectInfo(READER4);

					// For atom RD200, also start detection task for reader5
					LoadReaderAutoDetectInfo(READER5);
				}

				if((ReaderAutoDetectInfo.readerType[readerId] == ATOM_RD300_READER) && (readerId == READER3))
				{
					// For atom RD300, also start detection task for reader4
					tryForNextRdrTypeF = TRUE;
					ReaderAutoDetectInfo.rdrTypeIdx[READER4] += 4;  //To skip [SUP_FP_3030_READER -> CB_U_READER -> ATOM RD100 -> ATOM RD200] for READER4
					LoadReaderAutoDetectInfo(READER4);

					// For atom RD300, also start detection task for reader5
					LoadReaderAutoDetectInfo(READER5);
				}
			}

			// Update next reader type from auto detection list
			if(tryForNextRdrTypeF == TRUE)
			{
				tryForNextRdrTypeF = FALSE;
				// Increase reader index type so that next type of reader can be detected from list table
				if(++(ReaderAutoDetectInfo.rdrTypeIdx[readerId]) < MAX_AUTO_DETECT_READER_TYPE)
				{
					ReaderAutoDetectInfo.readerType[readerId] = GetAutoDetectReader(ReaderAutoDetectInfo.rdrTypeIdx[readerId], readerId);
					if(ReaderAutoDetectInfo.readerType[readerId] != NO_READER_PRESENT)
					{
						LoadReaderAutoDetectInfo(readerId);
					}
				}
			}
		}

		for(readerId = 0; readerId < MAX_AUTO_DETECT_READER; readerId++)
		{
			// In Panel door this task is only for READER3
			if( (readerId != READER3) && (GetDoorApplType() == PANEL_DOOR_APPL) )
			{
				continue;
			}

			if(ReaderAutoDetectInfo.rdrStatus[readerId] == RDR_DETECT_PROGRESS)
			{
				break;
			}
		}

		if(readerId == MAX_AUTO_DETECT_READER)
		{
			DPRINT(SYS_LOG,"\nReader type 1 [%d] \nReader type 2 [%d] \nReader type 3 [%d] \nReader type 4 [%d] \nReader type 5 [%d]", ReaderAutoDetectInfo.readerType[0],
					ReaderAutoDetectInfo.readerType[1], ReaderAutoDetectInfo.readerType[2], ReaderAutoDetectInfo.readerType[3],ReaderAutoDetectInfo.readerType[READER5]);
			// Auto detection process ends
			ReaderAutoDetectInfo.autoDetectF = FALSE;
			if(GetDoorApplType() == PANEL_DOOR_APPL)
			{
				for(readerId = 0; readerId < MAX_AUTO_DETECT_READER; readerId++)
				{
					// In Panel door this task is only for READER3
					if(readerId != READER3)
					{
						continue;
					}	
				
					if(ReaderAutoDetectInfo.rdrStatus[readerId] != RDR_DETECT_SUCCESS)
					{
						if(NO == CheckComboRdrPresent(COMBO_READER_ALL))
						{
							DisableReaderUart(readerId);
						}
					}
				}
			}
			else
			{
				for(readerId = 0; readerId < MAX_AUTO_DETECT_READER; readerId++)
				{
					if(ReaderAutoDetectInfo.rdrStatus[readerId] != RDR_DETECT_SUCCESS)
					{
						if( (readerId < READER3) || (NO == CheckComboRdrPresent(COMBO_READER_ALL)))
						{
							DisableReaderUart(readerId);
						}
					}
				}
			}
			return;
		}
	}
}

//*****************************************************************************
//	AutoReaderDetect()
//	Param:
//		IN :	readerId
//				readerType
//		OUT:	none
//	Returns:	none
//	Description:
//				It will run task to detect reader type for different reader types
//				mentioned in list
//*****************************************************************************
static void AutoReaderDetect(READER_ID_e readerId)
{
	// Switch based on reader type in Auto detection task
	switch(ReaderAutoDetectInfo.readerType[readerId])
	{
		case HID_ICLASS_U_READER:
			if(HidIclassReaderType[readerId] == HID_READER_OEM_150)
			{
				ReaderAutoDetectInfo.rdrStatus[readerId] = AutoDetectTaskHidIclass(readerId);
			}
			else
			{
				ReaderAutoDetectInfo.rdrStatus[readerId] = AutoDetectTaskHidSEReader(readerId);
			}
		break;

		case SM_PROX_READER:
			ReaderAutoDetectInfo.rdrStatus[readerId] = AutoDetectTaskSM125(readerId);
		break;

		case MIFARE_U_READER:
			if(MifareReaderType[readerId] == MIFARE_U_READER_CLR663)
			{
				ReaderAutoDetectInfo.rdrStatus[readerId] = AutoDetectTaskCLR663(readerId);
			}
			else if(MifareReaderType[readerId] == MIFARE_U_READER_PN532)
			{
				ReaderAutoDetectInfo.rdrStatus[readerId] = AutoDetectTaskPN532(readerId);
			}
			else
			{
				ReaderAutoDetectInfo.rdrStatus[readerId] = AutoDetectTaskSM130(readerId);
			}

		break;

		case SUP_FP_3030_READER:
			ReaderAutoDetectInfo.rdrStatus[readerId] = AutoDetectTaskSuprema3030(readerId);
		break;

		case HID_PROX_READER:
			ReaderAutoDetectInfo.rdrStatus[readerId] = AutoDetectTaskHIDProx(readerId);
		break;

		case COMBO_EXIT_READER:
			ReaderAutoDetectInfo.rdrStatus[readerId] = AutoDetectTaskCer();
		break;

		case CB_U_READER:
		{
			if(readerId == READER4)
			{
				if((ReaderAutoDetectInfo.rdrStatus[READER3] == RDR_DETECT_SUCCESS) || (ReaderAutoDetectInfo.rdrStatus[READER3] == RDR_DETECT_FAIL))
				{
					if(ReaderAutoDetectInfo.comboCardDetectF[COMBO_READER_CB_U] == TRUE)
					{
						ReaderAutoDetectInfo.rdrStatus[readerId] = RDR_DETECT_SUCCESS;
					}
					else
					{
						ReaderAutoDetectInfo.rdrStatus[readerId] = RDR_DETECT_FAIL;
					}
				}
				else
				{
					ReaderAutoDetectInfo.rdrStatus[readerId] = ReaderAutoDetectInfo.rdrStatus[READER3];
				}
			}
			else
			{
				ReaderAutoDetectInfo.rdrStatus[readerId] =  AutoDetectTaskCbr();
			}
		}
		break;


		case ATOM_RD100_READER:
		{
			if(readerId == READER4)
			{
				if((ReaderAutoDetectInfo.rdrStatus[READER3] == RDR_DETECT_SUCCESS) || (ReaderAutoDetectInfo.rdrStatus[READER3] == RDR_DETECT_FAIL))
				{
					if(ReaderAutoDetectInfo.comboCardDetectF[COMBO_READER_ATOM_RD100] == TRUE)
					{
						ReaderAutoDetectInfo.rdrStatus[readerId] = RDR_DETECT_SUCCESS;
					}
					else
					{
						ReaderAutoDetectInfo.rdrStatus[readerId] = RDR_DETECT_FAIL;
					}
				}
				else
				{
					ReaderAutoDetectInfo.rdrStatus[readerId] = ReaderAutoDetectInfo.rdrStatus[READER3];
				}
			}
			else
			{
				ReaderAutoDetectInfo.rdrStatus[readerId] =  AutoDetectTaskAtomRD100();
			}
		}
		break;

		case ATOM_RD200_READER:
		{
			if(readerId == READER4)
			{
				if((ReaderAutoDetectInfo.rdrStatus[READER3] == RDR_DETECT_SUCCESS) || (ReaderAutoDetectInfo.rdrStatus[READER3] == RDR_DETECT_FAIL))
				{
					if(ReaderAutoDetectInfo.comboCardDetectF[COMBO_READER_ATOM_RD200] == TRUE)
					{
						ReaderAutoDetectInfo.rdrStatus[readerId] = RDR_DETECT_SUCCESS;
					}
					else
					{
						ReaderAutoDetectInfo.rdrStatus[readerId] = RDR_DETECT_FAIL;
					}
				}
				else
				{
					ReaderAutoDetectInfo.rdrStatus[readerId] = ReaderAutoDetectInfo.rdrStatus[READER3];
				}
			}
			else
			{
				ReaderAutoDetectInfo.rdrStatus[readerId] =   AutoDetectCommonTaskAtomReader(COMBO_READER_ATOM_RD200);
			}
		}
		break;

		case ATOM_RD300_READER:
		{
			if(readerId == READER4)
			{
				if((ReaderAutoDetectInfo.rdrStatus[READER3] == RDR_DETECT_SUCCESS) || (ReaderAutoDetectInfo.rdrStatus[READER3] == RDR_DETECT_FAIL))
				{
					if(ReaderAutoDetectInfo.comboCardDetectF[COMBO_READER_ATOM_RD300] == TRUE)
					{
						ReaderAutoDetectInfo.rdrStatus[readerId] = RDR_DETECT_SUCCESS;
					}
					else
					{
						ReaderAutoDetectInfo.rdrStatus[readerId] = RDR_DETECT_FAIL;
					}
				}
				else
				{
					ReaderAutoDetectInfo.rdrStatus[readerId] = ReaderAutoDetectInfo.rdrStatus[READER3];
				}
			}
			else
			{
				ReaderAutoDetectInfo.rdrStatus[readerId] =   AutoDetectCommonTaskAtomReader(COMBO_READER_ATOM_RD300);
			}
		}
		break;

		default:
			ReaderAutoDetectInfo.rdrStatus[readerId] = RDR_DETECT_FAIL;
			break;
	} // End of switch- readerType
}

//*****************************************************************************
//	LoadReaderAutoDetectInfo()
//	Param:
//		IN :	readerId
//		OUT:	none
//	Returns:	none
//	Description:
//				It will Initiliaze reader auto detection related information
//*****************************************************************************
static void LoadReaderAutoDetectInfo(UINT8 readerId)
{
	// Change state and status to start reader auto detect related task
	ReaderInitInfo.state[readerId] = READER_INIT_IDLE;
	ReaderAutoDetectInfo.rdrStatus[readerId] = RDR_DETECT_PROGRESS;
	ReaderAutoDetectInfo.readerType[readerId] = GetAutoDetectReader(ReaderAutoDetectInfo.rdrTypeIdx[readerId], readerId);
}

//*****************************************************************************
//	CheckNextMifareReader()
//	Param:
//		IN :	readerId
//				resetF
//		OUT:	none
//	Returns:	TRUE/FALSE
//	Description:
//				It will change mifare reader type SM130/PN512/MAX or reset type to
//				SM130 and return TRUE if valid mifare reader type is set
//				else return FLASE
//*****************************************************************************
BOOL CheckNextMifareReader(UINT8 readerId, BOOL resetF)
{
		BOOL statusF = TRUE;

		// Reset state of mifare reader type is reader SM130
		if(resetF == TRUE)
		{
			MifareReaderType[readerId] = mifareReaderInitPrioritySeq[0];
		}
		else
		{
			// try another reader type from SM130/PN532
			if(MifareReaderType[readerId] == MAX_MIFARE_U_READER_TYPE)
			{
				MifareReaderType[readerId] = mifareReaderInitPrioritySeq[0];
			}
			else if(MifareReaderType[readerId] == mifareReaderInitPrioritySeq[0])
			{
				MifareReaderType[readerId] = mifareReaderInitPrioritySeq[1];
			}
			else if(MifareReaderType[readerId] == mifareReaderInitPrioritySeq[1])
			{
				MifareReaderType[readerId] = mifareReaderInitPrioritySeq[2];
			}
			else
			{
				MifareReaderType[readerId] = MAX_MIFARE_U_READER_TYPE;
			}
		}

		//All Mifare reader type are checked
		if(MifareReaderType[readerId] == MAX_MIFARE_U_READER_TYPE)
		{
			statusF = FALSE;
		}
		return statusF;
}

//*****************************************************************************
//	GetAutoDetectedReaderType()
//	Param:
//		IN :	readerId
//		OUT:	none
//	Returns:	reader type
//	Description:
//				It will retuen detected reader type for given reader Id
//*****************************************************************************
READER_TYPE_e GetAutoDetectedReaderType(UINT8	readerId)
{
	return ReaderAutoDetectInfo.readerType[readerId];
}

//*****************************************************************************
//	CheckNextHidIclassReader()
//	Param:
//		IN :	readerId
//				resetF
//		OUT:	none
//	Returns:	TRUE/FALSE
//	Description:
//				It will change HID reader type SE32X0/OEM150/MAX or reset type to
//				SE32X0 and return TRUE if valid HID reader type is set
//				else return FLASE
//*****************************************************************************
BOOL CheckNextHidIclassReader(UINT8 readerId, BOOL resetF)
{
	BOOL statusF = TRUE;

	// Reset state of HID iclass reader type is reader SE32X0
	if(resetF == TRUE)
	{
		HidIclassReaderType[readerId] = hidIclassReaderInitPrioritySeq[0];
	}
	else
	{
		// try another reader type from OEM150/SE32X0
		if(HidIclassReaderType[readerId] == MAX_HID_READER_TYPE)
		{
			HidIclassReaderType[readerId] = hidIclassReaderInitPrioritySeq[0];
		}
		else if(HidIclassReaderType[readerId] == hidIclassReaderInitPrioritySeq[0])
		{
			HidIclassReaderType[readerId] = hidIclassReaderInitPrioritySeq[1];
		}
		else
		{
			HidIclassReaderType[readerId] = MAX_HID_READER_TYPE;
		}
	}

	//All HID iclass reader type are checked
	if(HidIclassReaderType[readerId] == MAX_HID_READER_TYPE)
	{
		statusF = FALSE;
	}
	return statusF;
}
