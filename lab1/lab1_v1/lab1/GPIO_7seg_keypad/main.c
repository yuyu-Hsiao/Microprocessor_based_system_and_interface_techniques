//
// GPIO_7seg_keypad : 3x3 keypad inpt and display on 7-segment LEDs
//
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Seven_Segment.h"
#include "Scankey.h"

uint8_t id[4] = {3, 1, 0, 5};

// display an integer on four 7-segment LEDs
void Display_7seg(uint16_t value)
{
	uint8_t i;
  for(i=0;i<4;i++){
		CloseSevenSegment();
		if(value!=0 & i==0){
			id[0] = value;
			ShowSevenSegment(0,id[0]);
		}
		if(value==0){
			id[0] = 3;
			ShowSevenSegment(i,id[i]);
		}
		CLK_SysTickDelay(5000);		
	}
}

int main(void)
{
		uint16_t j;
    SYS_Init();
    OpenSevenSegment();
	  OpenKeyPad();

 	  while(1) {
		j=ScanKey();
    Display_7seg(j);
	  }
}
