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
//   File         : DeviceDetect.c
//   Description  : This file provides APIs to communicate between udev event
//					and application
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/errno.h>

#include "MxTypedef.h"

//******** Extern Variables **********


//******** Defines and Data Types ****
#define INTERNAL_SOCKET_FILE	"/tmp/IntSocket"
#define	INT_FSP					0x1
#define	INT_SOM					0x2
#define	INT_EOM					0x3
//******** Function Prototypes *******



//******** Global Variables **********



//******** Static Variables **********



//******** Function Definations ******

int main (int argc, char **argv)
{
	UINT8 				cnt;
	INT32 				clientFd;
	CHAR 				txRxBuffer[1024];
	socklen_t 			clientLen;
	struct sockaddr_un 	clientParam;

	clientLen = sizeof(clientParam);

	do
	{	//socket created
		clientFd = socket(AF_UNIX, SOCK_STREAM, 0);

		if(clientFd <= 0)
		{
			return 0;
		}

		//socket parameters loaded
		bzero(&clientParam, sizeof clientParam);
		clientParam.sun_family = AF_UNIX;
		strcpy(clientParam.sun_path, INTERNAL_SOCKET_FILE);

		//connecting to server
		while(connect(clientFd, (struct sockaddr*)&clientParam, clientLen) < 0)
		{
		  usleep(100000);
		}
		//construct message
		sprintf(txRxBuffer, "%c", INT_SOM);

		for(cnt = 1; cnt < argc; cnt++)
		{
			sprintf(txRxBuffer + strlen(txRxBuffer), "%s%c", argv[cnt], INT_FSP);
		}

		sprintf(txRxBuffer + strlen(txRxBuffer), "%c", INT_EOM);

		//send request to network manager
		if(send(clientFd, txRxBuffer, strlen(txRxBuffer), 0) < (INT32)strlen(txRxBuffer))
		{
			break;
		}
	}
	while(0);

	//closing connection
	close(clientFd);

	return 0;
}
