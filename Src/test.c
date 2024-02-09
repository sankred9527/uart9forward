#include "test.h"
#include <stdint.h>
#include "delay.h"
#include "gpio_utils.h"

void test1(void)
{
	static uint8_t ct0 = 0,ct1 = 0;
	SetLed(0,ct0,0);
	ct0++;
	if(ct0 >= 3) ct0 = 0;
	SetLed(0,ct0,1);


	//	SetLed(1,ct1,1);
	//	SetLed(1,ct1,1);
	SetLed(1,ct1,0);
	SetLed(2,8-ct1,0);
	ct1++;
	if(ct1 >= 9) ct1 = 0;
	SetLed(1,ct1,1);
	SetLed(2,8-ct1,1);

	delay_ms(100);
	//uart_up_print("Hello World\r\n");
}
