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

#define CMD     0x00
#define DATA    0x01
#define DELAY_FLAG 0xFAFA

#define lcdif_read(reg) __raw_readl(REGS_LCDIF_BASE + reg)
#define lcdif_write(reg,val) __raw_writel(val, REGS_LCDIF_BASE + reg)
#define ili9325_start_refresh mpulcd_start_refresh
#define DEBUG_ON 0

static struct clk *lcd_clk;
// Extra Function prototypes
static void lcdif_normal_config(void);
static void lcdif_alternate_config(void);
static void lcdif_clear_fifo(void);
static void lcdif_set_timings(unsigned char cmd_hold,unsigned char cmd_setup,
															unsigned char data_hold ,unsigned char data_setup);
static void ili9325_gram_transfer_init(void);
static void ili9325_data_transfer_sync(void);
static void lcdif_wait_ready(void);
static void ili9325_set_register(unsigned short reg, unsigned short value);
static void lcdif_pio_send(unsigned char data_mode,unsigned short value);
static unsigned int lcdif_manage_irqs(unsigned int new_irqs);

															


static const unsigned short ili9325_init_data[] =
{
    //--------------Start Initial Sequence -------//   
    0x00E5, 0x8000,
    
    0x0000, 0x0001,
    
    0x00E3, 0x3008, // Set internal timing   
    0x00E7, 0x0012, // Set internal timing   
    0x00EF, 0x1231, // Set internal timing
    //Panel Configuration   
    0x0001, 0x0000, // set SS and SM bit   
    0x0002, 0x0700, // set 1 line inversion   
    0x0003, 0xC130, // set GRAM write direction and BGR=1.   
    0x0004, 0x0000, // Resize register   
    0x0008, 0x0207, // set the back porch and front porch   
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
    
    0xFAFA, 200,//    DELAY_FLAG, 200ms, // Dis-charge capacitor power voltage   
    
    0x0010, 0x1690, // SAP, BT[3:0], AP, DSTB, SLP, STB   
    0x0011, 0x0227, // DC1[2:0], DC0[2:0], VC[2:0]  
    
    0xFAFA, 50,//    DELAY_FLAG, 50, 
    
    0x0012, 0x001C, // Internal reference voltage= Vci;  
    
    0xFAFA, 50,//    DELAY_FLAG, 50, 0x001A
    
    0x0013, 0x1800, // Set VDV[4:0] for VCOM amplitude   
    0x0029, 0x001C, // Set VCM[5:0] for VCOMH   
    0x002B, 0x000D, // Set Frame Rate 
    
    0xFAFA, 50,//    DELAY_FLAG, 50,  
    
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

    0xFAFA, 50,//    DELAY_FLAG, 50

    0x0007, 0x0133,

    0xFAFA, 50//    DELAY_FLAG, 50
};

static const unsigned short ili9325_turnon_cmdset[] =    
{   
    0x0010, 0x0000, // SAP, BT[3:0], AP, DSTB, SLP, STB   
    0x0011, 0x0007, // DC1[2:0], DC0[2:0], VC[2:0]   
    0x0012, 0x0000, // VREG1OUT voltage   
    0x0013, 0x0000, // VDV[4:0] for VCOM amplitude  
		
		0xFAFA, 200,//    DELAY_FLAG, 200ms, // Dis-charge capacitor power voltage   
    
    0x0010, 0x1790, // SAP, BT[3:0], AP, DSTB, SLP, STB   
    0x0011, 0x0221, // DC1[2:0], DC0[2:0], VC[2:0] (para 3.3V Vci1 = 0.9*3.3=2-2.97)
										//DDVDH=5.94 < 6V, VCL=-2.97 > -3V ,VGH=11.88 < 15, VGL=-8.91 >-12.5 
    
    0xFAFA, 50,//    DELAY_FLAG, 50   
    
    0x0012, 0x001B, // Internal reference voltage= Vci;VREG1OUT = 5.2 < (5.94 -0.5)   

    0xFAFA, 50,//    DELAY_FLAG, 50		
		
		0x0013, 0x1600, // Set VDV[4:0] for VCOM amplitude  VREG1OUT * 1.06 = 5.6
    0x0029, 0x0018, // Set VCM[5:0] for VCOMH   VREG1OUT * 0.885 = 4.69

    0xFAFA, 50,//    DELAY_FLAG, 50

    0x0007, 0x0133      // 262K color and display ON   
};   

static const unsigned short ili9325_turnoff_cmdset[] =    
{   
    //---------------Display Off------------------//       
    0x0007, 0x0000,      // display OFF   
       
    //------------ Power OFF sequence ------------//   
    0x0010, 0x0000,      // SAP, BT[3:0], APE, AP, DSTB, SLP   
    0x0011, 0x0000,      // DC1[2:0], DC0[2:0], VC[2:0]   
    0x0012, 0x0000,      // VREG1OUT voltage   
    0x0013, 0x0000,      // VDV[4:0] for VCOM amplitude   

    0xFAFA, 200,//    DELAY_FLAG, 200

    0x0010, 0x0002,      // SAP, BT[3:0], APE, AP, DSTB, SLP 
    
    0xFAFA, 50//    DELAY_FLAG, 50
};

static void ili9325_set_register(unsigned short reg, unsigned short value){
	lcdif_pio_send(CMD,reg);
	lcdif_pio_send(DATA,value);
}

static unsigned int lcdif_manage_irqs(unsigned int new_irqs){
	//Save previously enabled IRQs
	unsigned int old_irqs = ((lcdif_read(HW_LCDIF_CTRL1)) & (BM_LCDIF_CTRL1_OVERFLOW_IRQ_EN|
																	BM_LCDIF_CTRL1_UNDERFLOW_IRQ_EN|
																	BM_LCDIF_CTRL1_CUR_FRAME_DONE_IRQ_EN|
																	BM_LCDIF_CTRL1_VSYNC_EDGE_IRQ_EN|
																	BM_LCDIF_CTRL1_BM_ERROR_IRQ_EN));
																	

	//Clear current irqs
	lcdif_write(HW_LCDIF_CTRL1_CLR, BM_LCDIF_CTRL1_OVERFLOW_IRQ|
																	BM_LCDIF_CTRL1_UNDERFLOW_IRQ|
																	BM_LCDIF_CTRL1_CUR_FRAME_DONE_IRQ|
																	BM_LCDIF_CTRL1_VSYNC_EDGE_IRQ|
																	BM_LCDIF_CTRL1_BM_ERROR_IRQ);
																	
	//Disable IRQs
	lcdif_write(HW_LCDIF_CTRL1_CLR, BM_LCDIF_CTRL1_OVERFLOW_IRQ_EN|
																	BM_LCDIF_CTRL1_UNDERFLOW_IRQ_EN|
																	BM_LCDIF_CTRL1_CUR_FRAME_DONE_IRQ_EN|
																	BM_LCDIF_CTRL1_VSYNC_EDGE_IRQ_EN|
																	BM_LCDIF_CTRL1_BM_ERROR_IRQ_EN);
																	
	//Enable desired IRQs	
	lcdif_write(HW_LCDIF_CTRL1_SET, new_irqs);
	
	//Return old irqs
	return	old_irqs; 																																		 
	}

/*static void ili9325_setup_pannel_register(char data, char val)
{

    lcdif_write(HW_LCDIF_CTRL_CLR,BM_LCDIF_CTRL_LCDIF_MASTER |
BM_LCDIF_CTRL_RUN);

    lcdif_write(HW_LCDIF_TRANSFER_COUNT,
        BF_LCDIF_TRANSFER_COUNT_V_COUNT(1) |
        BF_LCDIF_TRANSFER_COUNT_H_COUNT(1));

    if(data){
				//printk("value %x \n",val);
        lcdif_write(HW_LCDIF_CTRL_SET, BM_LCDIF_CTRL_DATA_SELECT);}
    else{
				//printk("Register %x \n",val);
        lcdif_write(HW_LCDIF_CTRL_CLR, BM_LCDIF_CTRL_DATA_SELECT);}

    lcdif_write(HW_LCDIF_CTRL_SET, BM_LCDIF_CTRL_RUN);

    lcdif_write(HW_LCDIF_DATA, val);

    while(lcdif_read(HW_LCDIF_CTRL) & BM_LCDIF_CTRL_RUN)
    ;

    lcdif_write(HW_LCDIF_CTRL1_CLR, BM_LCDIF_CTRL1_CUR_FRAME_DONE_IRQ);
}
*/
static void ili9325_gram_transfer_init(void){
		//Set gram to address 0,0
		ili9325_set_register(0x0020,0x0000);	
		ili9325_set_register(0x0021,0x0000);

		//Start Gram write
    lcdif_pio_send(CMD,0x0022);		
	}

void ili9325_start_refresh(void)
{
		ili9325_gram_transfer_init();
    lcdif_write(HW_LCDIF_CTRL_SET, BM_LCDIF_CTRL_DATA_SELECT);
    lcdif_write(HW_LCDIF_TRANSFER_COUNT,
        BF_LCDIF_TRANSFER_COUNT_V_COUNT(240) |
        BF_LCDIF_TRANSFER_COUNT_H_COUNT(320*3));   
    mxs_lcdif_run();
}

static void lcdif_wait_ready(void){
	while(lcdif_read(HW_LCDIF_CTRL) & BM_LCDIF_CTRL_RUN);
	}

static void lcdif_clear_fifo(void){
	    //Clear RX and TX FIFO
    lcdif_write(HW_LCDIF_CTRL1_SET,BM_LCDIF_CTRL1_FIFO_CLEAR);   
    lcdif_write(HW_LCDIF_CTRL1_CLR,BM_LCDIF_CTRL1_FIFO_CLEAR);
	}	
	
static void lcdif_alternate_config(void){
		//invert outuput data -> upper byte first
		lcdif_write(HW_LCDIF_CTRL,lcdif_read(HW_LCDIF_CTRL)| 
															BF_LCDIF_CTRL_CSC_DATA_SWIZZLE(0x1));
		lcdif_write(HW_LCDIF_CTRL1_CLR,BM_LCDIF_CTRL1_MODE86);
		//Make valid only the first two bytes of a 32 bit word
		//printk("CTRL1 before :0x%x\n",lcdif_read(HW_LCDIF_CTRL1));
		lcdif_write(HW_LCDIF_CTRL1,lcdif_read(HW_LCDIF_CTRL1) ^ 
															BF_LCDIF_CTRL1_BYTE_PACKING_FORMAT(0xF));
		lcdif_write(HW_LCDIF_CTRL1,lcdif_read(HW_LCDIF_CTRL1) |
															BF_LCDIF_CTRL1_BYTE_PACKING_FORMAT(0x3));
		//printk("CTRL1 after :0x%x\n",lcdif_read(HW_LCDIF_CTRL1));													
}	

static void lcdif_normal_config(void){
					//Deactivate swizzle
	lcdif_write(HW_LCDIF_CTRL,
        lcdif_read(HW_LCDIF_CTRL) ^ BF_LCDIF_CTRL_CSC_DATA_SWIZZLE(0x1));
        //Make valid only all bytes of a 32 bit word
   lcdif_write(HW_LCDIF_CTRL1,
        lcdif_read(HW_LCDIF_CTRL1) | BF_LCDIF_CTRL1_BYTE_PACKING_FORMAT(0xF));	
	}
	
static void lcdif_set_timings(unsigned char cmd_hold,unsigned char cmd_setup,
															unsigned char data_hold ,unsigned char data_setup){
	//Set LCDIF System Interface timings in 
	//PIXCLK cycles (43ns for a 24Mhz clock)															
	lcdif_write(HW_LCDIF_TIMING,
	(cmd_hold << BP_LCDIF_TIMING_CMD_HOLD)|
	(cmd_setup << BP_LCDIF_TIMING_CMD_SETUP)|
	(data_hold << BP_LCDIF_TIMING_DATA_HOLD)|
	(data_setup << BP_LCDIF_TIMING_DATA_SETUP)
	);
}	

static void ili9325_data_transfer_sync(void){
	int old_irqs = lcdif_manage_irqs(0);  
		lcdif_write(HW_LCDIF_CTRL_CLR,BM_LCDIF_CTRL_LCDIF_MASTER | BM_LCDIF_CTRL_RUN);
    lcdif_write(HW_LCDIF_TRANSFER_COUNT,
        BF_LCDIF_TRANSFER_COUNT_V_COUNT(1) |
        BF_LCDIF_TRANSFER_COUNT_H_COUNT(4));

    lcdif_write(HW_LCDIF_CTRL_CLR, BM_LCDIF_CTRL_DATA_SELECT);

    lcdif_write(HW_LCDIF_CTRL_SET, BM_LCDIF_CTRL_RUN);

    lcdif_write(HW_LCDIF_DATA, 0x00000000);      

		lcdif_wait_ready();

    lcdif_write(HW_LCDIF_CTRL1_CLR, BM_LCDIF_CTRL1_CUR_FRAME_DONE_IRQ);
		//lcdif_clear_fifo();
		lcdif_manage_irqs(old_irqs);
		printk("LCD system interface data transfer synchronized.\n");
}


static void lcdif_pio_send(unsigned char data_mode, unsigned short value){
		//unsigned int old_irqs; 
		//lcdif_alternate_config();
		//lcdif_wait_ready();
		//old_irqs = lcdif_manage_irqs(0);
		//Turn lcdif off, deactivate bus master
		lcdif_write(HW_LCDIF_CTRL_CLR,BM_LCDIF_CTRL_LCDIF_MASTER|
																	BM_LCDIF_CTRL_RUN|
																	BM_LCDIF_CTRL_DATA_SELECT);
		//Set transfer count for next transfer 															
		lcdif_write(HW_LCDIF_TRANSFER_COUNT,
        BF_LCDIF_TRANSFER_COUNT_V_COUNT(1) |
        BF_LCDIF_TRANSFER_COUNT_H_COUNT(2));
    //Set lcdif to send data or command
    if(data_mode == DATA)    
    lcdif_write(HW_LCDIF_CTRL_SET, BM_LCDIF_CTRL_DATA_SELECT);
    else
    lcdif_write(HW_LCDIF_CTRL_CLR, BM_LCDIF_CTRL_DATA_SELECT);
		//Turn lcdif on
    lcdif_write(HW_LCDIF_CTRL_SET, BM_LCDIF_CTRL_RUN);
		//Send desired data
		//while(lcdif_read(HW_LCDIF_STAT) & BM_LCDIF_STAT_LFIFO_FULL);//Necesaria??
    lcdif_write(HW_LCDIF_DATA, 0x12131415);      
		//Wait for the lcdif to end
		lcdif_wait_ready();
		lcdif_write(HW_LCDIF_CTRL1_CLR, BM_LCDIF_CTRL1_CUR_FRAME_DONE_IRQ);
		//lcdif_normal_config();
		//lcdif_clear_fifo();
		//Return to default lcdif configuration
		//lcdif_manage_irqs(old_irqs);	
}

static void ili9325_init_lcdif(void)
{

    lcdif_write(HW_LCDIF_CTRL_CLR,
        BM_LCDIF_CTRL_CLKGATE |
        BM_LCDIF_CTRL_SFTRST);

    lcdif_write(HW_LCDIF_CTRL,
        BF_LCDIF_CTRL_LCD_DATABUS_WIDTH(BV_LCDIF_CTRL_LCD_DATABUS_WIDTH__8_BIT) |
        BF_LCDIF_CTRL_WORD_LENGTH(BV_LCDIF_CTRL_WORD_LENGTH__8_BIT));

    lcdif_set_timings(0x02,0x02,0x01,0x02);
}

static void ili9325_init_panel_hw(void)
{
    int i;
    for(i = 0; i < (sizeof(ili9325_init_data)/2); i += 2) {
			//Register address
				//upper byte
        if(ili9325_init_data[i] ==  (DELAY_FLAG)){
            mdelay(ili9325_init_data[i+1]);
#if DEBUG_ON            
            printk("LCD Command delay:%d ms\n",ili9325_init_data[i+1]);
#endif            
        }
        else{
            ili9325_set_register(ili9325_init_data[i],ili9325_init_data[i+1]);
#if DEBUG_ON            
            printk("LCD Command \tCMD:0x%x \tDATA:0x%x.\n",ili9325_init_data[i],ili9325_init_data[i+1]);
#endif            
        }        
    }
}

static void ili9325_display_reset(void){
    lcdif_write(HW_LCDIF_CTRL1_CLR,BM_LCDIF_CTRL1_RESET);//Low
    mdelay(10);
    lcdif_write(HW_LCDIF_CTRL1_SET,BM_LCDIF_CTRL1_RESET);//High
    mdelay(10);
    lcdif_write(HW_LCDIF_CTRL1_CLR,BM_LCDIF_CTRL1_RESET);//Low
    mdelay(10);
    lcdif_write(HW_LCDIF_CTRL1_SET,BM_LCDIF_CTRL1_RESET);//High
    mdelay(1);
    //printk("LCD was reset.\n");
}

static int ili9325_init_panel(struct device *dev, dma_addr_t phys, int memsize,
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

    ili9325_init_lcdif();
    mxs_lcdif_dma_init(dev, phys, memsize);
    ili9325_display_reset();
		ili9325_init_panel_hw();
    mxs_lcdif_notify_clients(MXS_LCDIF_PANEL_INIT, pentry);
		//printk("LCD inited\n");
    return 0;

out2:
    clk_disable(lcd_clk);
out1:
    clk_put(lcd_clk);
out:
    return ret;
}

static void ili9325_display_on(void)
{
    int i;
    for(i = 0; i < (sizeof(ili9325_turnon_cmdset)/2); i += 2) {
			//Register address
				//upper byte
        if(ili9325_turnon_cmdset[i] ==  (DELAY_FLAG)){
            mdelay(ili9325_turnon_cmdset[i+1]);
#if DEBUG_ON            
            printk("LCD Command delay:%d ms\n",ili9325_turnon_cmdset[i+1]);
#endif            
        }
        else{
            ili9325_set_register(ili9325_turnon_cmdset[i],ili9325_turnon_cmdset[i+1]);
#if DEBUG_ON            
            printk("LCD Turn ON Command CMD:\t0x%x \tDATA:0x%x.\n",ili9325_turnon_cmdset[i],
																													ili9325_turnon_cmdset[i+1]);
#endif
        }        
    }
    //printk("LCD powered on\n");
    ili9325_start_refresh();
}

static void ili9325_display_off(void)
{
    int i;
    for(i = 0; i < (sizeof(ili9325_turnoff_cmdset)/2); i += 2) {
			//Register address
				//upper byte
        if(ili9325_turnon_cmdset[i] ==  (DELAY_FLAG)){
            mdelay(ili9325_turnoff_cmdset[i+1]);
#if DEBUG_ON            
            printk("LCD Command delay:%d ms\n",ili9325_turnoff_cmdset[i+1]);
#endif            
        }
        else{
            ili9325_set_register(ili9325_turnoff_cmdset[i],ili9325_turnoff_cmdset[i+1]);
#if DEBUG_ON            
            printk("LCD Turn OFF Command CMD:\t0x%x \tDATA:0x%x.\n",ili9325_turnoff_cmdset[i],
																													ili9325_turnoff_cmdset[i+1]);
#endif																													
        }        
    }
    //printk("LCD powered off\n");
}

static void ili9325_release_panel(struct device *dev,
        struct mxs_platform_fb_entry *pentry)
{

    mxs_lcdif_notify_clients(MXS_LCDIF_PANEL_RELEASE, pentry);
    ili9325_display_off();
    mxs_lcdif_dma_release();
    clk_disable(lcd_clk);
    clk_put(lcd_clk);
}

static int ili9325_blank_panel(int blank)
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

int ili9325_pan_display(dma_addr_t addr)
{

    lcdif_write(HW_LCDIF_CUR_BUF, addr);

    return 0;
}

static struct mxs_platform_fb_entry fb_entry = {
    .name           = "ili9325",
    .x_res          = 240,
    .y_res          = 320,
    .bpp            = 24,
    .cycle_time_ns  = 150,
    .lcd_type       = MXS_LCD_PANEL_SYSTEM,
    .init_panel     = ili9325_init_panel,
    .release_panel  = ili9325_release_panel,
    .blank_panel    = ili9325_blank_panel,
    .run_panel      = ili9325_display_on,
    .stop_panel     = ili9325_display_off,
    .pan_display    = ili9325_pan_display,
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
