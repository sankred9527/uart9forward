#include "uart_utils.h"
#include "main.h"

uart_contex_t gl_all_uarts[5];

//#define MAX_UART_CHANNEL (2)
static int gl_current_channel = 0;


static void my_MspDeInitCallback(UART_HandleTypeDef *huart)
{
    uart_contex_t *ucontext = (uart_contex_t*)huart;
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    int channel = ucontext->current_channel;

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
    
    uint16_t tx_pin = ucontext->tx_gpio[channel].gpio_pin;
    GPIO_TypeDef *tx_group = ucontext->tx_gpio[channel].gpiox;
    uint16_t rx_pin = ucontext->rx_gpio[channel].gpio_pin;
    GPIO_TypeDef *rx_group = ucontext->rx_gpio[channel].gpiox;
    uint16_t rts_pin = ucontext->rts_gpio[channel].gpio_pin;
    GPIO_TypeDef *rts_group = ucontext->rts_gpio[channel].gpiox;

    HAL_GPIO_DeInit(tx_group, tx_pin);
    HAL_GPIO_DeInit(rx_group, rx_pin);
    if ( rts_pin > 0 )
        HAL_GPIO_DeInit(rts_group, rts_pin);

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
            uint16_t rts_pin = ucontext->rts_gpio[channel].gpio_pin;
            GPIO_TypeDef *rts_group = ucontext->rts_gpio[channel].gpiox;

            GPIO_InitStruct.Pin = tx_pin;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
            HAL_GPIO_Init(tx_group, &GPIO_InitStruct);

            GPIO_InitStruct.Pin = rts_pin;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
            HAL_GPIO_Init(rts_group, &GPIO_InitStruct);

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
                GPIO_InitStruct.Pin = ucontext->rts_gpio[n].gpio_pin;
                GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
                GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
                HAL_GPIO_Init(ucontext->rts_gpio[n].gpiox, &GPIO_InitStruct);
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

            GPIO_InitStruct.Pin = ucontext->rts_gpio[0].gpio_pin;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
            HAL_GPIO_Init(ucontext->rts_gpio[0].gpiox, &GPIO_InitStruct);
            break;
        default:
            return;
    }

    /* USART/UART interrupt Init */
    HAL_NVIC_SetPriority(ucontext->irqt, 0, 0);
    HAL_NVIC_EnableIRQ(ucontext->irqt);     
}

void uart_app_init(void)
{
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
        .index = 0
    };

    gl_all_uarts[1] = (uart_contex_t) {
        .uart_instance = MAKE_INSTANCE(USART2),
        .irqt = USART2_IRQn,
        .tx_gpio = {  { .gpiox = TXD2_0_GPIO_Port, .gpio_pin = TXD2_0_Pin } ,  { .gpiox = TXD2_1_GPIO_Port, .gpio_pin = TXD2_1_Pin } },
        .rx_gpio = {  { .gpiox = RXD2_0_GPIO_Port, .gpio_pin = RXD2_0_Pin } ,  { .gpiox = RXD2_1_GPIO_Port, .gpio_pin = RXD2_1_Pin } },
        .rts_gpio = { { .gpiox = RTS2_0_GPIO_Port, .gpio_pin = RTS2_0_Pin } ,  { .gpiox = RTS2_1_GPIO_Port, .gpio_pin = RTS2_1_Pin } },
        .index = 1
    };

    gl_all_uarts[2] = (uart_contex_t) {
        .uart_instance = MAKE_INSTANCE(USART3),
        .irqt = USART3_IRQn,
        .tx_gpio = {  { .gpiox = TXD3_0_GPIO_Port, .gpio_pin = TXD3_0_Pin } ,  { .gpiox = TXD3_1_GPIO_Port, .gpio_pin = TXD3_1_Pin } },
        .rx_gpio = {  { .gpiox = RXD3_0_GPIO_Port, .gpio_pin = RXD3_0_Pin } ,  { .gpiox = RXD3_1_GPIO_Port, .gpio_pin = RXD3_1_Pin } },
        .rts_gpio = { { .gpiox = RTS3_0_GPIO_Port, .gpio_pin = RTS3_0_Pin } ,  { .gpiox = RTS3_1_GPIO_Port, .gpio_pin = RTS3_1_Pin } },
        .index = 2
    };
        
    gl_all_uarts[3] = (uart_contex_t) {
        .uart_instance = MAKE_INSTANCE(UART4),
        .irqt = UART4_IRQn,
        .tx_gpio = {{.gpiox = TXD4_GPIO_Port, .gpio_pin = TXD4_Pin}},
        .rx_gpio = {{.gpiox = RXD4_GPIO_Port, .gpio_pin = RXD4_Pin}},
        .rts_gpio = { { .gpiox = RTS4_0S_GPIO_Port, .gpio_pin = RTS4_0S_Pin } ,  { .gpiox = RTS4_1S_GPIO_Port, .gpio_pin = RTS4_1S_Pin } },
        .index = 3
    };

    gl_all_uarts[4] = (uart_contex_t) {
        .uart_instance = MAKE_INSTANCE(UART5),
        .irqt = UART5_IRQn,
        .tx_gpio = {{.gpiox = TXD5_GPIO_Port, .gpio_pin = TXD5_Pin}},
        .rx_gpio = {{.gpiox = RXD5_GPIO_Port, .gpio_pin = RXD5_Pin}},
        .rts_gpio = { { .gpiox = RTS5S_GPIO_Port, .gpio_pin = RTS5S_Pin } },
        .index = 4
    };
    
    for(int n = 0; n < 5; n++) {
        HAL_UART_RegisterCallback(&(gl_all_uarts+n)->uart_instance,  HAL_UART_MSPINIT_CB_ID, my_MspInitCallback);
        HAL_UART_RegisterCallback(&(gl_all_uarts+n)->uart_instance,  HAL_UART_MSPDEINIT_CB_ID, my_MspDeInitCallback);
        //
    }

    //单独初始化 uart5
    if (HAL_UART_Init(&(gl_all_uarts+4)->uart_instance) != HAL_OK)
    {
        Error_Handler();
    }
}

void uart_app_init_device_with_channel(int channel)
{
    if ( channel < 0 || channel > 1 )
        return; 
    
    for (int n = 0; n <= 3; n++)
    {
        uart_contex_t *uart_contex = gl_all_uarts + n;
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


static void uart_up_recv(uart_contex_t *ucontex, uint8_t *data, uint16_t size)
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
    HAL_UARTEx_ReceiveToIdle_IT(&ucontex->uart_instance, data, size);
}


/// @brief HAL callback for tx complete in IRQ handler
/// @param huart /
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if ( huart->Instance == USART1 || huart->Instance == USART2 || huart->Instance == USART3 || huart->Instance == UART4 )
    {
    }
    else if ( huart->Instance == UART5 )
    {        
        // HAL_GPIO_WritePin(HANDLE_UART5->rts_gpio[0].gpiox, HANDLE_UART5->rts_gpio[0].gpio_pin, GPIO_PIN_RESET);
        // HANDLE_UART5->tx_completed = true;
    }
    
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{

}

void uart_thread(void)
{
    while(1)
    {

    }
}

void uart_test(void)
{
    
}
