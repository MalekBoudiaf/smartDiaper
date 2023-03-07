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
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "project.h"
#include "mI2C.h"
#include "sensorDriver.h"
#include <stdio.h>



float* currentReadings;
static uint8_t data[2]={0};
SemaphoreHandle_t bleSemaphore;
uint8_t powerMode=0;


void genericEventHandler(uint32_t event, void *eventParameter)
{
    
    /* Take an action based on the current event */
    switch (event)
    {
      
        /* This event is received when the BLE stack is Started */
        case CY_BLE_EVT_STACK_ON:
        case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:
        {
            // toggle the red led when connection state changes and start advertising
            Cy_GPIO_Inv(RED_LED_PORT, RED_LED_NUM);
            Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST, CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
            break;
        }
        
        // called when BLE device is connected
        case CY_BLE_EVT_GATT_CONNECT_IND: 
        {
            // turn off the red led when device is connected
            Cy_GPIO_Inv(RED_LED_PORT, RED_LED_NUM); 
             break;
        }
       // called when the central writes something to the periphiral
        case CY_BLE_EVT_GATTS_WRITE_REQ: 
        {
            // handle the written vales and update the GATT database. 
            cy_stc_ble_gatts_write_cmd_req_param_t *writeReqParameter = (cy_stc_ble_gatts_write_cmd_req_param_t *)eventParameter;
            
            /* Turns on the red LED when a value higher than 0 is sent through the BLE RED_LED_ON service */
           if(CY_BLE_SMARTDIAPER_POWER_MODE_CHAR_HANDLE == writeReqParameter->handleValPair.attrHandle)
            {
                // handel the value written by the central
                powerMode = writeReqParameter->handleValPair.value.val[0];
                printf("power value set to %d\n",powerMode);
                cy_stc_ble_gatt_handle_value_pair_t handle;
                cy_stc_ble_gatt_value_t handle_data;
                
                // update the power mode charactirestic in the GATT database
                handle_data.val = &powerMode;
                handle_data.len=1;
                handle.attrHandle=CY_BLE_SMARTDIAPER_POWER_MODE_CHAR_HANDLE;
                handle.value=handle_data;
                Cy_BLE_GATTS_WriteAttributeValueLocal(&handle);
                Cy_BLE_GATTS_WriteRsp(writeReqParameter->connHandle);
            }
            
            break;
        }
       
    }
}

// interrupt will be triggered when the ble stack needs the CPU to handle events
void bleInterruptNotify()
{
    //Cy_BLE_ProcessEvents();
    BaseType_t pxHigherPriorityTaskWoken;
    pxHigherPriorityTaskWoken=pdFALSE;
    // give (increment) the semaphore so that the bleHandle task will unblock and handle events
    xSemaphoreGiveFromISR(bleSemaphore,&pxHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
    
}

void bleHandleTask(void *param){
    (void)param;
    printf("in ble task \n");
    // declare a counting semaphore initialized to 0
    bleSemaphore= xSemaphoreCreateCounting(10,0);
    
    Cy_BLE_Start(genericEventHandler);

    while(Cy_BLE_GetState() != CY_BLE_STATE_ON){
        Cy_BLE_ProcessEvents();
    }
    
    Cy_BLE_RegisterAppHostCallback(bleInterruptNotify);
    
    for(;;){
        // decrement semaphore process ble events then block until the semaphore is given again
        xSemaphoreTake(bleSemaphore,portMAX_DELAY);
        Cy_BLE_ProcessEvents();
    }
}

void sensingTask(void *param){
    (void)param;
    printf("in sensing task \n");
    
    for(;;){
        printf("sensing... \n");
        // read from sensor
        currentReadings = getReadings();
        // send only the decimial part of the values
        printf("current humidity is %f: \n",*currentReadings);
        printf("current temperature is %f: \n",*(currentReadings + 1));
        data[0]=(uint8_t)(*currentReadings); //humidity value
        data[1]=(uint8_t)(*(currentReadings+1)); //temperature value
        // set up GATT handle to send
        cy_stc_ble_gatt_handle_value_pair_t handle;
        cy_stc_ble_gatt_value_t handle_data;
        handle_data.val=(uint8*)data;
        handle_data.len=2;
        // set the target characteristic to write to 
        handle.attrHandle=CY_BLE_SMARTDIAPER_READINGS_CHAR_HANDLE;
        handle.value=handle_data;
        // write the new sensor readings to the GATT database
        Cy_BLE_GATTS_WriteAttributeValueLocal(&handle);
        // send a notification to the central that the value changed
        Cy_BLE_GATTS_SendNotification(cy_ble_connHandle,&handle);
        
        // check if any changes occured to the sensing period
        int sensingPeriod=5000;
        if(powerMode==0){
            sensingPeriod=5000;
        }else if(powerMode==1){
            sensingPeriod=10000;
        }else{
            sensingPeriod=20000;
        }
        vTaskDelay(sensingPeriod);
    }
    vTaskDelete(NULL);
}

int main(void)
{
    __enable_irq(); /* Enable global interrupts. */
    
    mUART_Start();
    setvbuf(stdin,NULL,_IONBF,0);
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    
    sensor_Init();
    CyDelay(2000);
    
    printf("starting program\n");
    
    BaseType_t xReturned1=xTaskCreate(
                    bleHandleTask,       /* Function that implements the task. */
                    "bleHandle",          /* Text name for the task. */
                    8*1024,      /* Stack size in words, not bytes. */
                    NULL,    /* Parameter passed into the task. */
                    2,/* Priority at which the task is created. */
                    NULL );
    
    BaseType_t xReturned2=xTaskCreate(
                    sensingTask,       /* Function that implements the task. */
                    "sensing task",          /* Text name for the task. */
                    1024,      /* Stack size in words, not bytes. */
                    NULL,    /* Parameter passed into the task. */
                    1,/* Priority at which the task is created. */
                    NULL );
    
    
    vTaskStartScheduler();
   
    for(;;)
    {
        
    }
}
