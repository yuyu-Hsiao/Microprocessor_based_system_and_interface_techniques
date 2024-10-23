/****************************************************************************
 * @file     main.c
 * @version  V2.00
 * $Revision: 1 $
 * $Date: 14/12/08 11:50a $
 * @brief    Transmit and receive data from PC terminal through RS232 interface.
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
 
 
 //UART interrupt Enable register page.356
 //SYS->GPB_MFP page.73
 //UART page.340
 //UA_ISR page.367
 
#include <stdio.h>
#include <string.h>
#include "NUC100Series.h"


#define PLL_CLOCK           50000000

#define RXBUFSIZE 1024
#define CMD_BUFSIZE 64

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t g_u8RecData[RXBUFSIZE]  = {0};

volatile uint32_t g_u32comRbytes = 0;
volatile uint32_t g_u32comRhead  = 0;
volatile uint32_t g_u32comRtail  = 0;

char cmdBuffer[CMD_BUFSIZE];
uint32_t cmdIndex = 0;

volatile int32_t g_bEnter        = FALSE;

/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);
void UART_TEST_HANDLE(void);


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
    SYS->GPB_MFP |= SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD;

}

void UART0_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    
    SYS_ResetModule(UART0_RST);	/* Reset UART0 */   
    UART_Open(UART0, 9600);			/* Configure UART0 and set UART0 Baudrate */	
		
}

/*---------------------------------------------------------------------------------------------------------*/
/* UART Test Sample                                                                                        */
/* Test Item                                                                                               */
/* It sends the received data to HyperTerminal.                                                            */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/


void GPIO_Init(void)
{
		GPIO_SetMode(PA, (BIT12|BIT13|BIT14), GPIO_PMD_OUTPUT);
    PA12 = 1;
    PA13 = 1;
    PA14 = 1;
}

void ParseCommand(char *command)
{
    if (strcmp(command, "red on") == 0)
    {
        PA14 = 0; 
    }
    else if (strcmp(command, "red off") == 0)
    {
        PA14 = 1; 
    }
    else if (strcmp(command, "green on") == 0)
    {
        PA13 = 0; 
    }
    else if (strcmp(command, "green off") == 0)
    {
        PA13 = 1; 
    }
    else if (strcmp(command, "blue on") == 0)
    {
        PA12 = 0; 
    }
    else if (strcmp(command, "blue off") == 0)
    {
        PA12 = 1; 
    }
}


int main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf and testing */
    UART0_Init();
	
		GPIO_Init();

    /*---------------------------------------------------------------------------------------------------------*/
    /* SAMPLE CODE                                                                                             */
    /*---------------------------------------------------------------------------------------------------------*/

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);
	  printf("LAB2-UART");
		printf("\nInput:");
		
		UART_EnableInt(UART0, (UART_IER_RDA_IEN_Msk | UART_IER_THRE_IEN_Msk | UART_IER_TOUT_IEN_Msk));
    while(1);

}

/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle UART Channel 0 interrupt event                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void UART02_IRQHandler(void)
{
    UART_TEST_HANDLE();
}

/*---------------------------------------------------------------------------------------------------------*/
/* UART Callback function                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/


void UART_TEST_HANDLE()
{
    uint8_t u8InChar = 0xFF;
    uint32_t u32IntSts = UART0->ISR;

    if(u32IntSts & UART_ISR_RDA_INT_Msk)
    {
        /* Get all the input characters */
        while(UART_IS_RX_READY(UART0))
        {
            /* Get the character + */
            u8InChar = UART_READ(UART0);
						
            /* Check if buffer full */
            if(g_u32comRbytes < RXBUFSIZE)
            {
                /* Enqueue the character */
                g_u8RecData[g_u32comRtail] = u8InChar;
                g_u32comRtail = (g_u32comRtail == (RXBUFSIZE - 1)) ? 0 : (g_u32comRtail + 1);
                g_u32comRbytes++;
            }
						
            if(u8InChar == 0x0D)
            {
								g_bEnter = TRUE;
                cmdBuffer[cmdIndex] = '\0';
                cmdIndex = 0;
								ParseCommand(cmdBuffer);
                
            }
            else
            {
                if(cmdIndex < CMD_BUFSIZE - 1)
                {
                    cmdBuffer[cmdIndex++] = u8InChar;
                }
                else
                {
                    cmdIndex = 0;
                    printf("\nCommand too long!\n");
                    printf("\nInput:");
                }
            }			
        }
    }
		
    if(u32IntSts & UART_ISR_THRE_INT_Msk)
    {
			if(g_bEnter){
					uint16_t tmp;
					tmp = g_u32comRtail;
					if(g_u32comRhead != tmp)
					{
							u8InChar = g_u8RecData[g_u32comRhead];
							UART_WRITE(UART0, u8InChar);
							g_u32comRhead = (g_u32comRhead == (RXBUFSIZE - 1)) ? 0 : (g_u32comRhead + 1);
							g_u32comRbytes--;
					}else{
							g_bEnter = FALSE;
							printf("\nInput:");
					}
			}
    }					
}



