#if !defined MXENROLLMENT_H
#define MXENROLLMENT_H
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
//   Project      : ACS ( Access Control System)
//   Created By   : Ronak Tanna
//   File         : MxEnrollment.h
//   Description  : This file declares Enrollment related defines and prototypes
//
/////////////////////////////////////////////////////////////////////////////

//******** Include files ****
#include "MxConfig.h"
#include "MxSlaveCommonMsgList.h"
#include "MxCardReaderCommon.h"
#include "MxSysTimer.h"
#include "MxWebAPICommonDef.h"
#include "MxSlaveCommon.h"
#include "MxCommonEnrollment.h"

//******** Defines and Data Types ****

#define ENROLLMENT_USER_RESP_TIMER		3	// in seconds
#define DELETE_USER_RESP_TIMER			5	// in seconds

#define MAX_USER_NAME_SIZE	(4+1+15)		//User Id +  + User Name
#define MAX_SORT_LIST_SIZE	10
#define BASE_64_ENCODED_BIO_TMP_SIZE 	516
#define MAX_SIZE_BIO_TEMPLATE 			384
#define MAX_FP_IMAGE_COUNT				100

typedef enum
{
	ENROLL_CATEGORY_USER,	// enrollment is for user
	ENROLL_CATEGORY_SP_FN,	// enrollment is for special function
	MAX_ENROLL_CATEGORY
}ENROLL_CATEGORY_e;

// finger print quality information
// NOTE: the order in which following info is defined is used as reference in ARR. SO care must be taken while changing order or any information.
typedef enum	
{
	FP_QUALITY_WEAK,		// if score is less than 40
	FP_QUALITY_MODERATE,	// if score is less than 60
	FP_QUALITY_STRONG,		// if score is less than 80
	FP_QUALITY_STRONGEST,	// if score is above 80
	MAX_FP_QUALITY_INFO,
}FP_QUALITY_INFO_e;

typedef enum
{
	ENROLL_USER_SCREEN,
	ENROLL_SPL_FN_CARD_SCREEN,
	GET_USER_CREDENTIAL_SCREEN,
	GET_SPL_FN_CARD_SCREEN,
	ENROLL_USER_FACE_SCREEN,
	MAX_ENROLLMENT_SCREEN,
}ENROLLMENT_SCREEN_e;

typedef enum
{
	FAIL_RESPONSE_FROM_SERVER,
	USER_RECORD_NOT_FOUND,
	SERVER_API_SUCCESS,

	MAX_SERVER_API_RESP,
}SERVER_API_RESP_e;

typedef enum
{
	ENROLLMENT_FACECNT_1		= 1,
	ENROLLMENT_FACECNT_10 		= 10,
	ENROLLMENT_FACECNT_11		= 11,
	ENROLLMENT_FACECNT_20		= 20,
	ENROLLMENT_FACECNT_21		= 21,
	ENROLLMENT_FACECNT_30		= 30,
	
}ENROLLMENT_FACECNT_e;


typedef enum
{
	ENROLLMENT_SOURCE_QT_GUI			= 1,
	ENROLLMENT_SOURCE_DEVICE_GUI,
	ENROLLMENT_SOURCE_ACMS_GUI,
	ENROLLMENT_SOURCE_API,
	ENROLLMENT_SOURCE_MAX,
}ENROLLMENT_SOURCE_e;

typedef enum
{
	ENROLL_FACE_QT_POPUP_TYPE_USER_WARNING,
	ENROLL_FACE_QT_POPUP_TYPE_USER_CONFLICT,
	ENROLL_FACE_QT_POPUP_TYPE_USER_OTHER,
	ENROLL_FACE_QT_POPUP_TYPE_PROCESSING,
}ENROLL_FACE_QT_POPUP_TYPE_e;

//added_

typedef struct
{
	UINT8										timeOutSec; //Timeout in seconds
	ENROLL_FACE_QT_POPUP_TYPE_e					popupType; // popuptype with userinput or not
	UINT8										warningIconShow; // for warning show or not
	CHAR										AlertString[512]; // for alert string
	CHAR										CountDownString[512]; //for count down string
}ENROLL_FACE_MSG_INFO_TO_QT_t;

//Flags for each field to be written into card
typedef struct
{
	BOOL 			addlSecCodeF;		// if 'SET', write additional security code into smart card
	BOOL 			facCodeF;			// if 'SET', write facility code  into smart card
	BOOL 			userIdF;			// if 'SET', write user id into smart card
	UINT8 			noOfTemplates;		// write selected no. of finger print template into smart card
	
	BOOL 			userNameF;			// if 'SET', write user name into smart card
	BOOL 			designationF;		// if 'SET', write designation into smart card	
	BOOL 			branchF;			// if 'SET', write branch into smart card
	BOOL 			departmentF;		// if 'SET', write department into smart card
	BOOL 			bloodGrpF;			// if 'SET', write blood group into smart card
	BOOL 			emrgncyContactF;	// if 'SET', write contact number into smart card
	BOOL 			medicalHistoryF;	// if 'SET', write medical history into smart card
	BOOL			cardIdF;
}CARD_WRITE_FIELDS_t;

//Data for each field to be written into card
typedef struct
{
	CHAR			userName[CARD_FIELD_LEN_USER_NAME];			// user name
	CHAR			designation[CARD_FIELD_LEN_DESIGNATION];	// designation
	CHAR			branch[CARD_FIELD_LEN_BRANCH];				// branch
	CHAR			department[CARD_FIELD_LEN_DEPARTMENT];		// department
	CHAR			bloodGrp[CARD_FIELD_LEN_BLOOD_GRP];			// blood group
	CHAR			emrgncyContact[CARD_FIELD_LEN_CONTCAT_NO];	// emergency contact number
	CHAR			medicalHistory[CARD_FIELD_LEN_MED_HISTORY];	// medical history
}CARD_PERSONAL_INFO_t;

typedef struct
{
	ENROLL_SOURCE_e			source;				// source of enrollment
	ENROLL_CATEGORY_e		category;			// user or special function?
	ENROLLMENT_TYPE_e 		enrollmentType;		// what type of credential enrollment	
	UINT32					userId;				// user id of card being processed for enrollment
	CHAR					alphaNumId[ALPHANUM_ID_SIZE];// alphanumeric user ID
	SPECIAL_FUNCTIONS_e		splFnIdx;			// id of special function that is to be enrolled
	UINT8					fpCnt;				// no. of finger print to be enroll
	UINT8					cardCnt;			// no. of card to be enroll
	CARD_WRITE_FIELDS_t		cardWriteFields;	// flag for each field to be written into smartcard
	CARD_PERSONAL_INFO_t	cardPersonalInfo;	// personalization information to be written into smartcard
    UINT8                   accessCardSelect;   // either access card 1 or 2? Which needs to be enrolled?
    CHAR                    accessId[ACCESS_ID_SIZE];// access ID according to selected access card
    UINT8					faceCnt;			// no. of face to be enroll
    UINT8					faceFrameCnt;			// no. of face frame to be capture
}ENROLLMENT_INPUTS_t;

typedef struct
{
	ENROLLMENT_TYPE_e 		enrollmentType;		// enrollment type
	UINT8					fpCnt;				// no. of finger print to be enroll
	UINT8					cardCnt;			// no. of card to be enroll
	UINT8					faceCnt;			// no. of face to be enroll
	BOOL 					enrollRepeatF;		// indication wether
	CHAR					alphaNumId[ALPHANUM_ID_SIZE];//alphanumeric user ID
	CHAR					name[NAME_SIZE_UTF8];//User Name to be Enrolled
    	UINT8					faceFrameCnt;			// no. of face frame to be capture
	UINT8					ConflictType;
}ENROLLMENT_TEMP_INPUTS_t;

typedef struct
{
	UINT8 credType;
	UINT8 credPtr;
	BOOL compressEnrlF;
	UINT64 cardCread[MAX_USER_CARD];
	UINT8 bioTemplate[MAX_USER_FP][BASE_64_ENCODED_BIO_TMP_SIZE];
	UINT8 compressedTemplate[WEB_API_COMPRESS_PALM_SIZE];
	//todo: lumidigm and pvr

}UPDATE_ENROLLMENT_RESP_DATA_t;
//******** Function Prototypes *******
//added_
ENROLL_FACE_STATE_e GetCurrentFaceEnrollmentState(void);

void DcEnrollmentStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
ENROLLMENT_STATUS_e RequestEnrollment(ENROLLMENT_INPUTS_t * inputDataPtr);
BOOL DeleteUserFp(UINT16 userIdx);
UINT8 EnrollUserFpOnAllFpr(UINT16 userIdx, UINT8 fpIdx, ENROLL_SOURCE_e enrollmentSource);
void TerminateEnrollmentActivity(void);
UINT32 GetUserIdxBeingEnrolled(void);
SPECIAL_FUNCTIONS_e GetSplFnIdxBeingEnrolled(void);
ENROLL_CATEGORY_e GetEnrollmentCategory(void);
void InitEnrollmentActivity(void);
void SendMsgChangeSmartCardKey(UINT8 readerId, CARD_TYPE_e cardType);
void setEnrollActivityPera(void);
void setEnrollInputPera(ENROLL_CATEGORY_e enrlCategory);
UINT8 ParseResponseFromXml(CHARPTR filename);
BOOL CheckIsRespInXML();
SERVER_API_RESP_e ParseResponseFromText();
BOOL GetUserDetailFromId(CHARPTR alphaNumId);
BOOL IsMobileEnrollmentStarted();
BOOL ChkAndDeriveFaceCapturedFrame(ENROLLMENT_INPUTS_t * inputDataPtr);
void ChkUpdateEnrollType(ENROLLMENT_INPUTS_t * inputDataPtr,ENROLLMENT_SOURCE_e SourceType);
UINT32 GetCurrentEnrollmentState();
UINT8 GetAvailDuressUserFp(UINT32 userID);
BOOL isCmdEnrollDuressThenStoreFpIdx(UINT32 UserId,UINT8 FpIdx);
BOOL isCmdEnrollDuress(void);
void FpIndexForWriteSmartCard(UINT32 userIdx,UINT8 *FpIdx1,UINT8 *FpIdx2);
void DeleteUserDuressFp(UINT32 userID);
BOOL isUserIsVisitor(UINT32 userIdx);
#endif  // MXENROLLMENT_H

