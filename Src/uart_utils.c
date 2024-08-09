#include "uart_utils.h"
#include "mcu_timer.h"
#include "pt/pt.h"
#include "main.h"
#include "gpio_utils.h"
#include <stdarg.h>



uart_contex_t gl_all_uarts[5];

#define UART_NUM (5)
#define UART_UP (gl_all_uarts+(UART_NUM-1))

//#define MAX_UART_CHANNEL (2)
static int gl_current_channel = 0;

static struct {
    struct pt pt;
} gl_test1_pt;


static void uart_led_close(uart_contex_t *ucontex);
static void uart_led_deal(uart_contex_t *ucontex, int type , bool on_off);

void mylog(char* format, ...) {
    va_list args;  // 定义一个变量参数列表
    static char buffer[128];  // 定义一个缓冲区

    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);
    
    va_end(args);

    (gl_all_uarts+3)->current_channel = 0;
    //uart_send(gl_all_uarts+3, (uint8_t*)buffer, strlen(buffer));
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
             GPIO_InitStruct.Pin = ucontext->tx_gpio[0].gpio_pin;
             GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
             GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
             HAL_GPIO_Init(ucontext->tx_gpio[0].gpiox, &GPIO_InitStruct);

             GPIO_InitStruct.Pin = ucontext->rx_gpio[0].gpio_pin;
             GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
             GPIO_InitStruct.Pull = GPIO_NOPULL;
             HAL_GPIO_Init(ucontext->rx_gpio[0].gpiox, &GPIO_InitStruct);

			 GPIO_InitStruct.Pin = ucontext->rts_gpio[0].gpio_pin;
			 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			 HAL_GPIO_Init(ucontext->rts_gpio[0].gpiox, &GPIO_InitStruct);

             HAL_GPIO_WritePin(ucontext->rts_gpio[0].gpiox, ucontext->rts_gpio[0].gpio_pin, GPIO_PIN_RESET);

//             if (ucontext->index== 0)
//             {
//            	 while (1)
//            	 {
//            		 for(int n =0; n  < 5;n++)
//            			 uart_led_close(gl_all_uarts+n);
//            		 HAL_GPIO_WritePin(ucontext->rts_gpio[0].gpiox, ucontext->rts_gpio[0].gpio_pin, GPIO_PIN_RESET);
//            	 }
//
//             }

			switch(ucontext->index)
			{
			case 0:
				__HAL_AFIO_REMAP_USART1_ENABLE();
				break;
			case 1:
				//???
				__HAL_AFIO_REMAP_USART2_DISABLE();
				break;
			case 2:
				//???
				__HAL_AFIO_REMAP_USART3_ENABLE();
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
			GPIO_InitStruct.Pull = GPIO_PULLUP;
			HAL_GPIO_Init(ucontext->rx_gpio[0].gpiox, &GPIO_InitStruct);

			GPIO_InitStruct.Pin = ucontext->rts_gpio[0].gpio_pin;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(ucontext->rts_gpio[0].gpiox, &GPIO_InitStruct);

			HAL_GPIO_WritePin(ucontext->rts_gpio[0].gpiox, ucontext->rts_gpio[0].gpio_pin, GPIO_PIN_RESET);
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


void my_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uart_contex_t *ucontex = (uart_contex_t*)huart;
    ring_buffer_queue(&ucontex->ringbuf, ucontex->rx_buffer[0]);
//    if ( ucontex->index == 0 )
//    	while(1);

    if ( HAL_OK != HAL_UART_Receive_IT(&ucontex->uart_instance, ucontex->rx_buffer, 1) )
    {
    	//while(1);
    }
}

void my_TxCpltCallback(UART_HandleTypeDef *huart)
{    
    uart_contex_t *ucontex = (uart_contex_t*)huart;
    
    switch (ucontex->index)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        	HAL_GPIO_WritePin(ucontex->rts_gpio[0].gpiox, ucontex->rts_gpio[0].gpio_pin, GPIO_PIN_RESET);
            break;
        case 4:
            HAL_GPIO_WritePin(ucontex->rts_gpio[0].gpiox, ucontex->rts_gpio[0].gpio_pin, GPIO_PIN_RESET);
            break;
        default:
            return;
    }    
    ucontex->tx_completed = true;
    ucontex->led_tx_onoff = true;
}

void my_ErrorCallback(UART_HandleTypeDef *huart)
{
    static int cnt = 0;
    if ((huart->ErrorCode & HAL_UART_ERROR_ORE) != RESET)
    {
        cnt++;
        //__HAL_UART_CLEAR_OREFLAG(huart);
        __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_ORE);
    }
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_NE);
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_FE);
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_PE);
}

static int read_bitrate()
{
	GPIO_PinState v0 = HAL_GPIO_ReadPin(BR0_GPIO_Port, BR0_Pin);
	GPIO_PinState v1 = HAL_GPIO_ReadPin(BR1_GPIO_Port, BR1_Pin);
	uint16_t bb = ((int)v0<<4) + v1;
	int bitrate = 115200;
	switch (bb)
	{
/*
	case 0x00:
		bitrate = 115200;
		break;
	case 0x01:
		bitrate = 19200;
		break;
	case 0x10:
		bitrate = 9600;
		break;
	case 0x11:
		bitrate = 38400;
		break;
		*/
	case 0x00:
		bitrate = 38400;
		break;
	case 0x01:
		bitrate = 9600;
		break;
	case 0x10:
		bitrate = 19200;
		break;
	case 0x11:
		bitrate = 115200;
		break;
	}
	return bitrate;
}

void uart_app_init(void)
{
    PT_INIT((struct pt*)&gl_test1_pt);

    int bitrate = read_bitrate();

    #define MAKE_INSTANCE(x,y) {\
        .Instance = x,\
        .Init= {\
            .BaudRate = bitrate,\
            .WordLength = UART_WORDLENGTH_8B,\
            .StopBits = UART_STOPBITS_1,\
            .Parity = UART_PARITY_NONE,\
            .Mode = UART_MODE_TX_RX,\
            .HwFlowCtl = y,\
            .OverSampling = UART_OVERSAMPLING_16\
        }\
    }
    
    memset(gl_all_uarts, 0, sizeof(gl_all_uarts));
    
    gl_all_uarts[0] = (uart_contex_t) {
        .uart_instance = MAKE_INSTANCE(USART1, UART_HWCONTROL_NONE),
        .irqt = USART1_IRQn,
        .tx_gpio = {  { .gpiox = TXD1_GPIO_Port, .gpio_pin = TXD1_Pin }   },
        .rx_gpio = {  { .gpiox = RXD1_GPIO_Port, .gpio_pin = RXD1_Pin } },
        .rts_gpio = { { .gpiox = RTS1_GPIO_Port, .gpio_pin = RTS1_Pin } },
        .rled_gpio = { { .gpiox = LR2_GPIO_Port, .gpio_pin = LR2_Pin }, { .gpiox = LR3_GPIO_Port, .gpio_pin = LR3_Pin } },
        .gled_gpio = { { .gpiox = LG2_GPIO_Port, .gpio_pin = LG2_Pin }, { .gpiox = LG3_GPIO_Port, .gpio_pin = LG3_Pin } },
        .index = 0
    };

    gl_all_uarts[1] = (uart_contex_t) {
        .uart_instance = MAKE_INSTANCE(USART2,UART_HWCONTROL_NONE),
        .irqt = USART2_IRQn,
        .tx_gpio = {  { .gpiox = TXD2_GPIO_Port, .gpio_pin = TXD2_Pin } },
        .rx_gpio = {  { .gpiox = RXD2_GPIO_Port, .gpio_pin = RXD2_Pin } },
        .rts_gpio = { { .gpiox = RTS2_GPIO_Port, .gpio_pin = RTS2_Pin } },
        .rled_gpio = { { .gpiox = LR4_GPIO_Port, .gpio_pin = LR4_Pin }, { .gpiox = LR5_GPIO_Port, .gpio_pin = LR5_Pin } },
        .gled_gpio = { { .gpiox = LG4_GPIO_Port, .gpio_pin = LG4_Pin }, { .gpiox = LG5_GPIO_Port, .gpio_pin = LG5_Pin } },
        .index = 1
    };

    gl_all_uarts[2] = (uart_contex_t) {
        .uart_instance = MAKE_INSTANCE(USART3, UART_HWCONTROL_NONE),
        .irqt = USART3_IRQn,
        .tx_gpio = {  { .gpiox = TXD3_GPIO_Port, .gpio_pin = TXD3_Pin }  },
        .rx_gpio = {  { .gpiox = RXD3_GPIO_Port, .gpio_pin = RXD3_Pin } },
        .rts_gpio = { { .gpiox = RTS3_GPIO_Port, .gpio_pin = RTS3_Pin } },
        .rled_gpio = { { .gpiox = LR6_GPIO_Port, .gpio_pin = LR6_Pin }, { .gpiox = LR7_GPIO_Port, .gpio_pin = LR7_Pin } },
        .gled_gpio = { { .gpiox = LG6_GPIO_Port, .gpio_pin = LG6_Pin }, { .gpiox = LG7_GPIO_Port, .gpio_pin = LG7_Pin } },
        .index = 2
    };
        
    //uart4
    gl_all_uarts[3] = (uart_contex_t) {
        .uart_instance = MAKE_INSTANCE(UART4, UART_HWCONTROL_NONE),
        .irqt = UART4_IRQn,
        .tx_gpio = {{.gpiox = TXD4_GPIO_Port, .gpio_pin = TXD4_Pin}},
        .rx_gpio = {{.gpiox = RXD4_GPIO_Port, .gpio_pin = RXD4_Pin}},
        .rts_gpio = { { .gpiox = RTS4_GPIO_Port, .gpio_pin = RTS4_Pin } },
        .rled_gpio = { { .gpiox = LR8_GPIO_Port, .gpio_pin = LR8_Pin }, { .gpiox = LR9_GPIO_Port, .gpio_pin = LR9_Pin } },
        .gled_gpio = { { .gpiox = LG8_GPIO_Port, .gpio_pin = LG8_Pin }, { .gpiox = LG9_GPIO_Port, .gpio_pin = LG9_Pin } },
        .index = 3
    };

    gl_all_uarts[4] = (uart_contex_t) {
        .uart_instance = MAKE_INSTANCE(UART5, UART_HWCONTROL_NONE),
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
        HAL_UART_RegisterCallback(&UART_GET_HANDLE(n),  HAL_UART_RX_COMPLETE_CB_ID, my_RxCpltCallback);
        HAL_UART_RegisterCallback(&UART_GET_HANDLE(n),  HAL_UART_ERROR_CB_ID, my_ErrorCallback);
        
        //HAL_UART_RegisterRxEventCallback(&UART_GET_HANDLE(n), my_RxEventCallback );
        uart_led_close(&gl_all_uarts[n]);

        ring_buffer_init(&gl_all_uarts[n].ringbuf, gl_all_uarts[n]._ringbuf, UART_RX_BUFF_SIZE);
        ring_buffer_init(&gl_all_uarts[n].send_ringbuf, gl_all_uarts[n]._send_ringbuf, UART_RX_BUFF_SIZE);
        PT_INIT(&gl_all_uarts[n].pt_recv);
        PT_INIT(&gl_all_uarts[n].pt_send);
        PT_INIT(&gl_all_uarts[n].pt_led);

        if ( HAL_OK != HAL_UART_Receive_IT(&gl_all_uarts[n].uart_instance, gl_all_uarts[n].rx_buffer, 1) )
        {
        	while(1);
        }
    }

}

void uart_app_init_device_with_channel(int channel)
{
    if ( channel < 0 || channel > 1 )
        return; 
    
    if ( channel == gl_current_channel) 
        return;

}

static HAL_StatusTypeDef uart_send_all_channel(uart_contex_t *ucontex, uint8_t *data, uint16_t size)
{
    switch (ucontex->index)
    {
    case 0:
    case 1:
    case 2:
    case 3:        
    	HAL_GPIO_WritePin(ucontex->rts_gpio[0].gpiox, ucontex->rts_gpio[0].gpio_pin, GPIO_PIN_SET);
        break;
    case 4:
        HAL_GPIO_WritePin(ucontex->rts_gpio[0].gpiox, ucontex->rts_gpio[0].gpio_pin, GPIO_PIN_SET);
        break;
    default:
        return HAL_ERROR;
    }
    ucontex->tx_completed = false;
    return HAL_UART_Transmit_IT(&ucontex->uart_instance, data, size);
}



#define TX_FINISH (gl_all_uarts[0].tx_completed && gl_all_uarts[1].tx_completed && gl_all_uarts[2].tx_completed && gl_all_uarts[3].tx_completed)
#define RX_FINISH (gl_all_uarts[0].rx_completed || gl_all_uarts[1].rx_completed || gl_all_uarts[2].rx_completed || gl_all_uarts[3].rx_completed)

static void uart_led_close(uart_contex_t *ucontex)
{
	switch (ucontex->index)
	    {
	    case 0:
	    case 1:
	    case 2:
	    case 3:
	    	HAL_GPIO_WritePin(ucontex->gled_gpio[0].gpiox, ucontex->gled_gpio[0].gpio_pin, GPIO_PIN_SET);
	    	HAL_GPIO_WritePin(ucontex->gled_gpio[1].gpiox, ucontex->gled_gpio[1].gpio_pin, GPIO_PIN_SET);

	    	HAL_GPIO_WritePin(ucontex->rled_gpio[0].gpiox, ucontex->rled_gpio[0].gpio_pin, GPIO_PIN_SET);
	        HAL_GPIO_WritePin(ucontex->rled_gpio[1].gpiox, ucontex->rled_gpio[1].gpio_pin, GPIO_PIN_SET);
	        break;
	    case 4:
	    	HAL_GPIO_WritePin(ucontex->gled_gpio[0].gpiox, ucontex->gled_gpio[0].gpio_pin, GPIO_PIN_SET);
	    	HAL_GPIO_WritePin(ucontex->rled_gpio[0].gpiox, ucontex->rled_gpio[0].gpio_pin, GPIO_PIN_SET);
	    	break;
	    default:
	    	break;
	    }
}

static void uart_led_deal(uart_contex_t *ucontex, int type , bool on_off)
{
    GPIO_PinState state = on_off ? GPIO_PIN_RESET : GPIO_PIN_SET;
    
    //type = 0 is tx , type = 1 is rx
    switch (ucontex->index)
    {
    case 0:
    case 1:
    case 2:
    case 3:
        if ( type == 0 )
            HAL_GPIO_WritePin(ucontex->gled_gpio[ucontex->current_channel].gpiox, ucontex->gled_gpio[ucontex->current_channel].gpio_pin, state);
        else
            HAL_GPIO_WritePin(ucontex->rled_gpio[ucontex->current_channel].gpiox, ucontex->rled_gpio[ucontex->current_channel].gpio_pin, state);
        break;
    case 4:
        if ( type == 0 )
            HAL_GPIO_WritePin(ucontex->gled_gpio[0].gpiox, ucontex->gled_gpio[0].gpio_pin, state);
        else
            HAL_GPIO_WritePin(ucontex->rled_gpio[0].gpiox, ucontex->rled_gpio[0].gpio_pin, state);
    default:
        break;
    }
    
}

static char uart_led_thread(uart_contex_t *ucontex)
{
    const static int led_time = 100;

#define LED_PT  (&ucontex->pt_led)

    PT_BEGIN(LED_PT);
    while(1)
    {
    		PT_WAIT_UNTIL(LED_PT, ucontex->led_tx_onoff || ucontex->led_rx_onoff );
    		if ( ucontex->led_tx_onoff ) {
    		    //open red led = 0
				uart_led_deal(ucontex, 0, true);

				timer_set(&ucontex->led_tx_timer, led_time);
				PT_WAIT_UNTIL(LED_PT, timer_expired(&ucontex->led_tx_timer));

				uart_led_deal(ucontex, 0, false);
				ucontex->led_tx_onoff = false;
			}

			if ( ucontex->led_rx_onoff ) {
				uart_led_deal(ucontex, 1, true);

				timer_set(&ucontex->led_rx_timer, led_time);
				PT_WAIT_UNTIL(LED_PT, timer_expired(&ucontex->led_rx_timer));

				uart_led_deal(ucontex, 1, false);
				ucontex->led_rx_onoff = false;
			}
        
        PT_YIELD(LED_PT);
    }

    PT_END(LED_PT);
}

static char uart_recv_serial_thread(uart_contex_t *ctx);
static char uart_send_serial_thread(uart_contex_t *ctx);

void uart_thread(void)
{
	static int flag = 0;
	if ( flag == 0 ) {

		uart_led_deal(UART_UP, 0, true);
		ring_buffer_queue_arr(&UART_UP->send_ringbuf, "start", 5);
		flag = 1;
	}

    for (int n = 0; n < UART_NUM; n++)
    {
        uart_led_thread(gl_all_uarts+n);
        uart_recv_serial_thread(gl_all_uarts+n);
        uart_send_serial_thread(gl_all_uarts+n);
    }
//    uart_recv_serial_thread(UART_UP);
//    uart_send_serial_thread(UART_UP);
//
//    uart_send_serial_thread(gl_all_uarts+0);
//    uart_test1_thread();
}


static char uart_send_serial_thread(uart_contex_t *ctx)
{

	PT_BEGIN(&ctx->pt_send);
	while(1)
	{
#if 1
		PT_WAIT_UNTIL( &ctx->pt_send , ring_buffer_num_items(&ctx->send_ringbuf) > 0);

		int n = ring_buffer_dequeue_arr(&ctx->send_ringbuf, ctx->lowlevel_tx_buffer, UART_RX_BUFF_SIZE);

        if ( HAL_OK == uart_send_all_channel(ctx, (uint8_t*)(ctx->lowlevel_tx_buffer), n) )
        {
        	PT_WAIT_UNTIL( &ctx->pt_send, ctx->tx_completed);
        } else {
        	//while(1);

        	PT_YIELD(&ctx->pt_send);
        }
#else
        PT_WAIT_UNTIL( &ctx->pt_send , ring_buffer_num_items(&ctx->send_ringbuf) > 0);

        int n = ring_buffer_dequeue_arr(&ctx->send_ringbuf, ctx->lowlevel_tx_buffer, UART_RX_BUFF_SIZE);

        int wrong = 0; ;
        for (int k = 0; k < n ; k++ )
        {
        	if ( (unsigned char)(ctx->lowlevel_tx_buffer[k]) != 0xff )
        	{
        		wrong = 1;
        		break;
        	}
        }
//        if ( wrong != 0 ) {
//        	n = 4;
//        	memcpy(ctx->lowlevel_tx_buffer, "AAAA", n);
//        } else {
//        	n = 4;
//        	memcpy(ctx->lowlevel_tx_buffer, "5555", n);
//        }
        //n = 10;
        if ( HAL_OK == uart_send_all_channel(ctx, (uint8_t*)ctx->lowlevel_tx_buffer, n) )
		{
        	PT_WAIT_UNTIL( &ctx->pt_send, ctx->tx_completed);
//			PT_WAIT_UNTIL(pt, ctx->tx_completed);
//			timer_set(&ctx->test_timer, 100);
//			PT_WAIT_UNTIL(pt, timer_expired(&ctx->test_timer));
		} else {
			PT_YIELD(&ctx->pt_send);
		}

#endif
	}
	PT_END(&ctx->pt_send);
}

static char uart_recv_serial_thread(uart_contex_t *ctx)
{
    //static struct pt *pt;
	static int test = 0 ;

    PT_BEGIN(&ctx->pt_recv);
    while(1)
    {
        PT_WAIT_UNTIL(&ctx->pt_recv, ring_buffer_num_items(&ctx->ringbuf) > 0 );

//        timer_set(&timer,10);
//        PT_WAIT_UNTIL(pt, timer_expired(&timer));

		char data[UART_RX_BUFF_SIZE];
		__HAL_UART_DISABLE_IT(&ctx->uart_instance, UART_IT_RXNE);
		int k = ring_buffer_dequeue_arr(&ctx->ringbuf, data, UART_RX_BUFF_SIZE);
		__HAL_UART_ENABLE_IT(&ctx->uart_instance, UART_IT_RXNE);
		if ( ctx->index == (UART_NUM-1) ) {
			//index==4 是从上位机读的串口
#if 1
			for(int n = 0 ; n<1000;n++)
			{
				test++;
			}
			for(int n = 0;n < (UART_NUM-1) && k > 0 ;n++)
			{
				ring_buffer_queue_arr(&(gl_all_uarts[n].send_ringbuf), data, k);
			}
#else
			//test: 测试内容 uart5 收到全ff 的数据，才转发
			if  (k>0 ){
				int wrong = 0;

				for ( int i = 0; i < k;i++ ) {
					if ( ((unsigned char)(data[i])) != 0xff) {
						wrong = 1 ;
						break;
					}
				}
				if ( !wrong ) {
					for(int n = 0;n < (UART_NUM-1) && k > 0 ;n++)
					{
						ring_buffer_queue_arr(&(gl_all_uarts[n].send_ringbuf), data, k);
					}
				}
			}
#endif
		} else {
			//从下行口收到数据
			//TODO: 可能需要改

			ring_buffer_queue_arr(&UART_UP->send_ringbuf, data, k);
		}

        PT_YIELD(&ctx->pt_recv);
    }

    PT_END(&ctx->pt_recv);
}

char uart_test1_thread(void)
{
    static struct pt * pt = (struct pt*)&gl_test1_pt;
    static struct mcu_timer timer;

    PT_BEGIN(pt);

    uart_app_init_device_with_channel(0);

    while (1)
    {
        static char data[128];
        static int cnt = 0;
        static uart_contex_t *uart_test = gl_all_uarts + 0;


        timer_set(&timer,1000);
        PT_WAIT_UNTIL(pt, timer_expired(&timer));
        int n = snprintf(data, sizeof(data),"hello%d\r\n", cnt++);


        ring_buffer_queue_arr(&uart_test->send_ringbuf, data, strlen(data));
        ring_buffer_queue_arr(&UART_UP->send_ringbuf, data, strlen(data));


    }
    PT_END(pt);
}
