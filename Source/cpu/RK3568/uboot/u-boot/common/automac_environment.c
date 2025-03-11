/*
 * nvr_environment.c
 *
 *  Created on: Jun 9, 2012
 *      Author: Bharat Gohil
 */

//###########################################################################
// @INCLUDES
//###########################################################################
#include <common.h>
#include <automac_environment.h>
#include <asm-generic/gpio.h>


#define CARD_TYPE_0_GPIO_NO	96
#define CARD_TYPE_1_GPIO_NO	97
#define CARD_TYPE_2_GPIO_NO	98
#define CARD_TYPE_3_GPIO_NO	99

#define BOARD_VERSION_0_GPIO_NO	100
#define BOARD_VERSION_1_GPIO_NO	101
#define BOARD_VERSION_2_GPIO_NO	102
#define BOARD_VERSION_3_GPIO_NO	103
//###########################################################################
// @FUNCTIONS
//###########################################################################
//***************************************************************************
// @NAME        : update_ipnc_environment()
// @PARAM       : received mac
// @RETURNS     : none
// @DESCRIPTION :
// @NOTE        :
//***************************************************************************
// void update_ipnc_environment(uchar *mac,uchar ethnum)
// {	
// 	if(ethnum == 1)
// 	{
// 		setenv("ethaddr", mac);
// 		setenv("ipaddr", IP_ADDR);
// 		setenv("netmask", NET_MASK);
// 		setenv("gatewayip", GATEWAY_IP);
// 		setenv("serverip", SERVER_IP);
// 	}
// 	else{
// 		setenv("eth1addr", mac);
// 		setenv("ipaddr", IP_ADDR);
// 		setenv("netmask", NET_MASK);
// 		setenv("gatewayip", GATEWAY_IP);
// 		setenv("serverip", SERVER_IP);
// 	}
// }

//***************************************************************************
// @NAME        : update_mac()
// @PARAM       : received mac
// @RETURNS     : none
// @DESCRIPTION :
// @NOTE        :
//***************************************************************************
void update_mac(uchar *mac, uchar ethnum)
{
	if(ethnum == 1)
	{
		env_set("ethaddr", (char *)mac);
	}
	else{
		env_set("eth1addr", (char *)mac);
	}
	return;
}

int get_card_type(void)
{
	unsigned int card_type = 0;
	
	gpio_request(CARD_TYPE_0_GPIO_NO, "card_type_0");
	gpio_request(CARD_TYPE_1_GPIO_NO, "card_type_1");
	gpio_request(CARD_TYPE_2_GPIO_NO, "card_type_2");
	gpio_request(CARD_TYPE_3_GPIO_NO, "card_type_3");
	
	gpio_direction_input(CARD_TYPE_0_GPIO_NO);
	gpio_direction_input(CARD_TYPE_1_GPIO_NO);
	gpio_direction_input(CARD_TYPE_2_GPIO_NO);
	gpio_direction_input(CARD_TYPE_3_GPIO_NO);
	
	card_type = gpio_get_value(CARD_TYPE_0_GPIO_NO) | (gpio_get_value(CARD_TYPE_1_GPIO_NO) << 1) | \
	                      (gpio_get_value(CARD_TYPE_2_GPIO_NO) << 2) | (gpio_get_value(CARD_TYPE_3_GPIO_NO) << 3);
 	
 	gpio_free(CARD_TYPE_0_GPIO_NO);
	gpio_free(CARD_TYPE_1_GPIO_NO);
	gpio_free(CARD_TYPE_2_GPIO_NO);
	gpio_free(CARD_TYPE_3_GPIO_NO);
	
	
	printf("\ncard type: %d\n", card_type);
 	
 	return card_type;
}

int get_board_version(void)
{
	unsigned int board_version = 0;
	
	gpio_request(BOARD_VERSION_0_GPIO_NO, "board_version_0");
	gpio_request(BOARD_VERSION_1_GPIO_NO, "board_version_1");
	gpio_request(BOARD_VERSION_2_GPIO_NO, "board_version_2");
	gpio_request(BOARD_VERSION_3_GPIO_NO, "board_version_3");
	
	gpio_direction_input(BOARD_VERSION_0_GPIO_NO);
	gpio_direction_input(BOARD_VERSION_1_GPIO_NO);
	gpio_direction_input(BOARD_VERSION_2_GPIO_NO);
	gpio_direction_input(BOARD_VERSION_3_GPIO_NO);
	
	board_version = gpio_get_value(BOARD_VERSION_0_GPIO_NO) | (gpio_get_value(BOARD_VERSION_1_GPIO_NO) << 1) | \
	                      (gpio_get_value(BOARD_VERSION_2_GPIO_NO) << 2) | (gpio_get_value(BOARD_VERSION_3_GPIO_NO) << 3);
	                      
	gpio_free(BOARD_VERSION_0_GPIO_NO);
	gpio_free(BOARD_VERSION_1_GPIO_NO);
	gpio_free(BOARD_VERSION_2_GPIO_NO);
	gpio_free(BOARD_VERSION_3_GPIO_NO);
	
	printf("\nboard version: %d\n", board_version);
 	
 	return board_version;
}
