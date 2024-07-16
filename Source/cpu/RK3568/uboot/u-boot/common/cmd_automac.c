/*
 * cmd_automac.c
 *
 *  This code is used from the bootloader
 *  of EternityLite which was originally
 *  written by Elison Niven and adapted by
 *  Darshan Prajapati.
 *
 *	Created on: Jan 7, 2012
 *		Author: Jigar Baria
 *	Created on: Jun 7, 2012
 *		Author: Bharat Gohil
 */

/*
 * AutoMAC Support
 */
#include <common.h>
#include <command.h>
#include <net.h>
#include <automac_environment.h>

/*******************************************************************/
/* automac - get MAC address from server */
/*******************************************************************/
int	do_automac(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int	status;
	char 	*s = NULL,*s1 = NULL;

	/* Check if MAC address has already been set */
	s = env_get("ethaddr");
	if (NVRX_1602 == get_card_type())
	{
		s1 = env_get("eth1addr");
	}
	
	if (NVRX_1602 == get_card_type())
	{
		if(s != NULL && s1 != NULL)
	    {
			/* no need to do automac process */
		     printf("\nethaddr : %s\n", s);
		     printf("eth1addr : %s\n", s1);
        	 printf("exiting from automac without any process\n");
		     return 0;
		} 
	}
	else
	{
	    if(s != NULL)
		{
		     /* no need to do automac process */
		     printf("\nethaddr : %s\n", s);
        	 printf("exiting from automac without any process\n");
		     return 0;
	    }       	
	}

	/* get MAC address from server */
	printf("\n### starting automac process ###\n");
	
	while(1)
	{
	  status = net_loop(MATRIX_AUTOMAC);
	  if(status < 0)
	  {
		printf("Net loop failed...starting automac again\n");
		continue;
	  }
	  else
	  {
		/* automac process completed successfully */
		break;
	  }
	}
	return 0;
}

U_BOOT_CMD(
	automac, 1, 1, do_automac,
	"automac - get MAC address from server\n",
	"automac - get MAC address from server\n"
);
