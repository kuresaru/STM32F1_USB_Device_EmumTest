#include	"common.h"

void Delay_us(u32 i) {
	u32 tmp;
	SysTick->LOAD = 9 * i;
	SysTick->CTRL = 0x01;
	SysTick->VAL = 0;
	do {
		tmp = SysTick->CTRL;
	} while ((tmp & 0x01) && (!(tmp & (1 << 16))));
	SysTick->CTRL = 0x00;
	SysTick->VAL = 0;
}

void Delay_ms(u32 i) {
	u32 tmp;
	SysTick->LOAD = 9000 * i;
	SysTick->CTRL = 0x01;
	SysTick->VAL = 0;
	do {
		tmp = SysTick->CTRL;
	} while ((tmp & 0x01) && (!(tmp & (1 << 16))));
	SysTick->CTRL = 0x00;
	SysTick->VAL = 0;
}
