
#include <stdint.h>
#include "system.h"
#include "sys/alt_irq.h"
#include <stdio.h>

#define WAIT_UNITL_FALSE(x) while((x)){}
#define WAIT_UNITL_TRUE(x) while(!(x)){}

#define pio_p32 ((volatile uint32_t*)PIO_BASE)
#define digits_p32 ((volatile uint32_t*)(DIGITS_BASE+0x20))
#define timer_p32 ((volatile uint32_t*)(TIMER_BASE+0x20))


#define SEGM_0             1
#define SEGM_1             3
#define SEGM_2             2
#define SEGM_3             0
#define SEGM_PACK          4
#define TIMER_CNT          0
#define TIMER_MODULO       3
#define TIMER_CTRL_STATUS  2
#define TIMER_MAGIC        1
#define TIMER_RESET_FLAG   3
#define TIMER_PAUSE_FLAG   0
#define TIMER_WRAP_FLAG    1
#define TIMER_WRAPPED_FLAG 2
#define TIMER_RESET        (TIMER_RESET_FLAG+4)
#define TIMER_PAUSE        (TIMER_PAUSE_FLAG+4)
#define TIMER_WRAP         (TIMER_WRAP_FLAG+4)
#define TIMER_WRAPPED      (TIMER_WRAPPED_FLAG+4)


//TODO Написати битско поље за тајмер, на основу спецификације у PDF-у вежбе.

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
	uint32_t babadeda[15];
} bf_pio;
#define pio (*((volatile bf_pio*)PIO_BASE))

static void timer_isr(void* context) {
	static uint8_t pause = 0;
	static uint8_t reset = 0;

	uint8_t led_disp;
	
	
	uint8_t x;

	static uint8_t cnt[4] = {0};
	
	
	///////////////////
	// Read inputs.

	pause = pio.sw_unpacked[1];
	reset = pio.sw_unpacked[0];
			
	///////////////////
	// Calculate state.

	
	
	if(pause == 0)
			cnt[0]++;
		if(reset == 1)
		{
			cnt[0] = 0;
			cnt[1] = 0;
			cnt[2] = 0;
			cnt[3] = 0;
		}
		
		//stotinke jedinice
		
		if(cnt[0] == 10)
		{
			cnt[0] = 0;
			cnt[1]++;
		}	
		
		//stotinke desetice
		if(cnt[1] == 10)
		{
			cnt[1] = 0;
			cnt[2]--;
		}	
		
		//sekunde jedinice
		if(cnt[2] == 10)
		{
			cnt[2] = 0;
			cnt[3]++;
		}
		
		if(cnt[3] == 6)
			cnt[3] = 0;	
	
	
	///////////////////
	// Write outputs.
	
	digits_p32[SEGM_0] = cnt[0];
	digits_p32[SEGM_1] = cnt[1];
	digits_p32[SEGM_2] = cnt[2];
	digits_p32[SEGM_3] = cnt[3];

	led_disp = (cnt[2] << 4) | cnt[1];
	printf("%x", led_disp);		
	pio.led_packed = led_disp;
		
}


int main() {
	//TODO Прочитати MAGIC регистар и ако је различит од 0xbabadeda
	// упалити све LED као индикацију грешке и завршити програм.
	
	uint8_t led_disp;
	timer_p32[TIMER_MODULO] = 120000; // modulo.
	timer_p32[TIMER_CTRL_STATUS] = 0; // Start it.
	
	//TODO Направити штоперицу,
	// користећи функционалности периферија одговарајућег термина,
	// како је већ дефинисано у задатку у PDF-у вежбе.
	alt_ic_isr_register(
		TIMER_IRQ_INTERRUPT_CONTROLLER_ID, //alt_u32 ic_id
		TIMER_IRQ, //alt_u32 irq
		timer_isr, //alt_isr_func isr
		NULL, //void *isr_context
		NULL //void *flags
	);


	printf("timer_p32 cnt reg:\n");
	for(int j = 0; j < 300; j++){
		printf("%9d\n", (int)timer_p32[TIMER_CNT]);
		
		// Busy wait.
		for(int i = 0; i < 10*1000*1000; i++){}
	}

	while(1){
		if(timer_p32[TIMER_MAGIC]!=0xbabadeda){
			led_disp=0b11111111;
			pio.led_packed = led_disp;

		}
	}
	

	return 0;
}
