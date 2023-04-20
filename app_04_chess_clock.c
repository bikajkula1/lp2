

#include <stdint.h>
#include "system.h"

//TODO Написати битско поље за тајмер, на основу спецификације у PDF-у вежбе.


#include <stdint.h>
#include "system.h"
#include <stdio.h>

#define WAIT_UNITL_FALSE(x) while((x)){}
#define WAIT_UNITL_TRUE(x) while(!(x)){}

#define pio_p32 ((volatile uint32_t*)PIO_BASE)
#define digits_p32 ((volatile uint32_t*)DIGITS_BASE)
#define timer_p32 ((volatile uint32_t*)TIMER_BASE)

#define SEGM_0             0
#define SEGM_1             1
#define SEGM_2             2
#define SEGM_3             3
#define SEGM_PACK          4
#define TIMER_CNT          0
#define TIMER_MODULO       1
#define TIMER_CTRL_STATUS  2
#define TIMER_MAGIC        3
#define TIMER_RESET_FLAG   0
#define TIMER_PAUSE_FLAG   1
#define TIMER_WRAP_FLAG    2
#define TIMER_WRAPPED_FLAG 3
#define TIMER_RESET        (TIMER_RESET_FLAG+4)
#define TIMER_PAUSE        (TIMER_PAUSE_FLAG+4)
#define TIMER_WRAP         (TIMER_WRAP_FLAG+4)
#define TIMER_WRAPPED      (TIMER_WRAPPED_FLAG+4)

typedef struct {
	// Unpacked.
	// reg 0-7
	uint32_t led_unpacked[8];
	// reg 8-15
	uint32_t sw_unpacked[8];
	// Packed.
	// reg 16
	unsigned led_packed : 8;
	unsigned sw_packed  : 8;
	unsigned            : 16;
	unsigned timer_bf   : 32;
	uint32_t babadeda[15];
} bf_pio;
#define pio (*((volatile bf_pio*)PIO_BASE))



int main() {



	pio.led_packed = 0x81; // For debugging purposes.

	uint8_t cnt = 0;
	pio.led_packed = cnt;

	uint8_t led_disp;

	timer_p32[TIMER_MODULO] = 12000000; // modulo.
	timer_p32[TIMER_CTRL_STATUS] = 0; // Start it.

	uint8_t pause = 0;
	uint8_t reset = 0;
	uint8_t reset1 = 0;
	uint8_t reset2 = 0;
	uint8_t cnt_beli[2] = {0};
	uint8_t cnt_crni[2] = {0};
	uint16_t timer_packed = 0;

	pio.led_packed = 0;
	digits_p32[SEGM_0] = 0;
	digits_p32[SEGM_1] = 0;
	digits_p32[SEGM_2] = 0;
	digits_p32[SEGM_3] = 0;

	while(1){

	//TODO Прочитати MAGIC регистар и ако је различит од 0xbabadeda
	// упалити све LED као индикацију грешке и завршити програм.
	if(timer_p32[TIMER_MAGIC]!=0xbabadeda){
			led_disp=0b11111111;
			pio.led_packed = led_disp;

		}
	
	//TODO Направити сат за шах,
	// користећи функционалности периферија одговарајућег термина,
	// како је већ дефинисано у задатку у PDF-у вежбе.
		pause = pio.sw_unpacked[1];
		reset = pio.sw_unpacked[0];



	// Over packed.
		// Poll wrapped flag.
		WAIT_UNITL_TRUE(timer_p32[TIMER_CTRL_STATUS] & 						1<<TIMER_WRAPPED_FLAG);
		// Clear wrapped flag.
		timer_p32[TIMER_CTRL_STATUS] &= 						~(1<<TIMER_WRAPPED_FLAG);


	if(reset == 1)
		{
			cnt_beli[0] = 9;
			cnt_beli[1] = 5;
			cnt_crni[1] = 5;
			cnt_crni[0] = 9;
		}

	if(reset1 == 1)
		{
			cnt_beli[0] = 9;
			cnt_beli[1] = 5;
			cnt_crni[1] = 0;
			cnt_crni[0] = 0;
		}

	if(reset2 == 1)
		{
			cnt_beli[0] = 0;
			cnt_beli[1] = 0;
			cnt_crni[1] = 5;
			cnt_crni[0] = 9;
		}
		
		if(pause == 0)
		{
			if(pio.sw_unpacked[7])	//crni
			{
				pio.led_unpacked[7] = 1;
				pio.led_unpacked[0] = 0;
				cnt_crni[0]--;
			}
			else					//beli
			{ 
				pio.led_unpacked[7] = 0;
				pio.led_unpacked[0] = 1;
				cnt_beli[0]--;
			}
		}

		if(reset == 1)
		{
			cnt_beli[0] = 9;
			cnt_beli[1] = 5;
			cnt_crni[1] = 5;
			cnt_crni[0] = 9;
		}
		
		if(cnt_beli[0] == 0)
		{	
			cnt_beli[1]--;
			cnt_beli[0] = 9;
			
		}
		
		if(cnt_crni[0] == 0)
		{
			cnt_crni[1]--;
			cnt_crni[0] = 9;
			
		}
		
		if(cnt_beli[1] == 0 && cnt_beli[1] == 0)
		{
			reset1= 1;
		}
		
		if(cnt_crni[1] == 0 && cnt_crni[0] == 0)
		{
			reset2= 1;
		}



	//output
		digits_p32[SEGM_0] = cnt_beli[0];
		digits_p32[SEGM_1] = cnt_beli[1];
		digits_p32[SEGM_2] = cnt_crni[0];
		digits_p32[SEGM_3] = cnt_crni[1];
		
		
	

	}
	return 0;
}
