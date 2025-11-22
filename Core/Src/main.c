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

  static volatile uint8_t  bt_rx_byte;      /* 单字节中断接收缓冲 */
  static volatile uint16_t resp_len = 0;
  static volatile uint8_t  resp_updated = 0;
  const char *msg = "Hello from STM32!\r\n";
  char resp[128];
  
  static char key_evt_buf[32] = "";

  static char g_tx_hex_str[128];
  static char g_rx_hex_str[128];
  static char g_decoded_str[128];
  static uint8_t g_show_ln33 = 0;  // 是否当前有显示 3 行


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
    lcd_fill(120, 700, 340, 732, BLACK);
    lcd_show_string(120, 700, 220, 32, 32, key_evt_buf, CYAN);
}


/* 将字节数组转成十六进制字符串，如 "FE 08 91 90 ..." */
static void bytes_to_hex_str(const uint8_t *buf, uint16_t len, char *out, uint16_t out_size)
{
    uint16_t pos = 0;
    for (uint16_t i = 0; i < len; ++i) {
        if (pos + 3 >= out_size) break;
        int n = snprintf(&out[pos], out_size - pos, "%02X", buf[i]);
        pos += (uint16_t)n;
        if (i != len - 1 && pos + 1 < out_size) {
            out[pos++] = ' ';
        }
    }
    if (pos < out_size) out[pos] = '\0';
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
  //MX_FSMC_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET); // 错误指示灯熄灭
  lcd_init();
  lcd_clear(BLACK);

  ssd1306_Init();
  ssd1306_Fill(0);
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString(0, 0, "Hello World");
  ssd1306_UpdateScreen();
  int number = 0;

  BT_Init(NULL);
  HC_SR04_Init(GPIOG, GPIO_PIN_6, GPIOG, GPIO_PIN_7, GPIO_NOPULL);
  KEY_Init();

  

    float T = 10;
    float H = 10;
    float L = 0;
    float P = 0;
    float A = 0;
    float D = 0;



  //if (lcddev.id == 0x1018) maxp = 10;


    lcd_show_string(64, 32, 160, 32, 32,"Temp:   C", MAGENTA);
    // lcd_show_string(0,48,160,16,16,"Humi:    %", MAGENTA);
    lcd_show_string(64, 64, 160, 32, 32, "Humi:   %", MAGENTA);
    lcd_show_string(64, 96, 220, 32, 32, "PRESS:      Pa", MAGENTA);
    lcd_show_string(64, 128, 160, 32, 32, "ALTI:    m", MAGENTA);
    lcd_show_string(64, 160, 220, 32, 32, "LIGHT:      lx", MAGENTA);
    lcd_show_string(64, 192, 220, 32, 32, "DIST:    cm", MAGENTA);

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


  /* 发送一次（进入透传时 EN 需为低，BT_Init 已拉低） */
  BT_Send(NULL, (const uint8_t*)msg, (uint16_t)strlen(msg), 200);
  HAL_UART_Receive_IT(BT_DEFAULT_UART, (uint8_t*)&bt_rx_byte, 1);   /* 开始中断逐字节接收 */






  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    static uint32_t prev = 0;
    if (HAL_GetTick() - prev >= 10) {
        prev = HAL_GetTick();
        KEY_Update();
        KeyEvent_t ev;
        while (KEY_GetEvent(&ev)) {
          // 根据 ev.id 和 ev.type 做处理
          show_key_event(&ev);
        
          /* 单击“下键”清除刚刚 3 行显示 */
          if (ev.id == KEY_DOWN && ev.type == KEY_EVT_RELEASE) {
              if (g_show_ln33) {
                        uint16_t line_h = 16;
                        uint16_t total_h = line_h * 3;
                        uint16_t y0 = (lcddev.height > total_h) ?
                                      (lcddev.height - total_h) / 2 : 0;
                        lcd_fill(0, y0, lcddev.width - 1,
                                 y0 + total_h - 1, BLACK);
                        g_show_ln33 = 0;
                }
            }     
        }
    }




    

    
        
     static uint32_t tick_prev = 0;
    if (HAL_GetTick() - tick_prev >= 1000)
    {
        tick_prev = HAL_GetTick();
        if (SHT30_Read_SingleShot(&T, &H) != 0)
        {
            T = -1; H = -1;
            HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_10);
        }
        if (BT_GetState(NULL) == BT_CONNECTED)
        {
            //BT_Send(NULL, (const uint8_t*)msg, (uint16_t)strlen(msg), 200);
            //BT_SendAT(NULL, "AT+VERSION\r\n", NULL, 0, 500);
        }
        BMP280_ReadTempPressure(&T, &P);
        A = (int)BMP280_CalcAltitude(P, 101325.0f);
        BH1750_ReadLux(&L);
        HC_SR04_Measure(NULL, &D);
    }

     if (resp_updated)   /* 有新完整消息 */
    {
      resp_updated = 0;
      lcd_show_string(0, 0, 240, 16, 16, resp, g_point_color);
    }
		
     


    

    

    

    lcd_show_xnum(150, 32, T, 2, 32, 0, MAGENTA);
    lcd_show_xnum(150, 64, H, 2, 32, 0, MAGENTA);
    lcd_show_xnum(170, 96, P, 5, 32, 0, MAGENTA);
    lcd_show_xnum(150, 128, A, 3, 32, 0, MAGENTA);
    lcd_show_xnum(170, 160, L, 5, 32, 0, MAGENTA);
    lcd_show_xnum(150, 192, D, 3, 32, 0, MAGENTA);


    

    ssd1306_WriteInt(0, 16, number);
    ssd1306_WriteString(0, 32, "Temp:  C");
    ssd1306_WriteString(0, 48, "Humi:  %");
    ssd1306_WriteInt(30, 32, T);
    ssd1306_WriteInt(30, 48, H);
    ssd1306_WriteString(70, 16, "   lx");
    ssd1306_WriteInt(70, 16, (int)L);
    ssd1306_WriteString(70, 32, "     Pa");
    ssd1306_WriteString(70, 48, "   m");
    ssd1306_WriteInt(70, 32, P);
    ssd1306_WriteInt(70, 48, D);
    
    
    

    //HAL_Delay(1000);

    



   

    

    

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

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart == BT_DEFAULT_UART)
  {
    uint8_t c = bt_rx_byte;

    if (c == '\r' || c == '\n')
    {
      if (resp_len > 0)           /* 结束一条消息 */
      {
        resp[resp_len] = '\0';
        resp_updated = 1;
        resp_len = 0;
      }
    }
    else
    {
      if (resp_len < sizeof(resp) - 1)
      {
        resp[resp_len++] = (char)c;
      }
      else
      {
        /* 缓冲满：强制终止并标记更新 */
        resp[resp_len] = '\0';
        resp_updated = 1;
        resp_len = 0;
      }
    }

    /* 继续接收下一个字节 */
    HAL_UART_Receive_IT(BT_DEFAULT_UART, (uint8_t*)&bt_rx_byte, 1);
  }
}

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
