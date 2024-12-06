/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * $Revision: 2 $
 * $Date: 15/04/13 10:13a $
 * @brief    Implement timer counting in periodic mode.
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NUC100Series.h"


#define PLL_CLOCK           50000000

volatile uint32_t g_au32TMRINTCount[2] = {0};

uint8_t timer_f = FALSE;
volatile uint16_t key_output[3] = {0x003B, 0x003D, 0X003E};
uint16_t key_value = 0;
uint8_t key_detected = 0;
volatile uint32_t key_counter = 0;



void OpenKeyPad(void)
{
	//GPIO_SetMode(PA, (BIT0 | BIT1 | BIT2 | BIT3 | BIT4), GPIO_PMD_QUASI);	
	PA -> PMD = (PA -> PMD & 0xFFFFF000)|0x0FD5; 	//PA0-2 output PA3-5 input
}

//uint16_t ScanKey1(void)
void ScanKey1(void)
{
	PA->DOUT = (PA->DOUT & 0xFFC0)|key_output[key_counter];
	
	if ((PA->PIN & (1 << 3)) == 0){
		key_value = 1+key_counter;
		key_detected = 1;
	} 
	if ((PA->PIN & (1 << 4)) == 0){
		key_value = 4+key_counter;
		key_detected = 1;
	} 
	if ((PA->PIN & (1 << 5)) == 0){
		key_value = 7+key_counter;
		key_detected = 1;
	} 

	key_counter++;
	if(key_counter>2){
		key_counter=0;
		if (!key_detected) {
			key_value = 0;
		}
		key_detected = 0;
	}
}

/**
 * @brief       Timer0 IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The Timer0 default IRQ, declared in startup_NUC100Series.s.
 */
void TMR0_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER0) == 1)
    {
				timer_f = TRUE;
        /* Clear Timer0 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER0);        
    }
}


void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable Internal RC 22.1184MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

    /* Enable external XTAL 12MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);
    
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);    
    
    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));

    /* Enable Timer 0~3 module clock */
    CLK_EnableModuleClock(TMR0_MODULE);    
   

    /* Select Timer 0~3 module clock source */
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0_S_HXT, NULL);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD, TXD */
    SYS->GPB_MFP = SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD;
}

void UART0_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART0 */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  MAIN function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
    volatile uint32_t u32InitCount;
		uint32_t prev_key_value = 0; 
		uint32_t current_key_value = 0; 
		uint32_t counter1_flag = TRUE;
		uint32_t counter2_flag = TRUE;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();

    printf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    printf("+------------------------------------------------+\n");
    printf("|    Timer Periodic and Interrupt Sample Code    |\n");
    printf("+------------------------------------------------+\n\n");

    printf("# Key Settings:\n");
    printf("# Key1 press: stop counter1; release: resume counter1.\n");
		printf("# Key2 press: stop counter2; release: resume counter2.\n");
		printf("# Key3 reset all counter.\n");
		printf("# Key4 start/stop counter1.\n");
		printf("# Key5 start/stop counter2.\n");

    /* Open Timer0 in periodic mode, enable interrupt and 1 interrupt tick per second */
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER0);



    /* Enable Timer0 ~ Timer3 NVIC */
    NVIC_EnableIRQ(TMR0_IRQn);


    /* Clear Timer0 ~ Timer3 interrupt counts to 0 */
    g_au32TMRINTCount[0] = g_au32TMRINTCount[1] = 0;
    u32InitCount = 0;

    /* Start Timer0 ~ Timer3 counting */
    TIMER_Start(TIMER0);
		while(1){
			if(timer_f == TRUE){
				u32InitCount++;
				if(u32InitCount==1000){
					printf("\r                                          \r");
					printf("\rcounter1: %d   counter2: %d",g_au32TMRINTCount[0],g_au32TMRINTCount[1]);

					if(key_value!=1&&counter1_flag){
						g_au32TMRINTCount[0]+=2;
					}
					if(key_value!=2&&counter2_flag){
						g_au32TMRINTCount[1]+=3;
					}											
					
		
					u32InitCount=0;
				}

        current_key_value = key_value;
				if(key_value ==4){
					prev_key_value = key_value;
				}
        if (prev_key_value == 4 && current_key_value == 0) {
					counter1_flag = !counter1_flag;
					prev_key_value =0;
        }				
				
				if(key_value ==5){
					prev_key_value = key_value;
				}
        if (prev_key_value == 5 && current_key_value == 0) {
					counter2_flag = !counter2_flag;
					prev_key_value =0;
        }	
				
				//reset counter
				if(key_value==3){
					g_au32TMRINTCount[0] = 0;
					g_au32TMRINTCount[1] = 0;
				}
				ScanKey1();		
				timer_f = FALSE;			
			}			
		}
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
