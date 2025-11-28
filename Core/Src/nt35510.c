#include "nt35510.h"
#include "stm32f4xx_hal.h"
#include "font.h"
#include "image.h"
#include "string.h"

SRAM_HandleTypeDef hsram4;

/**
 * @brief       NT35510寄存器初始化代码 
 * @param       无
 * @retval      无
 */
void lcd_ex_nt35510_reginit(void)
{
    lcd_write_reg(0xF000, 0x55);
    lcd_write_reg(0xF001, 0xAA);
    lcd_write_reg(0xF002, 0x52);
    lcd_write_reg(0xF003, 0x08);
    lcd_write_reg(0xF004, 0x01);
    /* AVDD ratio */
    lcd_write_reg(0xB600, 0x34);
    lcd_write_reg(0xB601, 0x34);
    lcd_write_reg(0xB602, 0x34);
    /* AVDD 5.2V */
    lcd_write_reg(0xB000, 0x0D);
    lcd_write_reg(0xB001, 0x0D);
    lcd_write_reg(0xB002, 0x0D);
     
    /* AVEE ratio */
    lcd_write_reg(0xB700, 0x24);
    lcd_write_reg(0xB701, 0x24);
    lcd_write_reg(0xB702, 0x24);
    /* AVEE -5.2V */
    lcd_write_reg(0xB100, 0x0D);
    lcd_write_reg(0xB101, 0x0D);
    lcd_write_reg(0xB102, 0x0D);

    /* VCL */
    lcd_write_reg(0xB800, 0x24);
    lcd_write_reg(0xB801, 0x24);
    lcd_write_reg(0xB802, 0x24);

    lcd_write_reg(0xB200, 0x00);

    /***************VGH ratio***************/
    lcd_write_reg(0xB900, 0x24);
    lcd_write_reg(0xB901, 0x24);
    lcd_write_reg(0xB902, 0x24);
    /* VGH 10.2V */
    lcd_write_reg(0xB300, 0x05);
    lcd_write_reg(0xB301, 0x05);
    lcd_write_reg(0xB302, 0x05);

    /***************VGLX ratio***************/
    lcd_write_reg(0xBA00, 0x34);
    lcd_write_reg(0xBA01, 0x34);
    lcd_write_reg(0xBA02, 0x34);
    /* VGL_REG -12.6V */
    lcd_write_reg(0xB500, 0x0B);
    lcd_write_reg(0xB501, 0x0B);
    lcd_write_reg(0xB502, 0x0B);

    /***************VGMP/VGSP 4.5V***********/
    lcd_write_reg(0xBC00, 0x00);
    lcd_write_reg(0xBC01, 0xA3);
    lcd_write_reg(0xBC02, 0x00);

    /* GMN/VGSN -4.5V */
    lcd_write_reg(0xBD00, 0x00);
    lcd_write_reg(0xBD01, 0xA3);
    lcd_write_reg(0xBD02, 0x00);

    /* VCOM  -1.25V */
    lcd_write_reg(0xBE00, 0x00);
    lcd_write_reg(0xBE01, 0x37);

    /***************Gamma Setting***************/
    /* 以下 Gamma 设置保持原样，省略注释 */
    lcd_write_reg(0xD100, 0x00);
    lcd_write_reg(0xD101, 0x37);
    lcd_write_reg(0xD102, 0x00);
    lcd_write_reg(0xD103, 0x53);
    lcd_write_reg(0xD104, 0x00);
    lcd_write_reg(0xD105, 0x79);
    lcd_write_reg(0xD106, 0x00);
    lcd_write_reg(0xD107, 0x97);
    lcd_write_reg(0xD108, 0x00);
    lcd_write_reg(0xD109, 0xB1);
    lcd_write_reg(0xD10A, 0x00);
    lcd_write_reg(0xD10B, 0xD5);
    lcd_write_reg(0xD10C, 0x00);
    lcd_write_reg(0xD10D, 0xF4);
    lcd_write_reg(0xD10E, 0x01);
    lcd_write_reg(0xD10F, 0x23);
    lcd_write_reg(0xD110, 0x01);
    lcd_write_reg(0xD111, 0x49);
    lcd_write_reg(0xD112, 0x01);
    lcd_write_reg(0xD113, 0x87);
    lcd_write_reg(0xD114, 0x01);
    lcd_write_reg(0xD115, 0xB6);
    lcd_write_reg(0xD116, 0x02);
    lcd_write_reg(0xD117, 0x00);
    lcd_write_reg(0xD118, 0x02);
    lcd_write_reg(0xD119, 0x3B);
    lcd_write_reg(0xD11A, 0x02);
    lcd_write_reg(0xD11B, 0x3D);
    lcd_write_reg(0xD11C, 0x02);
    lcd_write_reg(0xD11D, 0x75);
    lcd_write_reg(0xD11E, 0x02);
    lcd_write_reg(0xD11F, 0xB1);
    lcd_write_reg(0xD120, 0x02);
    lcd_write_reg(0xD121, 0xD5);
    lcd_write_reg(0xD122, 0x03);
    lcd_write_reg(0xD123, 0x09);
    lcd_write_reg(0xD124, 0x03);
    lcd_write_reg(0xD125, 0x28);
    lcd_write_reg(0xD126, 0x03);
    lcd_write_reg(0xD127, 0x52);
    lcd_write_reg(0xD128, 0x03);
    lcd_write_reg(0xD129, 0x6B);
    lcd_write_reg(0xD12A, 0x03);
    lcd_write_reg(0xD12B, 0x8D);
    lcd_write_reg(0xD12C, 0x03);
    lcd_write_reg(0xD12D, 0xA2);
    lcd_write_reg(0xD12E, 0x03);
    lcd_write_reg(0xD12F, 0xBB);
    lcd_write_reg(0xD130, 0x03);
    lcd_write_reg(0xD131, 0xC1);
    lcd_write_reg(0xD132, 0x03);
    lcd_write_reg(0xD133, 0xC1);

    lcd_write_reg(0xD200, 0x00);
    lcd_write_reg(0xD201, 0x37);
    lcd_write_reg(0xD202, 0x00);
    lcd_write_reg(0xD203, 0x53);
    lcd_write_reg(0xD204, 0x00);
    lcd_write_reg(0xD205, 0x79);
    lcd_write_reg(0xD206, 0x00);
    lcd_write_reg(0xD207, 0x97);
    lcd_write_reg(0xD208, 0x00);
    lcd_write_reg(0xD209, 0xB1);
    lcd_write_reg(0xD20A, 0x00);
    lcd_write_reg(0xD20B, 0xD5);
    lcd_write_reg(0xD20C, 0x00);
    lcd_write_reg(0xD20D, 0xF4);
    lcd_write_reg(0xD20E, 0x01);
    lcd_write_reg(0xD20F, 0x23);
    lcd_write_reg(0xD210, 0x01);
    lcd_write_reg(0xD211, 0x49);
    lcd_write_reg(0xD212, 0x01);
    lcd_write_reg(0xD213, 0x87);
    lcd_write_reg(0xD214, 0x01);
    lcd_write_reg(0xD215, 0xB6);
    lcd_write_reg(0xD216, 0x02);
    lcd_write_reg(0xD217, 0x00);
    lcd_write_reg(0xD218, 0x02);
    lcd_write_reg(0xD219, 0x3B);
    lcd_write_reg(0xD21A, 0x02);
    lcd_write_reg(0xD21B, 0x3D);
    lcd_write_reg(0xD21C, 0x02);
    lcd_write_reg(0xD21D, 0x75);
    lcd_write_reg(0xD21E, 0x02);
    lcd_write_reg(0xD21F, 0xB1);
    lcd_write_reg(0xD220, 0x02);
    lcd_write_reg(0xD221, 0xD5);
    lcd_write_reg(0xD222, 0x03);
    lcd_write_reg(0xD223, 0x09);
    lcd_write_reg(0xD224, 0x03);
    lcd_write_reg(0xD225, 0x28);
    lcd_write_reg(0xD226, 0x03);
    lcd_write_reg(0xD227, 0x52);
    lcd_write_reg(0xD228, 0x03);
    lcd_write_reg(0xD229, 0x6B);
    lcd_write_reg(0xD22A, 0x03);
    lcd_write_reg(0xD22B, 0x8D);
    lcd_write_reg(0xD22C, 0x03);
    lcd_write_reg(0xD22D, 0xA2);
    lcd_write_reg(0xD22E, 0x03);
    lcd_write_reg(0xD22F, 0xBB);
    lcd_write_reg(0xD230, 0x03);
    lcd_write_reg(0xD231, 0xC1);
    lcd_write_reg(0xD232, 0x03);
    lcd_write_reg(0xD233, 0xC1);

    lcd_write_reg(0xD300, 0x00);
    lcd_write_reg(0xD301, 0x37);
    lcd_write_reg(0xD302, 0x00);
    lcd_write_reg(0xD303, 0x53);
    lcd_write_reg(0xD304, 0x00);
    lcd_write_reg(0xD305, 0x79);
    lcd_write_reg(0xD306, 0x00);
    lcd_write_reg(0xD307, 0x97);
    lcd_write_reg(0xD308, 0x00);
    lcd_write_reg(0xD309, 0xB1);
    lcd_write_reg(0xD30A, 0x00);
    lcd_write_reg(0xD30B, 0xD5);
    lcd_write_reg(0xD30C, 0x00);
    lcd_write_reg(0xD30D, 0xF4);
    lcd_write_reg(0xD30E, 0x01);
    lcd_write_reg(0xD30F, 0x23);
    lcd_write_reg(0xD310, 0x01);
    lcd_write_reg(0xD311, 0x49);
    lcd_write_reg(0xD312, 0x01);
    lcd_write_reg(0xD313, 0x87);
    lcd_write_reg(0xD314, 0x01); 
    lcd_write_reg(0xD315, 0xB6);
    lcd_write_reg(0xD316, 0x02);
    lcd_write_reg(0xD317, 0x00);
    lcd_write_reg(0xD318, 0x02);
    lcd_write_reg(0xD319, 0x3b);
    lcd_write_reg(0xD31A, 0x02);
    lcd_write_reg(0xD31B, 0x3d);
    lcd_write_reg(0xD31C, 0x02);
    lcd_write_reg(0xD31D, 0x75);
    lcd_write_reg(0xD31E, 0x02);
    lcd_write_reg(0xD31F, 0xB1);
    lcd_write_reg(0xD320, 0x02);
    lcd_write_reg(0xD321, 0xD5);
    lcd_write_reg(0xD322, 0x03);
    lcd_write_reg(0xD323, 0x09);
    lcd_write_reg(0xD324, 0x03);
    lcd_write_reg(0xD325, 0x28);
    lcd_write_reg(0xD326, 0x03);
    lcd_write_reg(0xD327, 0x52);
    lcd_write_reg(0xD328, 0x03);
    lcd_write_reg(0xD329, 0x6B);
    lcd_write_reg(0xD32A, 0x03);
    lcd_write_reg(0xD32B, 0x8D);
    lcd_write_reg(0xD32C, 0x03);
    lcd_write_reg(0xD32D, 0xA2);
    lcd_write_reg(0xD32E, 0x03);
    lcd_write_reg(0xD32F, 0xBB);
    lcd_write_reg(0xD330, 0x03);
    lcd_write_reg(0xD331, 0xC1);
    lcd_write_reg(0xD332, 0x03);
    lcd_write_reg(0xD333, 0xC1);

    lcd_write_reg(0xD400, 0x00);
    lcd_write_reg(0xD401, 0x37);
    lcd_write_reg(0xD402, 0x00);
    lcd_write_reg(0xD403, 0x53);
    lcd_write_reg(0xD404, 0x00);
    lcd_write_reg(0xD405, 0x79);
    lcd_write_reg(0xD406, 0x00);
    lcd_write_reg(0xD407, 0x97);
    lcd_write_reg(0xD408, 0x00);
    lcd_write_reg(0xD409, 0xB1);
    lcd_write_reg(0xD40A, 0x00);
    lcd_write_reg(0xD40B, 0xD5);
    lcd_write_reg(0xD40C, 0x00);
    lcd_write_reg(0xD40D, 0xF4);
    lcd_write_reg(0xD40E, 0x01);
    lcd_write_reg(0xD40F, 0x23);
    lcd_write_reg(0xD410, 0x01);
    lcd_write_reg(0xD411, 0x49);
    lcd_write_reg(0xD412, 0x01);
    lcd_write_reg(0xD413, 0x87);
    lcd_write_reg(0xD414, 0x01);
    lcd_write_reg(0xD415, 0xB6);
    lcd_write_reg(0xD416, 0x02);
    lcd_write_reg(0xD417, 0x00);
    lcd_write_reg(0xD418, 0x02);
    lcd_write_reg(0xD419, 0x3B);
    lcd_write_reg(0xD41A, 0x02);
    lcd_write_reg(0xD41B, 0x3D);
    lcd_write_reg(0xD41C, 0x02);
    lcd_write_reg(0xD41D, 0x75);
    lcd_write_reg(0xD41E, 0x02);
    lcd_write_reg(0xD41F, 0xB1);
    lcd_write_reg(0xD420, 0x02);
    lcd_write_reg(0xD421, 0xD5);
    lcd_write_reg(0xD422, 0x03);
    lcd_write_reg(0xD423, 0x09);
    lcd_write_reg(0xD424, 0x03);
    lcd_write_reg(0xD425, 0x28);
    lcd_write_reg(0xD426, 0x03);
    lcd_write_reg(0xD427, 0x52);
    lcd_write_reg(0xD428, 0x03);
    lcd_write_reg(0xD429, 0x6B);
    lcd_write_reg(0xD42A, 0x03);
    lcd_write_reg(0xD42B, 0x8D);
    lcd_write_reg(0xD42C, 0x03);
    lcd_write_reg(0xD42D, 0xA2);
    lcd_write_reg(0xD42E, 0x03);
    lcd_write_reg(0xD42F, 0xBB);
    lcd_write_reg(0xD430, 0x03);
    lcd_write_reg(0xD431, 0xC1);
    lcd_write_reg(0xD432, 0x03);
    lcd_write_reg(0xD433, 0xC1);

    lcd_write_reg(0xD500, 0x00);
    lcd_write_reg(0xD501, 0x37);
    lcd_write_reg(0xD502, 0x00);
    lcd_write_reg(0xD503, 0x53);
    lcd_write_reg(0xD504, 0x00);
    lcd_write_reg(0xD505, 0x79);
    lcd_write_reg(0xD506, 0x00);
    lcd_write_reg(0xD507, 0x97);
    lcd_write_reg(0xD508, 0x00);
    lcd_write_reg(0xD509, 0xB1);
    lcd_write_reg(0xD50A, 0x00);
    lcd_write_reg(0xD50B, 0xD5);
    lcd_write_reg(0xD50C, 0x00);
    lcd_write_reg(0xD50D, 0xF4);
    lcd_write_reg(0xD50E, 0x01);
    lcd_write_reg(0xD50F, 0x23);
    lcd_write_reg(0xD510, 0x01);
    lcd_write_reg(0xD511, 0x49);
    lcd_write_reg(0xD512, 0x01);
    lcd_write_reg(0xD513, 0x87);
    lcd_write_reg(0xD514, 0x01);
    lcd_write_reg(0xD515, 0xB6);
    lcd_write_reg(0xD516, 0x02);
    lcd_write_reg(0xD517, 0x00);
    lcd_write_reg(0xD518, 0x02);
    lcd_write_reg(0xD519, 0x3B);
    lcd_write_reg(0xD51A, 0x02);
    lcd_write_reg(0xD51B, 0x3D);
    lcd_write_reg(0xD51C, 0x02);
    lcd_write_reg(0xD51D, 0x75);
    lcd_write_reg(0xD51E, 0x02);
    lcd_write_reg(0xD51F, 0xB1);
    lcd_write_reg(0xD520, 0x02);
    lcd_write_reg(0xD521, 0xD5);
    lcd_write_reg(0xD522, 0x03);
    lcd_write_reg(0xD523, 0x09);
    lcd_write_reg(0xD524, 0x03);
    lcd_write_reg(0xD525, 0x28);
    lcd_write_reg(0xD526, 0x03);
    lcd_write_reg(0xD527, 0x52);
    lcd_write_reg(0xD528, 0x03);
    lcd_write_reg(0xD529, 0x6B);
    lcd_write_reg(0xD52A, 0x03);
    lcd_write_reg(0xD52B, 0x8D);
    lcd_write_reg(0xD52C, 0x03);
    lcd_write_reg(0xD52D, 0xA2);
    lcd_write_reg(0xD52E, 0x03);
    lcd_write_reg(0xD52F, 0xBB);
    lcd_write_reg(0xD530, 0x03);
    lcd_write_reg(0xD531, 0xC1);
    lcd_write_reg(0xD532, 0x03);
    lcd_write_reg(0xD533, 0xC1);

    lcd_write_reg(0xD600, 0x00);
    lcd_write_reg(0xD601, 0x37);
    lcd_write_reg(0xD602, 0x00);
    lcd_write_reg(0xD603, 0x53);
    lcd_write_reg(0xD604, 0x00);
    lcd_write_reg(0xD605, 0x79);
    lcd_write_reg(0xD606, 0x00);
    lcd_write_reg(0xD607, 0x97);
    lcd_write_reg(0xD608, 0x00);
    lcd_write_reg(0xD609, 0xB1);
    lcd_write_reg(0xD60A, 0x00);
    lcd_write_reg(0xD60B, 0xD5);
    lcd_write_reg(0xD60C, 0x00);
    lcd_write_reg(0xD60D, 0xF4);
    lcd_write_reg(0xD60E, 0x01);
    lcd_write_reg(0xD60F, 0x23);
    lcd_write_reg(0xD610, 0x01);
    lcd_write_reg(0xD611, 0x49);
    lcd_write_reg(0xD612, 0x01);
    lcd_write_reg(0xD613, 0x87);
    lcd_write_reg(0xD614, 0x01);
    lcd_write_reg(0xD615, 0xB6);
    lcd_write_reg(0xD616, 0x02);
    lcd_write_reg(0xD617, 0x00);
    lcd_write_reg(0xD618, 0x02);
    lcd_write_reg(0xD619, 0x3B);
    lcd_write_reg(0xD61A, 0x02);
    lcd_write_reg(0xD61B, 0x3D);
    lcd_write_reg(0xD61C, 0x02);
    lcd_write_reg(0xD61D, 0x75);
    lcd_write_reg(0xD61E, 0x02);
    lcd_write_reg(0xD61F, 0xB1);
    lcd_write_reg(0xD620, 0x02);
    lcd_write_reg(0xD621, 0xD5);
    lcd_write_reg(0xD622, 0x03);
    lcd_write_reg(0xD623, 0x09);
    lcd_write_reg(0xD624, 0x03);
    lcd_write_reg(0xD625, 0x28);
    lcd_write_reg(0xD626, 0x03);
    lcd_write_reg(0xD627, 0x52);
    lcd_write_reg(0xD628, 0x03);
    lcd_write_reg(0xD629, 0x6B);
    lcd_write_reg(0xD62A, 0x03);
    lcd_write_reg(0xD62B, 0x8D);
    lcd_write_reg(0xD62C, 0x03);
    lcd_write_reg(0xD62D, 0xA2);
    lcd_write_reg(0xD62E, 0x03);
    lcd_write_reg(0xD62F, 0xBB);
    lcd_write_reg(0xD630, 0x03);
    lcd_write_reg(0xD631, 0xC1);
    lcd_write_reg(0xD632, 0x03);
    lcd_write_reg(0xD633, 0xC1);

    /***************LV2 Page 0 enable***************/
    lcd_write_reg(0xF000, 0x55);
    lcd_write_reg(0xF001, 0xAA);
    lcd_write_reg(0xF002, 0x52);
    lcd_write_reg(0xF003, 0x08);
    lcd_write_reg(0xF004, 0x00);

    /***************480x800***************/
    //lcd_write_reg(0xB500, 0x50);
    //RGB Internal mode
    //REGW 0xB30001
    //RGB I/F Setting

    lcd_write_reg(0xB000, 0x00); 
    lcd_write_reg(0xB001, 0x05); 
    lcd_write_reg(0xB002, 0x02);
    lcd_write_reg(0xB003, 0x05); 
    lcd_write_reg(0xB004, 0x02);

    /* SDT */
    lcd_write_reg(0xB600, 0x08);
    lcd_write_reg(0xB500, 0x50); /* 0x50 480x800 */

    /***************Source hold time***************/
    lcd_write_reg(0xB600, 0x05);

    /***************Gate EQ control***************/
    lcd_write_reg(0xB700, 0x00);
    lcd_write_reg(0xB701, 0x00);

    /***************Source EQ control (Mode 2)***************/
    lcd_write_reg(0xB800, 0x01);
    lcd_write_reg(0xB801, 0x05);
    lcd_write_reg(0xB802, 0x05);
    lcd_write_reg(0xB803, 0x05);
                    
    /***************Inversion mode  (2-dot)***************/
    lcd_write_reg(0xBC00, 0x02);
    lcd_write_reg(0xBC01, 0x00);
    lcd_write_reg(0xBC02, 0x00);
                     
    /*************** BOE SETTING(DEFAULT) ***************/
    lcd_write_reg(0xCC00, 0x03);
    lcd_write_reg(0xCC01, 0x00);
    lcd_write_reg(0xCC02, 0x00);

    /***************Frame rate***************/
    lcd_write_reg(0xBD00, 0x01);
    lcd_write_reg(0xBD01, 0x84);
    lcd_write_reg(0xBD02, 0x07);
    lcd_write_reg(0xBD03, 0x31);
    lcd_write_reg(0xBD04, 0x00);

    lcd_write_reg(0xBA00, 0x01);

    lcd_write_reg(0xFF00, 0xAA);
    lcd_write_reg(0xFF01, 0x55);
    lcd_write_reg(0xFF02, 0x25);
    lcd_write_reg(0xFF03, 0x01);

    /*************** Timing control 4H w/ 4-Delayms ***************/
    lcd_write_reg(0x3600, 0x40);
    lcd_write_reg(0x3500, 0x00);
    lcd_write_reg(0x3A00, 0x05);

    lcd_wr_regno(0x1100);
    HAL_Delay(120);
    lcd_wr_regno(0x2900);
    HAL_Delay(10);
    lcd_wr_regno(0x2C00);
}

/* LCD的画笔颜色和背景色 */
uint32_t g_point_color = 0xF800;    /* 画笔颜色 */
uint32_t g_back_color  = GRAYBLUE;    /* 背景色 */

/* 管理LCD重要参数 */
_lcd_dev lcddev;

/*------------------- 底层读写函数 -------------------*/

void lcd_wr_data(volatile uint16_t data)
{
    data = data;            /* 防止 -O2 被优化 */
    LCD->LCD_RAM = data;
}

void lcd_wr_regno(volatile uint16_t regno)
{
    regno = regno;          /* 防止 -O2 被优化 */
    LCD->LCD_REG = regno;
}

void lcd_write_reg(uint16_t regno, uint16_t data)
{
    LCD->LCD_REG = regno;
    LCD->LCD_RAM = data;
}

static void lcd_opt_delay(uint32_t i)
{
    while (i--);
}

static uint16_t lcd_rd_data(void)
{
    volatile uint16_t ram;
    lcd_opt_delay(2);
    ram = LCD->LCD_RAM;
    return ram;
}

void lcd_write_ram_prepare(void)
{
    LCD->LCD_REG = lcddev.wramcmd;
}

/*------------------- 公共功能函数（已去掉其它IC分支） -------------------*/

uint32_t lcd_read_point(uint16_t x, uint16_t y)
{
    uint16_t r = 0, g = 0, b = 0;

    if (x >= lcddev.width || y >= lcddev.height)
    {
        return 0;
    }

    lcd_set_cursor(x, y);

    /* NT35510 发送读GRAM指令 */
    lcd_wr_regno(0x2E00);

    r = lcd_rd_data();      /* dummy read */
    r = lcd_rd_data();      /* 实际颜色高字节(RG) */
    b = lcd_rd_data();      /* B */

    g = r & 0xFF;
    g <<= 8;

    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11));
}

void lcd_display_on(void)
{
    lcd_wr_regno(0x2900);
}

void lcd_display_off(void)
{
    lcd_wr_regno(0x2800);
}

void lcd_set_cursor(uint16_t x, uint16_t y)
{
    /* 仅保留 NT35510 寄存器写法 */
    lcd_wr_regno(lcddev.setxcmd);
    lcd_wr_data(x >> 8);
    lcd_wr_regno(lcddev.setxcmd + 1);
    lcd_wr_data(x & 0xFF);

    lcd_wr_regno(lcddev.setycmd);
    lcd_wr_data(y >> 8);
    lcd_wr_regno(lcddev.setycmd + 1);
    lcd_wr_data(y & 0xFF);
}

/**
 * @brief       设置LCD扫描方向（只针对 NT35510）
 * @param       dir:0~7
 */
void lcd_scan_dir(uint8_t dir)
{
    uint16_t regval = 0;
    uint16_t temp;

    /* 根据扫描方式 设置 0x3600 寄存器 bit 5,6,7 位的值 */
    switch (dir)
    {
        case L2R_U2D:
            regval |= (0 << 7) | (0 << 6) | (0 << 5);
            break;
        case L2R_D2U:
            regval |= (1 << 7) | (0 << 6) | (0 << 5);
            break;
        case R2L_U2D:
            regval |= (0 << 7) | (1 << 6) | (0 << 5);
            break;
        case R2L_D2U:
            regval |= (1 << 7) | (1 << 6) | (0 << 5);
            break;
        case U2D_L2R:
            regval |= (0 << 7) | (0 << 6) | (1 << 5);
            break;
        case U2D_R2L:
            regval |= (0 << 7) | (1 << 6) | (1 << 5);
            break;
        case D2U_L2R:
            regval |= (1 << 7) | (0 << 6) | (1 << 5);
            break;
        case D2U_R2L:
            regval |= (1 << 7) | (1 << 6) | (1 << 5);
            break;
    }

    /* NT35510: 用 0x3600 */
    lcd_write_reg(0x3600, regval);

    /* NT35510 需要交换宽高的情况 */
    if (regval & 0x20)
    {
        if (lcddev.width < lcddev.height)
        {
            temp = lcddev.width;
            lcddev.width = lcddev.height;
            lcddev.height = temp;
        }
    }
    else
    {
        if (lcddev.width > lcddev.height)
        {
            temp = lcddev.width;
            lcddev.width = lcddev.height;
            lcddev.height = temp;
        }
    }

    /* 设置 NT35510 显示窗口为全屏 */
    lcd_wr_regno(lcddev.setxcmd);
    lcd_wr_data(0);
    lcd_wr_regno(lcddev.setxcmd + 1);
    lcd_wr_data(0);
    lcd_wr_regno(lcddev.setxcmd + 2);
    lcd_wr_data((lcddev.width - 1) >> 8);
    lcd_wr_regno(lcddev.setxcmd + 3);
    lcd_wr_data((lcddev.width - 1) & 0xFF);

    lcd_wr_regno(lcddev.setycmd);
    lcd_wr_data(0);
    lcd_wr_regno(lcddev.setycmd + 1);
    lcd_wr_data(0);
    lcd_wr_regno(lcddev.setycmd + 2);
    lcd_wr_data((lcddev.height - 1) >> 8);
    lcd_wr_regno(lcddev.setycmd + 3);
    lcd_wr_data((lcddev.height - 1) & 0xFF);
}

void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color)
{
    lcd_set_cursor(x, y);
    lcd_write_ram_prepare();
    LCD->LCD_RAM = color;
}

/* SSD1963 背光函数对 NT35510 实际无效，但若你的背光也是PWM引脚可保留或改成 GPIO 控制
   如果完全不需要，可删除这个函数以及外部对它的调用 */
void lcd_ssd_backlight_set(uint8_t pwm)
{
    (void)pwm;
}

/**
 * @brief   设置LCD显示方向 (仅 NT35510)
 * @param   dir:0竖屏;1横屏
 */
void lcd_display_dir(uint8_t dir)
{
    lcddev.dir = dir;

    if (dir == 0)   /* 竖屏 480 x 800 */
    {
        lcddev.wramcmd = 0x2C00;
        lcddev.setxcmd = 0x2A00;
        lcddev.setycmd = 0x2B00;
        lcddev.width  = 480;
        lcddev.height = 800;
    }
    else            /* 横屏 800 x 480 */
    {
        lcddev.wramcmd = 0x2C00;
        lcddev.setxcmd = 0x2A00;
        lcddev.setycmd = 0x2B00;
        lcddev.width  = 800;
        lcddev.height = 480;
    }

    lcd_scan_dir(DFT_SCAN_DIR);
}

/**
 * @brief   设置窗口并将画点位置移动到(sx,sy)
 */
void lcd_set_window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    uint16_t twidth, theight;
    twidth = sx + width - 1;
    theight = sy + height - 1;

    /* NT35510 写法 */
    lcd_wr_regno(lcddev.setxcmd);
    lcd_wr_data(sx >> 8);
    lcd_wr_regno(lcddev.setxcmd + 1);
    lcd_wr_data(sx & 0xFF);
    lcd_wr_regno(lcddev.setxcmd + 2);
    lcd_wr_data(twidth >> 8);
    lcd_wr_regno(lcddev.setxcmd + 3);
    lcd_wr_data(twidth & 0xFF);

    lcd_wr_regno(lcddev.setycmd);
    lcd_wr_data(sy >> 8);
    lcd_wr_regno(lcddev.setycmd + 1);
    lcd_wr_data(sy & 0xFF);
    lcd_wr_regno(lcddev.setycmd + 2);
    lcd_wr_data(theight >> 8);
    lcd_wr_regno(lcddev.setycmd + 3);
    lcd_wr_data(theight & 0xFF);
}

/*------------------- FSMC/LCD 初始化 -------------------*/

void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
{
    GPIO_InitTypeDef gpio_init_struct;

    __HAL_RCC_FSMC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    /* PD0,1,8,9,10,14,15 */
    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 \
                           | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_struct.Alternate = GPIO_AF12_FSMC;
    HAL_GPIO_Init(GPIOD, &gpio_init_struct);

    /* PE7~15 */
    gpio_init_struct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 \
                           | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &gpio_init_struct);
}

void lcd_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    FSMC_NORSRAM_TimingTypeDef fsmc_read_handle;
    FSMC_NORSRAM_TimingTypeDef fsmc_write_handle;

    LCD_CS_GPIO_CLK_ENABLE();
    LCD_WR_GPIO_CLK_ENABLE();
    LCD_RD_GPIO_CLK_ENABLE();
    LCD_RS_GPIO_CLK_ENABLE();
    LCD_BL_GPIO_CLK_ENABLE();
    
    gpio_init_struct.Pin = LCD_CS_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_struct.Alternate = GPIO_AF12_FSMC;
    HAL_GPIO_Init(LCD_CS_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = LCD_WR_GPIO_PIN;
    HAL_GPIO_Init(LCD_WR_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = LCD_RD_GPIO_PIN;
    HAL_GPIO_Init(LCD_RD_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = LCD_RS_GPIO_PIN;
    HAL_GPIO_Init(LCD_RS_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = LCD_BL_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(LCD_BL_GPIO_PORT, &gpio_init_struct);

    hsram4.Instance = FSMC_NORSRAM_DEVICE;
    hsram4.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
    
    hsram4.Init.NSBank = FSMC_NORSRAM_BANK4;
    hsram4.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
    hsram4.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
    hsram4.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
    hsram4.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
    hsram4.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
    hsram4.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
    hsram4.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
    hsram4.Init.ExtendedMode = FSMC_EXTENDED_MODE_ENABLE;
    hsram4.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
    hsram4.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
    
    /* 读时序 */
    fsmc_read_handle.AddressSetupTime = 0x0F;
    fsmc_read_handle.AddressHoldTime = 0x00;
    fsmc_read_handle.DataSetupTime = 60;
    fsmc_read_handle.AccessMode = FSMC_ACCESS_MODE_A;
    
    /* 写时序（初始安全时序） */
    fsmc_write_handle.AddressSetupTime = 9;
    fsmc_write_handle.AddressHoldTime = 0x00;
    fsmc_write_handle.DataSetupTime = 9;
    fsmc_write_handle.AccessMode = FSMC_ACCESS_MODE_A;
    
    HAL_SRAM_Init(&hsram4, &fsmc_read_handle, &fsmc_write_handle);
    HAL_Delay(50);

    /* 直接固定 ID 为 NT35510 */
    lcddev.id = 0x5510;

    /* 执行 NT35510 初始化 */
    lcd_ex_nt35510_reginit();

    /* 初始化完成后，加速写时序：NT35510 使用与 9341/9806 相同的较快参数 */
    fsmc_write_handle.AddressSetupTime = 2;
    fsmc_write_handle.DataSetupTime = 2;
    FSMC_NORSRAM_Extended_Timing_Init(hsram4.Extended,
                                      &fsmc_write_handle,
                                      hsram4.Init.NSBank,
                                      hsram4.Init.ExtendedMode);

    lcd_display_dir(0); /* 默认为竖屏 480x800 */
    LCD_BL(1);          /* 点亮背光（假定宏里是设置 GPIO） */
    lcd_clear(WHITE);
}

/*------------------- 图形/文本 公共接口（未改动逻辑，仅使用上面的底层接口） -------------------*/

void lcd_clear(uint16_t color)
{
    uint32_t index = 0;
    uint32_t totalpoint = lcddev.width;

    totalpoint *= lcddev.height;
    lcd_set_cursor(0x00, 0x0000);
    lcd_write_ram_prepare();

    for (index = 0; index < totalpoint; index++)
    {
        LCD->LCD_RAM = color;
    }
    g_back_color = color;
}

void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)
{
    uint16_t i, j;
    uint16_t xlen = 0;
    xlen = ex - sx + 1;

    for (i = sy; i <= ey; i++)
    {
        lcd_set_cursor(sx, i);
        lcd_write_ram_prepare();

        for (j = 0; j < xlen; j++)
        {
            LCD->LCD_RAM = color;
        }
    }
}

void lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint16_t height, width;
    uint16_t i, j;

    width = ex - sx + 1;
    height = ey - sy + 1;

    for (i = 0; i < height; i++)
    {
        lcd_set_cursor(sx, sy + i);
        lcd_write_ram_prepare();

        for (j = 0; j < width; j++)
        {
            LCD->LCD_RAM = color[i * width + j];
        }
    }
}

void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;
    delta_x = x2 - x1;
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0) incx = 1;
    else if (delta_x == 0) incx = 0;
    else { incx = -1; delta_x = -delta_x; }

    if (delta_y > 0) incy = 1;
    else if (delta_y == 0) incy = 0;
    else { incy = -1; delta_y = -delta_y; }

    if ( delta_x > delta_y) distance = delta_x;
    else distance = delta_y;

    for (t = 0; t <= distance + 1; t++)
    {
        lcd_draw_point(row, col, color);
        xerr += delta_x;
        yerr += delta_y;

        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color)
{
    if ((len == 0) || (x > lcddev.width) || (y > lcddev.height))
    {
        return;
    }

    lcd_fill(x, y, x + len - 1, y, color);
}

void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t i = 3;
    while (i--)
    {
        lcd_draw_line(x1, y1 + i, x2, y1 + i, color);
        lcd_draw_line(x1 + i, y1, x1 + i, y2, color);
        lcd_draw_line(x1, y2 - i, x2, y2 - i, color);
        lcd_draw_line(x2 - i, y1, x2 - i, y2, color);
    }
    
   
}

void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int a, b;
    int di;

    a = 0;
    b = r;
    di = 3 - (r << 1);

    while (a <= b)
    {
        lcd_draw_point(x0 + a, y0 - b, color);
        lcd_draw_point(x0 + b, y0 - a, color);
        lcd_draw_point(x0 + b, y0 + a, color);
        lcd_draw_point(x0 + a, y0 + b, color);
        lcd_draw_point(x0 - a, y0 + b, color);
        lcd_draw_point(x0 - b, y0 + a, color);
        lcd_draw_point(x0 - a, y0 - b, color);
        lcd_draw_point(x0 - b, y0 - a, color);
        a++;

        if (di < 0)
        {
            di += 4 * a + 6;
        }
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

void lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
    uint32_t i;
    uint32_t imax = ((uint32_t)r * 707) / 1000 + 1;
    uint32_t sqmax = (uint32_t)r * (uint32_t)r + (uint32_t)r / 2;
    uint32_t xr = r;

    lcd_draw_hline(x - r, y, 2 * r, color);

    for (i = 1; i <= imax; i++)
    {
        if ((i * i + xr * xr) > sqmax)
        {
            if (xr > imax)
            {
                lcd_draw_hline (x - i + 1, y + xr, 2 * (i - 1), color);
                lcd_draw_hline (x - i + 1, y - xr, 2 * (i - 1), color);
            }

            xr--;
        }

        lcd_draw_hline(x - xr, y + i, 2 * xr, color);
        lcd_draw_hline(x - xr, y - i, 2 * xr, color);
    }
}

/*------------------- 字符/数字显示 -------------------*/

void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = 0;
    uint8_t *pfont = 0;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
    chr = chr - ' ';

    switch (size)
    {
        case 12:
            pfont = (uint8_t *)asc2_1206[chr];
            break;
        case 16:
            pfont = (uint8_t *)asc2_1608[chr];
            break;
        case 24:
            pfont = (uint8_t *)asc2_2412[chr];
            break;
        case 32:
            pfont = (uint8_t *)asc2_3216[chr];
            break;
        default:
            return ;
    }

    for (t = 0; t < csize; t++)
    {
        temp = pfont[t];

        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)
            {
                lcd_draw_point(x, y, color);
            }
            else if (mode == 0)
            {
                lcd_draw_point(x, y, g_back_color);
            }

            temp <<= 1;
            y++;

            if (y >= lcddev.height) return;

            if ((y - y0) == size)
            {
                y = y0;
                x++;

                if (x >= lcddev.width)
                {
                    return;
                }

                break;
            }
        }
    }
}

static uint32_t lcd_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)
    {
        result *= m;
    }

    return result;
}

void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                lcd_show_char(x + (size / 2) * t, y, ' ', size, 0, color);
                continue;
            }
            else
            {
                enshow = 1;
            }
        }

        lcd_show_char(x + (size / 2) * t, y, temp + '0', size, 0, color);
    }
}

void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                if (mode & 0x80)
                {
                    lcd_show_char(x + (size / 2) * t, y, '0', size, mode & 0x01, color);
                }
                else
                {
                    lcd_show_char(x + (size / 2) * t, y, ' ', size, mode & 0x01, color);
                }

                continue;
            }
            else
            {
                enshow = 1;
            }
        }

        lcd_show_char(x + (size / 2) * t, y, temp + '0', size, mode & 0x01, color);
    }
}

void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color)
{
    uint8_t x0 = x;
    
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' '))
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)
        {
            break;
        }

        lcd_show_char(x, y, *p, size, 1, color);
        x += size / 2;
        p++;
    }
}

uint16_t RGB565_color(uint16_t R, uint16_t G, uint8_t B)
{
    uint16_t color = 0;

    if(R > 0x1F)    R = 0x1F;
    if(G > 0x3F)    G = 0x3F;
    if(B > 0x1F)    B = 0x1F;
    color = R << 11 | G << 5 | B;

    return color;
}


/**
 * @brief 根据图片名称在 image.h 中匹配数组并显示
 */
void LCD_ShowImage(uint16_t x, uint16_t y,
                         uint16_t w, uint16_t h,
                         const char *name)
{
    if (name == NULL) return;

    const uint8_t *data = NULL;

    /* ===== 在这里按名称匹配你在 image.h 中声明的数组 ===== */
    if (strcmp(name, "logo_cdtu") == 0)
    {
        /* logo_cdtu 长度 21700 = 310 * 70，正好一张 310x70 图 */
        data = (const uint8_t *)logo_cdtu;
        /* 如果调用时 w,h 不等于 310x70，也按你传入的 w,h 来裁剪/拉伸，
           所以这里不强制修改 w,h，只是说明一下原始尺寸。*/
    }
    else
    {
        /* 未找到该名称的图像，直接返回 */
        return;
    }

    /* 通过通用核心函数显示 */
      if (data == NULL) return;

    uint32_t idx = 0;

    for (uint16_t j = 0; j < h; j++)
    {
        for (uint16_t i = 0; i < w; i++)
        {
            uint8_t g = data[idx++];   // 0~255 灰度
            
            // // 灰度映射到 RGB565（R5 G6 B5）
            // uint16_t r  = (g >> 3) & 0x1F;   // 5 bit
            // uint16_t g6 = (g >> 2) & 0x3F;   // 6 bit
            // uint16_t b  = (g >> 3) & 0x1F;   // 5 bit
            // uint16_t color = (r << 11) | (g6 << 5) | b;

            lcd_draw_point(x + i, y + j, g);
        }
    }
}



