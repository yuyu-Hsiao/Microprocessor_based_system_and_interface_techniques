/****************************************************************************
 * @file     main.c
 * @version  V2.0
 * $Revision: 1 $
 * $Date: 14/12/08 11:49a $
 * @brief    Perform A/D Conversion with ADC single mode.
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
 
//ADC page.478

#include <stdio.h>
#include <stdlib.h>
#include "NUC100Series.h"


#define PLL_CLOCK       50000000

/*---------------------------------------------------------------------------------------------------------*/
/* Define Function Prototypes                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void SYS_Init(void);
void UART0_Init(void);
void ADC_Init(void);
void GPIO_Init(void);
void AdcSingleModeTest(void);


/*---------------------------------------------------------------------------------------------------------*/
/* Define global variables and constants                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint32_t g_u32AdcIntFlag;


void SYS_Init(void){
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
    SYS->GPA_MFP &= ~(SYS_GPA_MFP_PA0_Msk | SYS_GPA_MFP_PA1_Msk | SYS_GPA_MFP_PA2_Msk | SYS_GPA_MFP_PA3_Msk) ;
    SYS->GPA_MFP |= SYS_GPA_MFP_PA0_ADC0 | SYS_GPA_MFP_PA1_ADC1 | SYS_GPA_MFP_PA2_ADC2 | SYS_GPA_MFP_PA3_ADC3 ;
    SYS->ALT_MFP1 = 0;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Init UART                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void UART0_Init(){
    /* Reset IP */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}


void ADC_Init(){
	
		/* Set the ADC operation mode as single*/ 
		/* input mode as single-end and enable the analog input channel 2 */
		ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE, 0x1 << 2);

		/* Power on ADC module */
		ADC_POWER_ON(ADC);
	
		/* Clear the A/D interrupt flag for safe */
		ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
}

void GPIO_Init(void)
{
		GPIO_SetMode(PC, (BIT12|BIT13|BIT14|BIT15), GPIO_PMD_OUTPUT);
		PC->DOUT = (PC->DOUT & 0x0FFF)|0xF000;
}

void AdcSingleModeTest()
{
    int32_t  i32ConversionData;
		uint8_t scaledValue;
		uint8_t referenceValue = 0;
		uint8_t previousScaledValue = 0;
		uint8_t GPIO_counter = 15;
		uint8_t counter=0;
	
	
    printf("\n");
    printf("+----------------------------------------------------------------------+\n");
    printf("|                      ADC single mode sample code                     |\n");
    printf("+----------------------------------------------------------------------+\n");

		/* Continuous conversion loop */
		while (1)
		{
        if (abs(scaledValue - previousScaledValue) > 5)
        {
            referenceValue = 100-scaledValue;
            previousScaledValue = scaledValue; 
        }
				counter++;
			  if(counter>=referenceValue){
						if(GPIO_counter==11)GPIO_counter=15;
						PC->DOUT = (PC->DOUT & 0x0FFF)|0xF000;
						PC->DOUT &= ~(1<<GPIO_counter);
						GPIO_counter--;
						counter=0;
				}

			
				/* Enable the ADC interrupt */
				ADC_EnableInt(ADC, ADC_ADF_INT);
				NVIC_EnableIRQ(ADC_IRQn);

				/* Reset the ADC interrupt indicator and Start A/D conversion */
				g_u32AdcIntFlag = 0;
				ADC_START_CONV(ADC);		//通過設定ADCR.ADST為1來起動A/D轉換

				/* Wait ADC interrupt (g_u32AdcIntFlag will be set at IRQ_Handler function) */
				while (g_u32AdcIntFlag == 0);

				/* Disable the ADC interrupt */
				ADC_DisableInt(ADC, ADC_ADF_INT);

				/* Get the conversion result of the ADC channel 2 */
				i32ConversionData = ADC_GET_CONVERSION_DATA(ADC, 2);
				scaledValue = (i32ConversionData * 100) / 4095;
				//printf("Conversion result of channel 2: %d\n", i32ConversionData);
				printf("Result of channel: %d\n", scaledValue);

				/* Add a delay if necessary to avoid overwhelming the terminal with data */
				CLK_SysTickDelay(1000);  // 1ms delay for more readable output
		}

}



/*---------------------------------------------------------------------------------------------------------*/
/* ADC interrupt handler  在ADC轉換完成時被調用                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void ADC_IRQHandler(void)
{
    g_u32AdcIntFlag = 1;
	
		/* 對ADSR.ADF寫1，這樣可以將此位元變成0 */
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT); /* clear the A/D conversion flag */
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
	
		/* Init ADC */
		ADC_Init();
		
		/* Init GPIO */
		GPIO_Init();

    /* Single Mode test */
    AdcSingleModeTest();

    /* Disable ADC module */
    ADC_Close(ADC);

    /* Disable ADC IP clock */
    CLK_DisableModuleClock(ADC_MODULE);

    /* Disable External Interrupt */
    NVIC_DisableIRQ(ADC_IRQn);

    printf("\nExit ADC sample code\n");

    while(1);

}



