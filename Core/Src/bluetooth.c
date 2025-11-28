/**
  ******************************************************************************
  * @file    bluetooth.c
  * @author  Your Name
  * @brief   JDY-31 Bluetooth Module Driver Implementation
  ******************************************************************************
  */

#include "bluetooth.h"
#include "usart.h"
#include <string.h>

/* 默认实例 */
BT_Handle_t hBluetooth = {
    .huart       = BT_DEFAULT_UART,
    .EN_Port     = BT_DEFAULT_EN_PORT,
    .EN_Pin      = BT_DEFAULT_EN_PIN,
    .STATE_Port  = BT_DEFAULT_STATE_PORT,
    .STATE_Pin   = BT_DEFAULT_STATE_PIN,
    .baudrate    = BT_DEFAULT_BAUDRATE,
    .initialized = 0
};

/* 内部帮助：若传入 NULL 使用默认实例 */
static inline BT_Handle_t* _handle(BT_Handle_t *hbt) {
    return hbt ? hbt : &hBluetooth;
}

BT_Status_t BT_Init(BT_Handle_t *hbt_in)
{
    BT_Handle_t *hbt = _handle(hbt_in);
    if (!hbt || !hbt->huart) return BT_INVALID;

    /* EN 输出：默认低（透传模式） */
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin   = hbt->EN_Pin;
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pull  = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(hbt->EN_Port, &gpio);
    HAL_GPIO_WritePin(hbt->EN_Port, hbt->EN_Pin, GPIO_PIN_RESET);

    /* STATE 输入 */
    gpio.Pin  = hbt->STATE_Pin;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(hbt->STATE_Port, &gpio);

    hbt->initialized = 1;
    return BT_OK;
}

BT_Status_t BT_DeInit(BT_Handle_t *hbt_in)
{
    BT_Handle_t *hbt = _handle(hbt_in);
    if (!hbt) return BT_INVALID;
    hbt->initialized = 0;
    return BT_OK;
}

BT_Status_t BT_Send(BT_Handle_t *hbt_in, const uint8_t *data, uint16_t len, uint32_t timeout_ms)
{
    BT_Handle_t *hbt = _handle(hbt_in);
    if (!hbt || !hbt->initialized || !data || !len) return BT_INVALID;

    /* 仅在透传模式 (EN 低) 允许发送 */
    if (HAL_GPIO_ReadPin(hbt->EN_Port, hbt->EN_Pin) == GPIO_PIN_SET)
        return BT_ERROR;

    /* 中断方式发送 */
    if (UART_IT_Send(hbt->huart, data, len) != HAL_OK)
        return BT_ERROR;

    uint32_t start = HAL_GetTick();
    volatile uint8_t *tx_done_flag = NULL;

    if (hbt->huart == &huart1)      tx_done_flag = &uart1_tx_done;
    else if (hbt->huart == &huart2) tx_done_flag = &uart2_tx_done;
    else if (hbt->huart == &huart3) tx_done_flag = &uart3_tx_done;

    if (!tx_done_flag) return BT_ERROR;

    while (!(*tx_done_flag)) {
        if (HAL_GetTick() - start >= timeout_ms)
            return BT_TIMEOUT;
    }

    return BT_OK;
}

BT_Status_t BT_Receive(BT_Handle_t *hbt_in, uint8_t *buf, uint16_t len, uint32_t timeout_ms)
{
    BT_Handle_t *hbt = _handle(hbt_in);
    if (!hbt || !hbt->initialized || !buf || !len) return BT_INVALID;

    if (len > UART_IT_RX_BUF_SIZE) return BT_INVALID;

    if (UART_IT_StartRecv(hbt->huart, len) != HAL_OK)
        return BT_ERROR;

    uint32_t start = HAL_GetTick();
    volatile uint8_t *rx_done_flag = NULL;
    uint8_t *src_buf = NULL;

    if (hbt->huart == &huart1) {
        rx_done_flag = &uart1_rx_done;
        src_buf      = uart1_rx_buf;
    } else if (hbt->huart == &huart2) {
        rx_done_flag = &uart2_rx_done;
        src_buf      = uart2_rx_buf;
    } else if (hbt->huart == &huart3) {
        rx_done_flag = &uart3_rx_done;
        src_buf      = uart3_rx_buf;
    }

    if (!rx_done_flag || !src_buf) return BT_ERROR;

    while (!(*rx_done_flag)) {
        if (HAL_GetTick() - start >= timeout_ms)
            return BT_TIMEOUT;
    }

    memcpy(buf, src_buf, len);
    return BT_OK;
}

// /* 等待 AT 响应：一次性中断接收若干字节，在缓冲中找 '\n' */
// static BT_Status_t _wait_resp(BT_Handle_t *hbt, char *resp, uint16_t resp_len, uint32_t timeout_ms)
// {
//     if (!resp || resp_len < 2) {
//         HAL_Delay(timeout_ms);
//         return BT_OK;
//     }

//     uint16_t max_len = (resp_len - 1);
//     if (max_len > UART_IT_RX_BUF_SIZE) max_len = UART_IT_RX_BUF_SIZE;

//     if (UART_IT_StartRecv(hbt->huart, max_len) != HAL_OK)
//         return BT_ERROR;

//     uint32_t start = HAL_GetTick();
//     volatile uint8_t *rx_done_flag = NULL;
//     uint8_t *src_buf = NULL;
//     uint16_t rx_len = 0;

//     if (hbt->huart == &huart1) {
//         rx_done_flag = &uart1_rx_done;
//         src_buf      = uart1_rx_buf;
//         rx_len       = uart1_rx_len;
//     } else if (hbt->huart == &huart2) {
//         rx_done_flag = &uart2_rx_done;
//         src_buf      = uart2_rx_buf;
//         rx_len       = uart2_rx_len;
//     } else if (hbt->huart == &huart3) {
//         rx_done_flag = &uart3_rx_done;
//         src_buf      = uart3_rx_buf;
//         rx_len       = uart3_rx_len;
//     }

//     if (!rx_done_flag || !src_buf) return BT_ERROR;

//     while (!(*rx_done_flag)) {
//         if (HAL_GetTick() - start >= timeout_ms)
//             return BT_TIMEOUT;
//     }

//     /* 此时 src_buf[0..rx_len-1] 是收到的 AT 响应数据 */
//     uint16_t copy_len = (rx_len < max_len) ? rx_len : max_len;
//     memcpy(resp, src_buf, copy_len);
//     resp[copy_len] = '\0';
//     return (copy_len == 0) ? BT_TIMEOUT : BT_OK;
// }


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

// BT_Status_t BT_SendAT(BT_Handle_t *hbt_in, const char *cmd,
//                       char *resp_buf, uint16_t resp_len, uint32_t timeout_ms)
// {
//     BT_Handle_t *hbt = _handle(hbt_in);
//     if (!hbt || !hbt->initialized || !cmd) return BT_INVALID;

//     /* 进入 AT 模式：拉高 EN（模块会断开连接进入指令模式） */
//     while (HAL_GPIO_ReadPin(hbt->EN_Port, hbt->EN_Pin) == GPIO_PIN_RESET)
//     {
//         HAL_GPIO_WritePin(hbt->EN_Port, hbt->EN_Pin, GPIO_PIN_SET);
//         HAL_Delay(50);
//     }

//     /* 直接使用中断封装的 BT_Send 发送 AT 命令 */
//     if (BT_Send(hbt, (const uint8_t*)cmd, (uint16_t)strlen(cmd), 1000) != BT_OK) {
//         HAL_GPIO_WritePin(hbt->EN_Port, hbt->EN_Pin, GPIO_PIN_RESET);
//         return BT_ERROR;
//     }

//     /* 等待响应（中断接收） */
//     BT_Status_t st = _wait_resp(hbt, resp_buf, resp_len, timeout_ms);

//     /* 退出 AT 模式，恢复透传 */
//     HAL_GPIO_WritePin(hbt->EN_Port, hbt->EN_Pin, GPIO_PIN_RESET);
//     HAL_Delay(20);

//     return st;
// }

uint8_t BT_GetState(BT_Handle_t *hbt_in)
{
    BT_Handle_t *hbt = _handle(hbt_in);
    if (!hbt || !hbt->initialized) return 0;
    return (HAL_GPIO_ReadPin(hbt->STATE_Port, hbt->STATE_Pin) == GPIO_PIN_SET)
            ? 1 : 0;
}