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
//   Project      : ACS (Access Control System)
//   Created By   : Yashavadan Dalsaniya
//   File         : MxMain.c
//   Description  : This file check generated file by application to identify
//				application is of proper version if not found than reboot system.
///////////////////////////////////////////////////////////////////////////// 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//******** Extern Variables **********

//******** Defines and Data Types ****
//this file indicate newer version application is running
#define SYS_APPL_SIGN_FILE		"/tmp/applPresent.txt"
//this file indicate system is demo or normal
#define DEMO_FW_STATUS_FILE		"/usr/lib/DemoControl.txt"
//this file indicate system is demo or normal
#define GET_FW_TYPE_FILE		"/tmp/demoConfig.txt"


//For Samay door
#define GET_APP_TYPE_FILE			"/tmp/samayConfig.txt"
#define SAMAY_APP_TYPE_FILE			"/usr/lib/SamayControl.txt"
#define SYS_SAMAY_APPL_SIGN_FILE	"/tmp/samayApplPresent.txt"

//******** Function Prototypes *******

//******** Global Variables **********

//******** Static Variables **********

//***************************************************************************
//	main()
//	Param: 
//		IN :	NONE
//		OUT:	NONE
//	Returns:	application exit status (always 1)
//	Description:
//				This is ACS master main function.
//				First all modules are initialised and then forever loop
//				is started processing each task in a loop.
//				
//***************************************************************************
int main(void)
{
	FILE			*fp;
	char			buf[100];
	struct stat		statBuf;
	int				ret;
	
	sprintf(buf, "fw_printenv fwType > %s", GET_FW_TYPE_FILE);
	system(buf);
	
	fp = fopen(GET_FW_TYPE_FILE, "r");
	if(fp != NULL)
	{
		memset(buf, 0, sizeof(buf) );
		fread(buf, sizeof(buf), 1, fp);
		fclose(fp);
		if(strncmp(buf,"fwType=demo",11) == 0)
		{
			sprintf(buf, "touch %s", DEMO_FW_STATUS_FILE);
			system(buf);
		}
	}
	unlink(GET_FW_TYPE_FILE);


	sprintf(buf, "fw_printenv appType > %s", GET_APP_TYPE_FILE);
	system(buf);
	
	fp = fopen(GET_APP_TYPE_FILE, "r");
	if(fp != NULL)
	{
		memset(buf, 0, sizeof(buf) );
		fread(buf, sizeof(buf), 1, fp);
		fclose(fp);
		if(strncmp(buf,"appType=samay",11) == 0)
		{
			sprintf(buf, "touch %s", SAMAY_APP_TYPE_FILE);
			system(buf);
		}
	}
	unlink(GET_APP_TYPE_FILE);


	
	//sleep for 200 second to allow main application change through ftp if by 
	//mistake wrong application is upgraded by user.
	sleep(200);
	
	ret = stat(DEMO_FW_STATUS_FILE, &statBuf);
	if(ret == 0)
	{
		ret = stat(SYS_APPL_SIGN_FILE, &statBuf);
		if(ret < 0)
		{
			system("reboot");
		}
		unlink(SYS_APPL_SIGN_FILE);
	}

	ret = stat(SAMAY_APP_TYPE_FILE, &statBuf);
	if(ret == 0)
	{
		ret = stat(SYS_SAMAY_APPL_SIGN_FILE, &statBuf);
		if(ret < 0)
		{
			system("reboot");
		}
		unlink(SYS_SAMAY_APPL_SIGN_FILE);
	}

	return 0;
}
