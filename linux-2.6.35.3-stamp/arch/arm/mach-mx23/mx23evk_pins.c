/*
 * Copyright (C) 2009-2010 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/delay.h>

#include <mach/pinctrl.h>
#include <linux/leds.h>

#include "mx23_pins.h"

static struct pin_desc mx23evk_fixed_pins[] = {
	{
	 .name = "DUART.RX",
	 .id = PINID_PWM0,
	 .fun = PIN_FUN3,
	 },
	{
	 .name = "DUART.TX",
	 .id = PINID_PWM1,
	 .fun = PIN_FUN3,
	 }, 
#if defined(CONFIG_I2C_MXS) || \
	defined(CONFIG_I2C_MXS_MODULE)
	{
	 .name = "I2C_SCL",
	 .id = PINID_I2C_SCL,
	 .fun = PIN_FUN1,
	 .strength = PAD_4MA,
	 .voltage = PAD_3_3V,
	 .drive	= 1,
	 },
	{
	 .name = "I2C_SDA",
	 .id = PINID_I2C_SDA,
	 .fun = PIN_FUN1,
	 .strength = PAD_4MA,
	 .voltage = PAD_3_3V,
	 .drive	= 1,
	 },
#endif
};

#if defined(CONFIG_MMC_MXS) || defined(CONFIG_MMC_MXS_MODULE)
static struct pin_desc mx23evk_mmc_pins[] = {
	/* Configurations of SSP2 SD/MMC port pins */
	{
	 .name = "SSP2_DATA0",
	 .id = PINID_SSP2_DATA0,
	 .fun = PIN_FUN3,
	 .strength = PAD_8MA,
	 .voltage = PAD_3_3V,
	 .pullup = 1,
	 .drive = 1,
	 .pull = 1,
	 },
	{
	 .name = "SSP2_DATA1",
	 .id = PINID_SSP2_DATA1,
	 .fun = PIN_FUN3,
	 .strength = PAD_8MA,
	 .voltage = PAD_3_3V,
	 .pullup = 1,
	 .drive = 1,
	 .pull = 1,
	 },
	{
	 .name = "SSP2_DATA2",
	 .id = PINID_SSP2_DATA2,
	 .fun = PIN_FUN3,
	 .strength = PAD_8MA,
	 .voltage = PAD_3_3V,
	 .pullup = 1,
	 .drive = 1,
	 .pull = 1,
	 },
	{
	 .name = "SSP2_DATA3",
	 .id = PINID_SSP2_DATA3,
	 .fun = PIN_FUN3,
	 .strength = PAD_8MA,
	 .voltage = PAD_3_3V,
	 .pullup = 1,
	 .drive = 1,
	 .pull = 1,
	 },
	{
	 .name = "SSP2_CMD",
	 .id = PINID_SSP2_CMD,
	 .fun = PIN_FUN3,
	 .strength = PAD_8MA,
	 .voltage = PAD_3_3V,
	 .pullup = 1,
	 .drive = 1,
	 .pull = 1,
	 },
	{
	 .name = "SSP2_DETECT",
	 .id = PINID_SSP2_DETECT,
	 .fun = PIN_FUN3,
	 .strength = PAD_8MA,
	 .voltage = PAD_3_3V,
	 .pullup = 0,
	 .drive = 1,
	 .pull = 0,
	 },
	{
	 .name = "SSP1_SCK",
	 .id = PINID_SSP1_SCK,
	 .fun = PIN_FUN1,
	 .strength = PAD_8MA,
	 .voltage = PAD_3_3V,
	 .pullup = 0,
	 .drive = 1,
	 .pull = 0,
	 },
};
#endif

#if defined(CONFIG_SPI_MXS) || defined(CONFIG_SPI_MXS_MODULE)
static struct pin_desc mx23evk_spi_pins[] = {
	{
	 .name	= "SSP1_DATA0",
	 .id	= PINID_SSP1_DATA0,
	 .fun	= PIN_FUN1,
	 .strength	= PAD_4MA,
	 .voltage	= PAD_3_3V,
	 .drive 	= 1,
	 },
	{
	 .name	= "SSP1_DATA3",
	 .id	= PINID_SSP1_DATA3,
	 .fun	= PIN_FUN1,
	 .strength	= PAD_4MA,
	 .voltage	= PAD_3_3V,
	 .drive 	= 1,
	 },
	{
	 .name	= "SSP1_CMD",
	 .id	= PINID_SSP1_CMD,
	 .fun	= PIN_FUN1,
	 .strength	= PAD_4MA,
	 .voltage	= PAD_3_3V,
	 .drive 	= 1,
	 },
	{
	 .name	= "SSP1_SCK",
	 .id	= PINID_SSP1_SCK,
	 .fun	= PIN_FUN1,
	 .strength	= PAD_8MA,
	 .voltage	= PAD_3_3V,
	 .drive 	= 1,
	 },
};
#endif


static void mxs_request_pins(struct pin_desc *pins, int nr)
{
	int i;
	struct pin_desc *pin;

	/* configure the pins */
	for (i = 0; i < nr; i++) {
		pin = &pins[i];
		if (pin->fun == PIN_GPIO)
			gpio_request(MXS_PIN_TO_GPIO(pin->id), pin->name);
		else
			mxs_request_pin(pin->id, pin->fun, pin->name);
		if (pin->drive) {
			mxs_set_strength(pin->id, pin->strength, pin->name);
			mxs_set_voltage(pin->id, pin->voltage, pin->name);
		}
		if (pin->pull)
			mxs_set_pullup(pin->id, pin->pullup, pin->name);
		if (pin->fun == PIN_GPIO) {
			if (pin->output)
				gpio_direction_output(MXS_PIN_TO_GPIO(pin->id),
						      pin->data);
			else
				gpio_direction_input(MXS_PIN_TO_GPIO(pin->id));
		}
	}
}

static void mxs_release_pins(struct pin_desc *pins, int nr)
{
	int i;
	struct pin_desc *pin;

	/* release the pins */
	for (i = 0; i < nr; i++) {
		pin = &pins[i];
		if (pin->fun == PIN_GPIO)
			gpio_free(MXS_PIN_TO_GPIO(pin->id));
		else
			mxs_release_pin(pin->id, pin->name);
	}
}

#if defined(CONFIG_MMC_MXS) || defined(CONFIG_MMC_MXS_MODULE)

int mxs_mmc_get_wp_mmc0(void)
{
	return 0;
}

int mxs_mmc_hw_init_mmc0(void)
{
	mxs_request_pins(mx23evk_mmc_pins, ARRAY_SIZE(mx23evk_mmc_pins));
	return 0;
}

void mxs_mmc_hw_release_mmc0(void)
{
	mxs_release_pins(mx23evk_mmc_pins, ARRAY_SIZE(mx23evk_mmc_pins));
}

void mxs_mmc_cmd_pullup_mmc0(int enable)
{
	mxs_set_pullup(PINID_SSP2_CMD, enable, "mmc0_cmd");
}
#else
int mxs_mmc_get_wp_mmc0(void)
{
	return 0;
}
int mxs_mmc_hw_init_mmc0(void)
{
	return 0;
}

void mxs_mmc_hw_release_mmc0(void)
{
}

void mxs_mmc_cmd_pullup_mmc0(int enable)
{
}
#endif

#if defined(CONFIG_ENC28J60) || defined(CONFIG_ENC28J60_MODULE)
int mxs_spi_enc_pin_init(void)
{
	unsigned gpio = MXS_PIN_TO_GPIO(PINID_SSP1_DATA1);
	unsigned enc_rst = MXS_PIN_TO_GPIO(PINID_SSP1_DETECT);

	gpio_request(enc_rst, "enc_reset");
	gpio_direction_output(enc_rst, 1);

	mxs_request_pins(mx23evk_spi_pins, ARRAY_SIZE(mx23evk_spi_pins));

	gpio_request(gpio, "ENC28J60_INTR");
	gpio_direction_input(gpio);
	set_irq_type(gpio_to_irq(gpio), IRQ_TYPE_EDGE_FALLING);

	return 0;
}
int mxs_spi_enc_pin_release(void)
{
	unsigned gpio = MXS_PIN_TO_GPIO(PINID_SSP1_DATA1);


	gpio_free(gpio);
	set_irq_type(gpio_to_irq(gpio), IRQ_TYPE_NONE);

	/* release the pins */
	mxs_release_pins(mx23evk_spi_pins, ARRAY_SIZE(mx23evk_spi_pins));

	return 0;
}
#else
int mxs_spi_enc_pin_init(void)
{
	return 0;
}
int mxs_spi_enc_pin_release(void)
{
	return 0;
}
#endif

#if defined(CONFIG_FEC) || defined(CONFIG_FEC_MODULE)
int mx23evk_enet_gpio_init(void)
{
	/* pwr */
	gpio_request(MXS_PIN_TO_GPIO(PINID_SSP1_DATA3), "ENET_PWR");
	gpio_direction_output(MXS_PIN_TO_GPIO(PINID_SSP1_DATA3), 0);

	/* reset phy */
	gpio_request(MXS_PIN_TO_GPIO(PINID_ENET0_RX_CLK), "PHY_RESET");
	gpio_direction_output(MXS_PIN_TO_GPIO(PINID_ENET0_RX_CLK), 0);
	gpio_direction_output(MXS_PIN_TO_GPIO(PINID_ENET0_RX_CLK), 1);

	return 0;
}
#else
int mx23evk_enet_gpio_init(void)
{
	return 0;
}
#endif

/*
 * GPIO LED
 */
 
#define GPIO_LED_D6 MXS_PIN_TO_GPIO(PINID_GPMI_CLE)

static struct gpio_led km233_gpio_leds[] = {
   {
      .name = "D6",
      .default_trigger = "heartbeat",
      .gpio = GPIO_LED_D6,
   },
};

static struct gpio_led_platform_data km233_gpio_leds_platform_data = {
        .leds           = km233_gpio_leds,
        .num_leds       = ARRAY_SIZE(km233_gpio_leds),
};	

static struct platform_device km233_led_device = {
        .name   = "leds-gpio",
        .id     = -1,
        .dev    = {
                .platform_data  = &km233_gpio_leds_platform_data,
        },
};

static struct platform_device *platform_devices[] __initdata = {
        &km233_led_device,
};

void __init mx23evk_pins_init(void)
{
	gpio_request(GPIO_LED_D6, "D6");
	gpio_direction_output(GPIO_LED_D6, 0);
	gpio_free(GPIO_LED_D6);
	mxs_request_pins(mx23evk_fixed_pins, ARRAY_SIZE(mx23evk_fixed_pins));
	platform_add_devices(platform_devices, ARRAY_SIZE(platform_devices));
}
