/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * $Revision: 2 $
 * $Date: 14/12/29 3:22p $
 * @brief
 *           Show a I2C Master how to access Slave.
 *           This sample code needs to work with I2C_Slave.
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/

//i2c page.204

#include <stdio.h>
#include "NUC100Series.h"

#define PLLCON_SETTING      CLK_PLLCON_50MHz_HXT
#define PLL_CLOCK           50000000

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint8_t g_u8DeviceAddr;  				// slave device address
volatile uint8_t slave_reg_addr;					// slave register address
volatile uint8_t g_u8MstRxData;						// rx data
volatile uint8_t g_u8MstTxData;						// tx data
volatile uint8_t is_Tx_send = 0;					// tx data transfer completed or not
volatile uint8_t g_u8MstEndFlag = 0;			// i2c transfer finished or not




volatile uint8_t g_u8MstTxLen = 0; 

typedef void (*I2C_FUNC)(uint32_t u32Status);

static volatile I2C_FUNC s_I2C0HandlerFn = NULL;

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 IRQ Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void I2C0_IRQHandler(void)
{
    uint32_t u32Status;

    u32Status = I2C_GET_STATUS(I2C0);

    if(I2C_GET_TIMEOUT_FLAG(I2C0))
    {
        /* Clear I2C0 Timeout Flag */
        I2C_ClearTimeoutFlag(I2C0);
    }
    else
    {
        if(s_I2C0HandlerFn != NULL)
            s_I2C0HandlerFn(u32Status);
    }
}


/*---------------------------------------------------------------------------------------------------------*/
/*  I2C Rx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterRx(uint32_t u32Status)
{
    if(u32Status == 0x08)                       /* START has been transmitted and prepare SLA+W */
    {
        I2C_SET_DATA(I2C0, (g_u8DeviceAddr << 1));    /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
    }
    else if(u32Status == 0x18)                  /* SLA+W has been transmitted and ACK has been received */
    {
        I2C_SET_DATA(I2C0, slave_reg_addr);//g_au8MstTxData[g_u8MstDataLen++]);
        I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
    }
    else if(u32Status == 0x20)                  /* SLA+W has been transmitted and NACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_STA_STO_SI);
    }
    else if(u32Status == 0x28)                  /* DATA has been transmitted and ACK has been received */
    {
			I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_STA_SI);
    }
    else if(u32Status == 0x10)                  /* Repeat START has been transmitted and prepare SLA+R */
    {
        I2C_SET_DATA(I2C0, ((g_u8DeviceAddr << 1) | 0x01));   /* Write SLA+R to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
    }
    else if(u32Status == 0x40)                  /* SLA+R has been transmitted and ACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
    }
    else if(u32Status == 0x58)                  /* DATA has been received and NACK has been returned */
    {
        g_u8MstRxData = (unsigned char) I2C_GET_DATA(I2C0);
        I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_STO_SI);
        g_u8MstEndFlag = 1;
    }
    else
    {
        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
    }
}
/*---------------------------------------------------------------------------------------------------------*/
/*  I2C Tx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterTx(uint32_t u32Status)
{
    if(u32Status == 0x08)                       /* START has been transmitted */
    {
        I2C_SET_DATA(I2C0, g_u8DeviceAddr << 1);    /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
    }
    else if(u32Status == 0x18)                  /* SLA+W has been transmitted and ACK has been received */
    {
        I2C_SET_DATA(I2C0, slave_reg_addr);
        I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
    }
    else if(u32Status == 0x20)                  /* SLA+W has been transmitted and NACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_STA_STO_SI);
    }
    else if(u32Status == 0x28)                  /* DATA has been transmitted and ACK has been received */
    {
        if(is_Tx_send)
        {
            I2C_SET_DATA(I2C0, g_u8MstTxData);
            I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
						is_Tx_send = TRUE;
        }
        else
        {
            I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_STO_SI);
						is_Tx_send = FALSE;
            g_u8MstEndFlag = 1;
        }
    }
    else
    {
        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
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

    /* Enable I2C0 module clock */
    CLK_EnableModuleClock(I2C0_MODULE);

    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP = SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD;

    /* Set GPA multi-function pins for I2C0 SDA and SCL */
    SYS->GPA_MFP = SYS_GPA_MFP_PA8_I2C0_SDA | SYS_GPA_MFP_PA9_I2C0_SCL;
}

void UART0_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset IP */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

void I2C0_Init(void)
{
    /* Open I2C module and set bus clock */
    I2C_Open(I2C0, 100000);

    /* Get I2C0 Bus Clock */
    printf("I2C clock %d Hz\n", I2C_GetBusClockFreq(I2C0));

    /* Enable I2C interrupt */
    I2C_EnableInt(I2C0);
    NVIC_EnableIRQ(I2C0_IRQn);
}

void I2C0_Close(void)
{
    /* Disable I2C0 interrupt and clear corresponding NVIC bit */
    I2C_DisableInt(I2C0);
    NVIC_DisableIRQ(I2C0_IRQn);

    /* Disable I2C0 and close I2C0 clock */
    I2C_Close(I2C0);
    CLK_DisableModuleClock(I2C0_MODULE);
}



void ADXL_WriteBytes(uint8_t slvaddr, uint8_t reg_addr, uint8_t data) {
    g_u8DeviceAddr = slvaddr;            
		slave_reg_addr = reg_addr;
		g_u8MstTxData = data;
	
    g_u8MstEndFlag = 0;

    s_I2C0HandlerFn = I2C_MasterTx;

    I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_STA);

    while(g_u8MstEndFlag == 0);
}

uint8_t ADXL_ReadBytes(uint8_t slvaddr, uint8_t reg_addr) {
		uint8_t read_data;
    g_u8DeviceAddr = slvaddr;           
              
		slave_reg_addr = reg_addr;

    g_u8MstEndFlag = 0;

    s_I2C0HandlerFn = I2C_MasterRx;

    I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_STA);

    while(g_u8MstEndFlag == 0);
		read_data = g_u8MstRxData;
		
		return read_data;
}

void ADXL_init(void){
	/*-----------------------------*/
	/*  Initial ADXL345            */
	/*	POWER_CTL(0x2D): 0x08      */
	/*	DATA_FORMAT(0x31): 0x0B    */
	/*	FIFO_CTL(0x38): 0x80       */
	// ADXL345 address 0x53
	ADXL_WriteBytes(0x53, 0x2D, 0x08);
	ADXL_WriteBytes(0x53, 0x31, 0x0B);
	ADXL_WriteBytes(0x53, 0x38, 0x80);	
}

int8_t my_round(float number){
    return (number >= 0) ? (int)(number + 0.5f) : (int)(number - 0.5f);
}

void ADXL_calibrate(void){
    int16_t X_avg = 0, Y_avg = 0, Z_avg = 0;
		int16_t x, y, z;
    int8_t x_offset, y_offset, z_offset;
		uint8_t i;
	
	

    for(i = 0; i < 10; i++){

				x = (ADXL_ReadBytes(0x53,0x33)<<8)|ADXL_ReadBytes(0x53,0x32);
				y = (ADXL_ReadBytes(0x53,0x35)<<8)|ADXL_ReadBytes(0x53,0x34);
				z = (ADXL_ReadBytes(0x53,0x37)<<8)|ADXL_ReadBytes(0x53,0x36);

        X_avg += x;
        Y_avg += y;
        Z_avg += z;

        CLK_SysTickDelay(10000); // 10ms
    }

    X_avg /= 10;
    Y_avg /= 10;
    Z_avg /= 10;
		
		printf("XYZ_avg: X=%d, Y=%d, Z=%d\n\n", X_avg, Y_avg, Z_avg);
		
		CLK_SysTickDelay(5000000);  // 1s

    x_offset = -my_round((float)X_avg / 4.0f);
    y_offset = -my_round((float)Y_avg / 4.0f);
    z_offset = -my_round(((float)(Z_avg - 256)) / 4.0f);

    ADXL_WriteBytes(0x53, 0x1E, (uint8_t)x_offset);
    ADXL_WriteBytes(0x53, 0x1F, (uint8_t)y_offset);
    ADXL_WriteBytes(0x53, 0x20, (uint8_t)z_offset);

    printf("Calibration offsets written: X=%d, Y=%d, Z=%d\n\n", x_offset, y_offset, z_offset);
		
		CLK_SysTickDelay(5000000);  // 1s
		
}
/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
		uint8_t device_num;
		int16_t rd_xaxis,rd_yaxis,rd_zaxis;
		float cd_xaxis, cd_yaxis, cd_zaxis;	
	
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init UART0 for printf */
    UART0_Init();

    /* Lock protected registers */
    SYS_LockReg();
	
		

    /*
        This sample code sets I2C bus clock to 100kHz. Then, Master accesses Slave with Byte Write
        and Byte Read operations, and check if the read data is equal to the programmed data.
    */
    printf("\n");
    printf("+--------------------------------------------------------+\n");
    printf("| NUC100 I2C Driver Sample Code(Master) for access Slave |\n");
    printf("|                                                        |\n");
    printf("| I2C Master (I2C0) <---> I2C Slave(I2C0)                |\n");
    printf("+--------------------------------------------------------+\n");

    printf("Configure I2C0 as a master.\n");
    printf("The I/O connection for I2C0:\n");
    printf("I2C0_SDA(PA.8), I2C0_SCL(PA.9)\n");

    /* Init I2C0 */
    I2C0_Init();

		ADXL_init();
		
		device_num=ADXL_ReadBytes(0x53,0x00);
		printf("\ndevice: 0x%02X\n\n", device_num);
		
		
		ADXL_calibrate();
		
		while(1){			
			/*-------------------------------*/
			/*  ADXL data register           */
			/*	DATAX0(0x32), DATAX1(0x33)   */
			/*	DATAY0(0x34), DATAY1(0x35)   */
			/*	DATAZ0(0x36), DATAZ1(0x37)   */
			/*-------------------------------*/
			rd_xaxis = (ADXL_ReadBytes(0x53,0x33)<<8)|ADXL_ReadBytes(0x53,0x32);
			rd_yaxis = (ADXL_ReadBytes(0x53,0x35)<<8)|ADXL_ReadBytes(0x53,0x34);
			rd_zaxis = (ADXL_ReadBytes(0x53,0x37)<<8)|ADXL_ReadBytes(0x53,0x36);
			

			cd_xaxis = (float)rd_xaxis / 256.0;
			cd_yaxis = (float)rd_yaxis / 256.0;
			cd_zaxis = (float)rd_zaxis / 256.0;

			printf("x: %.2f, y: %.2f, z: %.2f\n", cd_xaxis, cd_yaxis, cd_zaxis);
			
			CLK_SysTickDelay(500000);  //delay 500ms
		}


    //s_I2C0HandlerFn = NULL;

    /* Close I2C0 */
    //I2C0_Close();

    //while(1);
}



