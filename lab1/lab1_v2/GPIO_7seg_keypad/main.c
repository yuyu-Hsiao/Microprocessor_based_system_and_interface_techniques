//
// GPIO_7seg_keypad : 3x3 keypad inpt and display on 7-segment LEDs
//
#include <stdio.h>
#include <stdbool.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Seven_Segment.h"
#include "Scankey.h"


volatile uint32_t counter[2] ={0,0};
volatile uint8_t id[4] = {3, 1, 0, 5};
volatile bool timer_f = FALSE;
volatile uint16_t key_output[3] = {0x003B, 0x003D, 0X003E};
volatile uint16_t key_temp=0x0000;
uint16_t key_value = 3;
uint8_t key_detected = 0;





// display an integer on four 7-segment LEDs


void TMR0_IRQHandler(void)
{
	timer_f = TRUE;
  TIMER_ClearIntFlag(TIMER0);
}

void Init_Timer0(void)
{
  TIMER_Open(TIMER0, TMR0_OPERATING_MODE, TMR0_OPERATING_FREQ);
  TIMER_EnableInt(TIMER0);
  NVIC_EnableIRQ(TMR0_IRQn);
  TIMER_Start(TIMER0);
}


void Scan_Seg(void){
	CloseSevenSegment();
	ShowSevenSegment(counter[0],id[counter[0]]);
	counter[0]++;
	if(counter[0]>3){
		counter[0]=0;
		//key_value = key_temp;
	}
}

//uint16_t ScanKey1(void)
void ScanKey1(void)
{
	PA->DOUT = (PA->DOUT & 0xFFC0)|key_output[counter[1]];
	
	if ((PA->PIN & (1 << 3)) == 0){
		key_value = 1+counter[1];
		key_detected = 1;
	} 
	if ((PA->PIN & (1 << 4)) == 0){
		key_value = 4+counter[1];
		key_detected = 1;
	} 
	if ((PA->PIN & (1 << 5)) == 0){
		key_value = 7+counter[1];
		key_detected = 1;
	} 

	
	counter[1]++;
	if(counter[1]>2){
		counter[1]=0;
		if (!key_detected) {
			key_value = 3;
		}
		key_detected = 0;
	}
}




int main(void)
{
    SYS_Init();
		Init_Timer0();
    OpenSevenSegment();
	  OpenKeyPad();

 	  while(1) {			
			if(timer_f == TRUE){
				ScanKey1();
				id[0]=key_value;
				Scan_Seg();
				timer_f = 0;			
			}			
	  }
}
