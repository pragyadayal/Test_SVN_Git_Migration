/*
 * automac_environment.h
 *
 *  Created on: Jun 9, 2012
 *      Author: Bharat Gohil
 */

/*This document is edited for Matrix IPCAMERA DM388 SoC based
 * Editor :Nitish Mehta
 */
#ifndef AUTOMAC_ENVIRONMENT_H_
#define AUTOMAC_ENVIRONMENT_H_

//###########################################################################
// @DEFINES
//###########################################################################
#define PRODUCT_NAME "ROCKCHIP_RK3568_NVR"

/* Product specific environment variables */
#define		IP_ADDR				"192.168.1.50"
#define		NET_MASK			"255.255.255.0"
#define		GATEWAY_IP			"192.168.1.1"
#define		SERVER_IP			"192.168.1.44"

#define NVRX_0801	0
#define NVRX_1601	1
#define NVRX_1602	2
#define NVRXS_0801	3
#define NVRXS_1601	4

//###########################################################################
// @PROTOTYPE DECLARATION
//###########################################################################
/* common/nvr_environment.c */
//void update_ipnc_environment(uchar *mac);
void update_mac(uchar *mac,uchar ethnum);

int get_card_type(void);
int get_board_version(void);
 

#endif /* AUTOMAC_ENVIRONMENT_H_ */
