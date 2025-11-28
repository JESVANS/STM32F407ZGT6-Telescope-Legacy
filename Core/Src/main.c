/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dcmi.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "string.h"
#include "nt35510.h"
#include "touch.h"
#include "sht30.h"
#include "bh1750.h"
#include "bmp280.h"
#include "hc_sr04.h"
#include "ssd1306.h"
#include "bluetooth.h"
#include "key.h"
#include "dl_ln33.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define DL_LOCAL_PORT 0x91  /* 本端默认源端口 (与注释示例一致) */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

  float T = 0;
  float H = 0;
  float L = 0;
  float P = 0;
  float A = 0;
  float D = 0;





  static volatile uint8_t  bt_rx_byte;      /* 单字节中断接收缓冲 */
  static volatile uint16_t resp_len = 0;
  static volatile uint8_t  resp_updated = 0;
  const char *msg = "HELLO FROM STM32 !";
  const uint8_t pkg[128] = {0};
  const uint8_t content[] = {123};


  char resp[128];
  
  static char key_evt_buf[32] = {0};

/* 用来显示串口消息的缓冲区 */
extern char disp_buf[128];             // 屏幕显示用的缓冲
extern uint8_t bt_line_idx;               // 当前行缓冲索引
extern uint8_t usart2_has_new;                 // 标记是否有新消息待显示

enum Page
{
  showdata = 1,
  network,
  draw,
  home,
}; 
enum Page current_page = home;
enum Operation
{
  n_opt = 0,
  yes,
  up,
  down,
  left,
  right,
};
enum Operation current_opt = n_opt;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */



// 原来的笑脸函数保留
void draw_smile(uint16_t f_color, uint16_t b_color)
{
  lcd_clear(b_color);
  /* 画一个蓝色线框
  笑脸（加宽线条，眼睛实心，背景青色） */
  uint16_t cx = 240;   /* 屏幕中心X */
  uint16_t cy = 400;   /* 屏幕中心Y */
  uint16_t r  = 80;    /* 脸的半径 */

  /* 脸轮廓：用三个同心圆加粗线条 */
  lcd_draw_circle(cx, cy, r - 1, f_color);
  lcd_draw_circle(cx, cy, r,     f_color);
  lcd_draw_circle(cx, cy, r + 1, f_color);

  /* 眼睛：两个实心圆（加一点半径，看起来更粗） */
  uint16_t eye_r = 10;
  lcd_fill_circle(cx - 25, cy - 20, eye_r, f_color);  /* 左眼 */
  lcd_fill_circle(cx + 25, cy - 20, eye_r, f_color);  /* 右眼 */

  /* 嘴巴：反向抛物线，中间最低，两边高；用加粗的点模拟线条 */
  uint16_t x;
  for (x = cx - 35; x <= cx + 35; x++)
  {
    int16_t dx = x - cx;
    /* 反向抛物线：y = cy + 20 - k * dx^2  （k 越大弯曲越明显）*/
    float k = 0.01f;
    int16_t y = cy + 40 - (int16_t)(k * dx * dx);

    /* 加粗嘴巴线条：竖直方向多点 */
    lcd_draw_point(x, y - 1, f_color);
    lcd_draw_point(x, y,     f_color);
    lcd_draw_point(x, y + 1, f_color);
  }
}

void load_draw_dialog(void)
{
    lcd_clear(RGB565_color(0, 63, 0));                                                /* 清屏 */
    lcd_show_string(lcddev.width - 24, 0, 200, 16, 16, "RST", BLUE); /* 显示清屏区域 */
}

/**
 * @brief       画粗线
 * @param       x1,y1: 起点坐标
 * @param       x2,y2: 终点坐标
 * @param       size : 线条粗细程度
 * @param       color: 线的颜色
 * @retval      无
 */
void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;

    if (x1 < size || x2 < size || y1 < size || y2 < size)
        return;

    delta_x = x2 - x1; /* 计算坐标增量 */
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0)
    {
        incx = 1; /* 设置单步方向 */
    }
    else if (delta_x == 0)
    {
        incx = 0; /* 垂直线 */
    }
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)
    {
        incy = 1;
    }
    else if (delta_y == 0)
    {
        incy = 0; /* 水平线 */
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if (delta_x > delta_y)
        distance = delta_x; /* 选取基本增量坐标轴 */
    else
        distance = delta_y;

    for (t = 0; t <= distance + 1; t++) /* 画线输出 */
    {
        lcd_fill_circle(row, col, size, color); /* 画点 */
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

/**
 * @brief 在指定位置画一个“返回”图标：矩形 + 向左箭头
 * @param x,y  图标左上角坐标
 * @param w,h  图标宽高
 */
void lcd_draw_back_icon(uint16_t x, uint16_t y,
                        uint16_t w, uint16_t h)
{
    if (w < 10 || h < 10) return;

    uint16_t border_color = WHITE;                   // 边框颜色
    uint16_t fill_color   = RGB565_color(10,10,10);  // 背景填充色（深灰）
    uint16_t arrow_color  = YELLOW;                 // 箭头颜色

    /* 1. 画外框矩形（边框） */
    lcd_draw_rectangle(x, y, x + w, y + h, border_color);

    /* 2. 内部填充 */
    uint16_t inner_left   = x + 2;
    uint16_t inner_top    = y + 2;
    uint16_t inner_right  = x + w - 2;
    uint16_t inner_bottom = y + h - 2;
    lcd_fill(inner_left, inner_top, inner_right, inner_bottom, fill_color);

    /* 3. 画向左箭头：左边竖线 + 右侧实心三角形 */

    // 3.1 竖直线（箭杆），居中放在内矩形的 1/3 处
    uint16_t mid_y   = (inner_top + inner_bottom) / 2;
    uint16_t line_x  = inner_left + (w / 6);  // 离左边一点
    uint16_t line_top    = inner_top  + h / 6;
    uint16_t line_bottom = inner_bottom - h / 6;

    for (uint16_t yy = line_top; yy <= line_bottom; yy++) {
        lcd_draw_point(line_x, yy, arrow_color);
        lcd_draw_point(line_x + 1, yy, arrow_color);   // 稍微加粗
    }

    // 3.2 实心三角箭头：尖端在左，底边在中间偏右
    uint16_t tri_tip_x = inner_left + w / 6;           // 和竖线差不多对齐
    uint16_t tri_tip_y = mid_y;

    uint16_t tri_base_x = inner_right - w / 8;         // 底边靠右
    uint16_t tri_half_h = h / 4;                       // 半高

    uint16_t tri_base_y1 = mid_y - tri_half_h;
    uint16_t tri_base_y2 = mid_y + tri_half_h;

    /* 用扫描线填充三角形：从尖端到底边，逐列画竖线 */
    for (uint16_t xx = tri_tip_x; xx <= tri_base_x; xx++) {
        // t 表示当前列在 [尖端, 底边] 的插值位置
        float t = 0.0f;
        if (tri_base_x != tri_tip_x) {
            t = (float)(xx - tri_tip_x) / (float)(tri_base_x - tri_tip_x);
        }

        // 当前列对应的上下边界（线性插值）
        uint16_t cur_y1 = (uint16_t)(tri_tip_y  + t * (tri_base_y1 - tri_tip_y));
        uint16_t cur_y2 = (uint16_t)(tri_tip_y  + t * (tri_base_y2 - tri_tip_y));

        if (cur_y1 > cur_y2) {
            uint16_t tmp = cur_y1;
            cur_y1 = cur_y2;
            cur_y2 = tmp;
        }

        if (cur_y1 < inner_top)    cur_y1 = inner_top;
        if (cur_y2 > inner_bottom) cur_y2 = inner_bottom;

        for (uint16_t yy = cur_y1; yy <= cur_y2; yy++) {
            lcd_draw_point(xx, yy, arrow_color);
        }
    }
}

/**
 * @brief 在指定位置画一个“刷新”图标：矩形 + 循环箭头
 * @param x,y  图标左上角坐标
 * @param w,h  图标宽高
 */
void lcd_draw_refresh_icon(uint16_t x, uint16_t y,
                           uint16_t w, uint16_t h)
{
    if (w < 16 || h < 16) return;

    uint16_t border_color = RED;                   // 边框颜色
    uint16_t fill_color   = RGB565_color(10,10,10);  // 深灰背景
    uint16_t arrow_color  = YELLOW;                  // 刷新箭头颜色

    /* 1. 外框矩形 */
    lcd_draw_rectangle(x, y, x + w, y + h, border_color);

    /* 2. 内部填充 */
    uint16_t inner_left   = x + 2;
    uint16_t inner_top    = y + 2;
    uint16_t inner_right  = x + w - 2;
    uint16_t inner_bottom = y + h - 2;
    lcd_fill(inner_left, inner_top, inner_right, inner_bottom, fill_color);

    /* 3. 计算圆心和半径（适配宽高） */
    uint16_t cx = (inner_left + inner_right) / 2;
    uint16_t cy = (inner_top + inner_bottom) / 2;
    uint16_t r  = ( (inner_right - inner_left) < (inner_bottom - inner_top)
    ? (inner_right - inner_left)
    : (inner_bottom - inner_top) ) / 2 - 2;
    //if ((int)r <= 2) return; 
    r -= 5;
    

    /* 3.1 画“圆”*/
    for (uint16_t i = r; i > r - 3; i--)
    {
      
      lcd_draw_circle(cx, cy, i, YELLOW);
    }
    lcd_fill(inner_left, inner_top + h / 2, inner_right - w / 2, inner_bottom, fill_color);
    

    /* 3.2 在右上断口处画一个小三角箭头，方向顺时针 */

    // 选一个靠近右上的参考点
    uint16_t ax = cx;
    uint16_t ay = cy + r;

    uint16_t ah = (h / 5);    // 箭头高度
    if (ah < 4) ah = 4;

    // 三角形三个点：尖端朝圆心，底边朝外
    int16_t tip_x = ax - ah;          // 尖端靠内
    int16_t tip_y = ay;
    int16_t b1_x  = ax;
    int16_t b1_y  = ay - ah / 2;
    int16_t b2_x  = ax;
    int16_t b2_y  = ay + ah / 2;

    for (int16_t xx = tip_x; xx <= b1_x; ++xx)
    {
        float t = (float)(xx - tip_x) / (float)(b1_x - tip_x + 1);
        int16_t y_top = (int16_t)(tip_y + t * (b1_y - tip_y));
        int16_t y_bot = (int16_t)(tip_y + t * (b2_y - tip_y));
        if (y_top > y_bot) {
            int16_t tmp = y_top; y_top = y_bot; y_bot = tmp;
        }

        for (int16_t yy = y_top; yy <= y_bot; ++yy) {
            if (xx >= inner_left && xx <= inner_right &&
                yy >= inner_top  && yy <= inner_bottom) {
                lcd_draw_point(xx, yy, arrow_color);
            }
        }
    }
}



/* 10个触控点的颜色(电容触摸屏用) */
const uint16_t POINT_COLOR_TBL[10] = {RED, GREEN, BLUE, BROWN, YELLOW, MAGENTA, CYAN, LIGHTBLUE, BRRED, GRAY};

/**
 * @brief       电容触摸屏测试函数
 * @param       无
 * @retval      无
 */
void ctp_test(void)
{
    uint8_t t = 0;
    uint8_t i = 0;
    uint16_t lastpos[10][2];        /* 最后一次的数据 */
    uint8_t maxp = 5;

    if (lcddev.id == 0x1018)maxp = 10;

    while (1)
    {
        tp_dev.scan(0);

        for (t = 0; t < maxp; t++)
        {
            if ((tp_dev.sta) & (1 << t))
            {
                if (tp_dev.x[t] < lcddev.width && tp_dev.y[t] < lcddev.height)  /* 坐标在屏幕范围内 */
                {
                    if (lastpos[t][0] == 0xFFFF)
                    {
                        lastpos[t][0] = tp_dev.x[t];
                        lastpos[t][1] = tp_dev.y[t];
                    }

                    lcd_draw_bline(lastpos[t][0], lastpos[t][1], tp_dev.x[t], tp_dev.y[t], 2, POINT_COLOR_TBL[t]); /* 画线 */
                    lastpos[t][0] = tp_dev.x[t];
                    lastpos[t][1] = tp_dev.y[t];

                    if (tp_dev.x[t] > (lcddev.width - 24) && tp_dev.y[t] < 20)
                    {
                        load_draw_dialog();/* 清除 */
                    }
                }
            }
            else 
            {
                lastpos[t][0] = 0xFFFF;
            }
        }

       HAL_Delay(5);
        i++;
        if (i % 20 == 0) HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0); /* 每100ms翻转一次LED */
    }
}


/* 将按键与事件类型转成字符串 */
static const char* key_id_to_str(KeyID_t id)
{
    switch(id){
      case KEY_WK_UP: return "WK_UP";
      case KEY_UP:    return "UP";
      case KEY_DOWN:  return "DOWN";
      case KEY_LEFT:  return "LEFT";
      case KEY_RIGHT: return "RIGHT";
      case KEY_OK:    return "OK";
      default:        return "NONE";
    }
}
static const char* key_type_to_str(KeyEventType_t t)
{
    switch(t){
      case KEY_EVT_PRESS:        return "PRESS";
      case KEY_EVT_RELEASE:      return "RELEASE";
      case KEY_EVT_CLICK:        return "CLICK";
      case KEY_EVT_DOUBLE_CLICK: return "DBL";
      case KEY_EVT_LONG_PRESS:   return "LONG";
      default:                   return "";
    }
}
/* 屏幕底部显示按键事件 */
static void show_key_event(const KeyEvent_t *ev)
{
    if(!ev) return;
    snprintf(key_evt_buf, sizeof(key_evt_buf), "%s %s",
    key_id_to_str(ev->id), key_type_to_str(ev->type));
    
    /* 清除显示区域 (根据实际屏幕宽度调整) */
    lcd_fill(200, 760, 420, 790, BLACK);
    lcd_show_string(200, 760, 220, 32, 32, key_evt_buf, CYAN);
}



void ui(){
  
  switch (current_page)
  {
  case home:
  LCD_ShowImage(85, 10, 310, 70, "logo_cdtu");
  //LCD_ShowImage(85, 30, 230, 60, "logo_cdtu2");
  lcd_fill(0, 90, 480, 800, GRAYBLUE);
  lcd_fill(85, 200, 395, 300, RGB565_color(25, 50, 5));
  lcd_fill(85, 350, 395, 450, RGB565_color(25, 50, 5));
  lcd_fill(85, 500, 395, 600, RGB565_color(25, 50, 5));
  lcd_fill(85, 650, 395, 750, RGB565_color(25, 50, 5));
  lcd_draw_rectangle(85, 200, 395, 300, RED);
    
  lcd_show_string(160, 230, 310, 100, 32, "ShowData", BLACK);
  lcd_show_string(160, 380, 310, 100, 32, "NetWork", BLACK);
  lcd_show_string(160, 530, 310, 100, 32, "Draw", BLACK);
  
  break;
  case showdata:
    LCD_ShowImage(85, 10, 310, 70, "logo_cdtu");
    lcd_fill(0, 90, 480, 800, GRAYBLUE);
    lcd_show_string(64, 100, 160, 32, 32,"Temp:   C", BLACK);
    lcd_show_string(64, 150, 160, 32, 32, "Humi:   %", BLACK);
    lcd_show_string(64, 200, 220, 32, 32, "PRESS:      Pa", BLACK);
    lcd_show_string(64, 250, 160, 32, 32, "ALTI:    m", BLACK);
    lcd_show_string(64, 300, 220, 32, 32, "LIGHT:      lx", BLACK);
    lcd_show_string(64, 350, 220, 32, 32, "DIST:    cm", BLACK);

    lcd_draw_back_icon(80, 450, 60, 40);
    lcd_draw_refresh_icon(180, 450, 60, 40);
    

    break;
  case network:

    break;
  case draw:
    //lcd_clear(WHITE);
    LCD_ShowImage(85, 10, 310, 70, "logo_cdtu");
    lcd_fill(0, 90, 480, 800, GRAYBLUE);
    lcd_fill(0, 131, 480, 800, WHITE);
    lcd_draw_back_icon(30, 90, 60, 40);
    lcd_draw_refresh_icon(130, 90, 60, 40);
    break;

  default:
    break;
  }

}

uint8_t num = 1;
bool is_back_now = 0;

void select()
{
  uint8_t start_x = 85;
  uint8_t start_y = 200;
  switch (current_opt)
  {
  case up:
    current_opt = n_opt;
    if (current_page == home)
    {
      lcd_draw_rectangle(85, 200 + 150 * (num - 1), 395, 300 + 150 * (num - 1), RGB565_color(25, 50, 5));
      num--;
    }
    
    
    break;
    
  case down:
    current_opt = n_opt;
    if (current_page  == home)
    {
      lcd_draw_rectangle(85, 200 + 150 * (num - 1), 395, 300 + 150 * (num - 1), RGB565_color(25, 50, 5));
      num++;
    }
    

    break;

  case left:
    if (current_page == showdata  || current_page == draw)
    { 
      is_back_now = !is_back_now;
    }


    
    break;

  case right:
    if (current_page == showdata  || current_page == draw)
    {
      is_back_now = !is_back_now;
    }




    break;

  case yes:
    current_opt = n_opt;
    lcd_clear(BLACK);
    if(current_page == home)
    {
      switch (num)
      {
      case 1:
        current_page = showdata;
        ui();
        return;
      case 2:
        current_page = network;
        ui();
        return;
      case 3:
        current_page = draw;
        ui();
        return; 
      case 4:
        current_page = home;
        //ui();
        return;
    
      default:
        return;
      }
    }
    if(current_page == showdata)
    {
      if(is_back_now) 
      {
        is_back_now = 0;
        num = 1;
        current_page = home;
      }
      ui();
    }

    if(current_page == draw)
    {
      if(is_back_now) 
      {
        is_back_now = 0;
        num = 1;
        current_page = home;
      }
      ui();

    }
  default:
    break;
  }
  if(num < 1) num = 4;
  if(num > 4) num = 1;
  if(current_page == home) lcd_draw_rectangle(85, 200 + 150 * (num - 1), 395, 300 + 150 * (num - 1), RED);
  if(current_page == showdata)
  {
    if(is_back_now){
      lcd_draw_rectangle(80, 450, 140, 490, RED);
      lcd_draw_rectangle(180, 450, 240, 490, WHITE);
    }else{
      lcd_draw_rectangle(80, 450, 140, 490, WHITE);
      lcd_draw_rectangle(180, 450, 240, 490, RED);
    }
  }


  if(current_page == draw)
  {
    if(is_back_now){
      lcd_draw_rectangle(30, 90, 90, 130, RED);
      lcd_draw_rectangle(130, 90, 190, 130, WHITE);
    }else{
      lcd_draw_rectangle(30, 90, 90, 130, WHITE);
      lcd_draw_rectangle(130, 90, 190, 130, RED);
    }
  }
}


/* 屏幕中间显示蓝牙消息（居中显示） */
static void LCD_ShowBTMessageCenter(const char *s)
{
    if (!s) return;

    /* 根据你屏幕的实际分辨率调整这些数字 */
    uint16_t screen_w = lcddev.width;
    uint16_t screen_h = lcddev.height;

    uint16_t char_w = 16;  // 如用 16x16 字体
    uint16_t char_h = 24;  // 行高可适当大一点

    uint16_t len = strlen(s);
    if (len > (screen_w / char_w)) {
        len = screen_w / char_w;
    }

    uint16_t text_w = len * char_w;
    uint16_t x = (screen_w - text_w) / 2;
    uint16_t y = (screen_h - char_h) / 2;   // 垂直居中

    /* 先清除一条中间区域，再画字 */
    lcd_fill(0, y, screen_w - 1, y + char_h + 4, BLACK);
    lcd_show_string(x, y, text_w, char_h, 16, (char*)s, WHITE);
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_ADC1_Init();
  MX_DCMI_Init();
  //MX_FSMC_Init();         //记得注释这一句
  //MX_SPI3_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET); // 错误指示灯熄灭
  lcd_init();
  lcd_clear(BLACK);
  
  tp_init();
  uint8_t t = 0;
  uint8_t i = 0;
  uint16_t lastpos[10][2];        /* 最后一次的数据 */
  uint8_t maxp = 5;

  // ssd1306_Init();
  // ssd1306_Fill(0);
  // ssd1306_SetCursor(0, 0);
  // ssd1306_WriteString(0, 0, "Hello World");
  // ssd1306_UpdateScreen();
  int number = 0;

  // BT_Init(NULL);
  // UART_IT_StartRecv(hBluetooth.huart, 1);
  DL_LN33_InitIT();


  KEY_Init();

  if(SHT30_Check())
  {
      SHT30_Read_SingleShot(&T, &H);
  }
  else
  {
      T = -1;
      H = -1;
      HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_10);
  }

  if(BMP280_Init() == BMP280_OK)
  {
      BMP280_ReadTempPressure(&T, &P);
      A = (int)BMP280_CalcAltitude(P, 101325.0f);
  }
  else
  {
      P = -1;
      A = -1;
      HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_10);
  }

    BH1750_PowerOn();
    BH1750_Reset();
    BH1750_SetMode(0x21); 

    HC_SR04_Init(GPIOG, GPIO_PIN_6, GPIOG, GPIO_PIN_7, GPIO_NOPULL);
    








  ui();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    static uint32_t prev = 0;
    if (HAL_GetTick() - prev >= 10)
    {
        prev = HAL_GetTick();
        KEY_Update();
        KeyEvent_t ev;
        while (KEY_GetEvent(&ev))
        {
          // 根据 ev.id 和 ev.type 做处理
          show_key_event(&ev);

          if(ev.id == KEY_UP && ev.type == KEY_EVT_RELEASE) {
            current_opt = up;
            select();
          }
          if (ev.id == KEY_DOWN && ev.type == KEY_EVT_RELEASE) {
            current_opt = down;
            select();
          }
          if(ev.id == KEY_LEFT && ev.type == KEY_EVT_RELEASE){
            current_opt = left;
            select();
          }
          if(ev.id == KEY_RIGHT && ev.type == KEY_EVT_RELEASE){
            current_opt = right;
            select();
          }
          if(ev.id == KEY_OK && ev.type == KEY_EVT_RELEASE){
            current_opt = yes;
            select();
          }

        }
    }
    
    
    static uint32_t tick_prev = 0;
    if (HAL_GetTick() - tick_prev >= 1200)
    {

        T = 0;
        H = 0;
        L = 0;
        P = 0;
        A = 0;
        tick_prev = HAL_GetTick();
        if (SHT30_Read_SingleShot(&T, &H) != 0)
        {
            T = -1; H = -1;
            HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_10);
        }
        BMP280_ReadTempPressure(&T, &P);
        A = (int)BMP280_CalcAltitude(P, 101325.0f);
        BH1750_ReadLux(&L);
        HC_SR04_Measure(NULL, &D);
        //BT_Send(&hBluetooth, (const uint8_t*)msg, strlen(msg), 500);
        //HAL_UART_Transmit_IT(&huart2,pkg,8);
       //DL_LN33_SendPacket(DL_LOCAL_PORT, 0x90, 0xbd1d, content, 1);

       uint8_t *frame = NULL;
       uint16_t frame_len = 0;
       DL_LN33_BuildFrame(DL_LOCAL_PORT, 0x90, 0xbd1d, content, sizeof(content), &frame, &frame_len);
       //DL_LN33_SendFrameIT(DL_LN33_DEFAULT_UART, frame, frame_len);




       if(current_page == showdata)
      {
        lcd_fill(150, 100, 180, 132, GRAYBLUE); 
        lcd_fill(150, 150, 180, 182, GRAYBLUE);
        lcd_fill(170, 200, 250, 232, GRAYBLUE);
        lcd_fill(150, 250, 200, 282, GRAYBLUE);
        lcd_fill(170, 300, 250, 332, GRAYBLUE);
        lcd_fill(150, 350, 200, 382, GRAYBLUE);
      
        lcd_show_xnum(150, 100, T, 2, 32, 1, BLACK);
        lcd_show_xnum(150, 150, H, 2, 32, 1, BLACK);
        lcd_show_xnum(170, 200, P, 5, 32, 1, BLACK);
        lcd_show_xnum(150, 250, A, 3, 32, 1, BLACK);
        lcd_show_xnum(170, 300, L, 5, 32, 1, BLACK);
        lcd_show_xnum(150, 350, D, 3, 32, 1, BLACK);
      }


    } 
    
    /* === 检查是否有 LN33 模块发来的完整帧 === */
    if (DL_LN33_IsRxFrameReady()) {
        uint8_t raw_frame[128];
        uint16_t rlen = DL_LN33_GetRxFrame(raw_frame, sizeof(raw_frame));
        if (rlen > 0) {
            /* 把原始帧转成十六进制字符串，例如 "FE 05 91 21 00 00 01 FF" */
            bytes_to_hex_str(raw_frame, rlen, disp_buf, sizeof(disp_buf));
            DL_Packet_t receive_pkg;
            DL_LN33_Read_data(raw_frame, rlen, &receive_pkg);  // 提取包中实际数据
            lcd_show_string(10, 700, 400, 32, 24, "Received LN33 Packet Data:", MAGENTA);
            lcd_show_xnum(350, 700, receive_pkg.data[0], 5, 24, 0, MAGENTA); //以十进制显示数据
            usart2_has_new = 1;   // 通知下面的显示逻辑更新屏幕
        }
    }


    if (usart2_has_new)
    {
        usart2_has_new = 0;
        LCD_ShowBTMessageCenter(disp_buf);   // 屏幕显示收到的内容
    }
		
    


    

    

    

    


    

    // ssd1306_WriteInt(0, 16, number);
    // ssd1306_WriteString(0, 32, "Temp:  C");
    // ssd1306_WriteString(0, 48, "Humi:  %");
    // ssd1306_WriteInt(30, 32, T);
    // ssd1306_WriteInt(30, 48, H);
    // ssd1306_WriteString(70, 16, "   lx");
    // ssd1306_WriteInt(70, 16, (int)L);
    // ssd1306_WriteString(70, 32, "     Pa");
    // ssd1306_WriteString(70, 48, "   m");
    // ssd1306_WriteInt(70, 32, P);
    // ssd1306_WriteInt(70, 48, D);
    
    
    

    //HAL_Delay(1000);

    tp_dev.scan(0);

    if(current_page == draw)
    {
      for (t = 0; t < maxp; t++)
        {
            if ((tp_dev.sta) & (1 << t))
            {
                if (tp_dev.x[t] < lcddev.width && tp_dev.y[t] < lcddev.height && tp_dev.y[t] > 135)  /* 坐标在屏幕范围内 */
                {
                    if (lastpos[t][0] == 0xFFFF)
                    {
                        lastpos[t][0] = tp_dev.x[t];
                        lastpos[t][1] = tp_dev.y[t];
                    }

                    lcd_draw_bline(lastpos[t][0], lastpos[t][1], tp_dev.x[t], tp_dev.y[t], 2, POINT_COLOR_TBL[t]); /* 画线 */
                    lastpos[t][0] = tp_dev.x[t];
                    lastpos[t][1] = tp_dev.y[t];

                  }
                  if (tp_dev.x[t] > 130 && tp_dev.x[t] < 190 && tp_dev.y[t] > 90 && tp_dev.y[t] < 130)
                  {
                      //load_draw_dialog();/* 清除 */
                      lcd_fill(0, 133, 480, 800, WHITE);
                      //ui();
                      //HAL_Delay(300);
                  }
                   if (tp_dev.x[t] > 30 && tp_dev.x[t] < 90 && tp_dev.y[t] > 90 && tp_dev.y[t] < 130)
                  {
                      current_page = home;
                      num = 1;
                      ui();
                  }
            }
            else 
            {
                lastpos[t][0] = 0xFFFF;
            }
        }
       HAL_Delay(5);
    }

    if(current_page == home)
    {
      for (t = 0; t < maxp; t++)
        {
            if ((tp_dev.sta) & (1 << t))
            {
                  if (tp_dev.x[t] > 85 && tp_dev.x[t] < 395 && tp_dev.y[t] > 200 && tp_dev.y[t] < 300)
                  {
                      current_page = showdata;
                      ui();
                  }
                  if (tp_dev.x[t] > 85 && tp_dev.x[t] < 395 && tp_dev.y[t] > 350 && tp_dev.y[t] < 450)
                  {
                      //current_page = network;
                      ui();
                  }
                  if (tp_dev.x[t] > 85 && tp_dev.x[t] < 395 && tp_dev.y[t] > 500 && tp_dev.y[t] < 600)
                  {
                      current_page = draw;
                      ui();
                  }
                  if (tp_dev.x[t] > 85 && tp_dev.x[t] < 395 && tp_dev.y[t] > 650 && tp_dev.y[t] < 750)
                  {

                  }
            }
            else 
            {
                lastpos[t][0] = 0xFFFF;
            }
        }
       HAL_Delay(5);
    }
  
    if(current_page == showdata)
    {
      for (t = 0; t < maxp; t++)
        {
            if ((tp_dev.sta) & (1 << t))
            {
                  if (tp_dev.x[t] > 180 && tp_dev.x[t] < 240 && tp_dev.y[t] > 450 && tp_dev.y[t] < 490)
                  {
                      ui();
                  }
                  if (tp_dev.x[t] > 80 && tp_dev.x[t] < 140 && tp_dev.y[t] > 450 && tp_dev.y[t] < 490)
                  {
                      current_page = home;
                      num = 1;
                      ui();
                  }
            }
            else 
            {
                lastpos[t][0] = 0xFFFF;
            }
        }
       HAL_Delay(5);
    }

        
       


   

    

    

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
