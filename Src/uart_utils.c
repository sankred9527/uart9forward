#include "uart_utils.h"
#include "mcu_timer.h"
#include "pt/pt.h"
#include "main.h"
#include "gpio_utils.h"
#include <stdarg.h>

uart_contex_t gl_all_uarts[5];

#define UART_UP (gl_all_uarts+4)

//#define MAX_UART_CHANNEL (2)
static int gl_current_channel = 0;

static struct {
    struct pt pt;
} gl_test1_pt;

static struct pt gl_recv_pt;
static struct pt gl_main_pt;

static void uart_send(uart_contex_t *ucontex, uint8_t *data, uint16_t size);

void mylog(char* format, ...) {
    va_list args;  // 定义一个变量参数列表
    static char buffer[128];  // 定义一个缓冲区

    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);
    
    va_end(args);

    (gl_all_uarts+3)->current_channel = 0;
    uart_send(gl_all_uarts+3, (uint8_t*)buffer, strlen(buffer));    
}

static void my_MspDeInitCallback(UART_HandleTypeDef *huart)
{
    uart_contex_t *ucontext = (uart_contex_t*)huart;
    //GPIO_InitTypeDef GPIO_InitStruct = {0};
    //int channel = ucontext->current_channel;

    /* Peripheral clock enable */
    switch(ucontext->index)
    {
        case 0:
            __HAL_RCC_USART1_CLK_DISABLE();
            break;
        case 1:
            __HAL_RCC_USART2_CLK_ENABLE();
            break;
        case 2:
            __HAL_RCC_USART3_CLK_DISABLE();
            break;
        case 3:
            __HAL_RCC_UART4_CLK_DISABLE();
            break;                                
        default :
            return;
    }
    
    // uint16_t tx_pin = ucontext->tx_gpio[channel].gpio_pin;
    // GPIO_TypeDef *tx_group = ucontext->tx_gpio[channel].gpiox;
    // uint16_t rx_pin = ucontext->rx_gpio[channel].gpio_pin;
    // GPIO_TypeDef *rx_group = ucontext->rx_gpio[channel].gpiox;
    // uint16_t rts_pin = ucontext->rts_gpio[channel].gpio_pin;
    // GPIO_TypeDef *rts_group = ucontext->rts_gpio[channel].gpiox;

    // HAL_GPIO_DeInit(tx_group, tx_pin);
    // HAL_GPIO_DeInit(rx_group, rx_pin);
    // if ( rts_pin > 0 )
    //     HAL_GPIO_DeInit(rts_group, rts_pin);

    /* USART interrupt DeInit */    
    HAL_NVIC_DisableIRQ(ucontext->irqt);
}

static void my_MspInitCallback(UART_HandleTypeDef *huart)
{
    uart_contex_t *ucontext = (uart_contex_t*)huart;
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    int channel = ucontext->current_channel;

    /* Peripheral clock enable */
    switch(ucontext->index)
    {
        case 0:
            __HAL_RCC_USART1_CLK_ENABLE();
            break;
        case 1:
            __HAL_RCC_USART2_CLK_ENABLE();
            break;
        case 2:
            __HAL_RCC_USART3_CLK_ENABLE();
            break;
        case 3:
            __HAL_RCC_UART4_CLK_ENABLE();
            break;
        case 4:            
            __HAL_RCC_UART5_CLK_ENABLE();
            break;
        default :
            return;
    }
    //不需要调用   __HAL_RCC_GPIO?_CLK_ENABLE(); 因为其他地方已经enable 了
    
    /**USART1 GPIO Configuration */    


    
    switch(ucontext->index)
    {
        case 0:
        case 1:
        case 2:
        {
            uint16_t tx_pin = ucontext->tx_gpio[channel].gpio_pin;
            GPIO_TypeDef *tx_group = ucontext->tx_gpio[channel].gpiox;
            uint16_t rx_pin = ucontext->rx_gpio[channel].gpio_pin;
            GPIO_TypeDef *rx_group = ucontext->rx_gpio[channel].gpiox;
            //uint16_t rts_pin = ucontext->rts_gpio[channel].gpio_pin;
            //GPIO_TypeDef *rts_group = ucontext->rts_gpio[channel].gpiox;

            GPIO_InitStruct.Pin = tx_pin;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
            HAL_GPIO_Init(tx_group, &GPIO_InitStruct);

            // GPIO_InitStruct.Pin = rts_pin;
            // GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            // GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
            // HAL_GPIO_Init(rts_group, &GPIO_InitStruct);

            GPIO_InitStruct.Pin = rx_pin;
            GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            HAL_GPIO_Init(rx_group, &GPIO_InitStruct);    
      
            switch(ucontext->index)
            {
            case 0:
                if (channel==0) __HAL_AFIO_REMAP_USART1_DISABLE(); else __HAL_AFIO_REMAP_USART1_ENABLE();
                break;
            case 1:
                if (channel==0) __HAL_AFIO_REMAP_USART2_DISABLE(); else __HAL_AFIO_REMAP_USART2_ENABLE();
                break;
            case 2:
                if (channel==0) __HAL_AFIO_REMAP_USART3_DISABLE(); else __HAL_AFIO_REMAP_USART3_ENABLE();
                break;
            }
        }
            break;
        case 3:
            //uart4 只有1组 tx/rx gpio，不需要切换
        
            GPIO_InitStruct.Pin = ucontext->tx_gpio[0].gpio_pin;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
            HAL_GPIO_Init(ucontext->tx_gpio[0].gpiox, &GPIO_InitStruct);

            GPIO_InitStruct.Pin = ucontext->rx_gpio[0].gpio_pin;
            GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            HAL_GPIO_Init(ucontext->rx_gpio[0].gpiox, &GPIO_InitStruct);

            for (int n =0; n<2; n++)
            {
                // GPIO_InitStruct.Pin = ucontext->rts_gpio[n].gpio_pin;
                // GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                // GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
                // HAL_GPIO_Init(ucontext->rts_gpio[n].gpiox, &GPIO_InitStruct);
            }            
            break;
        case 4:
            GPIO_InitStruct.Pin = ucontext->tx_gpio[0].gpio_pin;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
            HAL_GPIO_Init(ucontext->tx_gpio[0].gpiox, &GPIO_InitStruct);

            GPIO_InitStruct.Pin = ucontext->rx_gpio[0].gpio_pin;
            GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            HAL_GPIO_Init(ucontext->rx_gpio[0].gpiox, &GPIO_InitStruct);

            // GPIO_InitStruct.Pin = ucontext->rts_gpio[0].gpio_pin;
            // GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            // GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
            // HAL_GPIO_Init(ucontext->rts_gpio[0].gpiox, &GPIO_InitStruct);

            break;
        default:
            return;
    }

    /* USART/UART interrupt Init */
    HAL_NVIC_SetPriority(ucontext->irqt, 0, 0);
    HAL_NVIC_EnableIRQ(ucontext->irqt);     
}

static void my_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    uart_contex_t *ucontex = (uart_contex_t*)huart;
    if ( size >= UART_RX_BUFF_SIZE )
        size = UART_RX_BUFF_SIZE;

    ucontex->rx_completed = true;
    ucontex->rx_size = size;
}

void my_TxCpltCallback(UART_HandleTypeDef *huart)
{    
    uart_contex_t *ucontex = (uart_contex_t*)huart;
    
    switch (ucontex->index)
    {
        case 0:
        case 1:
        case 2:
            HAL_GPIO_WritePin(ucontex->rts_gpio[ucontex->current_channel].gpiox, ucontex->rts_gpio[ucontex->current_channel].gpio_pin, GPIO_PIN_RESET);
            break;
        case 3:
            HAL_GPIO_WritePin(ucontex->rts_gpio[0].gpiox, ucontex->rts_gpio[0].gpio_pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(ucontex->rts_gpio[1].gpiox, ucontex->rts_gpio[1].gpio_pin, GPIO_PIN_RESET);
            break;
        case 4:
            HAL_GPIO_WritePin(ucontex->rts_gpio[0].gpiox, ucontex->rts_gpio[0].gpio_pin, GPIO_PIN_RESET);
            break;
        default:
            return;
    }    
    ucontex->tx_completed = true;
}

void uart_app_init(void)
{
    PT_INIT((struct pt*)&gl_test1_pt);
    PT_INIT(&gl_main_pt);
    PT_INIT(&gl_recv_pt);

    #define MAKE_INSTANCE(x) {\
        .Instance = x,\
        .Init= {\
            .BaudRate = 115200,\
            .WordLength = UART_WORDLENGTH_8B,\
            .StopBits = UART_STOPBITS_1,\
            .Parity = UART_PARITY_NONE,\
            .Mode = UART_MODE_TX_RX,\
            .HwFlowCtl = UART_HWCONTROL_NONE,\
            .OverSampling = UART_OVERSAMPLING_16\
        }\
    }
    
    memset(gl_all_uarts, 0, sizeof(gl_all_uarts));
    
    gl_all_uarts[0] = (uart_contex_t) {
        .uart_instance = MAKE_INSTANCE(USART1),
        .irqt = USART1_IRQn,
        .tx_gpio = {  { .gpiox = TXD1_0_GPIO_Port, .gpio_pin = TXD1_0_Pin } ,  { .gpiox = TXD1_1_GPIO_Port, .gpio_pin = TXD1_1_Pin } },
        .rx_gpio = {  { .gpiox = RXD1_0_GPIO_Port, .gpio_pin = RXD1_0_Pin } ,  { .gpiox = RXD1_1_GPIO_Port, .gpio_pin = RXD1_1_Pin } },
        .rts_gpio = { { .gpiox = RTS1_0_GPIO_Port, .gpio_pin = RTS1_0_Pin } ,  { .gpiox = RTS1_1S_GPIO_Port, .gpio_pin = RTS1_1S_Pin } },
        .rled_gpio = { { .gpiox = LR2_GPIO_Port, .gpio_pin = LR2_Pin }, { .gpiox = LR3_GPIO_Port, .gpio_pin = LR3_Pin } },
        .gled_gpio = { { .gpiox = LG2_GPIO_Port, .gpio_pin = LG2_Pin }, { .gpiox = LG3_GPIO_Port, .gpio_pin = LG3_Pin } },
        .index = 0
    };

    gl_all_uarts[1] = (uart_contex_t) {
        .uart_instance = MAKE_INSTANCE(USART2),
        .irqt = USART2_IRQn,
        .tx_gpio = {  { .gpiox = TXD2_0_GPIO_Port, .gpio_pin = TXD2_0_Pin } ,  { .gpiox = TXD2_1_GPIO_Port, .gpio_pin = TXD2_1_Pin } },
        .rx_gpio = {  { .gpiox = RXD2_0_GPIO_Port, .gpio_pin = RXD2_0_Pin } ,  { .gpiox = RXD2_1_GPIO_Port, .gpio_pin = RXD2_1_Pin } },
        .rts_gpio = { { .gpiox = RTS2_0_GPIO_Port, .gpio_pin = RTS2_0_Pin } ,  { .gpiox = RTS2_1_GPIO_Port, .gpio_pin = RTS2_1_Pin } },
        .rled_gpio = { { .gpiox = LR4_GPIO_Port, .gpio_pin = LR4_Pin }, { .gpiox = LR5_GPIO_Port, .gpio_pin = LR5_Pin } },
        .gled_gpio = { { .gpiox = LG4_GPIO_Port, .gpio_pin = LG4_Pin }, { .gpiox = LG5_GPIO_Port, .gpio_pin = LG5_Pin } },
        .index = 1
    };

    gl_all_uarts[2] = (uart_contex_t) {
        .uart_instance = MAKE_INSTANCE(USART3),
        .irqt = USART3_IRQn,
        .tx_gpio = {  { .gpiox = TXD3_0_GPIO_Port, .gpio_pin = TXD3_0_Pin } ,  { .gpiox = TXD3_1_GPIO_Port, .gpio_pin = TXD3_1_Pin } },
        .rx_gpio = {  { .gpiox = RXD3_0_GPIO_Port, .gpio_pin = RXD3_0_Pin } ,  { .gpiox = RXD3_1_GPIO_Port, .gpio_pin = RXD3_1_Pin } },
        .rts_gpio = { { .gpiox = RTS3_0_GPIO_Port, .gpio_pin = RTS3_0_Pin } ,  { .gpiox = RTS3_1_GPIO_Port, .gpio_pin = RTS3_1_Pin } },
        .rled_gpio = { { .gpiox = LR6_GPIO_Port, .gpio_pin = LR6_Pin }, { .gpiox = LR7_GPIO_Port, .gpio_pin = LR7_Pin } },
        .gled_gpio = { { .gpiox = LG6_GPIO_Port, .gpio_pin = LG6_Pin }, { .gpiox = LG7_GPIO_Port, .gpio_pin = LG7_Pin } },
        .index = 2
    };
        
    gl_all_uarts[3] = (uart_contex_t) {
        .uart_instance = MAKE_INSTANCE(UART4),
        .irqt = UART4_IRQn,
        .tx_gpio = {{.gpiox = TXD4_GPIO_Port, .gpio_pin = TXD4_Pin}},
        .rx_gpio = {{.gpiox = RXD4_GPIO_Port, .gpio_pin = RXD4_Pin}},
        .rts_gpio = { { .gpiox = RTS4_0S_GPIO_Port, .gpio_pin = RTS4_0S_Pin } ,  { .gpiox = RTS4_1S_GPIO_Port, .gpio_pin = RTS4_1S_Pin } },
        .rled_gpio = { { .gpiox = LR8_GPIO_Port, .gpio_pin = LR8_Pin }, { .gpiox = LR9_GPIO_Port, .gpio_pin = LR9_Pin } },
        .gled_gpio = { { .gpiox = LG8_GPIO_Port, .gpio_pin = LG8_Pin }, { .gpiox = LG9_GPIO_Port, .gpio_pin = LG9_Pin } },
        .index = 3
    };

    gl_all_uarts[4] = (uart_contex_t) {
        .uart_instance = MAKE_INSTANCE(UART5),
        .irqt = UART5_IRQn,
        .tx_gpio = {{.gpiox = TXD5_GPIO_Port, .gpio_pin = TXD5_Pin}},
        .rx_gpio = {{.gpiox = RXD5_GPIO_Port, .gpio_pin = RXD5_Pin}},
        .rts_gpio = { { .gpiox = RTS5S_GPIO_Port, .gpio_pin = RTS5S_Pin } },
        .rled_gpio = { { .gpiox = LR1_GPIO_Port, .gpio_pin = LR1_Pin }},
        .gled_gpio = { { .gpiox = LG1_GPIO_Port, .gpio_pin = LG1_Pin }},
        .index = 4
    };
    
    for(int n = 0; n < 5; n++) {        
        if  ( HAL_OK != HAL_UART_RegisterCallback(&UART_GET_HANDLE(n),  HAL_UART_MSPINIT_CB_ID, my_MspInitCallback) ) {
        
        }

        if ( HAL_OK != HAL_UART_RegisterCallback(&UART_GET_HANDLE(n),  HAL_UART_MSPDEINIT_CB_ID, my_MspDeInitCallback) ) {

        }

        if ( HAL_UART_Init(&UART_GET_HANDLE(n)) != HAL_OK )
        {
            Error_Handler();
        }
        
        HAL_UART_RegisterCallback(&UART_GET_HANDLE(n),  HAL_UART_TX_COMPLETE_CB_ID, my_TxCpltCallback);
        HAL_UART_RegisterRxEventCallback(&UART_GET_HANDLE(n), my_RxEventCallback );        
    }

}

void uart_app_init_device_with_channel(int channel)
{
    if ( channel < 0 || channel > 1 )
        return; 
    
    if ( channel == gl_current_channel) 
        return;

    for (int n = 0; n <= 3; n++)
    {
        uart_contex_t *uart_contex = gl_all_uarts + n;
    
        HAL_UART_DeInit(&uart_contex->uart_instance);

        uart_contex->current_channel = channel;
        if (HAL_UART_Init(&uart_contex->uart_instance) != HAL_OK)
        {
            Error_Handler();
        }
    }
}

static void uart_send(uart_contex_t *ucontex, uint8_t *data, uint16_t size)
{    
    switch (ucontex->index)
    {
    case 0:
    case 1:
    case 2:
        HAL_GPIO_WritePin(ucontex->rts_gpio[ucontex->current_channel].gpiox, ucontex->rts_gpio[ucontex->current_channel].gpio_pin, GPIO_PIN_SET);        
        break;
    case 3:
        if ( ucontex->current_channel == 0 ) {
            HAL_GPIO_WritePin(ucontex->rts_gpio[0].gpiox, ucontex->rts_gpio[0].gpio_pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(ucontex->rts_gpio[1].gpiox, ucontex->rts_gpio[1].gpio_pin, GPIO_PIN_RESET);
        } else {
            HAL_GPIO_WritePin(ucontex->rts_gpio[0].gpiox, ucontex->rts_gpio[0].gpio_pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(ucontex->rts_gpio[1].gpiox, ucontex->rts_gpio[1].gpio_pin, GPIO_PIN_SET);
        }
        break;
    case 4:
        HAL_GPIO_WritePin(ucontex->rts_gpio[0].gpiox, ucontex->rts_gpio[0].gpio_pin, GPIO_PIN_SET);
        break;
    default:
        return;
    }
    ucontex->tx_completed = false;
    HAL_UART_Transmit_IT(&ucontex->uart_instance, data, size); 
}


static void uart_recv(uart_contex_t *ucontex, uint8_t *data, uint16_t size)
{    
    switch (ucontex->index)
    {
    case 0:
    case 1:
    case 2:
        HAL_GPIO_WritePin(ucontex->rts_gpio[ucontex->current_channel].gpiox, ucontex->rts_gpio[ucontex->current_channel].gpio_pin, GPIO_PIN_RESET);
        break;
    case 3:
        HAL_GPIO_WritePin(ucontex->rts_gpio[0].gpiox, ucontex->rts_gpio[0].gpio_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(ucontex->rts_gpio[1].gpiox, ucontex->rts_gpio[1].gpio_pin, GPIO_PIN_RESET);
        break;
    case 4:
        HAL_GPIO_WritePin(ucontex->rts_gpio[0].gpiox, ucontex->rts_gpio[0].gpio_pin, GPIO_PIN_RESET);
        break;
    default:
        return;
    }
    ucontex->rx_completed = false;
    ucontex->rx_size = 0;
    HAL_UARTEx_ReceiveToIdle_IT(&ucontex->uart_instance, data, size);
}

#define TX_FINISH (gl_all_uarts[0].tx_completed && gl_all_uarts[1].tx_completed && gl_all_uarts[2].tx_completed && gl_all_uarts[3].tx_completed)
#define RX_FINISH (gl_all_uarts[0].rx_completed || gl_all_uarts[1].rx_completed || gl_all_uarts[2].rx_completed || gl_all_uarts[3].rx_completed)


char uart_thread(void)
{
    static struct pt *pt = (struct pt*)&gl_main_pt;
    static struct mcu_timer timer;
    static int channel ;    
    static int to_send_index = -1;


    PT_BEGIN(pt);
    while(1)
    {        
        PT_WAIT_UNTIL(pt, UART_UP->rx_completed);
        if ( UART_UP->rx_size == 0 ) 
            continue;

        to_send_index = -1;
        for (channel=0;channel<2;channel++)
        {
            uart_app_init_device_with_channel(channel);
            for (int n = 0; n < 4; n++)
                uart_send(gl_all_uarts+n, UART_UP->rx_buffer, UART_UP->rx_size);        
            timer_set(&timer,100);        
            PT_WAIT_UNTIL(pt,  TX_FINISH || timer_expired(&timer) );

            if ( !TX_FINISH ) {
                continue;
            }
            
            for (int n = 0; n < 4; n++)
                uart_recv(gl_all_uarts+n, gl_all_uarts[n].rx_buffer, UART_RX_BUFF_SIZE);

            timer_set(&timer,100);
            PT_WAIT_UNTIL(pt, RX_FINISH || timer_expired(&timer) );
            if ( !RX_FINISH ) 
                continue;            

            for (int n = 0; n < 4; n++)
            {
                if ( gl_all_uarts[n].rx_size > 0 ) 
                {
                    to_send_index = n;
                    break;
                }
            }
            if ( to_send_index >= 0 )
            {
                //send to up
                uart_send(UART_UP, gl_all_uarts[to_send_index].rx_buffer, gl_all_uarts[to_send_index].rx_size);
                timer_set(&timer,100);
                PT_WAIT_UNTIL(pt,  UART_UP->rx_completed || timer_expired(&timer) );
                break;
            }
        }
        
        UART_UP->rx_size = 0;
    }
    PT_END(pt);
}

char uart_recv_up_serial_thread(void)
{
    static struct pt * pt = (struct pt*)&gl_recv_pt;
    PT_BEGIN(pt);

    while(1)
    {                
        uart_recv(UART_UP, UART_UP->rx_buffer, UART_RX_BUFF_SIZE);
        PT_WAIT_UNTIL(pt, UART_UP->rx_completed); 

        //等待处理完成
        PT_WAIT_UNTIL(pt, UART_UP->rx_size == 0 );
    }

    PT_END(pt);
}

char uart_test1_thread(void)
{
    static struct pt * pt = (struct pt*)&gl_test1_pt;
    static struct mcu_timer timer;
    
    PT_BEGIN(pt);
    while (1)
    {
        static uint8_t data[] = "hello\r\n";

        timer_set(&timer,1000);
        PT_WAIT_UNTIL(pt, timer_expired(&timer));        
        SetLed(1,8,0);

        SetLed(2,0,1);
        uart_send(UART_UP, data, sizeof(data));
        PT_WAIT_UNTIL(pt, UART_UP->tx_completed);
        SetLed(2,0,0);

        timer_set(&timer,1000);
        PT_WAIT_UNTIL(pt, timer_expired(&timer));
        SetLed(1,8,1);    
        
        //uart_send(UART_UP, "hello\r\n", 7);
        //PT_WAIT_UNTIL(pt, UART_UP->tx_completed);
    }
    PT_END(pt);
}
