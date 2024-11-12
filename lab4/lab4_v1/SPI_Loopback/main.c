
//SPI page.288

#include <stdio.h>
#include "NUC100Series.h"

#define PLL_CLOCK           50000000

/* Function prototype declaration */
void SYS_Init(void);
void SPI_Init(void);


void ADXL_write(uint8_t address, uint8_t data);
uint8_t ADXL_read(uint8_t address);
void ADXL_init(void);

/* ------------- */
/* Main function */
/* ------------- */
int main(void)
{
		uint8_t device_num;
		int16_t rd_xaxis,rd_yaxis,rd_zaxis;
		int8_t offset;
		float cd_xaxis, cd_yaxis, cd_zaxis;
	
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init system, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Configure UART0: 115200, 8-bit word, no parity bit, 1 stop bit. */
    UART_Open(UART0, 115200);

    
    SPI_Init();		/* Init SPI */
	  ADXL_init();	/* Init ADXL */

    printf("\n\n");
    printf("+--------------------------------------------------------------------+\n");
    printf("|                   NUC100 SPI Driver Sample Code                    |\n");
    printf("+--------------------------------------------------------------------+\n");
    printf("\n");
		
		device_num = ADXL_read(0x00);
		printf("\ndevice: 0x%02X\n", device_num);

		

		while(1)
		{
			/*-------------------------------*/
			/*  ADXL data register           */
			/*	DATAX0(0x32), DATAX1(0x33)   */
			/*	DATAY0(0x34), DATAY1(0x35)   */
			/*	DATAZ0(0x36), DATAZ1(0x37)   */
			/*-------------------------------*/
			rd_xaxis = (ADXL_read(0x33)<<8)|ADXL_read(0x32);
			rd_yaxis = (ADXL_read(0x35)<<8)|ADXL_read(0x34);
			rd_zaxis = (ADXL_read(0x37)<<8)|ADXL_read(0x36);
			

			offset = ADXL_read(0x1E);
			cd_xaxis = (float)(rd_xaxis - offset) / (256 - offset);
			
			
			offset = ADXL_read(0x1F);
			cd_yaxis = (float)(rd_yaxis - offset) / (256 - offset);
			
			offset = ADXL_read(0x20);
			cd_zaxis = (float)(rd_zaxis - offset) / (256 - offset);

			printf("x: %.2f, y: %.2f, z: %.2f\n", cd_xaxis, cd_yaxis, cd_zaxis);
			
			CLK_SysTickDelay(500000);  //delay 500ms
			
		}

    /* Close SPI2 */
    //SPI_Close(SPI2);
}

void SYS_Init(void)
{
    /* Enable Internal RC 22.1184 MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Enable external 12 MHz XTAL */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);

    /* Select HXT as the clock source of UART0 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));

    /* Select HCLK as the clock source of SPI2 */
    CLK_SetModuleClock(SPI2_MODULE, CLK_CLKSEL1_SPI2_S_HCLK, MODULE_NoMsk);

    /* Enable UART peripheral clock */
    CLK_EnableModuleClock(UART0_MODULE);
    /* Enable SPI2 peripheral clock */
    CLK_EnableModuleClock(SPI2_MODULE);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set PB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP = SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD;

    /* Setup SPI2 multi-function pins */  //GPD_MFP  page.80
    SYS->GPD_MFP = SYS_GPD_MFP_PD0_SPI2_SS0 | SYS_GPD_MFP_PD1_SPI2_CLK | SYS_GPD_MFP_PD2_SPI2_MISO0 | SYS_GPD_MFP_PD3_SPI2_MOSI0;
    
    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and CyclesPerUs automatically. */
    SystemCoreClockUpdate();
}

void SPI_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init SPI                                                                                                */
    /*---------------------------------------------------------------------------------------------------------*/
    SPI_Open(SPI2, SPI_MASTER, SPI_MODE_3, 8, 2000000);   
		SPI_DisableAutoSS(SPI2);
}



void ADXL_write(uint8_t address, uint8_t data){
		/* #define SPI_WRITE_TX0(spi, u32TxData)   ((spi)->TX[0] = (u32TxData)) */
		
		SPI_WRITE_TX0(SPI2, 0x3F&address);	//set bit7 low to write ADXL   ADXL page.16
		SPI_SET_SS0_LOW(SPI2);
		SPI_TRIGGER(SPI2);
		while(SPI_IS_BUSY(SPI2));	/* Check SPI2 busy status */
	
		SPI_WRITE_TX0(SPI2, data);
		SPI_TRIGGER(SPI2);
		while(SPI_IS_BUSY(SPI2));
		SPI_SET_SS0_HIGH(SPI2);
		
}


uint8_t ADXL_read(uint8_t address){
		
		uint8_t read_content;
		SPI_WRITE_TX0(SPI2, 0x80|address);	//set bit7 high to read ADXL   ADXL page.16
		SPI_SET_SS0_LOW(SPI2);							//set ss(slave-select)(ADXL) low
		SPI_TRIGGER(SPI2);
		while(SPI_IS_BUSY(SPI2));	/* Check SPI2 busy status */

		//Read the contents of register
		SPI_TRIGGER(SPI2);
		while(SPI_IS_BUSY(SPI2));
		read_content = SPI_READ_RX0(SPI2);
	
		SPI_SET_SS0_HIGH(SPI2);							//set ss(slave-select)(ADXL) high
	
		return read_content;
}

void ADXL_init(void){
	/*-----------------------------*/
	/*  Initial ADXL345            */
	/*	POWER_CTL(0x2D): 0x08      */
	/*	DATA_FORMAT(0x31): 0x0B    */
	/*	FIFO_CTL(0x38): 0x80       */
	/*-----------------------------*/
		ADXL_write(0x2D, 0x08);
		ADXL_write(0x31, 0x0B);
		ADXL_write(0x38, 0x80);
}


/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/

