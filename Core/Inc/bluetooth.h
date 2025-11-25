/**
  ******************************************************************************
  * @file    bluetooth.h
  * @author  Your Name
  * @brief   JDY-31 Bluetooth Module Driver Header File
  ******************************************************************************
  */

#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 默认资源（工程里已有 huart2） */
extern UART_HandleTypeDef huart2;

/* 默认参数 */
#define BT_DEFAULT_UART        (&huart2)
#define BT_DEFAULT_BAUDRATE    115200U
#define BT_DEFAULT_EN_PORT     GPIOC
#define BT_DEFAULT_EN_PIN      GPIO_PIN_5      /* EN: 高→AT模式, 低→透传 */
#define BT_DEFAULT_STATE_PORT  GPIOC
#define BT_DEFAULT_STATE_PIN   GPIO_PIN_4      /* STATE: 高→已连接, 低→未连接 */

/* 返回状态 */
typedef enum {
    BT_OK = 0,
    BT_ERROR,
    BT_TIMEOUT,
    BT_INVALID
} BT_Status_t;

/* 连接状态 */
typedef enum {
    BT_DISCONNECTED = 0,
    BT_CONNECTED    = 1
} BT_ConnState_t;

/* 设备句柄（用户可先填想要的 UART/引脚后调用 Init） */
typedef struct {
    UART_HandleTypeDef *huart;
    GPIO_TypeDef *EN_Port;
    uint16_t EN_Pin;
    GPIO_TypeDef *STATE_Port;
    uint16_t STATE_Pin;
    uint32_t baudrate;
    uint8_t initialized;
} BT_Handle_t;

/* 使用默认配置的全局实例（也可自己定义局部实例） */
extern BT_Handle_t hBluetooth;

/* 初始化：配置 EN(推挽输出, 默认拉低) 与 STATE(输入) 并标记已初始化 */
BT_Status_t BT_Init(BT_Handle_t *hbt);

/* 反初始化：仅复位标志，可按需添加 GPIO 反初始化 */
BT_Status_t BT_DeInit(BT_Handle_t *hbt);

/* 发送透传数据（EN 必须为低） */
BT_Status_t BT_Send(BT_Handle_t *hbt, const uint8_t *data, uint16_t len, uint32_t timeout_ms);

/* 接收透传数据 */
BT_Status_t BT_Receive(BT_Handle_t *hbt, uint8_t *buf, uint16_t len, uint32_t timeout_ms);

/* 读取连接状态（直接读 STATE 引脚） */
BT_ConnState_t BT_GetState(BT_Handle_t *hbt);

/*
!!注意!!
BT_SendAT已不再使用, 需要发送AT指令时 可直接在 蓝牙断开连接 的情况下使用BT_Send发送"AT+......"
连接状态下直接发送"AT+DISC\r\n"可使模块断开连接
*/
/*AT+VERSION 查询版本 
    AT+RESET    复位 
    AT+DISC    断开连接
    AT+LADDR    查询MAC地址
    AT+BAUD<Param>  AT+BAUD    波特率设置/查询 Param:4=9600,5=19200,6=38400,7=57600,8=115200,9=128000
    AT+PIN<Param>  AT+PIN    蓝牙配对密码设置/查询 Param:4位数字密码，如 AT+PIN1234
    AT+NAME<Param>  AT+NAME    广播名设置/查询  Param: BLE 广播名(最长:18字节) ，如 AT+NAMEMyBTDevice
    AT+ENLOG<Param>  AT+ENLOG    串口状态输出使能设置/查询  1:打开串口状态输出
    AT+DEFAULT    恢复出厂设置
*/



// /* 发送 AT 指令：
//    流程：拉高 EN→延时→发送命令→等待响应→拉低 EN
//    resp_buf 可为 NULL；timeout_ms 超时；返回 BT_OK/BT_TIMEOUT */
// BT_Status_t BT_SendAT(BT_Handle_t *hbt, const char *cmd,
//                       char *resp_buf, uint16_t resp_len, uint32_t timeout_ms);


#ifdef __cplusplus
}
#endif

#endif