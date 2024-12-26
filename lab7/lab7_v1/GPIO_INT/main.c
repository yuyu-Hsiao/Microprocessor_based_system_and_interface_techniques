/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * $Revision: 1 $
 * $Date: 14/12/08 11:49a $
 * @brief    Show the usage of GPIO interrupt function.
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NUC100Series.h"


#define PLL_CLOCK   50000000


/**
 * @brief       GPIO PA/PB IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The PA/PB default IRQ, declared in startup_NUC100Series.s.
 */
volatile uint8_t GPIO_statue = TRUE;
 
 void OpenKeyPad(void)
{
  GPIO_SetMode(PA, BIT0, GPIO_PMD_QUASI);
  GPIO_SetMode(PA, BIT1, GPIO_PMD_QUASI);
  GPIO_SetMode(PA, BIT2, GPIO_PMD_QUASI);
  GPIO_SetMode(PA, BIT3, GPIO_PMD_QUASI);
  GPIO_SetMode(PA, BIT4, GPIO_PMD_QUASI);
  GPIO_SetMode(PA, BIT5, GPIO_PMD_QUASI);
	PA0=1; PA1=1; PA2=0; PA3=1; PA4=1; PA5=1;	
}

 
void GPAB_IRQHandler(void)
{
    /* To check if PB.3 interrupt occurred */

    if(GPIO_GET_INT_FLAG(PA, BIT3))
    {
        GPIO_CLR_INT_FLAG(PA, BIT3);
				GPIO_statue = !GPIO_statue;
        printf("change!!!\n");
    }		
    else
    {
        /* Un-expected interrupt. Just clear all PA, PB interrupts */
        PA->ISRC = PA->ISRC;
        PB->ISRC = PB->ISRC;
        printf("Un-expected interrupts.\n");
    }
}


void WDT_IRQHandler(void)
{
    if(WDT_GET_TIMEOUT_INT_FLAG() == 1)
    {
        /* Clear WDT time-out interrupt flag */
        WDT_CLEAR_TIMEOUT_INT_FLAG();
			
				printf("WDT time-out interrupt occurred.\n");
				if(GPIO_statue){
					WDT_RESET_COUNTER();
					printf("No problem!!!\n");
				}
				else{
					printf("Alarm!!!!~~~reset\n");
				}
		        
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

    /* Waiting for external XTAL clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= (SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD);

}

void UART0_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();
		
		OpenKeyPad();

    printf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    printf("+------------------------------------------------+\n");
    printf("|                   start lab5                   |\n");
    printf("+------------------------------------------------+\n\n");


    //GPIO_EnableInt(PA, 3, GPIO_INT_RISING);
		GPIO_EnableInt(PA, 3, GPIO_INT_FALLING);
    NVIC_EnableIRQ(GPAB_IRQn);
		
		GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_1024);
		GPIO_ENABLE_DEBOUNCE(PA, BIT3 | BIT4 | BIT5);
		
		
		/*------------------------------------------------------------------------------------------------*/
    /* Because of all bits can be written in WDT Control Register are write-protected;
       To program it needs to disable register protection first. */
    SYS_UnlockReg();

		
    /* Enable WDT time-out reset function and select time-out interval to 2^14 * WDT clock then start WDT counting */
    WDT_Open(WDT_TIMEOUT_2POW14, WDT_RESET_DELAY_1026CLK, TRUE, FALSE); 
		
    WDT_EnableInt();						/* Enable WDT interrupt function */    
    NVIC_EnableIRQ(WDT_IRQn);		/* Enable WDT NVIC */
		/*------------------------------------------------------------------------------------------------*/
		
		
    /* Waiting for interrupts */
    while(1){
			if(GPIO_statue){
				printf("Safe!\n");
			}
			else{
				printf("Alarm!\n");
			}
			CLK_SysTickDelay(1000000);  // 1s
		
		}

}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
