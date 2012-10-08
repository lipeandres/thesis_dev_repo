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
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/i2c/at24.h>
#include <linux/spi/spi.h>
//#include <linux/spi/spi_gpio.h>
#include <linux/spi/ads7846.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/fixed.h>

#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>

#include <mach/hardware.h>
#include <mach/device.h>
#include <mach/pinctrl.h>
#include <mach/regs-ocotp.h>


#include "device.h"
#include "mx23evk.h"
#include "mx23_pins.h"

// #define AT24C256
//#define TS_PENIRQ_GPIO PINID_SSP1_DATA1

static struct at24_platform_data at24cXXX_platdata = {
    .byte_len    = SZ_8K / 8,
    .page_size    = 128,
#ifdef AT24C256    
    .flags = AT24_FLAG_ADDR16,
#else
    .flags = AT24_FLAG_TAKE8ADDR,
#endif    
};

static struct i2c_board_info __initdata at24cXXX_i2c_device = {
#ifdef AT24C256 	
	I2C_BOARD_INFO("at24c256", 0x50),
#else
	I2C_BOARD_INFO("at24c08", 0x50),
#endif
	.platform_data = (void *)&at24cXXX_platdata,
};

static void i2c_device_init(void)
{
	i2c_register_board_info(0, &at24cXXX_i2c_device, 1);
}

#if defined(CONFIG_SPI_MXS) || defined(CONFIG_SPI_MXS_MODULE)
static struct mxs_spi_platform_data ssp1_data = {
	.hw_pin_init = mxs_spi_enc_pin_init,
	.hw_pin_release = mxs_spi_enc_pin_release,
	.clk = "ssp.0",
};
#endif

#if defined(CONFIG_TOUCHSCREEN_ADS7846)
//#define TS_PENIRQ_GPIO PINID_PWM2
#define TS_PENIRQ_GPIO PINID_GPMI_D05




struct platform_device sk_spi_gpio_device = {
       .name    = "mxs-spi",
       .id      = 1,
       .dev     = {
                .platform_data = &ssp1_data,
       },
};

static int ads7846_dev_init(void)
{ 
        if (gpio_request(MXS_PIN_TO_GPIO(TS_PENIRQ_GPIO), "ADS7846 pendown") < 0)
                printk(KERN_ERR "can't get ads7846 pen down GPIO\n");

        gpio_direction_input(MXS_PIN_TO_GPIO(TS_PENIRQ_GPIO));
        set_irq_type(gpio_to_irq(MXS_PIN_TO_GPIO(TS_PENIRQ_GPIO)), IRQ_TYPE_EDGE_FALLING);

        return gpio_to_irq(MXS_PIN_TO_GPIO(TS_PENIRQ_GPIO));
}

static int ads7843_pendown_state(void)
{
        return !gpio_get_value(MXS_PIN_TO_GPIO(TS_PENIRQ_GPIO));      /* Touchscreen PENIRQ */
}

static struct ads7846_platform_data ads_info = {
        .model                  = 7843,
        .x_min                  = 150,
        .x_max                  = 3830,
        .y_min                  = 190,
        .y_max                  = 3830,
        .vref_delay_usecs       = 0,//100
		.penirq_recheck_delay_usecs = 200,//Nuevo
		.pressure_min			= 128,	
		.pressure_max			= 1024,
        .debounce_max           = 15,//10
        .debounce_tol           = 5,//3
        .debounce_rep           = 1,//1
		.x_plate_ohms           = 250,
        .y_plate_ohms           = 450,
		.swap_xy 				= 1,
        .get_pendown_state      = ads7843_pendown_state,
//		.wakeup 				= 1,
//	    .keep_vref_on			=1,
};
#endif

static struct spi_board_info spi_board_info[] __initdata = {
#if defined(CONFIG_TOUCHSCREEN_ADS7846)
//       [0]= {       /* Touchscreen support */
//                .modalias       = "ads7846",
//                .max_speed_hz   = 100 * 1000,
//                .bus_num        = 1,
//                .controller_data = (void *) MXS_PIN_TO_GPIO(PINID_SSP1_DATA3),
//                .platform_data  = &ads_info,
//        },
//#endif
//#if defined(CONFIG_KS8851) || defined(CONFIG_KS8851_MODULE) 
	[0] = {
		.modalias       = "ads7846",
		.max_speed_hz   = 120000,
 		.bus_num	= 1,
 		.chip_select    = 0,
		.irq            = PINID_SSP1_DATA1,
 		.platform_data  = &ads_info,
 	},
 #endif
};

static void spi_device_init(void)
{
#if defined(CONFIG_TOUCHSCREEN_ADS7846)
        platform_device_register(&sk_spi_gpio_device);
        spi_board_info[0].irq = ads7846_dev_init();
#endif
	//spi_board_info[0].irq = gpio_to_irq(MXS_PIN_TO_GPIO(PINID_SSP1_DATA1));
	spi_register_board_info(spi_board_info, ARRAY_SIZE(spi_board_info));
}

static void __init fixup_board(struct machine_desc *desc, struct tag *tags,
			       char **cmdline, struct meminfo *mi)
{
	mx23_set_input_clk(24000000, 24000000, 32000, 50000000);
}

#if defined(CONFIG_SND_MXS_SOC_ADC) || defined(CONFIG_SND_MXS_SOC_ADC_MODULE)
static void __init mx23evk_init_adc(void)
{
	struct platform_device *pdev;
	pdev = mxs_get_device("mxs-adc", 0);
	if (pdev == NULL)
		return;
	mxs_add_device(pdev, 3);
}
#else
static void __init mx23evk_init_adc(void)
{

}
#endif

#define REGS_OCOTP_BASE	IO_ADDRESS(OCOTP_PHYS_ADDR)
int get_evk_board_version()
{
	int boardid;
	boardid = __raw_readl(REGS_OCOTP_BASE + HW_OCOTP_CUSTCAP);
	boardid &= 0x30000000;
	boardid = boardid >> 28;

	return boardid;
}
EXPORT_SYMBOL_GPL(get_evk_board_version);

static void __init mx23evk_device_init(void)
{
	/* Add mx23evk special code */
	i2c_device_init();
	spi_device_init();
	mx23evk_init_adc();
}


static void __init mx23evk_init_machine(void)
{
	mx23_pinctrl_init();

	/* Init iram allocate */
#ifdef CONFIG_VECTORS_PHY_ADDR
	/* reserve the first page for irq vectors table*/
	iram_init(MX23_OCRAM_PHBASE + PAGE_SIZE, MX23_OCRAM_SIZE - PAGE_SIZE);
#else
	iram_init(MX23_OCRAM_PHBASE, MX23_OCRAM_SIZE);
#endif

	mx23_gpio_init();
	mx23evk_pins_init();
	mx23_device_init();
	mx23evk_device_init();
}

MACHINE_START(MX23EVK, "Freescale MX23EVK board")
	.phys_io	= 0x80000000,
	.io_pg_offst	= ((0xf0000000) >> 18) & 0xfffc,
	.boot_params	= 0x40000100,
	.fixup		= fixup_board,
	.map_io		= mx23_map_io,
	.init_irq	= mx23_irq_init,
	.init_machine	= mx23evk_init_machine,
	.timer		= &mx23_timer.timer,
MACHINE_END
