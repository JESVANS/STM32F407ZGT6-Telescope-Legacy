#include "dl_ln33.h"
#include "usart.h"   // 提供 UART_SendEx
#include <string.h>

/* 协议常量 */
#define DL_HEAD_BYTE     0xFE
#define DL_TAIL_BYTE     0xFF
#define DL_ESC_BYTE      0xFE
#define DL_ESC_FF_CODE   0xFD  // FF -> FE FD
#define DL_ESC_FE_CODE   0xFC  // FE -> FE FC

/* 组帧最大有效载荷（原始数据，不含转义），按需要修改 */
#define DL_MAX_PAYLOAD_LEN   256

/* 默认本端端口号（可视需要调整，只要在 0x80~0xFF 即可） */
#define DL_LOCAL_PORT        0x91
#define DL_REMOTE_PORT       0x90

/* 最近一次发送/接收的原始帧（包含 FE ... FF），供调试显示 */
uint8_t  g_dl_last_tx_raw[4 + 4 + DL_MAX_PAYLOAD_LEN * 2];
uint16_t g_dl_last_tx_len = 0;
uint8_t  g_dl_last_rx_raw[4 + 4 + DL_MAX_PAYLOAD_LEN * 2];
uint16_t g_dl_last_rx_len = 0;

/* 发送底层封装：默认使用 USART2 */
static HAL_StatusTypeDef dl_uart_send(const uint8_t *data, uint16_t len, uint32_t timeout)
{
    return UART_SendEx(&huart2, data, len, timeout);
}

/* 单字节阻塞接收（简单版），超时返回 HAL_TIMEOUT */
static HAL_StatusTypeDef dl_uart_recv_byte(uint8_t *b, uint32_t timeout)
{
    return HAL_UART_Receive(&huart2, b, 1, timeout);
}

/**
 * @brief 对单个字节做转义写入到 buf 中
 * @param b    原始字节
 * @param out  输出缓冲
 * @param idx  当前写入位置，函数内部会自增
 */
static void dl_append_escaped(uint8_t b, uint8_t *out, uint16_t *idx)
{
    if (b == 0xFF) {
        out[(*idx)++] = DL_ESC_BYTE;
        out[(*idx)++] = DL_ESC_FF_CODE;
    } else if (b == 0xFE) {
        out[(*idx)++] = DL_ESC_BYTE;
        out[(*idx)++] = DL_ESC_FE_CODE;
    } else {
        out[(*idx)++] = b;
    }
}

/**
 * @brief  封包并通过 USART2 发送给 DL-LN3X
 *         帧: FE LEN src dst addrL addrH DATA... FF
 *         LEN = 4 + payload_len (src+dst+addrL+addrH+DATA 的未转义长度)
 */
DL_Status_t DL_LN33_SendPacket(uint8_t src_port,
                               uint8_t dst_port,
                               uint16_t remote_addr,
                               const uint8_t *payload,
                               uint16_t payload_len)
{
    /* 端口检查：MCU 端口必须 0x80~0xFF */
    if (src_port < 0x80 || dst_port < 0x80) {
        return DL_ERR_PORT_RANGE;
    }
    if ((payload_len > 0 && payload == NULL) || payload_len > DL_MAX_PAYLOAD_LEN) {
        return DL_ERR_PARAM;
    }

    /* 计算逻辑包长：数据长度 + 4 (src + dst + addr_low + addr_high) */
    uint16_t logic_len = payload_len + 4;
    if (logic_len > 0xFF) {
        /* 协议中长度字段只有 1 字节，这里简单限制为 255 */
        return DL_ERR_PARAM;
    }
    uint8_t len_byte = (uint8_t)logic_len;

    /* 构造帧（转义后长度会变大，预留两倍空间 + 头尾） */
    uint8_t frame[4 + 4 + DL_MAX_PAYLOAD_LEN * 2]; // 足够大
    uint16_t idx = 0;

    /* 包头：FE + 长度(未转义前的逻辑长度) */
    frame[idx++] = DL_HEAD_BYTE;
    frame[idx++] = len_byte;

    /* 源端口（需转义） */
    dl_append_escaped(src_port, frame, &idx);

    /* 目的端口（需转义） */
    dl_append_escaped(dst_port, frame, &idx);

    /* 远程地址，2 字节小端，均需转义 */
    uint8_t addr_low  = (uint8_t)(remote_addr & 0xFF);
    uint8_t addr_high = (uint8_t)((remote_addr >> 8) & 0xFF);
    dl_append_escaped(addr_low,  frame, &idx);
    dl_append_escaped(addr_high, frame, &idx);

    /* 负载数据，逐字节转义 */
    for (uint16_t i = 0; i < payload_len; ++i) {
        dl_append_escaped(payload[i], frame, &idx);
    }

    /* 包尾 FF（不参与长度计算，也不转义） */
    frame[idx++] = DL_TAIL_BYTE;

    /* 记录最近一次发送的原始帧 */
    if (idx <= sizeof(g_dl_last_tx_raw)) {
        memcpy(g_dl_last_tx_raw, frame, idx);
        g_dl_last_tx_len = idx;
    } else {
        g_dl_last_tx_len = 0;
    }

    /* 通过 UART 发送整帧 */
    if (dl_uart_send(frame, idx, 1000) != HAL_OK) {
        return DL_ERR_UART;
    }
    return DL_OK;
}

/**
 * @brief  从模块接收一帧并解析
 *         格式: FE LEN src dst addrL addrH DATA... FF
 *         LEN 为 src~addrH~DATA 的字节数 (未转义前, =4+payload_len)
 */
DL_Status_t DL_LN33_RecvPacket(DL_Packet_t *pkt, uint32_t timeoutMs)
{
    if (!pkt) return DL_ERR_PARAM;
    memset(pkt, 0, sizeof(*pkt));

    uint32_t start = HAL_GetTick();
    uint8_t b;

    /* 1. 等待包头 FE */
    while (1) {
        if (HAL_GetTick() - start >= timeoutMs)
            return DL_ERR_TIMEOUT;

        if (dl_uart_recv_byte(&b, 10) != HAL_OK)
            continue;
        if (b == DL_HEAD_BYTE)
            break;  // 找到帧头
    }

    /* 2. 读取 LEN 字节（未转义的长度） */
    if (dl_uart_recv_byte(&b, timeoutMs) != HAL_OK)
        return DL_ERR_TIMEOUT;
    uint8_t len_logic = b;  // src + dst + addrL + addrH + DATA (未转义)

    if (len_logic < 4)      // 至少有 4 字节: src dst addrL addrH
        return DL_ERR_FORMAT;

    /* 3. 反转义读取 LEN 字节的有效区到缓冲 buf_raw[] */
    uint8_t buf_raw[4 + 256];   // src/dst/addr(2) + payload <= 256
    uint16_t raw_idx = 0;

    /* 为了记录原始帧，还需要存储串口上实际收到的字节（已含转义） */
    uint8_t  rx_raw_tmp[4 + 4 + DL_MAX_PAYLOAD_LEN * 2];
    uint16_t rx_raw_len = 0;

    /* 先把头和 LEN 放进去 */
    rx_raw_tmp[rx_raw_len++] = DL_HEAD_BYTE;
    rx_raw_tmp[rx_raw_len++] = len_logic;

    while (raw_idx < len_logic) {
        if (HAL_GetTick() - start >= timeoutMs)
            return DL_ERR_TIMEOUT;
        if (dl_uart_recv_byte(&b, 10) != HAL_OK)
            continue;

        /* 所有经过的字节都放入原始缓冲 */
        if (rx_raw_len < sizeof(rx_raw_tmp)) {
            rx_raw_tmp[rx_raw_len++] = b;
        }

        if (b == DL_ESC_BYTE) {
            /* 转义字节后面应跟 FC/FD */
            uint8_t code;
            if (dl_uart_recv_byte(&code, timeoutMs) != HAL_OK)
                return DL_ERR_TIMEOUT;

            if (rx_raw_len < sizeof(rx_raw_tmp)) {
                rx_raw_tmp[rx_raw_len++] = code;
            }

            if (code == DL_ESC_FF_CODE) {
                buf_raw[raw_idx++] = 0xFF;
            } else if (code == DL_ESC_FE_CODE) {
                buf_raw[raw_idx++] = 0xFE;
            } else {
                /* 非法转义 */
                return DL_ERR_FORMAT;
            }
        } else if (b == DL_TAIL_BYTE) {
            /* 不应在 LEN 还没读完时提前遇到尾部 FF */
            return DL_ERR_FORMAT;
        } else {
            buf_raw[raw_idx++] = b;
        }
    }

    /* 4. 读尾部 FF */
    if (dl_uart_recv_byte(&b, timeoutMs) != HAL_OK)
        return DL_ERR_TIMEOUT;
    if (b != DL_TAIL_BYTE)
        return DL_ERR_FORMAT;

    if (rx_raw_len < sizeof(rx_raw_tmp)) {
        rx_raw_tmp[rx_raw_len++] = DL_TAIL_BYTE;
    }

    /* 记录最近一次接收到的原始帧 */
    if (rx_raw_len <= sizeof(g_dl_last_rx_raw)) {
        memcpy(g_dl_last_rx_raw, rx_raw_tmp, rx_raw_len);
        g_dl_last_rx_len = rx_raw_len;
    } else {
        g_dl_last_rx_len = 0;
    }

    /* 5. 解析 buf_raw -> 结构体 */
    pkt->src_port   = buf_raw[0];
    pkt->dst_port   = buf_raw[1];
    pkt->remote_addr = (uint16_t)buf_raw[2] | ((uint16_t)buf_raw[3] << 8);

    uint16_t payload_len = len_logic - 4; // 去掉 src/dst/addrL/addrH
    if (payload_len > sizeof(pkt->data))
        return DL_ERR_FORMAT;

    pkt->data_len = payload_len;
    if (payload_len > 0) {
        memcpy(pkt->data, &buf_raw[4], payload_len);
    }

    return DL_OK;
}