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
//   File         : MxReaderConv.c
//   Description  : This file declares system utility API.
//
/////////////////////////////////////////////////////////////////////////////

#include <fcntl.h>

#include "MxTypedef.h"	
#include "MxLogs.h"
#include "MxReaderConv.h"

//******** Extern Variables **********

//******** Defines and Data Types ****
//READER TYPE
typedef enum
{
	WEB_NO_READER_PRESENT = 0,

	WEB_SM_PROX_READER1 = 1,
	WEB_HID_PROX_READER1,
	WEB_MIFARE_U_READER1,
	WEB_HID_ICLASS_U_READER1,
	WEB_HID_ICLASS_W_READER1,
	
	WEB_SUP_FP_3030_READER2 = 1,

	WEB_SM_PROX_READER3 = 1,
	WEB_HID_PROX_READER3,
	WEB_MIFARE_U_READER3,
	WEB_HID_ICLASS_U_READER3,
	WEB_SUP_FP_3030_READER3,
	WEB_HID_ICLASS_W_READER3,
	WEB_UHF_W_READER3,
	WEB_MIFARE_W_READER3,
	WEB_COMBO_EXIT_READER3,
	WEB_PIN_W_READER3,
	WEB_CARD_PIN_W_READER,
	WEB_CB_U_READER,
	WEB_CB_W_READER,
	WEB_ATOM_RD300_READER,
	WEB_ATOM_RD200_READER,
	WEB_ATOM_RD100_READER,
	WEB_MAX_READER_TYPE = 18,
}WEB_READER_TYPE_e;

//READER TYPE
typedef enum
{
	AMCS_NO_READER_PRESENT = 0,

	AMCS_SM_PROX_READER1 = 1,
	AMCS_HID_PROX_READER1,
	AMCS_MIFARE_U_READER1,
	AMCS_HID_ICLASS_U_READER1,
	AMCS_HID_ICLASS_W_READER1,

	AMCS_SUP_FP_3030_READER2 = 1,

	AMCS_SM_PROX_READER3 = 1,
	AMCS_HID_PROX_READER3,
	AMCS_MIFARE_U_READER3,
	AMCS_HID_ICLASS_U_READER3,
	AMCS_SUP_FP_3030_READER3,
	AMCS_HID_ICLASS_W_READER3,
	AMCS_UHF_W_READER3 = 8,
	AMCS_COMBO_EXIT_READER3,
	AMCS_MIFARE_W_READER3,
	AMCS_PIN_W_READER3,
	ACMS_CARD_PIN_W_READER,
	ACMS_CB_U_READER,
	ACMS_CB_W_READER,
	ACMS_ATOM_RD300_READER,
	ACMS_ATOM_RD200_READER,
	ACMS_ATOM_RD100_READER,
	AMCS_MAX_READER_TYPE = 18,
}AMCS_READER_TYPE_e;

//******** Function Prototypes *******

//******** Global Variables **********

//******** Static Variables **********
// Reader type conversion from ACMS TO SYSTEM
static const READER_TYPE_e rdrConvAcmsToSys[AMCS_MAX_READER_TYPE][MAX_READERS] =
{
	//Reader 1					Reader 2				Reader 3
	{NO_READER_PRESENT, 		NO_READER_PRESENT,		NO_READER_PRESENT	},
	{SM_PROX_READER,			SUP_FP_3030_READER, 	SM_PROX_READER	 	},
	{HID_PROX_READER,			NO_READER_PRESENT,		HID_PROX_READER	 	},
	{MIFARE_U_READER,			NO_READER_PRESENT,		MIFARE_U_READER	 	},
	{HID_ICLASS_U_READER,		NO_READER_PRESENT,		HID_ICLASS_U_READER	},
	{HID_ICLASS_W_READER,		NO_READER_PRESENT,		SUP_FP_3030_READER	},
	{NO_READER_PRESENT,			NO_READER_PRESENT,		HID_ICLASS_W_READER	},
	{NO_READER_PRESENT, 		NO_READER_PRESENT,		NO_READER_PRESENT	},
	{NO_READER_PRESENT, 		NO_READER_PRESENT,		UHF_W_READER		},
	{NO_READER_PRESENT, 		NO_READER_PRESENT,		COMBO_EXIT_READER	},
	{NO_READER_PRESENT, 		NO_READER_PRESENT,		MIFARE_W_READER		},
	{NO_READER_PRESENT, 		NO_READER_PRESENT,		PIN_W_READER		},
	{NO_READER_PRESENT, 		NO_READER_PRESENT,		CARD_PIN_W_READER	},
	{NO_READER_PRESENT, 		NO_READER_PRESENT,		CB_U_READER			},
	{NO_READER_PRESENT, 		NO_READER_PRESENT,		CB_W_READER			},
	{NO_READER_PRESENT, 		NO_READER_PRESENT,		ATOM_RD300_READER			},
	{NO_READER_PRESENT, 		NO_READER_PRESENT,		ATOM_RD200_READER			},
	{NO_READER_PRESENT, 		NO_READER_PRESENT,		ATOM_RD100_READER			},
};

// Reader type conversion from SYSTEM TO WEB
static const WEB_READER_TYPE_e rdrConvSysToWeb[MAX_READER_TYPE][MAX_READERS] =
{
	//Reader 1					Reader 2					Reader 3
	{WEB_NO_READER_PRESENT,		WEB_NO_READER_PRESENT,		WEB_NO_READER_PRESENT	},
	{WEB_SM_PROX_READER1,		WEB_NO_READER_PRESENT,		WEB_SM_PROX_READER3		},
	{WEB_HID_PROX_READER1,		WEB_NO_READER_PRESENT,		WEB_HID_PROX_READER3	},
	{WEB_MIFARE_U_READER1,		WEB_NO_READER_PRESENT,		WEB_MIFARE_U_READER3	},
	{WEB_HID_ICLASS_U_READER1,	WEB_NO_READER_PRESENT,		WEB_HID_ICLASS_U_READER3},
	{WEB_NO_READER_PRESENT,		WEB_SUP_FP_3030_READER2,	WEB_SUP_FP_3030_READER3	},
	{WEB_HID_ICLASS_W_READER1,	WEB_NO_READER_PRESENT,		WEB_HID_ICLASS_W_READER3},
	{WEB_NO_READER_PRESENT,		WEB_NO_READER_PRESENT,		WEB_NO_READER_PRESENT	},
	{WEB_NO_READER_PRESENT,		WEB_NO_READER_PRESENT,		WEB_UHF_W_READER3		},
	{WEB_NO_READER_PRESENT,		WEB_NO_READER_PRESENT,		WEB_COMBO_EXIT_READER3	},
	{WEB_NO_READER_PRESENT,		WEB_NO_READER_PRESENT,		WEB_MIFARE_W_READER3	},
	{WEB_NO_READER_PRESENT,		WEB_NO_READER_PRESENT,		WEB_PIN_W_READER3		},
	{WEB_NO_READER_PRESENT,		WEB_NO_READER_PRESENT,		WEB_CARD_PIN_W_READER	},
	{WEB_NO_READER_PRESENT,		WEB_NO_READER_PRESENT,		WEB_CB_U_READER			},
	{WEB_NO_READER_PRESENT,		WEB_NO_READER_PRESENT,		WEB_CB_W_READER			},
	{WEB_NO_READER_PRESENT,		WEB_NO_READER_PRESENT,		WEB_ATOM_RD300_READER			},
	{WEB_NO_READER_PRESENT,		WEB_NO_READER_PRESENT,		WEB_ATOM_RD200_READER			},
	{WEB_NO_READER_PRESENT,		WEB_NO_READER_PRESENT,		WEB_ATOM_RD100_READER			},
};

// Reader type conversion from SYSTEM TO ACMS
static const AMCS_READER_TYPE_e rdrConvSysToAcms[MAX_READER_TYPE][MAX_READERS] =
{
	//Reader 1						Reader 2					Reader 3
	{AMCS_NO_READER_PRESENT,		AMCS_NO_READER_PRESENT,		AMCS_NO_READER_PRESENT		},		// No reader
	{AMCS_SM_PROX_READER1,			AMCS_NO_READER_PRESENT,		AMCS_SM_PROX_READER3		},	// Reads EM prox card (SonMicro- SM125- UART)
	{AMCS_HID_PROX_READER1,			AMCS_NO_READER_PRESENT,		AMCS_HID_PROX_READER3		},	// Reads HID prox card (HID- prox pint plus- WIEGAND)
	{AMCS_MIFARE_U_READER1,			AMCS_NO_READER_PRESENT,		AMCS_MIFARE_U_READER3		},// Reads/Writes MIFARE cards (SonMicro- SM130- UART)
	{AMCS_HID_ICLASS_U_READER1,		AMCS_NO_READER_PRESENT,		AMCS_HID_ICLASS_U_READER3	},// Reads/Writes HID iclass cards (HID- ICLASS GEN2- UART)
	{AMCS_NO_READER_PRESENT,		AMCS_SUP_FP_3030_READER2,	AMCS_SUP_FP_3030_READER3	},	// Scans finger print (SUPREMA- SFM3030OD-OPTICAL SENSOR- UART)
	{AMCS_HID_ICLASS_W_READER1,		AMCS_NO_READER_PRESENT,		AMCS_HID_ICLASS_W_READER3	},	// Reads HID iclass card CSN (HID- ICLASS- WIEGAND)
	{AMCS_NO_READER_PRESENT,		AMCS_NO_READER_PRESENT,		AMCS_NO_READER_PRESENT		},	// Scans Palm vein image(FUJITSU- FAT13M1S1- USB)
	{AMCS_NO_READER_PRESENT,		AMCS_NO_READER_PRESENT,		AMCS_UHF_W_READER3			},// UHF reader Wiegand (Only as READER 3)
	{AMCS_NO_READER_PRESENT,		AMCS_NO_READER_PRESENT,		AMCS_COMBO_EXIT_READER3		},
	{AMCS_NO_READER_PRESENT,		AMCS_NO_READER_PRESENT,		AMCS_MIFARE_W_READER3		},
	{AMCS_NO_READER_PRESENT,		AMCS_NO_READER_PRESENT,		AMCS_PIN_W_READER3			},
	{AMCS_NO_READER_PRESENT,		AMCS_NO_READER_PRESENT,		ACMS_CARD_PIN_W_READER		},
	{AMCS_NO_READER_PRESENT,		AMCS_NO_READER_PRESENT,		ACMS_CB_U_READER			},
	{AMCS_NO_READER_PRESENT,		AMCS_NO_READER_PRESENT,		ACMS_CB_W_READER			},
	{AMCS_NO_READER_PRESENT,		AMCS_NO_READER_PRESENT,		ACMS_ATOM_RD300_READER			},
	{AMCS_NO_READER_PRESENT,		AMCS_NO_READER_PRESENT,		ACMS_ATOM_RD200_READER			},
	{AMCS_NO_READER_PRESENT,		AMCS_NO_READER_PRESENT,		ACMS_ATOM_RD100_READER			},
};

//******** Function Definations ******

//*****************************************************************************
//	ConvertRdrType
//	Param: 
//		IN :	readerId	- reader id
//				convType	- conversion type like 'SYS to WEB'
//				inReaderType- reader type
//		OUT:	none
//	Returns:	none
//	Description:
//			It Converts reader type value from system to web or web to system
//			and returns converted value.
//				
//*****************************************************************************
UINT8 ConvertRdrType(UINT8 readerId, UINT8 inReaderType, READER_CONV_TYPE_e convType)
{
	UINT8 outReaderType;

	if( (convType >= MAX_READER_CONV_TYPE) || (inReaderType >= WEB_MAX_READER_TYPE)
												  || (readerId >= MAX_READERS) )
	{
		outReaderType = WEB_NO_READER_PRESENT;
	}
	else
	{
		if(convType == ACMS_TO_SYS)
		{
			outReaderType = rdrConvAcmsToSys[inReaderType][readerId];
		}
		else if(convType == SYS_TO_ACMS)
		{
			outReaderType = rdrConvSysToAcms[inReaderType][readerId];
		}
		else if(convType == SYS_TO_WEB)
		{
			outReaderType = rdrConvSysToWeb[inReaderType][readerId];
		}
	}
	return outReaderType;
}

//*****************************************************************************
//ConvertPanelRdrTypToDD
//Param
//		Input:		PANEL_READER_TYPE_e
//		Output:		NONE
//		Return:		READER_TYPE_e
//
//Description
//			This function convert reader type coming form panel to door enum list.
//*****************************************************************************
READER_TYPE_e ConvertPanelRdrTypToDD(PANEL_READER_TYPE_e panelRdrTyp)
{
	READER_TYPE_e		retRdrTyp = MAX_READER_TYPE;

	switch(panelRdrTyp)
	{
	case PANEL_SM_PROX_READER:
		retRdrTyp = SM_PROX_READER;
		break;
	case PANEL_HID_PROX_READER:
		retRdrTyp = HID_PROX_READER;
		break;
	case PANEL_MIFARE_U_READER:
		retRdrTyp = MIFARE_U_READER;
		break;
	case PANEL_HID_ICLASS_U_READER:
		retRdrTyp = HID_ICLASS_U_READER;
		break;
	case PANEL_SUP_FP_3030_READER:
		retRdrTyp = SUP_FP_3030_READER;
		break;
	case PANEL_NO_READER_PRESENT:
		retRdrTyp = NO_READER_PRESENT;
		break;
	case PANEL_HID_ICLASS_W_READER:
		retRdrTyp = HID_ICLASS_W_READER;
		break;
	case PANEL_FUJ_PALM_VEIN_READER:
		retRdrTyp = FUJ_PALM_VEIN_READER;
		break;
	case PANEL_UHF_W_READER:
		retRdrTyp = UHF_W_READER;
		break;
	case PANEL_COMBO_EXIT_READER:
		retRdrTyp = COMBO_EXIT_READER;
		break;
	case PANEL_MIFARE_W_READER:
		retRdrTyp = MIFARE_W_READER;
		break;
	case PANEL_PIN_W_READER:
		retRdrTyp = PIN_W_READER;
		break;
	case PANEL_CB_U_READER:
		retRdrTyp = CB_U_READER;
		break;
	case PANEL_CB_W_READER:
		retRdrTyp = CB_W_READER;
		break;
	case PANEL_ATOM_RD300_READER:
		retRdrTyp = ATOM_RD300_READER;
		break;
	case PANEL_ATOM_RD200_READER:
		retRdrTyp = ATOM_RD200_READER;
		break;
	case PANEL_ATOM_RD100_READER:
		retRdrTyp = ATOM_RD100_READER;
		break;
		
	default:
		break;
	}
	return retRdrTyp;
}

//*****************************************************************************
//ConvertDDRdrTypToPanel
//Param
//		Input:		READER_TYPE_e
//		Output:		NONE
//		Return:		PANEL_READER_TYPE_e
//
//Description
//			This function convert reader type coming form panel to door enum list.
//*****************************************************************************
PANEL_READER_TYPE_e ConvertDDRdrTypToPanel(READER_TYPE_e ddRdrTyp)
{
	PANEL_READER_TYPE_e		retRdrTyp = MAX_PANEL_READER_TYPE;

	switch(ddRdrTyp)
	{
	case SM_PROX_READER:
		retRdrTyp = PANEL_SM_PROX_READER;
		break;
	case HID_PROX_READER:
		retRdrTyp = PANEL_HID_PROX_READER;
		break;
	case MIFARE_U_READER:
		retRdrTyp = PANEL_MIFARE_U_READER;
		break;
	case HID_ICLASS_U_READER:
		retRdrTyp = PANEL_HID_ICLASS_U_READER;
		break;
	case SUP_FP_3030_READER:
		retRdrTyp = PANEL_SUP_FP_3030_READER;
		break;
	case NO_READER_PRESENT:
		retRdrTyp = PANEL_NO_READER_PRESENT;
		break;
	case HID_ICLASS_W_READER:
		retRdrTyp = PANEL_HID_ICLASS_W_READER;
		break;
	case FUJ_PALM_VEIN_READER:
		retRdrTyp = PANEL_FUJ_PALM_VEIN_READER;
		break;
	case UHF_W_READER:
		retRdrTyp = PANEL_UHF_W_READER;
		break;
	case COMBO_EXIT_READER:
		retRdrTyp = PANEL_COMBO_EXIT_READER;
		break;
	case MIFARE_W_READER:
		retRdrTyp = PANEL_MIFARE_W_READER;
		break;
	case PIN_W_READER:
		retRdrTyp = PANEL_PIN_W_READER;
		break;
	case CARD_PIN_W_READER:
		retRdrTyp = PANEL_CARD_PIN_W_READER;
		break;
	case CB_U_READER:
		retRdrTyp = PANEL_CB_U_READER;
		break;
	case CB_W_READER:
		retRdrTyp = PANEL_CB_W_READER;
		break;
	case ATOM_RD300_READER:
		retRdrTyp = PANEL_ATOM_RD300_READER;
		break;
	case ATOM_RD200_READER:
		retRdrTyp = PANEL_ATOM_RD200_READER;
		break;
	case ATOM_RD100_READER:
		retRdrTyp = PANEL_ATOM_RD100_READER;
		break;
		
	default:
		break;
	}
	return retRdrTyp;
}


