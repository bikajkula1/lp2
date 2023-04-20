#include <stdint.h>
#include "system.h"
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

#define WAIT_UNITL_TRUE(x) while(x){}

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
	// reg 17 
	unsigned sw_packed : 8;
	unsigned : 24;
	uint32_t babadeda[14];
	
} bf_pio;

# define pio (*(( volatile bf_pio *) PIO_BASE ) )

int main() {

	timer_p32[TIMER_MODULO] = 12000000*5;
	timer_p32[TIMER_CTRL_STATUS] = 0;
	digits_p32[SEGM_PACK] = 0;
	bool p5_sec = false;
	while(1){
		///////////////////
		// Read inputs.
		uint8_t x = pio.sw_packed & 0x0f;
		bool rst = pio.sw_unpacked[7];
		if (timer_p32[TIMER_WRAPPED]) {
			p5_sec = true;
			timer_p32[TIMER_WRAPPED] = 0;
		}
		
		///////////////////
		// Calculate state.
		uint8_t b0 = x >> 0 & 1;
		uint8_t b1 = x >> 1 & 1;
		uint8_t b2 = x >> 2 & 1;
		uint8_t b3 = x >> 3 & 1;
		uint8_t parity = b0 ^ b1 ^ b2 ^ b3;


		///////////////////
		// Write outputs.
		if (rst) {
			pio.led_packed = 0;
			digits_p32[SEGM_PACK] = 0;
		}
		else {
			pio.led_packed = parity  << 4 | x;
			if (p5_sec) {
				if (parity) {
					digits_p32[SEGM_2]++;
				}
				else
				{
					digits_p32[SEGM_0]++;
				}
				p5_sec = false;
			}
		}
	

		
	}

	return 0;
}
