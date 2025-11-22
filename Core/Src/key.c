#include "key.h"
#include <string.h>

/* 引脚宏 */
#define WK_UP_PORT   GPIOA
#define WK_UP_PIN    GPIO_PIN_0    // 下拉输入, 按下=1

#define K_UP_PORT    GPIOC
#define K_UP_PIN     GPIO_PIN_1    // 上拉输入, 按下=0
#define K_DOWN_PORT  GPIOE
#define K_DOWN_PIN   GPIO_PIN_4
#define K_LEFT_PORT  GPIOE
#define K_LEFT_PIN   GPIO_PIN_2
#define K_RIGHT_PORT GPIOC
#define K_RIGHT_PIN  GPIO_PIN_0
#define K_OK_PORT    GPIOE
#define K_OK_PIN     GPIO_PIN_3

/* 时序参数 (ms) */
#define DEBOUNCE_MS        20
#define LONG_PRESS_MS      600
#define DOUBLE_CLICK_GAPMS 300
#define SCAN_INTERVAL_MS   10   // 与 KEY_Update 实际调用周期匹配

/* 事件队列 */
#define KEY_EVENT_BUF_SIZE 16
static KeyEvent_t evt_buf[KEY_EVENT_BUF_SIZE];
static volatile uint8_t evt_head = 0, evt_tail = 0;
static void push_evt(KeyID_t id, KeyEventType_t t) {
    uint8_t n = (evt_head + 1) % KEY_EVENT_BUF_SIZE;
    if (n == evt_tail) return; // 满丢弃
    evt_buf[evt_head].id = id;
    evt_buf[evt_head].type = t;
    evt_head = n;
}

/* 单个按键状态机 */
typedef struct {
    KeyID_t id;
    GPIO_TypeDef *port;
    uint16_t pin;
    uint8_t active_level;   // 1:按下为高, 0:按下为低
    uint8_t stable_state;   // 0:未按,1:按下 (消抖后)
    uint8_t last_raw;       // 上次原始电平
    uint16_t debounce_cnt;
    uint16_t press_time;    // 已按下时长
    uint16_t release_time;  // 松开后时长(用于双击识别)
    uint8_t click_pending;  // 已有首次点击等待双击
    uint8_t long_reported;  // 长按事件是否已发
} KeySM_t;

/* 按键列表 */
static KeySM_t keys[] = {
    { KEY_WK_UP,  WK_UP_PORT,  WK_UP_PIN, 1,0,0,0,0,0,0,0 },
    { KEY_UP,     K_UP_PORT,   K_UP_PIN,  0,0,0,0,0,0,0,0 },
    { KEY_DOWN,   K_DOWN_PORT, K_DOWN_PIN,0,0,0,0,0,0,0,0 },
    { KEY_LEFT,   K_LEFT_PORT, K_LEFT_PIN,0,0,0,0,0,0,0,0 },
    { KEY_RIGHT,  K_RIGHT_PORT,K_RIGHT_PIN,0,0,0,0,0,0,0,0 },
    { KEY_OK,     K_OK_PORT,   K_OK_PIN,  0,0,0,0,0,0,0,0 },
};
#define KEY_NUM (sizeof(keys)/sizeof(keys[0]))

void KEY_Init(void)
{
    evt_head = evt_tail = 0;
    for (uint32_t i=0;i<KEY_NUM;i++) {
        keys[i].stable_state = 0;
        keys[i].last_raw = 0;
        keys[i].debounce_cnt = 0;
        keys[i].press_time = 0;
        keys[i].release_time = 0;
        keys[i].click_pending = 0;
        keys[i].long_reported = 0;
    }
}

/* 读取按下状态原始值 (未消抖) */
static uint8_t read_pressed(KeySM_t *k)
{
    GPIO_PinState s = HAL_GPIO_ReadPin(k->port, k->pin);
    return (s == GPIO_PIN_SET) ? k->active_level : (uint8_t)(!k->active_level);
}

void KEY_Update(void)
{
    for (uint32_t i=0;i<KEY_NUM;i++) {
        KeySM_t *k = &keys[i];
        uint8_t raw_pressed = read_pressed(k);

        if (raw_pressed != k->last_raw) {
            k->debounce_cnt = 0;
            k->last_raw = raw_pressed;
        } else {
            if (k->debounce_cnt < DEBOUNCE_MS/SCAN_INTERVAL_MS)
                k->debounce_cnt++;
            else {
                /* 达到消抖 */
                if (raw_pressed != k->stable_state) {
                    k->stable_state = raw_pressed;
                    if (raw_pressed) { // 按下事件
                        push_evt(k->id, KEY_EVT_PRESS);
                        k->press_time = 0;
                        k->long_reported = 0;
                        // 若上次有 click_pending 且未超双击时间, 直接判定双击
                        if (k->click_pending &&
                            k->release_time <= DOUBLE_CLICK_GAPMS/SCAN_INTERVAL_MS) {
                            push_evt(k->id, KEY_EVT_DOUBLE_CLICK);
                            k->click_pending = 0;
                        }
                    } else { // 松开事件
                        push_evt(k->id, KEY_EVT_RELEASE);
                        if (!k->long_reported) {
                            // 可能是单击或等待双击
                            k->click_pending = 1;
                            k->release_time = 0;
                        }
                        k->press_time = 0;
                    }
                } else {
                    /* 状态稳定维持中 */
                    if (k->stable_state) {
                        if (k->press_time < 0xFFFF) k->press_time++;
                        if (!k->long_reported &&
                            k->press_time >= LONG_PRESS_MS/SCAN_INTERVAL_MS) {
                            push_evt(k->id, KEY_EVT_LONG_PRESS);
                            k->long_reported = 1;
                            k->click_pending = 0; // 长按不再当单击
                        }
                    } else {
                        if (k->click_pending) {
                            if (k->release_time < 0xFFFF) k->release_time++;
                            if (k->release_time > DOUBLE_CLICK_GAPMS/SCAN_INTERVAL_MS) {
                                push_evt(k->id, KEY_EVT_CLICK);
                                k->click_pending = 0;
                            }
                        }
                    }
                }
            }
        }
    }
}

int KEY_GetEvent(KeyEvent_t *e)
{
    if (evt_tail == evt_head) return 0;
    *e = evt_buf[evt_tail];
    evt_tail = (evt_tail + 1) % KEY_EVENT_BUF_SIZE;
    return 1;
}