/*
 * automac.c
 *
 *	This code is used from the bootloader
 *  of EternityLite which was originally
 *  written by Elison Niven and adapted by
 *  Darshan Prajapati.
 *
 *  Created on: Jan 7, 2012
 *      Author: Jigar Baria
 */

//###########################################################################
// @INCLUDES
//###########################################################################
#include <common.h>
#include <command.h>
#include <net.h>
#include "automac.h"
#include <automac_environment.h>
#include <environment.h>

//###########################################################################
// @EXTERN
//###########################################################################
unsigned long long	get_timercount(void);

//###########################################################################
// @DEFINES
//###########################################################################
#define		AUTOMAC_SERVER_PORT		65000

/* Automac Client States */
#define		AUTOMAC_CLIENTST_BROADCASTIG		1
#define		AUTOMAC_CLIENTST_BROADCASTIG_WAIT	2
#define		AUTOMAC_CLIENTST_GRANTED_MAC		3

/* Protocol Messages & offset values */
#define		BROADCAST_MESSAGE_TX			"MAC REQUEST#ID:%04X:%s\r\n"
#define		GRANTACCEPT_MESSAGE_RX			"ACCEPT MAC#"
#define		GRANTSUCCESS_MESSAGE_TX			"SUCCESS MAC#ID:%X\r\n"
#define		GRANTACCEPT_MESSAGE_IDOFFSET	strlen(GRANTACCEPT_MESSAGE_RX) + 21
#define		MACADDR_LEN						18
#define		MESSAGE_MAX_LEN					256

/* Transaction timeouts */
#define		TIMEOUT			5		/* Seconds to timeout for a lost pkt */
#define		TIMEOUT_COUNT	10
#define		AUTOMAC_RETRY_COUNT	30

//###########################################################################
// @STATIC_VARS
//###########################################################################
static int	AutoMacServerPort;		/* The UDP port at their end */
static int	AutoMacOurPort;			/* The UDP port at our end */
static int	AutoMacTimeoutCount;
static int	AutoMacState;
static int 	transactionId;

static unsigned int rprime;

static int  macreceived;
static int required_macaddr = 1;

//###########################################################################
// @PROTOTYPE DECLARATION
//###########################################################################
static void AutoMacSend (void);
static void AutoMacTimeout (void);
static int getTransactionId(unchar * packet);
static void setenvironment(void);
static void setrandomnumberseed(unsigned int val);
static unsigned int getrandomnumber(unsigned int max);

//###########################################################################
// @FUNCTIONS
//###########################################################################
//***************************************************************************
// @NAME        : setrandomnumberseed()
// @PARAM       : init value
// @RETURNS     : none
// @DESCRIPTION : Pseudo random generator init
// @NOTE        :
//***************************************************************************
static void setrandomnumberseed(unsigned int val)
{
  rprime = val;
}

//***************************************************************************
// @NAME        : setprng()
// @PARAM       : max value
// @RETURNS     : random number
// @DESCRIPTION : Pseudo random generator
// @NOTE        :
//***************************************************************************
static unsigned int getrandomnumber(unsigned int max)
{
	static unsigned int r;
	static unsigned int Y;

	if (r == 0 || r == 1 || r == -1)
	{
		r = rprime; /* keep from getting stuck */
    }
    
	r = (9973 * ~r) + ((Y) % 701); /* the actual algorithm */
	Y = (r >> 16) % max; /* choose upper bits and reduce */
	return Y;
}

//***************************************************************************
// @NAME        : AutoMacSend()
// @PARAM       :
// @RETURNS     :
// @DESCRIPTION : send udp packets as per current state
// @NOTE        :
//***************************************************************************
static void AutoMacSend(void)
{
	volatile uchar *pkt;
	volatile uchar *xp;
	int			len = 0;
	//int cnt = 0;

	/*
	 *	We will always be sending some sort of packet, so
	 *	cobble together the packet headers now.
	 */
	pkt = net_tx_packet + net_eth_hdr_size() + IP_UDP_HDR_SIZE;

	switch (AutoMacState)
	{
		case AUTOMAC_CLIENTST_BROADCASTIG:
			transactionId = getrandomnumber(65534);
			xp = pkt;

			sprintf((char *)pkt, BROADCAST_MESSAGE_TX, transactionId, PRODUCT_NAME);

			pkt += strlen((char *)pkt) + 1;
			printf("\n### send broadcast %s\n", (char *)xp);
			pkt += strlen((char *)pkt) + 1;
			len = pkt - xp;
			AutoMacState = AUTOMAC_CLIENTST_BROADCASTIG_WAIT;
			break;

		case AUTOMAC_CLIENTST_GRANTED_MAC:
			xp = pkt;
			sprintf((char *)pkt, GRANTSUCCESS_MESSAGE_TX, transactionId);
			pkt += strlen((char *)pkt) + 1;
			printf("\n### send Success %s\n", (char *)xp);
			pkt += strlen((char *)pkt) + 1;
			len = pkt - xp;
			break;
	}
	net_send_udp_packet((uchar*)net_bcast_ethaddr, net_server_ip, AutoMacServerPort, AutoMacOurPort, len);
	return;
}

//***************************************************************************
// @NAME        : AutoMacHandler()
// @PARAM       : recvd packet, dest port, source port
// @RETURNS     : none
// @DESCRIPTION : grep correct response for particular state
// @NOTE        :
//***************************************************************************
static void AutoMacHandler(uchar *pkt, unsigned dest, struct in_addr addr, unsigned src, unsigned len)
{
	int 	receivedId;
	unsigned char 	recvdMac[MACADDR_LEN];

	if (dest != AutoMacOurPort)
	{
		return;
	}

	if ((AutoMacState != AUTOMAC_CLIENTST_BROADCASTIG_WAIT) &&
	(src != AutoMacServerPort))
	{
		return;
	}

	printf("\n### AutoMacHandler: %s\n", pkt);
	if (0 == strncmp((const char*)pkt, GRANTACCEPT_MESSAGE_RX, strlen(GRANTACCEPT_MESSAGE_RX)))
	{
		receivedId = getTransactionId(pkt);
		printf("\n### recvd transaction id = %X\n", receivedId);
		if (receivedId == transactionId)
		{
			strncpy((char*)recvdMac, (const char*)pkt+strlen(GRANTACCEPT_MESSAGE_RX), MACADDR_LEN -1);
			recvdMac[MACADDR_LEN -1] = 0;
			printf("\n### Granted MAc is: %s\n", recvdMac);
			/* AutoMac ok, update environment ether addr, ipaddr, bootcommand */
			macreceived++;

			if(required_macaddr == macreceived)
			{
				update_mac(recvdMac,macreceived);
				env_save();
				AutoMacState = AUTOMAC_CLIENTST_GRANTED_MAC;
				AutoMacSend();
				printf("\n### done ###\n");
				net_set_state(NETLOOP_SUCCESS);
			}
			else
			{
				update_mac(recvdMac,macreceived);
				AutoMacState = AUTOMAC_CLIENTST_GRANTED_MAC;
				AutoMacSend();
				net_set_state(NETLOOP_CONTINUE);
				automac_start();
			}

		}
		else
		{
			printf("### invalid udp recvd\n");
		}
	}
	return;
}

//***************************************************************************
// @NAME        : AutoMacTimeout()
// @PARAM       : none
// @RETURNS     : none
// @DESCRIPTION :
// @NOTE        :
//***************************************************************************
static void AutoMacTimeout(void)
{
	static int	retrycnt = 0;

	if (AUTOMAC_CLIENTST_BROADCASTIG_WAIT == AutoMacState )
	{
		if (++AutoMacTimeoutCount > TIMEOUT_COUNT)
		{
            if(retrycnt > AUTOMAC_RETRY_COUNT)
			{
				printf("\nAutomac terminated after max retry\n");
			}
			else
			{
				retrycnt++;
			    printf("\nRetry count exceeded; starting again\n");
			    net_start_again();
            }
		}
		else
		{
			printf("\n RETRY\n");
			net_set_timeout_handler (TIMEOUT * CONFIG_SYS_HZ, AutoMacTimeout);
			AutoMacState = AUTOMAC_CLIENTST_BROADCASTIG;
			AutoMacSend ();
		}
		return;
	}
}

//***************************************************************************
// @NAME        : automac_start()
// @PARAM       : none
// @RETURNS     : none
// @DESCRIPTION :
// @NOTE        :
//***************************************************************************
void automac_start(void)
{
	printf("AUTO MAC from server %pI4", &net_server_ip);
	printf("; our IP address is %pI4", &net_ip);
	net_set_timeout_handler(TIMEOUT * CONFIG_SYS_HZ, AutoMacTimeout);
	net_set_udp_handler(AutoMacHandler);
	AutoMacServerPort = AUTOMAC_SERVER_PORT;
	AutoMacTimeoutCount = 0;
	AutoMacOurPort = 1024 + (get_timer(0) % 3072);
	AutoMacState = AUTOMAC_CLIENTST_BROADCASTIG;
	AutoMacSend();
	return;
}

//***************************************************************************
// @NAME        : get_timercount()
// @PARAM       : none
// @RETURNS     : timer tick
// @DESCRIPTION :
// @NOTE        :
//***************************************************************************
unsigned long long get_timercount(void)
{
	return get_ticks();
}
//***************************************************************************
// @NAME        : automac_init()
// @PARAM       : none
// @RETURNS     : none
// @DESCRIPTION :
// @NOTE        :
//***************************************************************************
void automac_init(void)
{
	unsigned long long val;

	macreceived = 0;

    if (NVRX_1602 == get_card_type())
	{
		required_macaddr = 2;
	}

	/* give seeds to prng */
	val = get_timercount();
	setrandomnumberseed(val);
	setenvironment();
}

//***************************************************************************
// @NAME        : getTransactionId()
// @PARAM       : none
// @RETURNS     : none
// @DESCRIPTION :
// @NOTE        :
//***************************************************************************
static int getTransactionId(unchar *packet)
{
	u_char cnt;
	int transId = 0;

	for(cnt = GRANTACCEPT_MESSAGE_IDOFFSET; ; cnt++)
	{
		if( (packet[cnt] == 0x0A) || (packet[cnt] == 0x0D) )
			break;

		transId <<= 4;
		if( (packet[cnt] >= '0') && (packet[cnt] <= '9') )
		{
			transId |= packet[cnt] - '0';
		}
		else if( (packet[cnt] >= 'A') && (packet[cnt] <= 'F') )
		{
			transId |= packet[cnt] - 55;
		}
		else /* in valid case if found go home and have rest */
		{
			printf("### failed transactioId grep\n");
		}
	}

	return transId;
}

//***************************************************************************
// @NAME        : setenvironment()
// @PARAM       : request
// @RETURNS     : none
// @DESCRIPTION :
// @NOTE        :
//***************************************************************************
static void setenvironment(void)
{
	int 	i;
	uchar 	tmp[64];
	char 	ethaddr[64];
	char 	ipaddr[16];

	/* Use a random IP address in 10.x.y.z */
	tmp[0] = getrandomnumber(253);
	tmp[1] = getrandomnumber(253);
	do
	{
		tmp[2] = getrandomnumber(253);
	} while (tmp[2] == 0 || tmp[2] == 1 || tmp[2] == 10) ;
	sprintf(ipaddr, "10.%d.%d.%d",tmp[0], tmp[1], tmp[2]);
	env_set("ipaddr", ipaddr);
	env_set("netmask", "255.0.0.0");

	/* Use a random ethaddr */
	tmp[0] = (getrandomnumber(253) | 0x02) & ~0x01;
	for (i = 1; i < 6; i++)
	{
		tmp[i] = getrandomnumber(253);
	}
// 	sprintf(ethaddr,"%x:%x:%x:%x:%x:%x",tmp[0],tmp[1],tmp[2],tmp[3],tmp[4],tmp[5]);
	sprintf(ethaddr,"00:1b:09:%x:%x:%x",tmp[3],tmp[4],tmp[5]);
	env_set("ethaddr", ethaddr);

	/* WE will broadcast to find the server. */
	env_set("serverip", "255.255.255.255");
}
