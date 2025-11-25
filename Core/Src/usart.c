/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "bluetooth.h"
#include "dl_ln33.h"
#define UART_PRINTF_BUF_SIZE 256


char disp_buf[128] = {0};
char bt_line_buf[128] = {0};
uint8_t bt_line_idx = 0;
uint8_t usart2_has_new = 0;

/* ==== USARTx 中断收发相关（1/2/3 通用） ==== */

/* USART1 */
volatile uint8_t  uart1_tx_done = 0;
volatile uint8_t  uart1_rx_done = 0;
uint8_t           uart1_rx_buf[UART_IT_RX_BUF_SIZE];
uint16_t          uart1_rx_len = 0;

/* USART2 */
volatile uint8_t  uart2_tx_done = 0;
volatile uint8_t  uart2_rx_done = 0;
uint8_t           uart2_rx_buf[UART_IT_RX_BUF_SIZE];
uint16_t          uart2_rx_len = 0;

/* USART3 */
volatile uint8_t  uart3_tx_done = 0;
volatile uint8_t  uart3_rx_done = 0;
uint8_t           uart3_rx_buf[UART_IT_RX_BUF_SIZE];
uint16_t          uart3_rx_len = 0;

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}
/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);

    /* USART3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/* 阻塞发送任意数据 */
HAL_StatusTypeDef UART_SendEx(UART_HandleTypeDef *huart, const uint8_t *data, uint16_t len, uint32_t timeout)
{
    if (!huart || !data || !len) return HAL_ERROR;
    return HAL_UART_Transmit(huart, (uint8_t*)data, len, timeout);
}

/* 发送以 '\0' 结尾字符串 */
HAL_StatusTypeDef UART_SendStringEx(UART_HandleTypeDef *huart, const char *s)
{
    if (!s) return HAL_ERROR;
    return UART_SendEx(huart, (const uint8_t*)s, (uint16_t)strlen(s), 1000);
}

/* printf 风格输出（本地缓冲 256 字节） */
HAL_StatusTypeDef UART_PrintfEx(UART_HandleTypeDef *huart, const char *fmt, ...)
{
    if (!fmt) return HAL_ERROR;
    char buf[UART_PRINTF_BUF_SIZE];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (n < 0) return HAL_ERROR;
    if (n > (int)sizeof(buf)) n = sizeof(buf); /* 截断 */
    return UART_SendEx(huart, (uint8_t*)buf, (uint16_t)n, 1000);
}

/* 阻塞接收固定长度 */
HAL_StatusTypeDef UART_ReceiveEx(UART_HandleTypeDef *huart, uint8_t *buf, uint16_t len, uint32_t timeout)
{
    if (!huart || !buf || !len) return HAL_ERROR;
    return HAL_UART_Receive(huart, buf, len, timeout);
}

/* 接收单字符 */
HAL_StatusTypeDef UART_ReceiveCharEx(UART_HandleTypeDef *huart, uint8_t *ch, uint32_t timeout)
{
    if (!huart || !ch) return HAL_ERROR;
    return HAL_UART_Receive(huart, ch, 1, timeout);
}

/* 阻塞发送任意数据（USART2） */
HAL_StatusTypeDef UART2_IT_Send(const uint8_t *data, uint16_t len)
{
    if (!data || !len) return HAL_ERROR;
    uart2_tx_done = 0;
    return HAL_UART_Transmit_IT(&huart2, (uint8_t *)data, len);
}

/* ====== 通用中断发送 ====== */
HAL_StatusTypeDef UART_IT_Send(UART_HandleTypeDef *huart, const uint8_t *data, uint16_t len)
{
    if (!huart || !data || !len) return HAL_ERROR;

    if (huart->Instance == USART1) {
        uart1_tx_done = 0;
    } else if (huart->Instance == USART2) {
        uart2_tx_done = 0;
    } else if (huart->Instance == USART3) {
        uart3_tx_done = 0;
    }

    return HAL_UART_Transmit_IT(huart, (uint8_t *)data, len);
}

/* ====== 通用中断接收到固定长度 ====== */
HAL_StatusTypeDef UART_IT_StartRecv(UART_HandleTypeDef *huart, uint16_t expect_len)
{
    if (!huart || expect_len == 0 || expect_len > UART_IT_RX_BUF_SIZE)
        return HAL_ERROR;

    if (huart->Instance == USART1) {
        uart1_rx_done = 0;
        uart1_rx_len  = expect_len;
        return HAL_UART_Receive_IT(&huart1, uart1_rx_buf, expect_len);
    } else if (huart->Instance == USART2) {
        uart2_rx_done = 0;
        uart2_rx_len  = expect_len;
        return HAL_UART_Receive_IT(&huart2, uart2_rx_buf, expect_len);
    } else if (huart->Instance == USART3) {
        uart3_rx_done = 0;
        uart3_rx_len  = expect_len;
        return HAL_UART_Receive_IT(&huart3, uart3_rx_buf, expect_len);
    }

    return HAL_ERROR; // 其他 UART 暂未支持
}

/* 完成发送回调 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        uart1_tx_done = 1;
    } else if (huart->Instance == USART2) {
        uart2_tx_done = 1;
    } else if (huart->Instance == USART3) {
        uart3_tx_done = 1;
    }
}

/* 接收完成回调 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        uart1_rx_done = 1;
        /* 数据已经在 uart1_rx_buf[0..uart1_rx_len-1] */
    } else if (huart->Instance == USART2) {
        uart2_rx_done = 1;
        /* 数据在 uart2_rx_buf[0..uart2_rx_len-1] */
    } else if (huart->Instance == USART3) {
        uart3_rx_done = 1;
        /* 数据在 uart3_rx_buf[0..uart3_rx_len-1] */
    }

    /* ===== 针对蓝牙（默认使用 USART2）的应用层钩子 ===== */
    if (huart == BT_DEFAULT_UART) {
        extern char disp_buf[128];
        extern uint8_t usart2_has_new;

        /* 这里我们是按 1 字节接收的 */
        uint8_t ch = uart2_rx_buf[0];

        if (ch == '\r') {
            /* 忽略裸回车 */
        } else if (ch == '\n') {
            /* 一行结束，拷贝到显示缓冲 */
            bt_line_buf[bt_line_idx] = '\0';
            strncpy(disp_buf, bt_line_buf, sizeof(disp_buf) - 1);
            disp_buf[sizeof(disp_buf) - 1] = '\0';
            usart2_has_new = 1;
            bt_line_idx = 0;  // 清空，准备下一行
        } else {
            /* 普通字符，累加到当前行（防溢出） */
            if (bt_line_idx < sizeof(bt_line_buf) - 1) {
                bt_line_buf[bt_line_idx++] = (char)ch;
            }
        }

        /* 继续下一次接收（按你期望长度，可改成固定 N 字节） */
        UART_IT_StartRecv(huart, 1);
    }

    /* 3. LN33 协议单字节输入（默认也在 USART2 上） */
    if (huart == DL_LN33_DEFAULT_UART) {
        /* 将当前收到的 1 字节喂给 LN33 状态机 */
        uint8_t ch = uart2_rx_buf[0];
        DL_LN33_RxByteIT(ch);
        /* 注意：若上面 BT_DEFAULT_UART 与 DL_LN33_DEFAULT_UART 相同，
           这里会再次喂相同字节，这是允许的，因为你要的就是“同一串口既接 LN33，又显示”。 */
    }
    

}
/* USER CODE END 1 */
