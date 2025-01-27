/****************************************************************************
 * @file     main.c
 * @version  V2.0
 * $Revision: 1 $
 * $Date: 14/12/08 11:49a $
 * @brief    Perform A/D Conversion with ADC single cycle scan mode.
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "NUC100Series.h"
#include <stdlib.h>

#define PLL_CLOCK 50000000

/*---------------------------------------------------------------------------------------------------------*/
/* Define Function Prototypes                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void SYS_Init(void);
void UART0_Init(void);
void AdcSingleCycleScanModeTest(void);


void ShuffleData(uint32_t data[30][3], uint32_t size)
{
		uint32_t i, j, k;
    for (i = size - 1; i > 0; i--)
    {
        j = rand() % (i + 1); // Generate a random index
        // Swap data[i] and data[j]
        for (k = 0; k < 3; k++)
        {
            uint32_t temp = data[i][k];
            data[i][k] = data[j][k];
            data[j][k] = temp;
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

    /* Enable ADC module clock */
    CLK_EnableModuleClock(ADC_MODULE);

    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_PLL, CLK_CLKDIV_UART(1));

    /* ADC clock source is 22.1184MHz, set divider to 7, ADC clock is 22.1184/7 MHz */
    CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL1_ADC_S_HIRC, CLK_CLKDIV_ADC(7));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD;

    /* Disable the GPA0 - GPA3 digital input path to avoid the leakage current. */
    GPIO_DISABLE_DIGITAL_PATH(PA, 0xF);

    /* Configure the GPA0 - GPA3 ADC analog input pins */
    //SYS->GPA_MFP &= ~(SYS_GPA_MFP_PA0_Msk | SYS_GPA_MFP_PA1_Msk | SYS_GPA_MFP_PA2_Msk | SYS_GPA_MFP_PA3_Msk);
    //SYS->GPA_MFP |= SYS_GPA_MFP_PA0_ADC0 | SYS_GPA_MFP_PA1_ADC1 | SYS_GPA_MFP_PA2_ADC2 | SYS_GPA_MFP_PA3_ADC3;
    SYS->GPA_MFP &= ~(SYS_GPA_MFP_PA0_Msk | SYS_GPA_MFP_PA1_Msk | SYS_GPA_MFP_PA2_Msk);
    SYS->GPA_MFP |= SYS_GPA_MFP_PA0_ADC0 | SYS_GPA_MFP_PA1_ADC1 | SYS_GPA_MFP_PA2_ADC2;
    SYS->ALT_MFP1 = 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Init UART                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void UART0_Init()
{
    /* Reset IP */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: AdcSingleCycleScanModeTest                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*   None.                                                                                                 */
/*                                                                                                         */
/* Returns:                                                                                                */
/*   None.                                                                                                 */
/*                                                                                                         */
/* Description:                                                                                            */
/*   ADC single cycle scan mode test.                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

void init_Light(void){
		GPIO_SetMode(PC, BIT0, GPIO_PMD_OUTPUT);
		//PC0=1;	
		PC->DOUT &= ~(1 << 0); 
}
void AdcSingleCycleScanModeTest()
{
    // uint8_t u8Option;
    uint32_t u32ChannelCount;
    // int32_t i32ConversionData[8] = {0};
	
    uint32_t u32ChannelData[30][3]; // Array to store 30 sets of 3-channel data
    uint32_t u32Index = 0;

    printf("\n");
    printf("+----------------------------------------------------------------------+\n");
    printf("|                 ADC single cycle scan mode sample code               |\n");
    printf("+----------------------------------------------------------------------+\n");

    ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE_CYCLE, 0x7);

    /* Power on ADC module */
    ADC_POWER_ON(ADC);

    /* Clear the A/D interrupt flag for safe */
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);

		
    //while (u32Index<30)
		//while(1)
		while (u32Index<30)
    {
        /* Set the ADC operation mode as single-cycle, input mode as single-end and
             enable the analog input channel 0, 1, 2 and 3 */

        /* Start A/D conversion */
        ADC_START_CONV(ADC);

        /* Wait conversion done */
        while (!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT));

        //printf("\nConversion result: ");
        //for (u32ChannelCount = 0; u32ChannelCount < 3; u32ChannelCount++)
				//printf("%4d   %4d   %4d", ADC_GET_CONVERSION_DATA(ADC, 0),ADC_GET_CONVERSION_DATA(ADC, 1),ADC_GET_CONVERSION_DATA(ADC, 2));
        //printf("\n");
			
        /* Read data from channels 0, 1, and 2 */
        u32ChannelData[u32Index][0] = ADC_GET_CONVERSION_DATA(ADC, 0);
        u32ChannelData[u32Index][1] = ADC_GET_CONVERSION_DATA(ADC, 1);
        u32ChannelData[u32Index][2] = ADC_GET_CONVERSION_DATA(ADC, 2);	
				CLK_SysTickDelay(100000);	//100ms delay
				
				u32Index++;
    }
		
		
    /* Shuffle data */
    printf("\nShuffling data...\n");
    ShuffleData(u32ChannelData, 30);		
		
    /* Print the collected data */		
    printf("\nCollected Data:\n");
    for (u32Index = 0; u32Index < 30; u32Index++)
    {
        printf("{%4d, %4d, %4d}%s", 
            u32ChannelData[u32Index][0], 
            u32ChannelData[u32Index][1], 
            u32ChannelData[u32Index][2], 
            (u32Index%10 == 9) ? ",\n" : ", ");
    }	
				
		
}

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int main(void)
{

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();
	
		init_Light();

    /*---------------------------------------------------------------------------------------------------------*/
    /* SAMPLE CODE                                                                                             */
    /*---------------------------------------------------------------------------------------------------------*/

    printf("\nSystem clock rate: %d Hz", SystemCoreClock);

    /* Single cycle scan mode test */
    AdcSingleCycleScanModeTest();

    /* Disable ADC module */
    ADC_Close(ADC);

    /* Disable ADC IP clock */
    CLK_DisableModuleClock(ADC_MODULE);

    /* Disable External Interrupt */
    NVIC_DisableIRQ(ADC_IRQn);

    printf("\nExit ADC sample code\n");

    while (1);
}
