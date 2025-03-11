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
//   File         : MxNwUtils.c
//   Description  : This file declares system utility API.
//
/////////////////////////////////////////////////////////////////////////////

// macro; to make use of recursive mutex
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <pthread.h>
#include <dirent.h>

#include "MxTypedef.h"	
#include "MxSysCommonDef.h"
#include "MxLogs.h"
#include "MxSysTimer.h"
#include "MxNwUtils.h"
#include "MxConfig.h"
#include "MxConfigFileIo.h"
#include "MxAvResp.h"
#include "MxMobileBroadband.h"
#include "MxWifiPort.h"
#include "MxAcmsSocket.h"

//******** Extern Variables **********
extern ETHERNET_PARA_t		EthernetParameters;
extern WIFI_PARA_t			WifiParameters;
extern MODEM_NW_PARA_t		BroadBandNwPara;
extern COMM_PORT_e			acmsCurActiveInterface;

//******** Defines and Data Types ****
#define	DNS_RESOLV_CONF_FILE			"/etc/resolv.conf"

typedef struct
{
	CHARPTR		dnsAddr;
	CHARPTR		gwAddr;
}DNS_GATEWAY_ADDR_t;

//******** Function Prototypes *******
static VOIDPTR SetInterfaceParaThreadFn(VOIDPTR dummy);
static void SetSysDNS(void);

//******** Global Variables **********
CHARPTR InterfaceStr[MAX_PORT] =
{
	ETH_INTERFACE,
	WIFI_INTERFACE,
	BroadBandNwPara.modemInterfaceStr,
};
pthread_mutex_t			IpResolveMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

//******** Static Variables **********
static pthread_t		interfaceThreadData = 0;

//******** Function Definations ******

//*****************************************************************************
//	ValidateNetworkAddr()
//	Param:
//		IN:	 Network address string
//			 CHECK_NW_ADDR_e
//		OUT: ipAddrPtr
//	Returns: TRUE/FALSE
//	Description:
//			It validate netwrok address string.
//
//*****************************************************************************
BOOL ValidateNetworkAddr(CHARPTR nwString, CHARPTR ipAddrPtr, CHECK_NW_ADDR_e addrType)
{
	UINT8	length;
	UINT32	ip1, ip2, ip3, ip4;
	BOOL	allowBlankF = FALSE;

	length = strlen(nwString);

	switch(addrType)
	{
		default:
		case CHECK_IP_ADDR:
		case CHECK_SERVER_IP:
			allowBlankF = FALSE;
			break;

		case CHECK_GATEWAY_ADDR:
		case CHECK_DNS_ADDR:
			allowBlankF = TRUE;
			break;
	}

	if( (length != 0) && (length < MAX_IP_ADDRESS) )
	{
		ip1 = 0;
		ip2 = 0;
		ip3 = 0;
		ip4 = 0;

		if(sscanf(nwString, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4) != 4)
		{
			return FALSE;
		}

		// All bytes must not be out of 0-255
		if( (ip1 > 255) || (ip2 > 255) || (ip3 > 255) || (ip4 > 255) )
		{
			return FALSE;
		}

		// MSB : 1 to 223
		// 127 is used for Local address
		if( (ip1 < 1) || (ip1 > 223) || (ip1 == 127) )
		{
			return FALSE;
		}

		// LSB : Invalid values as they are used for broadcast(255) and network id(0)
		// Only check for IP address.
		// Valid for Gateways, DNS
		if(allowBlankF == FALSE)
		{
			if( (ip4 == 0) || (ip4 == 255) )
			{
				return FALSE;
			}
		}

		if(ipAddrPtr)
		{
			sprintf(ipAddrPtr, "%d.%d.%d.%d", (UINT8)ip1, (UINT8)ip2, (UINT8)ip3, (UINT8)ip4);
		}
		return TRUE;
	}
	else if( (length == 0) && (allowBlankF == TRUE) )
	{
		if(ipAddrPtr)
		{
			memset(ipAddrPtr, 0, MAX_IP_ADDRESS);
		}
		return TRUE;
	}
	return FALSE;
}

//*****************************************************************************
//	ValidateServerPort()
//	Param:
//		IN:	 Server port address string
//		OUT: server port
//	Returns: TRUE/FALSE
//	Description:
//			It validate server port string.
//
//*****************************************************************************
BOOL ValidateServerPort(CHARPTR portString, UINT16PTR serverPortPtr)
{
	UINT32 	serverPort;
	UINT8 	length;

	length = strlen(portString);

	if( (length > 0) && (length <= MAX_SERVER_PORT_DIGITS) )
	{
		serverPort = atol(portString);
		if( (serverPort >= MIN_SERVER_PORT_VAL) && (serverPort <= MAX_SERVER_PORT_VAL) )
		{
			*serverPortPtr = (UINT16)serverPort;
			return TRUE;
		}
	}
	return FALSE;
}

//*****************************************************************************
//	ValidateSubnetAddr()
//	Param:
//		IN:	 Subnet address string
//		OUT: ipAddr
//	Returns: TRUE/FALSE
//	Description:
//			It validate subnet address string.
//
//*****************************************************************************
BOOL ValidateSubnetAddr(CHARPTR nwString, CHARPTR sbAddrPtr)
{
	UINT32			ip1, ip2, ip3, ip4, addrNB;
	DOUBLE_WORD_u 	addrDBW;
	UINT8			idx, length;
	BOOL			bitFlip = FALSE, lastBit, currBit;

	length = strlen(nwString);

	if( (length != 0) && (length < MAX_IP_ADDRESS) )
	{
		ip1 = 0;
		ip2 = 0;
		ip3 = 0;
		ip4 = 0;

		if(sscanf(nwString, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4) != 4)
		{
			return FALSE;
		}

		// All bytes must not be out of 0-255
		// LSB must be 0-252
		if( (ip1 > 255) || (ip2 > 255) || (ip3 > 255) || (ip4 > 252) )
		{
			return FALSE;
		}

		addrDBW.byte[3] = ip1;
		addrDBW.byte[2] = ip2;
		addrDBW.byte[1] = ip3;
		addrDBW.byte[0] = ip4;

		addrNB = (UINT32)addrDBW.dWord;

		// Most significant bit must be 1.
		// Least significant bit must be 0.
		lastBit = (addrNB & 1);
		if( (lastBit != 0) || ( (addrNB >> 31) != 1 ) )
		{
			return FALSE;
		}

		//Only one time bit flip is accepted
		for(idx = 1; idx < 32; idx++)
		{
			currBit = ( (addrNB >> idx) & 1);

			if(currBit != lastBit)
			{
				if(bitFlip == TRUE)
				{
					return FALSE;
				}
				bitFlip = TRUE;
			}
			lastBit = currBit;
		}

		sprintf(sbAddrPtr, "%d.%d.%d.%d", (UINT8)ip1, (UINT8)ip2, (UINT8)ip3, (UINT8)ip4);
		return TRUE;
	}
	return FALSE;
}

//***************************************************************************
//	SetInterfacePara()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function set default gateway and dns address according to
//			interface selected for server communication.
//***************************************************************************
void SetInterfacePara(void)
{
	if(pthread_create(&interfaceThreadData, NULL, SetInterfaceParaThreadFn, NULL) == 0)
	{
		// Thread created successfully now detach thread ??
		pthread_detach(interfaceThreadData);
	}
}

//***************************************************************************
//	SetInterfaceParaThreadFn()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function set default gateway and dns address according to
//			interface selected for server communication.
//***************************************************************************
static VOIDPTR SetInterfaceParaThreadFn(UNUSED_PARA VOIDPTR dummy)
{
	CHAR					buf[MAX_TMP_BUFF_SIZE];
	struct in_addr			tempInAddr;
	COMM_PORT_e				tmpComPort;

	// lock the node, in order to prevent multiple simultaneous access
	pthread_mutex_lock(&IpResolveMutex);

	for(tmpComPort = 0; tmpComPort < MAX_PORT; tmpComPort++)
	{
		sprintf(buf,"route del default gw 0.0.0.0 %s", InterfaceStr[tmpComPort]);
		system(buf);
	}

	if(inet_aton(EthernetParameters.gateway, &tempInAddr) != 0)
	{
		sprintf(buf,"route add default gw %s %s", EthernetParameters.gateway, ETH_INTERFACE);
		system(buf);
	}

	if(inet_aton(WifiParameters.gateway, &tempInAddr) != 0)
	{
		sprintf(buf,"route add default gw %s %s", WifiParameters.gateway, WIFI_INTERFACE);
		system(buf);
	}

	if(inet_aton(BroadBandNwPara.gateway[MODEM_NW_ADDR_TYP_CONNECT], &tempInAddr) != 0)
	{
		sprintf(buf,"route add default gw %s %s", BroadBandNwPara.gateway[MODEM_NW_ADDR_TYP_CONNECT], BroadBandNwPara.modemInterfaceStr);
		system(buf);
	}
	SetSysDNS();
	// unlock the resource
	pthread_mutex_unlock(&IpResolveMutex);
	pthread_exit(NULL);
}

//******************************************************************************
//	SetSysDNS()
//	Param:
//		IN :	none
//		OUT:    NONE
//	Returns:	True/False
//	Description:
//				Sets dns ip and domain name in resolve.conf file.
//******************************************************************************
static void SetSysDNS(void)
{
	/* File Pointer For resolv.conf File */
	FILE					*fp = NULL;
	DNS_GATEWAY_ADDR_t		dnsGwAddr[(MAX_PORT)*2];
	UINT8					tmpIdx, dnsCount = 0;
	static char				resolvDnsAddr[MAX_PORT][MAX_DNS_ADDRESS];
	CHAR					buf[MAX_TMP_BUFF_SIZE];
	struct in_addr			tempInAddr;

	dnsGwAddr[0].dnsAddr = EthernetParameters.dns1;
	dnsGwAddr[0].gwAddr = EthernetParameters.gateway;
	dnsGwAddr[1].dnsAddr = WifiParameters.dns1;
	dnsGwAddr[1].gwAddr = WifiParameters.gateway;
	dnsGwAddr[2].dnsAddr = BroadBandNwPara.dns1[MODEM_NW_ADDR_TYP_CONNECT];
	dnsGwAddr[2].gwAddr = BroadBandNwPara.gateway[MODEM_NW_ADDR_TYP_CONNECT];
	dnsGwAddr[3].dnsAddr = EthernetParameters.dns2;
	dnsGwAddr[3].gwAddr = EthernetParameters.gateway;
	dnsGwAddr[4].dnsAddr = WifiParameters.dns2;
	dnsGwAddr[4].gwAddr = WifiParameters.gateway;
	dnsGwAddr[5].dnsAddr = BroadBandNwPara.dns2[MODEM_NW_ADDR_TYP_CONNECT];
	dnsGwAddr[5].gwAddr = BroadBandNwPara.gateway[MODEM_NW_ADDR_TYP_CONNECT];

	/* Write DNS Information into resolv.conf File */
	fp = fopen(DNS_RESOLV_CONF_FILE, "w+");

	/* Failed To Open File */
	if (NULL == fp)
	{
		EPRINT(CONFIG_LOG,"Failed to open resolve.conf file");
		return;
	}

	for(tmpIdx = 0; tmpIdx < ((MAX_PORT)*2); tmpIdx++)
	{
		if (strlen(dnsGwAddr[tmpIdx].dnsAddr) > 0)
		{
			fprintf(fp, "nameserver %s\n", dnsGwAddr[tmpIdx].dnsAddr);
			sprintf(buf, "route del -net %s netmask 255.255.255.255 gw 0.0.0.0", resolvDnsAddr[dnsCount]);
			system(buf);
			sprintf(resolvDnsAddr[dnsCount], "%s", dnsGwAddr[tmpIdx].dnsAddr);
			if(inet_aton(dnsGwAddr[tmpIdx].gwAddr, &tempInAddr) != 0)
			{
				sprintf(buf, "route add -net %s netmask 255.255.255.255 gw %s", resolvDnsAddr[dnsCount],
																				dnsGwAddr[tmpIdx].gwAddr);
				system(buf);
			}
			dnsCount++;
			//in resolve.conf we can add maximum 3 name server
			if(dnsCount >= 3)
				break;
		}
	}
	sync();
	fclose(fp);

	if(-1 == res_init()) // This function is deprecated new API res_ninit()
		EPRINT(CONFIG_LOG,"Failed to apply resolve.conf");
}

//*****************************************************************************
//	GetServerInterfaceIpAddr()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function will start Ethernet link Scan cyclic timer.
//
//*****************************************************************************
void GetServerInterfaceIpAddr(CHARPTR intfaceAddr)
{
	strcpy(intfaceAddr, EthernetParameters.ipAddress);

	switch(acmsCurActiveInterface)
	{
	default:
	case ETHERNET_PORT:
		break;

	case WIFI_PORT:
		if(strlen(WifiParameters.ipAddress) > 0)
		{
			strcpy(intfaceAddr, WifiParameters.ipAddress);
		}
		break;

	case PPP_PORT:
		if(strlen(BroadBandNwPara.ipAddress[MODEM_NW_ADDR_TYP_DISP] ) > 0)
		{
			strcpy(intfaceAddr, BroadBandNwPara.ipAddress[MODEM_NW_ADDR_TYP_DISP] );
		}
		break;
	}
}
//*****************************************************************************
//	ValidateUrl()
//	Param:
//		IN:	 Url string
//		OUT: 
//	Returns: TRUE/FALSE
//	Description:
//			It validate Enter URL string.
//
//*****************************************************************************
BOOL ValidateUrl(CHARPTR urlString, UINT8 len)
{
	UINT8 i;
	for(i = 0; i < len; i++)
	{
		if (!isalpha(urlString[i]) && !isdigit(urlString[i]) && urlString[i] != '.' && urlString[i] != '-' &&
			urlString[i] != '_' && urlString[i] != '+' && urlString[i] != '=' && urlString[i] != '&' &&
			urlString[i] != '%' && urlString[i] != '#' && urlString[i] != ':' && urlString[i] != ';' &&
			urlString[i] != '?' && urlString[i] != '/')
		{
			return FALSE;
		}
	}
	return TRUE;
}
//*****************************************************************************
//	ValidateEnterUrl()
//	Param:
//		IN:	 Url string
//		OUT: 
//	Returns: TRUE/FALSE
//	Description:
//			It validate Enter URL string.
//
//*****************************************************************************
BOOL ValidateEnterUrl(CHARPTR urlString, UINT8 len)
{
	UINT8 i;
	for(i = 0; i < len; i++)
	{
		if (!isalpha(urlString[i]) && !isdigit(urlString[i]) && urlString[i] != '.' && urlString[i] != '-')
		{
			return FALSE;
		}
	}
	return TRUE;
}