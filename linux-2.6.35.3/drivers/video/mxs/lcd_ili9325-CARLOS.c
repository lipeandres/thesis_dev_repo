/**  
  4, according to the data in order of the LCD controller, setting data exchange (Swizzle), and data of the offset direction and offset size. According to the manual ili9325 here we do not need to be offset.
  5, set to the frame of the resolution of the buffer register, here we select WORD_LENRTH bit 00, i.e. the bbp is 16 pixels.
  6, then we set BYTE_PACKING_FORMAT bit set how the data is written to the data calculator, because we choose 16bit mode, the bit is set to 0 is 0x03.
  7, the start needed to interrupt.

  These are all modes need to set the following system mode requires special set.
  1, according to ili9325 set the write mode 8080 mode
  2, the write timing diagram according to the system mode setting BF_CS4 (LCDIF_TIMING, CMD_HOLD, 2, CMD_SETUP, 2, DATA_HOLD, 2, DATA_SETUP, 2);  
 
    3, the number of pixels of the setting screen aspect BF_CS2 (LCDIF_TRANSFER_COUNT H_COUNT, 320, V_COUNT, 240);
    4, the system mode must be set to 1, the system stop mode automatically after each frame and then data transfer, this bit is 0, indicating that the system stops automatically restart. BF_CS1 (LCDIF_CTRL, BYPASS_COUNT, 0);
    5, the start of the LCD screen. BF_CS1 (LCDIF_CTRL, RUN, 1);
Now we want to write the drive is actually a function of filling of the the stmp3xxx_platform_fb_entry fb_entry data structure, such as to achieve init_panel, register configuration in accordance with the above analysis initialize the LCD screen on the line. Other functions are simple setup register. At this point, we can easily write the drive mode, the LCD system.
GOOD LUCK!
*/   
   
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


#define DELAY_FLAG 0xFFFE   // first parameter is 0xfffe, then 2nd parameter is delay time count   
#define END_FLAG   0xFFFF   // first parameter is 0xffff, then parameter table is over  

#define CMD     0
#define DATA    1

#define lcdif_read(reg) __raw_readl(REGS_LCDIF_BASE + reg)
#define lcdif_write(reg,val) __raw_writel(val, REGS_LCDIF_BASE + reg)





static struct mxs_platform_bl_data bl_data;
static struct clk *lcd_clk;



static const unsigned short ili_init_cmdset[] =    
{        
    //--------------Start Initial Sequence -------//   
    0x00E5, 0x8000,
    0x0000, 0x0001,
    0x00E3, 0x3008, // Set internal timing   
    0x00E7, 0x0012, // Set internal timing   
    0x00EF, 0x1231, // Set internal timing   
    0x0001, 0x0100, // set SS and SM bit   
    0x0002, 0x0700, // set 1 line inversion   
    0x0003, 0x1030, // set GRAM write direction and BGR=1.   
    0x0004, 0x0000, // Resize register   
    0x0008, 0x0202, // set the back porch and front porch   
    0x0009, 0x0000, // set non-display area refresh cycle ISC[3:0]   
    0x000A, 0x0000, // FMARK function   
    0x000C, 0x0000, // RGB interface setting   
    0x000D, 0x0000, // Frame marker Position   
    0x000F, 0x0000, // RGB interface polarity   
       
    //--------------Power On sequence ------------//   
    0x0010, 0x0000, // SAP, BT[3:0], AP, DSTB, SLP, STB   
    0x0011, 0x0007, // DC1[2:0], DC0[2:0], VC[2:0]   
    0x0012, 0x0000, // VREG1OUT voltage   
    0x0013, 0x0000, // VDV[4:0] for VCOM amplitude   
//    DELAY_FLAG, 200, // Dis-charge capacitor power voltage   
    0x0010, 0x1690, // SAP, BT[3:0], AP, DSTB, SLP, STB   
    0x0011, 0x0227, // DC1[2:0], DC0[2:0], VC[2:0]  
//    DELAY_FLAG, 50, 
    0x0012, 0x001C, // Internal reference voltage= Vci;  
//    DELAY_FLAG, 50, 
    0x0013, 0x1800, // Set VDV[4:0] for VCOM amplitude   
    0x0029, 0x001C, // Set VCM[5:0] for VCOMH   
    0x002B, 0x000D, // Set Frame Rate 
//    DELAY_FLAG, 50,  
    0x0020, 0x0000, // GRAM horizontal Address   
    0x0021, 0x0000, // GRAM Vertical Address   
       
    //----------- Adjust the Gamma Curve ----------//   
    0x0030, 0x0000,   
    0x0031, 0x0404,   
    0x0032, 0x0304,   
    0x0035, 0x0005,   
    0x0036, 0x1604,   
    0x0037, 0x0304,   
    0x0038, 0x0303,   
    0x0039, 0x0707,   
    0x003C, 0x0500,   
    0x003D, 0x000F,   
       
    //------------------ Set GRAM area ---------------//   
    0x0050, 0x0000, // Horizontal GRAM Start Address   
    0x0051, 0x00EF, // Horizontal GRAM End Address   
    0x0052, 0x0000, // Vertical GRAM Start Address   
    0x0053, 0x013F, // Vertical GRAM Start Address   
    0x0060, 0xA700, // Gate Scan Line   
    0x0061, 0x0001, // NDL,VLE, REV   
    0x006A, 0x0000, // set scrolling line   
       
    //-------------- Partial Display Control ---------//   
    0x0080, 0x0000,   
    0x0081, 0x0000,   
    0x0082, 0x0000,   
    0x0083, 0x0000,   
    0x0084, 0x0000,   
    0x0085, 0x0000,   
       
    //-------------- Panel Control -------------------//   
    0x0090, 0x0010,   
    0x0092, 0x0600,  
    0x0093, 0x0003,
    0x0095, 0x0110,
    0x0097, 0x0000,
    0x0098, 0x0000,
    0x0007, 0x0133, // 262K color and display ON   
};   
   
static const unsigned short ili_turnon_cmdset[] =    
{   
    0x0010, 0x0000, // SAP, BT[3:0], AP, DSTB, SLP, STB   
    0x0011, 0x0007, // DC1[2:0], DC0[2:0], VC[2:0]   
    0x0012, 0x0000, // VREG1OUT voltage   
    0x0013, 0x0000, // VDV[4:0] for VCOM amplitude  
//    DELAY_FLAG, 200, 
    0x0010, 0x1690, // SAP, BT[3:0], AP, DSTB, SLP, STB   
    0x0011, 0x0227, // DC1[2:0], DC0[2:0], VC[2:0]   
    0x0012, 0x001B, // Internal reference voltage= Vci;   
//    DELAY_FLAG, 50,
    0x0013, 0x1600, // Set VDV[4:0] for VCOM amplitude   
    0x0029, 0x0018, // Set VCM[5:0] for VCOMH   
//    DELAY_FLAG, 50,
    0x0007, 0x0133,      // 262K color and display ON   
//    DELAY_FLAG, 50,
};   
    
static const unsigned short ili_turnoff_cmdset[] =    
{   
    //---------------Display Off------------------//       
    0x0007, 0x0000,      // display OFF   
       
    //------------ Power OFF sequence ------------//   
    0x0010, 0x0000,      // SAP, BT[3:0], APE, AP, DSTB, SLP   
    0x0011, 0x0000,      // DC1[2:0], DC0[2:0], VC[2:0]   
    0x0012, 0x0000,      // VREG1OUT voltage   
    0x0013, 0x0000,      // VDV[4:0] for VCOM amplitude   
//    DELAY_FLAG, 200,
    0x0010, 0x0002,      // SAP, BT[3:0], APE, AP, DSTB, SLP 
//    DELAY_FLAG, 50,
};   
/***********************************************************/    

static void ili_setup_pannel_register(char data, char val)
{
        printk("setup_pannel_register\n");
    lcdif_write(HW_LCDIF_CTRL_CLR,BM_LCDIF_CTRL_LCDIF_MASTER | BM_LCDIF_CTRL_RUN);

    lcdif_write(HW_LCDIF_TRANSFER_COUNT, BF_LCDIF_TRANSFER_COUNT_V_COUNT(1) | BF_LCDIF_TRANSFER_COUNT_H_COUNT(1));

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
        printk("mpulcd_start_refresh\n");


    lcdif_write(HW_LCDIF_CTRL_SET, BM_LCDIF_CTRL_DATA_SELECT);
    lcdif_write(HW_LCDIF_TRANSFER_COUNT,
        BF_LCDIF_TRANSFER_COUNT_V_COUNT(240) |
        BF_LCDIF_TRANSFER_COUNT_H_COUNT(320*3));

    mxs_lcdif_run();
}


static void ili_init_lcdif(void)
{
        printk("ili_init_lcdif\n");
    lcdif_write(HW_LCDIF_CTRL_CLR,
        BM_LCDIF_CTRL_CLKGATE |
        BM_LCDIF_CTRL_SFTRST);

    lcdif_write(HW_LCDIF_CTRL,
        BF_LCDIF_CTRL_LCD_DATABUS_WIDTH(BV_LCDIF_CTRL_LCD_DATABUS_WIDTH__8_BIT) |
        BF_LCDIF_CTRL_WORD_LENGTH(BV_LCDIF_CTRL_WORD_LENGTH__8_BIT));

    lcdif_write(HW_LCDIF_TIMING, 0x01010101);
}

static void ili_init_panel_hw(void)
{
    int i;
        printk("ili_init_panel_hw\n");

     for(i = 0; i < sizeof(ili_init_cmdset); i += 2) {
       printk("%x, %x\n", ( ili_init_cmdset[i] >> 8 ) & (0xFF), ( ( ili_init_cmdset[i] ) & ( 0xFF ) ) );

       ili_setup_pannel_register( 0x00, ( ( ili_init_cmdset[i] >> 8 ) & (0xFF) ) );
       udelay(100);
       ili_setup_pannel_register( 0x00, ( ( ili_init_cmdset[i] ) & ( 0xFF ) ) );
       udelay(100);

       printk("%x, %x\n", ( ili_init_cmdset[i+1] >> 8 ) & (0xFF), ( ( ili_init_cmdset[i+1] ) & ( 0xFF ) ) );

       ili_setup_pannel_register( 0x01, ( ( ili_init_cmdset[i + 1] >> 8 ) & ( 0xFF ) ) );
       udelay(100);
       ili_setup_pannel_register( 0x01, ( ili_init_cmdset[i + 1] ) & ( 0xFF ) );
       mdelay(200);
   }

}



static int init_panel(struct device *dev, dma_addr_t phys, int memsize,
		      struct mxs_platform_fb_entry *pentry)
{

    int ret = 0;
        printk("init_panel\n");

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

	__raw_writel(BM_LCDIF_CTRL1_RESET, REGS_LCDIF_BASE + HW_LCDIF_CTRL1_CLR);	// low
	mdelay(100);
	__raw_writel(BM_LCDIF_CTRL1_RESET, REGS_LCDIF_BASE + HW_LCDIF_CTRL1_SET);	// high
	mdelay(10);
	__raw_writel(BM_LCDIF_CTRL1_RESET, REGS_LCDIF_BASE + HW_LCDIF_CTRL1_CLR);	// low
	mdelay(10);
	__raw_writel(BM_LCDIF_CTRL1_RESET, REGS_LCDIF_BASE + HW_LCDIF_CTRL1_SET);	// high
	mdelay(1);


    ili_init_lcdif();
    mxs_lcdif_dma_init(dev, phys, memsize);
    ili_init_panel_hw();
    mxs_lcdif_notify_clients(MXS_LCDIF_PANEL_INIT, pentry);

    return 0;
out2:
    clk_disable(lcd_clk);
out1:
    clk_put(lcd_clk);
out:
    return ret;
}

static void ili_lcdif_run(void)
{
    int i;
        printk("ili_lcdif_run\n");

     for(i = 0; i < sizeof(ili_turnon_cmdset); i += 2) {
       ili_setup_pannel_register( 0x00, ( ( ili_turnon_cmdset[i] >> 8 ) & (0xFF) ) );
       udelay(100);
       ili_setup_pannel_register( 0x00, ( ( ili_turnon_cmdset[i] ) & ( 0xFF ) ) );
       udelay(100);
       ili_setup_pannel_register( 0x01, ( ( ili_turnon_cmdset[i + 1] >> 8 ) & ( 0xFF ) ) );
       udelay(100);
       ili_setup_pannel_register( 0x01, ( ili_turnon_cmdset[i + 1] ) & ( 0xFF ) );
       udelay(100);
   }
    lcdif_write(HW_LCDIF_CTRL1_SET, BM_LCDIF_CTRL1_CUR_FRAME_DONE_IRQ_EN);
    mpulcd_start_refresh();

}

static void ili_lcdif_stop(void)
{
    int i;
        printk("ili_lcdif_stop\n");

     for(i = 0; i < sizeof(ili_turnoff_cmdset); i += 2) {
       ili_setup_pannel_register( 0x00, ( ( ili_turnoff_cmdset[i] >> 8 ) & (0xFF) ) );
       udelay(100);
       ili_setup_pannel_register( 0x00, ( ( ili_turnoff_cmdset[i] ) & ( 0xFF ) ) );
       udelay(100);
       ili_setup_pannel_register( 0x01, ( ( ili_turnoff_cmdset[i + 1] >> 8 ) & ( 0xFF ) ) );
       udelay(100);
       ili_setup_pannel_register( 0x01, ( ili_turnoff_cmdset[i + 1] ) & ( 0xFF ) );
       udelay(100);
   }
    lcdif_write(HW_LCDIF_CTRL1_CLR, BM_LCDIF_CTRL1_CUR_FRAME_DONE_IRQ_EN);


}


static void release_panel(struct device *dev,
			  struct mxs_platform_fb_entry *pentry)
{
        printk("release_panel\n");

	mxs_lcdif_notify_clients(MXS_LCDIF_PANEL_RELEASE, pentry);
        ili_lcdif_stop();
	mxs_lcdif_dma_release();
	clk_disable(lcd_clk);
	clk_put(lcd_clk);
}

static int blank_panel(int blank)
{
	int ret = 0;
        printk("\n");

	switch (blank) {
	case FB_BLANK_NORMAL:
	case FB_BLANK_VSYNC_SUSPEND:
	case FB_BLANK_HSYNC_SUSPEND:
	case FB_BLANK_POWERDOWN:
	case FB_BLANK_UNBLANK:
		break;

	default:
		ret = -EINVAL;
	}
	return ret;
}


int ili_lcdif_pan_display(dma_addr_t addr)
{
        printk("ili_lcdif_pan_display\n");

    lcdif_write(HW_LCDIF_CUR_BUF, addr);

    return 0;
}

static struct mxs_platform_fb_entry fb_entry = {
	.name          = "ili9325",
	.x_res         = 240,
	.y_res         = 320,
	.bpp           = 24,
	.cycle_time_ns = 150,
	.lcd_type      = MXS_LCD_PANEL_SYSTEM,
	.init_panel    = init_panel,
	.release_panel = release_panel,
	.blank_panel   = blank_panel,
	.run_panel     = ili_lcdif_run,            // cambiar a ili_lcdif_run
	.stop_panel    = ili_lcdif_stop,          // cambiar a ili_lcdif_stop
	.pan_display   = ili_lcdif_pan_display,  // cambiar a ili_lcdif_pan_display
};




static int __init register_devices(void)
{
	struct platform_device *pdev;

        printk("register_devices\n");


	pdev = mxs_get_device("mxs-fb", 0);
	if (pdev == NULL || IS_ERR(pdev))
		return -ENODEV;

	mxs_lcd_register_entry(&fb_entry, pdev->dev.platform_data);

	return 0;
}

subsys_initcall(register_devices);


