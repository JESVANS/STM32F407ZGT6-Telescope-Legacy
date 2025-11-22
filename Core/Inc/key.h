#ifndef __KEY_H__
#define __KEY_H__
#include "stm32f4xx_hal.h"

typedef enum {
    KEY_NONE = 0,
    KEY_WK_UP,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_OK,
} KeyID_t;

typedef enum {
    KEY_EVT_NONE = 0,
    KEY_EVT_PRESS,        // 按下（通过消抖确认）
    KEY_EVT_RELEASE,      // 松开
    KEY_EVT_CLICK,        // 单击
    KEY_EVT_DOUBLE_CLICK, // 双击
    KEY_EVT_LONG_PRESS    // 长按(只触发一次)
} KeyEventType_t;

typedef struct {
    KeyID_t id;
    KeyEventType_t type;
} KeyEvent_t;

void KEY_Init(void);
void KEY_Update(void);          // 固定周期调用（建议每10ms）
int  KEY_GetEvent(KeyEvent_t *e); // 获取事件，返回1表示拿到

#endif