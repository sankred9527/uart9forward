#include "gpio_utils.h"
#include "main.h"

GPIO_TypeDef* GPIO_TypeList[] = {GPIOA,GPIOB,GPIOC,GPIOD,GPIOE};

const uint16_t PLeds[3][2] = {{3,PWR1_Pin},{1,PWR2_Pin},{1,PWR3_Pin}};
const uint16_t RLeds[9][2] = {{1,LR1_Pin},{1,LR2_Pin},{1,LR3_Pin},{3,LR4_Pin},{3,LR5_Pin},{3,LR6_Pin},{3,LR7_Pin},{3,LR8_Pin},{3,LR9_Pin}};
const uint16_t GLeds[9][2] = {{0,LG1_Pin},{0,LG2_Pin},{0,LG3_Pin},{0,LG4_Pin},{1,LG5_Pin},{1,LG6_Pin},{1,LG7_Pin},{1,LG8_Pin},{1,LG9_Pin}};


//  ledtype:    0:PLeds    1:RLeds    2:GLeds
void SetLed(uint8_t ledtype, uint8_t ledidx, uint8_t status)
{
	switch (ledtype)
	{
		case LED_POWER_TYPE:
		{
			if(ledidx >= 3) return;
			HAL_GPIO_WritePin(GPIO_TypeList[PLeds[ledidx][0]], PLeds[ledidx][1], status > 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
		}break;
		case LED_RED_TYPE:
		{
			if(ledidx >= 9) return;
			HAL_GPIO_WritePin(GPIO_TypeList[RLeds[ledidx][0]], RLeds[ledidx][1], status > 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
		}break;
		case LED_GREEN_TYPE:
		{
			if(ledidx >= 9) return;
			HAL_GPIO_WritePin(GPIO_TypeList[GLeds[ledidx][0]], GLeds[ledidx][1], status > 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
		}break;
	}
}

