#ifndef UART_UTILS_H
#define UART_UTILS_H


#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include "pt/pt.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "mcu_timer.h"
#include "ringbuffer.h"

#define UART_RX_BUFF_SIZE (32)


typedef struct  {
    GPIO_TypeDef *gpiox;
    uint16_t gpio_pin;
} gpio_combine_t;

typedef struct __uart_contex {
    UART_HandleTypeDef uart_instance;
    volatile bool tx_completed; 
    volatile bool rx_completed; 
    volatile uint32_t rx_size;
    uint8_t rx_buffer[1];

    ring_buffer_t ringbuf;
    char _ringbuf[UART_RX_BUFF_SIZE];

    ring_buffer_t send_ringbuf;
    char _send_ringbuf[UART_RX_BUFF_SIZE];
    char lowlevel_tx_buffer[UART_RX_BUFF_SIZE];

    IRQn_Type irqt;
    int current_channel;
    int index;

    /*
     usart1-5 通过端口复用来配置, 需要定义2组不同的gpio
    */
    gpio_combine_t tx_gpio[2]; 
    gpio_combine_t rx_gpio[2];    
    gpio_combine_t rts_gpio[2];
    gpio_combine_t rled_gpio[2]; //red led
    gpio_combine_t gled_gpio[2]; //green led
    
    struct mcu_timer test_timer;

    struct pt pt_send;
    struct pt pt_recv;
    struct pt pt_led;
    volatile bool led_tx_onoff;
    struct mcu_timer led_tx_timer;    
    volatile bool led_rx_onoff;
    struct mcu_timer led_rx_timer;
} uart_contex_t;

extern uart_contex_t gl_all_uarts[5];
#define UART_GET_HANDLE(n) (gl_all_uarts[n].uart_instance)

char uart_test1_thread(void);
void uart_thread(void);
void uart_app_init(void);

#endif

