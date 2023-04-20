#include <stdint.h>
#include "system.h"
#include "sys/alt_irq.h"
#include <stdio.h>
#include<stdbool.h>


#define pio_p32 ((volatile uint32_t*)PIO_BASE)
#define timer_p32 ((volatile uint32_t*)TIMER_BASE)
#define digits_p32 ((volatile uint32_t*)DIGITS_BASE)
#define TIMER_CNT 0
#define TIMER_MODULO 1
#define TIMER_CTRL_STATUS 2
#define TIMER_MAGIC 3
#define TIMER_RESET_FLAG 0
#define TIMER_PAUSE_FLAG 1
#define TIMER_WRAP_FLAG 2
#define TIMER_WRAPPED_FLAG 3
#define TIMER_RESET (TIMER_RESET_FLAG + 4)
#define TIMER_PAUSE (TIMER_PAUSE_FLAG + 4)
#define TIMER_WRAP (TIMER_WRAP_FLAG + 4)
#define TIMER_WRAPPED (TIMER_WRAPPED_FLAG + 4)
#define SEGM_0 0
#define SEGM_1 1
#define SEGM_2 2
#define SEGM_3 3
#define SEGM_PACK 4

#define WAIT_UNITL_TRUE(x) while(!x){}


typedef struct {
	// Unpacked .
		// reg 0 -7
	uint32_t led_unpacked[8];
	// reg 8 -15
	uint32_t sw_unpacked[8];
	// Packed .
		// reg 16
	unsigned led_packed : 8;
	unsigned : 24;
	unsigned sw_packed : 8;
	unsigned : 24;
	uint32_t babadeda[14];

} bf_pio;

#define pio (((volatile bf_pio)PIO_BASE))

int main() {
	uint8_t num = 0x80;
	//num = num << 7;
	timer_p32[TIMER_MODULO] = 10*12000000;
	timer_p32[TIMER_CTRL_STATUS] = 0;
	bool left = false;
	uint8_t rst;
	digits_p32[SEGM_PACK] = 0;
	while (1) {

		WAIT_UNITL_TRUE(timer_p32[TIMER_WRAPPED]);
		timer_p32[TIMER_WRAPPED] = 0;
		//reset 
		rst = pio.sw_unpacked[7];
		if (rst) {
			num = 0x80;
			left = false;
			pio.sw_packed=0;
		}
		//write
		

		//Calculate

	
		if (num == 1) {
			left = true;
			
		}
		else if (num == 0x80) {
			left = false;
		}
		
		if (left) {
			num = num << 1;
		}
		else {
			num = num >> 1;
			
		}
		pio.led_packed = num;
		printf("%d\n", num);
		//timer
		uint8_t sw0 = pio.sw_unpacked[0];
		uint8_t sw1 = pio.sw_unpacked[1];

		if (sw0 == 0 && sw1 == 0) {
			timer_p32[TIMER_MODULO] = 2 * 12000000;
		}
		else if (sw0 == 1 && sw1 == 0) {
			timer_p32[TIMER_MODULO] = 12000000;
		}
		else if (sw0 == 0 && sw1 == 1) {
			timer_p32[TIMER_MODULO] = 12000000 * 0.5;
		}
		else {
			timer_p32[TIMER_MODULO] = 12000000 * 0.25;
		}



	}

	return 0;
}
