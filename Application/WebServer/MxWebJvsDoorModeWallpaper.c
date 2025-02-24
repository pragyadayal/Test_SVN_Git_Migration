///////////////////////////////////////////////////////////////////////////
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
//   Created By   : Niraj Usadadiya
//   File         : MxWebJvsDoorModeWallpaper.c
//   Description  :
//					This file defines the API related to Upload/Edit Door
//					Mode Customize wallpapers.
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
//system includes
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

//Matrix include files
#include "MxWebJeeves.h"
#include "MxTypedef.h"
#include "MxConfig.h"
#include "MxLogs.h"
#include "MxWebJvsDoorModeWallpaper.h"
#include "MxSysUtils.h"
#include "MxAvResp.h"
#include "MxGuiHandler.h"
#include "MxDcTaskUtils.h"
#include "MxConfigFileIo.h"

//******** Extern Variables **********
extern SYS_BASIC_PARA_t	  		SysBasicPara;

//******** Defines and Data Types ****
typedef enum
{
	IDX_OF_WALLPAPER,
	FILE_PATH_WALLPAPER,
}WALLPAPER_INFO_TYPE_e;

typedef enum
{
	WALLPAPER_FILE_TYPE_NONE,
	WALLPAPER_IMAGE,
}WALLPAPER_FILE_TYPE_e;

#define GEN_MODE_SPECIFIC_WALLPAPER_IMAGE_ENUM(MODE_SPECIFIC_WALLPAPER_IMAGE_ENUM, MODE_SPECIFIC_WALLPAPER_IMAGE_NAME) MODE_SPECIFIC_WALLPAPER_IMAGE_ENUM,
#define GEN_MODE_SPECIFIC_WALLPAPER_IMAGE_NAME(MODE_SPECIFIC_WALLPAPER_IMAGE_ENUM, MODE_SPECIFIC_WALLPAPER_IMAGE_NAME) MODE_SPECIFIC_WALLPAPER_IMAGE_NAME,

#define MODE_SPECIFIC_WALLPAPER_IMAGE(API)\
	API(MODE_SPECIFIC_WALLPAPER_IMAGE_DATA_TYPE,		"dataType")\
	API(MODE_SPECIFIC_WALLPAPER_IMAGE_INDEX,         	"index")\
	API(MODE_SPECIFIC_WALLPAPER_IMAGE_FILE,          	"file")\
	API(MODE_SPECIFIC_WALLPAPER_IMAGE_1,          		"WP_1")\
	API(MODE_SPECIFIC_WALLPAPER_IMAGE_2,          		"WP_2")\
	API(MODE_SPECIFIC_CURRENT_WALLPAPER,          		"currentWallPaper")\
	API(MODE_SPECIFIC_WALLPAPER_REMOVE_1,          		"WP_REMOVE_1")\
	API(MODE_SPECIFIC_WALLPAPER_REMOVE_2,          		"WP_REMOVE_2")\
	API(MODE_SPECIFIC_WALLPAPER_ENTRY_SET_WP,       	"WP_ENTRY_SET_WP")\
	API(MODE_SPECIFIC_WALLPAPER_EXIT_SET_WP,          	"WP_EXIT_SET_WP")\



typedef enum
{
	MODE_SPECIFIC_WALLPAPER_IMAGE(GEN_MODE_SPECIFIC_WALLPAPER_IMAGE_ENUM)
	MAX_MODE_SPECIFIC_WALLPAPER_IMAGE,
}MAX_MODE_SPECIFIC_WALLPAPER_IMAGE_e;


//******** Function Prototypes *******

//******** Global Variables **********

//******** Static Variables **********
static UINT8	nextFileIdx = 0;
static UINT8	nextFileType = FILE_TYPE_NONE;
static CHARPTR	DfltDoorModeWallpapeerStr[] =
				{
					"IN_Wallpaper",
					"OUT_Wallpaper"
				};
static CHARPTR	CustomDoorModeWallpapeerStr[] =
				{
					"Entry",
					"Exit"
				};

static CHARPTR modeSpecificWallpaperimage [MAX_MODE_SPECIFIC_WALLPAPER_IMAGE]=
{
		MODE_SPECIFIC_WALLPAPER_IMAGE(GEN_MODE_SPECIFIC_WALLPAPER_IMAGE_NAME)
};
//******** Function Definations ******





//*****************************************************************************
//	RemoveModeSpecificWallpaper()
//	Param:
//		IN: pRXDataFile - pointer to  received data file (RxData.txt).
//		OUT:NONE.
//	Returns:NONE
//	Description:
//			When user select update file(wallpaper image) at client
//	 		side, function uploads file at respective folder.
//*****************************************************************************
UINT8 RemoveModeSpecificWallpaper(FILE *pRXDataFile, FILE *pTXDataFile)
{
	UINT32 	tempDigit;
	UINT8 	parseStatus, getElementStatus, elementIdx,status = FAIL;
	CHAR 	tempFieldName[MAX_WEB_JVS_FIELD_NAME_LENGTH], tempFieldValue[MAX_WEB_JVS_FIELD_VALUE_LENGTH], cmdBuf[255];

	while( (parseStatus = ParseQueryField(pRXDataFile, &tempFieldName[0], MAX_WEB_JVS_FIELD_NAME_LENGTH, &tempFieldValue[0], MAX_WEB_JVS_FIELD_VALUE_LENGTH) ) != FAIL)
	{
		DPRINT(WEBJVS_LOG,"%s=%s", tempFieldName, tempFieldValue);
		if( (getElementStatus = GetWebPageElementIdx(&modeSpecificWallpaperimage[0], MAX_MODE_SPECIFIC_WALLPAPER_IMAGE, tempFieldName, &elementIdx)) == SUCCESS)
		{
			switch(elementIdx)
			{
				case MODE_SPECIFIC_WALLPAPER_IMAGE_INDEX:
				{
					tempDigit = (UINT8)atoi(tempFieldValue);
					nextFileIdx = tempDigit;
				}break;

			default:
				break;
			}
		}
		if( (parseStatus == LAST_FIELD) || (parseStatus == LAST_FIELD_WITHOUT_DATA) )
		{
			if( (nextFileIdx == ENTRY_READER) || (nextFileIdx == EXIT_READER) )
			{
				//Now DELETE the file from CustomImage/Wallpapers/ folder
				sprintf(cmdBuf, "rm -f %s%s%s.*", CUSTOM_IMAGE_PATH, DOOR_MODE_WALLPAPER_PREFIX, CustomDoorModeWallpapeerStr[nextFileIdx]);
				system(cmdBuf);

				status = SUCCESS;
			}
			else
			{
				EPRINT(WEBJVS_LOG,"Delete request for invalid door mode wallpaper[%d]", nextFileIdx);
				status = FAIL;
			}
			break;
		}

	}
	if(status == SUCCESS)
	{
		UpdateDoorIdleDisplay();
		SendMessageToQt("", GUI_MSG_REFRESH_WALLPAPER);
		WriteWebResponse(pTXDataFile, "0");
		WriteJSONStartTag(pTXDataFile);	//JSON Data Start
		FillModeSpecificWallpaper(pTXDataFile);
		WriteJSONENDTag(pTXDataFile);	//JSON Data END
	}
	return status;
}

//*****************************************************************************
//	SetXmlDoorModeWallpaper()
//	Param:
//		IN: pRXDataFile - pointer to  received data file (RxData.txt).
//		OUT:NONE.
//	Returns:NONE
//	Description:
//			When user select update file(wallpaper image) at client
//	 		side, function uploads file at respective folder.
//*****************************************************************************
UINT8 SetModeSpecificWallpaper(FILE *pRXDataFile, FILE *pTXDataFile)
{
	UINT32 	tempDigit, dataType;
	struct stat buf;
	CHAR 	tempFieldName[MAX_WEB_JVS_FIELD_NAME_LENGTH], tempFieldValue[MAX_WEB_JVS_FIELD_VALUE_LENGTH];
	UINT8 	parseStatus, getElementStatus, elementIdx,status = FAIL;
	CHAR	cmdBuf[255], extStr[10];
	CHARPTR	strPtr= NULL_CHAR;
	CHAR	fileStr[256];

	while( (parseStatus = ParseQueryField(pRXDataFile, &tempFieldName[0], MAX_WEB_JVS_FIELD_NAME_LENGTH, &tempFieldValue[0], MAX_WEB_JVS_FIELD_VALUE_LENGTH) ) != FAIL)
	{
		DPRINT(WEBJVS_LOG,"%s=%s", tempFieldName, tempFieldValue);
		if( (getElementStatus = GetWebPageElementIdx(&modeSpecificWallpaperimage[0], MAX_MODE_SPECIFIC_WALLPAPER_IMAGE, tempFieldName, &elementIdx)) == SUCCESS)
		{
			switch(elementIdx)
			{

				case MODE_SPECIFIC_WALLPAPER_IMAGE_DATA_TYPE:
				{
					dataType = (MSG_INFO_TYPE_e)atoi(tempFieldValue);
				}break;

				case MODE_SPECIFIC_WALLPAPER_IMAGE_INDEX:
				{
					tempDigit = (UINT8)atoi(tempFieldValue);
					nextFileIdx = tempDigit;
				}break;

				case MODE_SPECIFIC_WALLPAPER_IMAGE_FILE:
				{
					strncpy(fileStr ,tempFieldValue ,(strlen(tempFieldValue)+1));
				}break;

				case MODE_SPECIFIC_WALLPAPER_ENTRY_SET_WP:
				{
					SysBasicPara.entryModeWallpaperF = (BOOL)atoi(tempFieldValue);
				}break;

				case MODE_SPECIFIC_WALLPAPER_EXIT_SET_WP:
				{
					SysBasicPara.exitModeWallpaperF = (BOOL)atoi(tempFieldValue);
				}break;

				default:
				break;
			}
		}
		if( (parseStatus == LAST_FIELD) || (parseStatus == LAST_FIELD_WITHOUT_DATA) )
		{
			DPRINT(WEBJVS_LOG, "Successfully get images index and file type");
			WriteBasicParaConfig();
			status= SUCCESS;
			break;
		}

	}

	if(dataType == WALLPAPER_IDX)
	{
		nextFileType = IMG_DOOR_MODE_WALLPAPER;
	}
	else if(dataType == FILE_PATH)
	{
		strPtr= strstr(fileStr, ".");
		if(strPtr != NULL_CHAR)
		{
			snprintf(extStr, 10, "%s", strPtr);

			sprintf(cmdBuf, "%s%s", WEB_BIN_PATH, fileStr);
			if(stat(cmdBuf, &buf) == 0)
			{
				switch(nextFileType)
				{
					case IMG_DOOR_MODE_WALLPAPER:
					{
						//Now remove file from Custom Image Folder
						sprintf(cmdBuf, "rm -rf %s%s%s.*", CUSTOM_IMAGE_PATH, DOOR_MODE_WALLPAPER_PREFIX, CustomDoorModeWallpapeerStr[nextFileIdx]);
						system(cmdBuf);

						//Now move the file into CustomImage/Wallpapers/ folder
						sprintf(cmdBuf, "mv %s%s %s%s%s%s", WEB_BIN_PATH, fileStr, CUSTOM_IMAGE_PATH, DOOR_MODE_WALLPAPER_PREFIX, CustomDoorModeWallpapeerStr[nextFileIdx], extStr);
						system(cmdBuf);

						status = SUCCESS;
						break;
					}

					default:
						break;
				}
			}
		}
	}
	else
	{
		status = SUCCESS;
	}
	
	if(status == SUCCESS)
	{
		//Refersh wallpaper to reflect any changes on display
		UpdateDoorIdleDisplay();
		SendMessageToQt("", GUI_MSG_REFRESH_WALLPAPER);
		WriteWebResponse(pTXDataFile, "0");
		WriteJSONStartTag(pTXDataFile);	//JSON Data Start
		FillModeSpecificWallpaper(pTXDataFile);
		WriteJSONENDTag(pTXDataFile);	//JSON Data END
	}
	return status;
}

//*****************************************************************************
//	GetXmlDoorModeWallpaper()
//	Param:
//		IN:	pHeader - Pointer to Page Header.
//		OUT:pDataFile - Pointer to Xml file discriptor where data is stored.
//	Returns:SUCCESS/FAIL/WAIT
//	Description:This function sends path of existing wallpapers.
//*****************************************************************************
UINT8 GetModeSpecificWallpaper(UNUSED_PARA FILE *pRXDataFile, FILE *pTXDataFile)
{
	WriteWebResponse(pTXDataFile, "0");
	WriteJSONStartTag(pTXDataFile);	//JSON Data Start
	FillModeSpecificWallpaper(pTXDataFile);
	WriteJSONENDTag(pTXDataFile);	//JSON Data END
	return SUCCESS;
}
//*****************************************************************************
//	FillWallpaperImage()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************
UINT8 FillModeSpecificWallpaper(FILE* pTXDataFile)
{
	UINT8 status = SUCCESS, idx;
	CHAR buf[255],valueStr[MAX_WEB_JVS_FIELD_VALUE_LENGTH];
	FILE *fp;

	for(idx = 0; idx <= 1; idx++)
	{
		sprintf(buf, "ls /matrix/customImage/Wallpapers/DoorModeWallpapers/%s* > /tmp/wallpaperPath.txt", CustomDoorModeWallpapeerStr[idx]);
		system(buf);
		
		fp = fopen("/tmp/wallpaperPath.txt", "r");
		if(fp)
		{
			if(fgets(buf, sizeof(buf), fp) == NULL)
			{
				//Custom Door mode wallpaper not found
				DPRINT(CONFIG_LOG, "Failed to read temp wallpaper[%s] path file", CustomDoorModeWallpapeerStr[idx]);
				fclose(fp);
				//Check if default door mode wallpaper is present
				sprintf(buf, "ls /matrix/customImage/Wallpapers/DoorModeWallpapers/%s* > /tmp/wallpaperPath.txt", DfltDoorModeWallpapeerStr[idx]);
				system(buf);

				fp = fopen("/tmp/wallpaperPath.txt", "r");
				if(fp)
				{
					if(fgets(buf, sizeof(buf), fp) == NULL)
					{
						EPRINT(CONFIG_LOG, "Failed to read temp wallpaper[%s] path file", DfltDoorModeWallpapeerStr[idx]);
						sprintf(valueStr,"%s", ";");
						WriteJSONStringData(pTXDataFile, modeSpecificWallpaperimage[idx+3], valueStr);
					}
					else
					{
						buf[(strlen(buf)-1)] = '\0';
						sprintf(valueStr, "%s%s",  DfltDoorModeWallpapeerStr[idx],strstr(buf, "."));
						WriteJSONStringData(pTXDataFile, modeSpecificWallpaperimage[idx+3], valueStr);
					}
				}
				WriteJSONNumericData(pTXDataFile, modeSpecificWallpaperimage[idx+6], DISABLED);
			}
			else
			{
				buf[(strlen(buf)-1)] = '\0';
				sprintf(valueStr, "%s%s",  CustomDoorModeWallpapeerStr[idx],strstr(buf, "."));
				WriteJSONStringData(pTXDataFile, modeSpecificWallpaperimage[idx+3], valueStr);
				WriteJSONNumericData(pTXDataFile, modeSpecificWallpaperimage[idx+6], ENABLED);
			}
			fclose(fp);
		}
	}

	WriteJSONNumericData(pTXDataFile, modeSpecificWallpaperimage[MODE_SPECIFIC_WALLPAPER_ENTRY_SET_WP], SysBasicPara.entryModeWallpaperF);
	WriteJSONNumericData(pTXDataFile, modeSpecificWallpaperimage[MODE_SPECIFIC_WALLPAPER_EXIT_SET_WP], SysBasicPara.exitModeWallpaperF);
	status = SUCCESS;
	return status;
}
