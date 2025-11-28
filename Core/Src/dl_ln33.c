#include "dl_ln33.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

#define DL_LN33_MAX_FRAME      128U
#define DL_LN33_TX_BUF_SIZE    DL_LN33_MAX_FRAME
#define DL_LN33_RX_BUF_SIZE    DL_LN33_MAX_FRAME

static uint8_t tx_buf[DL_LN33_TX_BUF_SIZE];
static uint8_t rx_buf[DL_LN33_RX_BUF_SIZE];
static uint8_t rx_frame_ready = 0;
static uint16_t rx_len = 0;

static uint8_t *frame_ptr = NULL;
static uint16_t frame_len = 0;

typedef enum {
    RX_STATE_IDLE = 0,
    RX_STATE_FRAME
} RX_State_t;

static RX_State_t rx_state = RX_STATE_IDLE;

static inline void rx_reset(void)
{
    rx_state = RX_STATE_IDLE;
    rx_len = 0;
    rx_frame_ready = 0;
}

void DL_LN33_InitIT(void)
{
    rx_reset();
    if (DL_LN33_DEFAULT_UART != NULL) {
        HAL_UART_Receive_IT(DL_LN33_DEFAULT_UART, rx_buf, 1);
    }
}

static uint8_t calc_len(uint16_t payload_len)
{
    return (uint8_t)(4U + payload_len);
}

static DL_Status_t append_byte(uint8_t **ptr, uint16_t *len,
                               uint8_t byte)
{
    if ((*len) >= DL_LN33_MAX_FRAME) {
        return DL_ERROR;
    }

    **ptr = byte;
    (*ptr)++;
    (*len)++;
    return DL_OK;
}

/* 
 * 转义规则 (根据图片):
 * FF -> FE FD
 * FE -> FE FC
 */
static DL_Status_t append_escaped(uint8_t **ptr, uint16_t *len,
                                  uint8_t byte)
{
    if (byte == 0xFF) {
        if (append_byte(ptr, len, 0xFE) != DL_OK) return DL_ERROR;
        if (append_byte(ptr, len, 0xFD) != DL_OK) return DL_ERROR;
    } else if (byte == 0xFE) {
        if (append_byte(ptr, len, 0xFE) != DL_OK) return DL_ERROR;
        if (append_byte(ptr, len, 0xFC) != DL_OK) return DL_ERROR;
    } else {
        if (append_byte(ptr, len, byte) != DL_OK) return DL_ERROR;
    }
    return DL_OK;
}

DL_Status_t DL_LN33_BuildFrame(uint8_t src, uint8_t dst, uint16_t addr,
                               const uint8_t *payload, uint16_t payload_len,
                               uint8_t **frame_out, uint16_t *frame_len_out)
{
    if (payload_len > (DL_LN33_MAX_FRAME - 10U)) { // 预留足够的转义空间
        return DL_ERROR;
    }
    if ((frame_out == NULL) || (frame_len_out == NULL)) {
        return DL_INVALID;
    }

    uint8_t *p = tx_buf;
    uint16_t len = 0;

    /* 1. 包头 FE */
    if (append_byte(&p, &len, 0xFE) != DL_OK) return DL_ERROR;
    
    /* 2. 长度 (数据长度+4), 不转义 */
    if (append_byte(&p, &len, calc_len(payload_len)) != DL_OK) return DL_ERROR;
    
    /* 3. 源端口 (转义) */
    if (append_escaped(&p, &len, src) != DL_OK) return DL_ERROR;
    
    /* 4. 目的端口 (转义) */
    if (append_escaped(&p, &len, dst) != DL_OK) return DL_ERROR;
    
    /* 5. 远程地址 (小端, 转义) */
    if (append_escaped(&p, &len, (uint8_t)(addr & 0xFFU)) != DL_OK) return DL_ERROR;
    if (append_escaped(&p, &len, (uint8_t)((addr >> 8) & 0xFFU)) != DL_OK) return DL_ERROR;

    /* 6. 数据 (转义) */
    for (uint16_t i = 0; i < payload_len; ++i) {
        if (append_escaped(&p, &len, payload[i]) != DL_OK) return DL_ERROR;
    }

    /* 7. 包尾 FF */
    if (append_byte(&p, &len, 0xFF) != DL_OK) return DL_ERROR;

    frame_ptr = tx_buf;
    frame_len = len;
    *frame_out = tx_buf;
    *frame_len_out = len;
    return DL_OK;
}

DL_Status_t DL_LN33_SendPacket(uint8_t src, uint8_t dst, uint16_t addr,
                               const uint8_t *payload, uint16_t payload_len)
{
    uint8_t *frame = NULL;
    uint16_t len = 0;
    DL_Status_t st = DL_LN33_BuildFrame(src, dst, addr, payload, payload_len,
                                        &frame, &len);
    if (st != DL_OK) {
        return st;
    }

    if (HAL_UART_Transmit(DL_LN33_DEFAULT_UART, frame, len, 1000U) != HAL_OK) {
        return DL_ERROR;
    }
    return DL_OK;
}

DL_Status_t DL_LN33_SendFrameIT(UART_HandleTypeDef *huart,
                                const uint8_t *frame,
                                uint16_t length)
{
    if ((frame == NULL) || (length == 0U)) {
        return DL_ERROR;
    }

    if (huart == NULL) {
        huart = DL_LN33_DEFAULT_UART;
    }

    if (huart == NULL) {
        return DL_INVALID;
    }

    if (HAL_UART_Transmit_IT(huart, (uint8_t *)frame, length) != HAL_OK) {
        return DL_ERROR;
    }

    return DL_OK;
}

/* 解包函数：处理转义并校验 */
static DL_Status_t dl_extract_packet(const uint8_t *frame,
                                     uint16_t length,
                                     DL_Packet_t *pkt)
{
    if ((frame == NULL) || (length < 4U) || (pkt == NULL)) {
        return DL_INVALID;
    }

    /* 校验包头包尾 */
    if (frame[0] != 0xFE || frame[length - 1U] != 0xFF) {
        return DL_ERROR;
    }

    /* 获取声明的长度 (DataLen + 4) */
    uint8_t declared_len = frame[1];

    /* 开始解码 (跳过 FE 和 Length) */
    uint16_t idx = 2U;
    uint8_t decoded_buf[128];
    uint16_t decoded_cnt = 0;

    while (idx < (length - 1U)) {
        uint8_t b = frame[idx++];

        if (b == 0xFE) {
            /* 处理转义 */
            if (idx >= (length - 1U)) return DL_ERROR; // 转义符后无数据
            uint8_t next = frame[idx++];
            if (next == 0xFD) {
                b = 0xFF;
            } else if (next == 0xFC) {
                b = 0xFE;
            } else {
                return DL_ERROR; // 无效转义
            }
        }
        /* 注意：FF 不应出现在此处，因为 FF 是包尾，且数据中的 FF 已被转义 */

        if (decoded_cnt < sizeof(decoded_buf)) {
            decoded_buf[decoded_cnt++] = b;
        } else {
            return DL_ERROR; // 缓冲区溢出
        }
    }

    /* 校验解码后的长度是否符合声明 */
    if (decoded_cnt != declared_len) {
        return DL_ERROR;
    }
    if (decoded_cnt < 4) {
        return DL_ERROR; // 至少包含 Src, Dst, AddrL, AddrH
    }

    /* 填充结构体 */
    pkt->src = decoded_buf[0];
    pkt->dst = decoded_buf[1];
    pkt->addr = (uint16_t)decoded_buf[2] | ((uint16_t)decoded_buf[3] << 8);
    
    pkt->data_len = decoded_cnt - 4;
    if (pkt->data_len > 0) {
        memcpy(pkt->data, &decoded_buf[4], pkt->data_len);
    }

    return DL_OK;
}

DL_Status_t DL_LN33_RecvPacket(DL_Packet_t *pkt, uint32_t timeout_ms)
{
    if (pkt == NULL) {
        return DL_INVALID;
    }

    uint32_t start = HAL_GetTick();
    uint8_t buf[DL_LN33_MAX_FRAME];
    uint16_t len = 0U;

    rx_reset();

    while ((HAL_GetTick() - start) < timeout_ms) {
        if (HAL_UART_Receive(DL_LN33_DEFAULT_UART, buf + len, 1U, 1U) == HAL_OK) {
            uint8_t byte = buf[len];

            if (rx_state == RX_STATE_IDLE) {
                if (byte == 0xFE) {
                    len = 0;
                    buf[len++] = byte;
                    rx_state = RX_STATE_FRAME;
                }
            } else if (rx_state == RX_STATE_FRAME) {
                if (len < DL_LN33_MAX_FRAME) {
                    buf[len++] = byte;
                    if (byte == 0xFF) {
                        /* 收到包尾，进行解包 */
                        DL_Status_t st = dl_extract_packet(buf, len, pkt);
                        rx_reset();
                        return st;
                    }
                } else {
                    rx_reset(); // 超长复位
                }
            }
        }
    }

    rx_reset();
    return DL_TIMEOUT;
}

void DL_LN33_RxByteIT(uint8_t byte)
{
    if (rx_frame_ready) {
        return;
    }

    switch (rx_state) {
    case RX_STATE_IDLE:
        if (byte == 0xFE) {
            rx_len = 0U;
            rx_buf[rx_len++] = byte;
            rx_state = RX_STATE_FRAME;
        }
        break;

    case RX_STATE_FRAME:
        if (rx_len < DL_LN33_RX_BUF_SIZE) {
            rx_buf[rx_len++] = byte;
            if (byte == 0xFF) {
                /* 收到包尾，标记帧就绪 */
                /* 注意：因为数据中的 FF 必被转义为 FE FD，所以 FF 只能是包尾 */
                rx_frame_ready = 1U;
                rx_state = RX_STATE_IDLE;
            }
        } else {
            rx_reset(); // 溢出复位
        }
        break;

    default:
        rx_reset();
        break;
    }
}

uint8_t DL_LN33_IsRxFrameReady(void)
{
    return rx_frame_ready;
}

uint16_t DL_LN33_GetRxFrame(uint8_t *buf, uint16_t buf_len)
{
    if (!rx_frame_ready || buf == NULL) {
        return 0;
    }

    uint16_t copy_len = (rx_len < buf_len) ? rx_len : buf_len;
    memcpy(buf, rx_buf, copy_len);
    rx_reset();
    return copy_len;
}

/* 将字节数组转成十六进制字符串，如 "FE 08 91 90 ..." */
void bytes_to_hex_str(const uint8_t *buf, uint16_t len, char *out, uint16_t out_size)
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


void DL_LN33_Read_data(const uint8_t *buf, uint16_t len, DL_Packet_t *repkt)
{
    char temp[128];
    

    if (buf == NULL || repkt->data == NULL) {
        return;
    }

    uint16_t idx = 6; // 跳过包头等
    uint16_t data_idx = 0;

    repkt->data_len = buf[1] - 4;
    repkt->src = buf[2];
    repkt->dst = buf[3];
    repkt->addr = (uint16_t)buf[4] | ((uint16_t)buf[5] << 8);

    while (buf[idx] != 0xFF) { // 遇到包尾 FF 停止
        
        uint8_t byte = buf[idx];

        if (byte == 0xFE) {
            // 处理转义
            uint8_t next = buf[idx+1];
            if (next == 0xFD) {
                byte = 0xFF;
            } else if (next == 0xFC) {
                byte = 0xFE;
            } else {
                byte = 0xFE;
            }
        }

        repkt->data[data_idx++] = byte;
        idx++;
    }
}




//数据拆分装填函数
DL_Status_t DL_LN33_Load_Data(uint8_t *content, uint8_t content_p, int data)
{
    
    if(data > 0)
       {
        uint8_t part = 1;
        content_p++;
        while (data > 99)
        {
          content[content_p] = data % 100;
          content_p++;
          data = data / 100;
          part++;
        }
        content[content_p] = data;
        content[content_p - part] = part;
        content_p++;
        return DL_OK;
       }
    return DL_INVALID;
}



