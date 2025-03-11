/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/* #define	DEBUG	*/

#include <common.h>
#include <autoboot.h>
#include <cli.h>
#include <console.h>
#include <version.h>
#include <asm-generic/gpio.h>
#include <automac_environment.h>

void mx_set_pin_stat(void);

DECLARE_GLOBAL_DATA_PTR;

/*
 * Board-specific Platform code can reimplement show_boot_progress () if needed
 */
__weak void show_boot_progress(int val) {}

static void run_preboot_environment_command(void)
{
#ifdef CONFIG_PREBOOT
	char *p;

	p = env_get("preboot");
	if (p != NULL) {
# ifdef CONFIG_AUTOBOOT_KEYED
		int prev = disable_ctrlc(1);	/* disable Control C checking */
# endif

		run_command_list(p, -1, 0);

# ifdef CONFIG_AUTOBOOT_KEYED
		disable_ctrlc(prev);	/* restore Control C checking */
# endif
	}
#endif /* CONFIG_PREBOOT */
}

#define GREEN_LED_GPIO_NO	16
#define RED_LED_GPIO_NO		23
#define BUZZER_GPIO_NO		5

void mx_set_pin_stat()
{
	gpio_request(GREEN_LED_GPIO_NO, "green_led");
	gpio_request(RED_LED_GPIO_NO, "red_led");
	gpio_request(BUZZER_GPIO_NO, "buzzer");
	
	gpio_direction_output(GREEN_LED_GPIO_NO,0);
	gpio_direction_output(RED_LED_GPIO_NO,0);
	gpio_direction_output(BUZZER_GPIO_NO,0);

	gpio_set_value(GREEN_LED_GPIO_NO, 0); /* green led off */
	gpio_set_value(RED_LED_GPIO_NO, 0); /* red led off */
	gpio_set_value(BUZZER_GPIO_NO, 0); /* buzzer in activate*/
	
	gpio_free(GREEN_LED_GPIO_NO);
	gpio_free(RED_LED_GPIO_NO);
	gpio_free(BUZZER_GPIO_NO);
}

/* We come here after U-Boot is initialised and ready to process commands */
void main_loop(void)
{
	const char *s;
	char card_type_env[5] = {0}; 
	char board_version_env[5] = {0};

	bootstage_mark_name(BOOTSTAGE_ID_MAIN_LOOP, "main_loop");

#ifdef CONFIG_VERSION_VARIABLE
	env_set("ver", version_string);  /* set version variable */
#endif /* CONFIG_VERSION_VARIABLE */

	mx_set_pin_stat();
	
	sprintf(card_type_env, "%d", get_card_type());
	env_set("card_type", (char *)card_type_env);
	
	sprintf(board_version_env, "%d", get_board_version());
	env_set("board_version", (char *)board_version_env);
	
	cli_init();

	run_preboot_environment_command();

#if defined(CONFIG_UPDATE_TFTP)
	update_tftp(0UL, NULL, NULL);
#endif /* CONFIG_UPDATE_TFTP */

	s = bootdelay_process();
	if (cli_process_fdt(&s))
		cli_secure_boot_cmd(s);

	autoboot_command(s);

	cli_loop();
	panic("No CLI available");
}
