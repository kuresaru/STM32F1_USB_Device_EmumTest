#include "common.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"

#include "usb_lib.h"

int main (void) 
{
	GPIO_InitTypeDef g;
	NVIC_InitTypeDef n;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	g.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	g.GPIO_Mode	= GPIO_Mode_AF_PP;
	g.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &g);
	GPIOA->BRR = 1 << 12;

	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	n.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	n.NVIC_IRQChannelPreemptionPriority = 1;
	n.NVIC_IRQChannelSubPriority = 0;
	n.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&n);

	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);

	USB_Init();

	for (;;)
	{
	}
}
