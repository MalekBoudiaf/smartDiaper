
/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include "sensorDriver.h"
#include "mI2C.h"
#include <stdio.h>



#define SENSOR_ADDRESS    0x27
#define I2C_TIMEOUT       100UL



/* Command valid status */
#define TRANSFER_CMPLT        (0x00UL)
#define TRANSFER_ERROR        (0xFFUL)
#define READ_CMPLT            (TRANSFER_CMPLT)
#define READ_ERROR            (TRANSFER_ERROR)


uint32_t sensor_ReadBytes(uint8_t *buffer){
    
    //printf("in the read bytes function ..\n");
        
        cy_en_scb_i2c_status_t  errorStatus;
        uint32_t status = TRANSFER_ERROR;
    
    /* Using low level function initiating master to read data. */
    errorStatus = Cy_SCB_I2C_MasterSendStart(mI2C_HW, SENSOR_ADDRESS, CY_SCB_I2C_WRITE_XFER, I2C_TIMEOUT, &mI2C_context);
    if(errorStatus == CY_SCB_I2C_SUCCESS)
    { 
        //send a stop bit if masterSendStart is succussfull 
        errorStatus = Cy_SCB_I2C_MasterSendStop(mI2C_HW, I2C_TIMEOUT, &mI2C_context);
        if(errorStatus == CY_SCB_I2C_SUCCESS){
            //wait for 60ms for data to be ready
            CyDelay(60);
        }
    }
     
    // initiate a data fetch command
    errorStatus = Cy_SCB_I2C_MasterSendStart(mI2C_HW, SENSOR_ADDRESS, CY_SCB_I2C_READ_XFER, I2C_TIMEOUT, &mI2C_context);
    if(errorStatus == CY_SCB_I2C_SUCCESS){
       
        uint32_t ByteCount = 0UL;        
        cy_en_scb_i2c_command_t cmd = CY_SCB_I2C_ACK;
       

        /* Read data from the slave into the buffer 4 bytes of data in this case */
        do
        {
            if (ByteCount == 3)
            {
                /* The last byte must be NACKed */
                cmd = CY_SCB_I2C_NAK;
            }

            /* Read byte and generate ACK / or prepare for NACK */
            errorStatus = Cy_SCB_I2C_MasterReadByte(mI2C_HW, cmd, buffer + ByteCount, I2C_TIMEOUT, &mI2C_context);
            ++ByteCount;
            printf("reading byte...\n");
        }
        while ((errorStatus == CY_SCB_I2C_SUCCESS) && (ByteCount < 4));
    }
    
    /* Check status of transaction */
    if ((errorStatus == CY_SCB_I2C_SUCCESS)           ||
        (errorStatus == CY_SCB_I2C_MASTER_MANUAL_NAK) ||
        (errorStatus == CY_SCB_I2C_MASTER_MANUAL_ADDR_NAK))
    {
        /* Send Stop condition on the bus */
        if (Cy_SCB_I2C_MasterSendStop(mI2C_HW, I2C_TIMEOUT, &mI2C_context) == CY_SCB_I2C_SUCCESS)
        {
          
                status = TRANSFER_CMPLT;
            
        }
    }
       
    return status;
}



void sensor_Init(void){

    /* Configure pins for I2C operation */
    Cy_GPIO_SetDrivemode(P6_4_PORT, P6_4_NUM, CY_GPIO_DM_PULLUP);
    Cy_GPIO_SetDrivemode(P6_5_PORT, P6_5_NUM, CY_GPIO_DM_PULLUP);
    
    
    uint32_t dataRateSet;
    cy_en_scb_i2c_status_t initStatus;
    
    __enable_irq(); /* Enable global interrupts. */

    /* Initilaize the master I2C. */
    
    /* Configure component. */
    initStatus = Cy_SCB_I2C_Init(mI2C_HW, &mI2C_config, &mI2C_context);
    if(initStatus!=CY_SCB_I2C_SUCCESS)
    {
        HandleError();
    }
    /* Configure desired data rate. */
    dataRateSet = Cy_SCB_I2C_SetDataRate(mI2C_HW, mI2C_DATA_RATE_HZ, mI2C_CLK_FREQ_HZ);
    
    /* check whether data rate set is not greather then required reate. */
    if( dataRateSet > mI2C_DATA_RATE_HZ )
    {
        HandleError();
    }
    
    /* Enable I2C master hardware. */
    Cy_SCB_I2C_Enable(mI2C_HW);
   
}

static void HandleError(void)
{   
     /* Disable all interrupts. */
    __disable_irq();
    
    /* Infinite loop. */
    while(1u) {}
}


float* getReadings(void)
{
    uint32_t errorStatus = TRANSFER_ERROR;
    static float readings[2];
    uint8_t raw_bytes[4] = {0};
    int16_t raw_humidity_data = 0UL;
    int16_t raw_temperature_data = 0UL;
    
    errorStatus = sensor_ReadBytes(raw_bytes);
    if(errorStatus == TRANSFER_ERROR)
    {
        printf("in the error handler\n");
        HandleError();
    }
    // removing the the status bits from the raw humidity reading
    uint8_t maskedMSB= raw_bytes[0] & 0b00111111;
    // combining the first 2 bytes to form the full humidity value 
    raw_humidity_data = maskedMSB << 8 | raw_bytes[1]; 
    // converting to the actual humidity value
    readings[0]=(raw_humidity_data / 16383.0) * 100;
    
    // removing the the two LSB don't care bits bits from the raw temperature reading
    // and forming the full 14bit temperature value
    raw_temperature_data = ((raw_bytes[2] << 8) | raw_bytes[3]) >> 2;
    // converting to the actual temperature value
    readings[1]= (raw_temperature_data / 16383.0) * 165 - 40;
   
    return readings;
}
