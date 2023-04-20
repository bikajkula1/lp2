#include <stdint.h>
#include "system.h"
#include "sys/alt_irq.h"
#include <stdio.h>
#include<stdbool.h>

#define pio_p32 ((volatile uint32_t*)PIO_BASE)
#define digits_p32 ((volatile uint32_t *)DIGITS_BASE)
#define timer_p32 ((volatile uint32_t*)TIMER_BASE )

#define WAIT_UNITL_TRUE(x) while(!(x)){} 

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
//treba promeniti reg
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
#define pio (*((volatile bf_pio *)PIO_BASE))

int main() {
	timer_p32[TIMER_MODULO] = 12000000;
	timer_p32[TIMER_CTRL_STATUS] = 0;
	digits_p32[SEGM_PACK] = 0;
	uint8_t x = 0x01;
	uint8_t pom = 0x01;
	x = x << 7;
	pom = pom << 6;
	bool right = true;
	uint8_t br=-1;
	bool write_o = false;
	bool pause=false;
	while (1) {
		///////////////////
		// Read inputs.
		uint8_t sw = pio.sw_packed;

		//granica
		uint8_t sw7 = sw >> 7 & 1;
		uint8_t sw6 = sw >> 6 & 1;
		uint8_t sw5 = sw >> 5 & 1;

		//brzina
		uint8_t sw4 = sw >> 4 & 1;
		uint8_t sw3 = sw >> 3 & 1;

		//restart
		uint8_t sw0 = sw >> 0 & 1;

		if (timer_p32[TIMER_WRAPPED]) {

			write_o = true;
			timer_p32[TIMER_WRAPPED]=0;
		}

		if (sw3 == 0 && sw4 == 0) {

			timer_p32[TIMER_MODULO] = 12000000 *0.25;
			pause = false;

		} 
		else if (sw3 == 1 && sw4 == 0) {

			timer_p32[TIMER_MODULO] = 12000000 * 0.5;
			pause = false;

		}
		else if (sw3 == 0 && sw4 == 1) {

			timer_p32[TIMER_MODULO] = 12000000;
			pause = false;

		}
		else
		{
			pause = true;
		}
		
		

		///////////////////
		// Calculate state.
		if (sw5 == 0 && sw6 == 0 && sw7 == 0) {
			br = 0;
		} 
		else if (sw5 == 1 && sw6 == 0 && sw7 == 0) {
			br = 1;
		}
		else if (sw5 == 0 && sw6 == 1 && sw7 == 0) {
			br = 2;
		}
		else if (sw5 == 0 && sw6 == 0 && sw7 == 1) {
			br = 4;
		}
		else if (sw5 == 1 && sw6 == 1 && sw7 == 0) {
			br = 3;
		}
		else if (sw5 == 1 && sw6 == 0 && sw7 == 1) {
			br = 5;
		}
		else if (sw5 == 0 && sw6 == 1 && sw7 == 1) {
			br = 6;
		}
		else if (sw5 == 1 && sw6 == 1 && sw7 == 1) {
			br = 7;
		}



		if (sw0 == 1) {
			uint8_t x = 0x01;
			uint8_t pom = 0x01;
			x = x << 7;
			pom = pom << 6;
			digits_p32[SEGM_PACK] = 0;
			
		}
		else
		{
			if (write_o  && pause==false) {
				pio.led_packed=x;
				printf("%d\n", x);
				if (right) {
					if (x >> br & 1 == 1) {
						right = false;
					}
				}
				else
				{
					if (x == 0x80) {
						right = true;
						pom = 0x01;
						pom=pom << 6;
						digits_p32[SEGM_2]++;
					}
				}
				if (right) {
					x = x | pom;
					pom = pom >> 1;

				}
				else
				{
					x = x << 1;
				}
				write_o = false;
			}
		}
		

	}

	return 0;
}
