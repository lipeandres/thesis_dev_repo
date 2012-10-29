#include <linux/init.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/notifier.h>
#include <linux/regulator/consumer.h>
#include <linux/platform_device.h>

#include <mach/device.h>
#include <mach/lcdif.h>
#include <mach/regs-pwm.h>
#include <mach/system.h>

#define CMD     0
#define DATA    1

#define lcdif_read(reg) __raw_readl(REGS_LCDIF_BASE + reg)
#define lcdif_write(reg,val) __raw_writel(val, REGS_LCDIF_BASE + reg)

#define LCD_FLIPPED 0

static struct clk *lcd_clk;

static char lcd_init_data[] =
{
//Lcdif Sync 	
	0x00, 0x01, 
	0x00, 0x01, 
	0x00, 0x01,
//Start PLL 
	0x00, 0xe0,
		0x01, 0x01,
//PLL src clk set to system clock 
	0x00, 0xe0, 
		0x01, 0x03,
//BGR pixel format
   	0x00, 0x36,
#if LCD_FLIPPED 
	 	0x01, 0x8A,//0x08 No framebuffer flipping, just BGR format  
#else
		0x01, 0x08,
#endif
//general conf 
	0x00, 0xb0, 
		0x01, 0x08,//TTL Mode 0x0c 
		0x01, 0x80,//Dith ON, latch data on rising edge else  
		0x01, 0x01,//Horizontal size HDP = 479 -> 0x1df
		0x01, 0xdf,//
    	0x01, 0x01,//Vertical Size VDP = 271 -> 0x10F
		0x01, 0x0f,// 
		0x01, 0x00,//Even/Odd sync set to RGB/RGB
//MPU interface 
	0x00, 0xf0, 
		0x01, 0x00,// MPU 8-bit interface 
//LCD interface
	0x00, 0x3a, 
		0x01, 0x60,// LCD 18-bit -> 6 6 6 RGB interface
//Pixel Clock Conf
    0x00, 0xe6,
		0x01, 0x03,//0x01,//if CLK=10Mhz -> 150Mhz (PLL)??  
		0x01, 0x33,//0x45,//PCLK = PLL_CLK*((LCDC_FPR+1)/2^20) 
		0x01, 0x32,//0x47,//PCLK 2Mhz, 30Mhz??
//Horizontal Blanking sync  
	0x00, 0xb4, 
		0x01, 0x02,//0x02,//|HT=531 
		0x01, 0x13,//0x0d,// |
    	0x01, 0x00,//0x00,//|HPS=43 
		0x01, 0x2b,//0x2b,// |   
		0x01, 0x01,//0x28,//HPW=1 
		0x01, 0x00,//0x00,//|LPS=8 
		0x01, 0x08,//0x08,// | 
		0x01, 0x00,//0x00,//
//Vertical Blanking sync 
	0x00, 0xb6,
    	0x01, 0x01,//0x01,//|VT=288 
		0x01, 0x20,//0x1d,// | 
		0x01, 0x00,//0x00,//|VPS=12 
		0x01, 0x0c,//0x0c,// | 
		0x01, 0x0a,//0x09,//VPW=10 
		0x01, 0x00,//0x00,//|FPS=4
		0x01, 0x04,//0x00,// |
//FB column access area conf.
    0x00, 0x2a, 
		0x01, 0x00,//|First Column -> 0 
		0x01, 0x00,// |
		0x01, 0x01,//|Last column -> 479
		0x01, 0xdf,// |
//FB row access area conf.
	0x00, 0x2b, 
		0x01, 0x00,//|First row -> 0
    	0x01, 0x00,// |
		0x01, 0x01,//|Last row -> 271 
		0x01, 0x0f,// |
//SSD Image post-processing
	0x00, 0xbc,
		0x01,0x55,//contrast
		0x01,0x67,//brightness
		0x01,0x72,//saturation
		0x01,0x01,//enable		
//PWM conf.
	0x00, 0xbe, 
		0x01, 0x06,
    	0x01, 0xb4,
		0x01, 0x01, 
		0x01, 0xf0,
		0x01, 0x00,
		0x01, 0x00,
//DBC (digital backlight control) init.
	0x00, 0xd0,
		0x01, 0x00,
//Init LCD
	0x00, 0x29,
//Start memory read 
	0x00, 0x2c
};

static void mpulcd_setup_pannel_register(char data, char val)
{

    lcdif_write(HW_LCDIF_CTRL_CLR,BM_LCDIF_CTRL_LCDIF_MASTER |
BM_LCDIF_CTRL_RUN);

    lcdif_write(HW_LCDIF_TRANSFER_COUNT,
        BF_LCDIF_TRANSFER_COUNT_V_COUNT(1) |
        BF_LCDIF_TRANSFER_COUNT_H_COUNT(1));

    if(data)
        lcdif_write(HW_LCDIF_CTRL_SET, BM_LCDIF_CTRL_DATA_SELECT);
    else
        lcdif_write(HW_LCDIF_CTRL_CLR, BM_LCDIF_CTRL_DATA_SELECT);

    lcdif_write(HW_LCDIF_CTRL_SET, BM_LCDIF_CTRL_RUN);

    lcdif_write(HW_LCDIF_DATA, val);

    while(lcdif_read(HW_LCDIF_CTRL) & BM_LCDIF_CTRL_RUN)
    ;

    lcdif_write(HW_LCDIF_CTRL1_CLR, BM_LCDIF_CTRL1_CUR_FRAME_DONE_IRQ);
}

void mpulcd_start_refresh(void)
{

    mpulcd_setup_pannel_register(CMD, 0x2c);

    lcdif_write(HW_LCDIF_CTRL_SET, BM_LCDIF_CTRL_DATA_SELECT);
    lcdif_write(HW_LCDIF_TRANSFER_COUNT,
        BF_LCDIF_TRANSFER_COUNT_V_COUNT(272) |
        BF_LCDIF_TRANSFER_COUNT_H_COUNT(480*3));

    mxs_lcdif_run();
}

static void mpulcd_init_lcdif(void)
{

    lcdif_write(HW_LCDIF_CTRL_CLR,
        BM_LCDIF_CTRL_CLKGATE |
        BM_LCDIF_CTRL_SFTRST);

    lcdif_write(HW_LCDIF_CTRL,
        BF_LCDIF_CTRL_LCD_DATABUS_WIDTH(BV_LCDIF_CTRL_LCD_DATABUS_WIDTH__8_BIT) |
        BF_LCDIF_CTRL_WORD_LENGTH(BV_LCDIF_CTRL_WORD_LENGTH__8_BIT));

    lcdif_write(HW_LCDIF_TIMING, 0x01010101);
}

static void mpulcd_init_panel_hw(void)
{
    int i;

    for(i = 0; i < sizeof(lcd_init_data); i += 2) {
        mpulcd_setup_pannel_register(lcd_init_data[i], lcd_init_data[i + 1]);
        udelay(100);
    }
}

static int mpulcd_init_panel(struct device *dev, dma_addr_t phys, int memsize,
        struct mxs_platform_fb_entry *pentry)
{
    int ret = 0;

    lcd_clk = clk_get(dev, "lcdif");
    if (IS_ERR(lcd_clk)) {
        ret = PTR_ERR(lcd_clk);
        goto out;
    }

    ret = clk_enable(lcd_clk);
    if (ret)
        goto out1;

    ret = clk_set_rate(lcd_clk, 24000000);
    if (ret)
        goto out2;

    mpulcd_init_lcdif();
    mxs_lcdif_dma_init(dev, phys, memsize);
    mpulcd_init_panel_hw();
    mxs_lcdif_notify_clients(MXS_LCDIF_PANEL_INIT, pentry);

    return 0;

out2:
    clk_disable(lcd_clk);
out1:
    clk_put(lcd_clk);
out:
    return ret;
}

static void mpulcd_display_on(void)
{

    mpulcd_setup_pannel_register(CMD, 0x29);
    lcdif_write(HW_LCDIF_CTRL1_SET, BM_LCDIF_CTRL1_CUR_FRAME_DONE_IRQ_EN);
    mpulcd_start_refresh();
}

static void mpulcd_display_off(void)
{

    lcdif_write(HW_LCDIF_CTRL1_CLR, BM_LCDIF_CTRL1_CUR_FRAME_DONE_IRQ_EN);
    mpulcd_setup_pannel_register(CMD, 0x28);
}

static void mpulcd_release_panel(struct device *dev,
        struct mxs_platform_fb_entry *pentry)
{

    mxs_lcdif_notify_clients(MXS_LCDIF_PANEL_RELEASE, pentry);
    mpulcd_display_off();
    mxs_lcdif_dma_release();
    clk_disable(lcd_clk);
    clk_put(lcd_clk);
}

static int mpulcd_blank_panel(int blank)
{
    int ret = 0;

    switch (blank)
    {
        case FB_BLANK_NORMAL:
        case FB_BLANK_HSYNC_SUSPEND:
        case FB_BLANK_POWERDOWN:
        case FB_BLANK_VSYNC_SUSPEND:
        case FB_BLANK_UNBLANK:
            break;

        default:
            ret = -EINVAL;
    }

    return ret;
}

int mpulcd_pan_display(dma_addr_t addr)
{

    lcdif_write(HW_LCDIF_CUR_BUF, addr);

    return 0;
}

static struct mxs_platform_fb_entry fb_entry = {
    .name           = "ssd1963",
    .x_res          = 272,
    .y_res          = 480,
    .bpp            = 24,
    .cycle_time_ns  = 150,
    .lcd_type       = MXS_LCD_PANEL_SYSTEM,
    .init_panel     = mpulcd_init_panel,
    .release_panel  = mpulcd_release_panel,
    .blank_panel    = mpulcd_blank_panel,
    .run_panel      = mpulcd_display_on,
    .stop_panel     = mpulcd_display_off,
    .pan_display    = mpulcd_pan_display,
};

static int __init register_devices(void)
{
    struct platform_device *pdev;
    pdev = mxs_get_device("mxs-fb", 0);
    if (pdev == NULL || IS_ERR(pdev))
        return -ENODEV;

    mxs_lcd_register_entry(&fb_entry, pdev->dev.platform_data);

    return 0;
}

subsys_initcall(register_devices);
