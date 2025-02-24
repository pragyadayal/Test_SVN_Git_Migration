#if !defined MXLICENSEDONGLE_H
#define MXLICENSEDONGLE_H
///////////////////////////////////////////////////////////////////////////////
//
//   MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//   MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//   MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
//   MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
//   MM       MM   AA       AA       TT      RR  RR       II      XX XX
//   MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//   Company      : Matrix Comsec Pvt. Ltd., Baroda, India.
//   Project      : Access Control System
//   Created By   : Manish chandra
//   File         : MxLicenseDongle.h
//   Description  :	License related data types etc.
//
///////////////////////////////////////////////////////////////////////////////

//******** Place all include files ***
#include "MxTypedef.h"
#include "MxAcmsTask.h"

//******** Defines and Data Types ****
#define DEFAULT_LICENSE_POLL_DURATION	30
#define DEFAULT_LICENSE_POLL_INTERVAL	15

#define SERIAL_NO_STARTING_ADDRS		0
#define DONGLE_KEY_STARTING_ADDRS		300
#define MAC_STARTING_ADDS				600
#define AUP_STARTING_ADDS				900		//Annual Subcription package detail

#define DONGLE_SERIAL_NO_SIZE			8
#define LICENSE_KEY_SIZE				159
#define MAC_SIZE						20
#define	AUP_SIZE						8


typedef enum
{
	LICENSE_DONGLE_URL_AND_PORT_CHANGE,
	LICENSE_DONGLE_MAC_CHANGE,
	MAX_LICENSE_DONGLE_CHANGE_PARA,
}CHANGE_PARA_e;


/*#ifndef _SecureDongle_32_*/
#define _SecureDongle_32_


#define  SD_FIND                        1		//Find Dongle
#define  SD_FIND_NEXT					2		//Find Next Dongle
#define  SD_OPEN                        3		//Open Dongle
#define  SD_CLOSE                       4		//Close Dongle
#define  SD_READ                        5		//Read Dongle
#define  SD_WRITE                       6		//Write Dongle
#define  SD_RANDOM                      7		//Generate Random Number
#define  SD_SEED                        8		//Generate Seed Code
#define  SD_WRITE_USERID				9		//Write User ID
#define  SD_READ_USERID					10		//Read User ID
#define  SD_SET_MODULE					11		//Set Module
#define  SD_CHECK_MODULE				12		//Check Module
#define  SD_WRITE_ARITHMETIC            13		//Write Algorithm
#define  SD_CALCULATE1					14		//Calculate 1
#define  SD_CALCULATE2					15		//Calculate 2
#define  SD_CALCULATE3					16		//Calculate 3
#define  SD_CALCULATE4					18		//Calculate 4
#define  SD_CALCULATE5					19		//Calculate 5
#define  SD_DECREASE					17		//Decrease Module Unit

#define  SD_SETPASSWORDID            	0xf0
#define  SD_AGENTBURN	             	0xf3
#define  SD_GETVERSION					0xf7

#define  SD_SET_COUNTER					20		//Set Counter
#define  SD_GET_COUNTER					21		//Get Counter
#define  SD_DEC_COUNTER					22
#define  SD_SET_TIMER					23		//Set Timer
#define  SD_GET_TIMER					24		//Get Timer
#define  SD_ADJUST_TIMER				25              //Adjust System Clock
#define  SD_SET_TIMER_ITV				26
#define  SD_GET_TIMER_ITV				27
#define  SD_DEC_TIMER					28
#define  SD_SET_RSAKEY_N				29		//Write N into RSA keypair
#define  SD_SET_RSAKEY_D				30		//Write D into RSA keypair
#define  SD_UPDATE_GEN_HEADER			31		//Generate header of cipher-text file
#define  SD_UPDATE_GEN					32		//Generate cipher-text file
#define  SD_UPDATE_CHECK				33		//Check and Upgrade cipher-text file
#define  SD_UPDATE				        34		//Upgrade cipher-text file
#define  SD_UNPACK				        35


#define  SD_FREEEPROM					89

#define  SD_SET_DES_KEY				    41		//Set DES Key
#define  SD_DES_ENC						42		//DES Encryption
#define  SD_DES_DEC						43		//DES Decryption
#define  SD_RSA_ENC						44		//RSA Encryption
#define  SD_RSA_DEC						45		//RSA Decryption

#define SD_READ_EX                 		46
#define SD_WRITE_EX						47

#define SD_SET_COUNTER_EX				0xA0	//Set value to Count Unit (DWORD)
#define SD_GET_COUNTER_EX				0xA1	//Get value from Count Unit (DWORD)
#define SD_SET_TIMER_EX					0xA2	//Set value to Time Unit (DWORD)
#define SD_GET_TIMER_EX					0xA3	//Get value from Time Unit (DWORD)
#define SD_ADJUST_TIMER_EX				0xA4	//Adjust Timer in Dongle

#define SD_UPDATE_GEN_EX				0xA6	//Generate Update File

#define SD_UPDATE_EX					0xA8	//Update Dongle
#define SD_SET_UPDATE_KEY				0xA9	//Set RSA Keys for Generate Update File
#define SD_ADD_UPDATE_HEADER		    0xAA	//Fill in the content of Update File Head
#define SD_ADD_UPDATE_CONTENT		    0xAB	//Fill in the content of Update File Body
#define SD_GET_TIME_DWORD				0xAC	//Translate Time to DWORD value (the minutes from 2006.1.1)

#define  SD_VERSION				        100     //Get COS Version

//add by bgc on 09/2006
#define DES_SINGLE_MODE					0
#define DES_TRIPLE_MODE					1

#define RSA_PRIVATE_KEY					0
#define RSA_PUBLIC_KEY					1

#define RSA_DONGLE_PADDING				0
#define RSA_USER_PADDING				1
//end

// Error Codes
#define ERR_SUCCESS						0 //Success
#define ERR_NO_DONGLE					3 // No driver installed
#define ERR_INVALID_PASSWORD			4 //Found Dongle, but basic passwords (pass1, pass2) are wrong
#define ERR_INVALID_PASSWORD_OR_ID		5 //Wrong password or Hardware ID
#define ERR_SETID				        6 //Error in Hardware ID setting
#define ERR_INVALID_ADDR_OR_SIZE		7 //Error in read/write address or length
#define ERR_UNKNOWN_COMMAND				8 //No such command
#define ERR_NOTBELEVEL3					9 //Internal Error
#define ERR_READ						10 //Read Error
#define ERR_WRITE						11 //Write Error
#define ERR_RANDOM						12 //Random Number Error
#define ERR_SEED						13 //Seed Code Error
#define ERR_CALCULATE					14 //Error in Calculation
#define ERR_NO_OPEN						15 //Unopen dongle before operation
#define ERR_OPEN_OVERFLOW				16 //Too many open dongles (>16)
#define ERR_NOMORE						17 //No more dongles
#define ERR_NEED_FIND					18 //No "Find" function called before using "FindNext"
#define ERR_DECREASE					19 //Decrease Error
#define ERR_AR_BADCOMMAND				20 //Error in Arithmetic Instruction
#define ERR_AR_UNKNOWN_OPCODE			21 //Error in Arithmetic Operator
#define ERR_AR_WRONGBEGIN				22 //Constant cannot be used in the first arithmetic instruction
#define ERR_AR_WRONG_END				23 //Constant cannot be used in the last arithmetic instruction
#define ERR_AR_VALUEOVERFLOW			24 //The value of Constant cannot be greater than 63
#define ERR_TOOMUCHTHREAD				25 //The threads opened in the same process cannot be greater than 100

#define ERR_INVALID_SD					30 //Try to handle non-Securedongle
#define ERR_INVALID_PARAMETER           31 //Invalid parameter
#define ERR_INVALID_TIMEVALUE           32 //Invalid Date-Time value

//Set DES Key Error
#define ERR_SET_DES_KEY					40

//DES Encryption Error
#define ERR_DES_ENCRYPT					41

//DES Decryption Error
#define ERR_DES_DECRYPT					42

//Error in writing N into RSA keypair
#define ERR_SET_RSAKEY_N				43

//Error in writing D into RSA keypair
#define ERR_SET_RSAKEY_D				44

//RSA Encryption Error
#define ERR_RSA_ENCRYPT					45

//RSA Decryption Error
#define ERR_RSA_DECRYPT					46

//Invalid data length
#define ERR_INVALID_LENGTH				47

#define ERR_RECEIVE_NULL				0x100 //Receive nothing
#define ERR_UNKNOWN_SYSTEM				0x102 //Unknown Operating System
#define ERROR_UNINIT_TIME_UNIT			0x103 //Time uint uninitialized
#define ERR_UNKNOWN						0xffff //Unknown Error

#define ERR_XUTEST						0xAA


//******** Function Prototypes *******

short SecureDongle(unsigned short function, unsigned short * handle, unsigned int * lp1,  unsigned int * lp2, unsigned short * p1, unsigned short * p2, unsigned short * p3, unsigned short * p4,unsigned char* buffer);

void InitLicenseDongleState(void);
void RunLicenseDongleTask(void);
UINT8 WriteLicenseDongle(INT32 addrs, UINT8 length, unsigned char* buffer);
UINT8 ReadLicenseDongle(INT32 addrs, UINT8 length, unsigned char* buffer);
ACMS_COMM_STATUS_e GetLicenseConnctionServerStatus(void);
UINT8 ReadLockCountAndMacAddress(UINT8PTR buffer, CHANGE_PARA_e changePara);
void IncreaseLockCount(void);
VOID CreateIdentityCode(UINT8PTR finalEncrpData);
BOOL VerifyIdentityCode(UINT8PTR encData);
VOID ReadMacIpInfoOfLockCountFromDongle(VOID);
VOID WriteMacIpInfoOfLockCountInDongle(VOID);
void LicenseDongleIpResolver(void);
UINT8 GetDongleHardwareId(UINT32PTR);

#endif  // MXLICENSEDONGLE_H
