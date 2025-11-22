#ifndef __DL_LN33_H__
#define __DL_LN33_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 默认使用 USART2（在 usart.c 中已定义 huart2） */
extern UART_HandleTypeDef huart2;

/* 最近一次发送/接收的原始帧（包含 FE ... FF），供调试显示 */
extern uint8_t  g_dl_last_tx_raw[];
extern uint16_t g_dl_last_tx_len;
extern uint8_t  g_dl_last_rx_raw[];
extern uint16_t g_dl_last_rx_len;

/* 返回状态 */
typedef enum {
    DL_OK = 0,
    DL_ERR_PARAM,
    DL_ERR_PORT_RANGE,   // 端口号不在 0x80~0xFF
    DL_ERR_UART,
    DL_ERR_TIMEOUT,
    DL_ERR_FORMAT        // 帧格式/长度/转义错误
} DL_Status_t;

/**
 * @brief 封包并发送到 DL-LN3X 模块
 * @param src_port    源端口号 (0x80~0xFF，<0x80 包无法发出)
 * @param dst_port    目标端口号 (0x80~0xFF，<0x80 由模块内部处理)
 * @param remote_addr 远程地址 (0x0000=本模块, 0xFFFF=广播, 其它=目标模块地址，小端发送)
 * @param payload     要发送的数据缓冲区
 * @param payload_len 数据长度（未转义前的原始长度）
 * @retval DL_OK / DL_ERR_PARAM / DL_ERR_PORT_RANGE / DL_ERR_UART
 */
DL_Status_t DL_LN33_SendPacket(uint8_t src_port,
                               uint8_t dst_port,
                               uint16_t remote_addr,
                               const uint8_t *payload,
                               uint16_t payload_len);

/* 解析后的接收结果 */
typedef struct {
    uint8_t  src_port;
    uint8_t  dst_port;
    uint16_t remote_addr;          // 小端还原后的地址
    uint8_t  data[256];            // 反转义后的数据
    uint16_t data_len;             // 有效数据长度
} DL_Packet_t;

/**
 * @brief  从模块接收一帧并解析
 *         帧格式: FE LEN src dst addrL addrH DATA... FF
 *         LEN 为 src~addrH~DATA 的字节数 (未转义前, =4+payload_len)
 * @param  pkt       输出结构体指针
 * @param  timeoutMs 整帧超时时间
 * @retval DL_OK / DL_ERR_TIMEOUT / DL_ERR_FORMAT / DL_ERR_UART
 */
DL_Status_t DL_LN33_RecvPacket(DL_Packet_t *pkt, uint32_t timeoutMs);

#ifdef __cplusplus
}
#endif

#endif /* __DL_LN33_H__ */