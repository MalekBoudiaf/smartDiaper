
#include "project.h"

int main(void)
{
    __enable_irq(); /* Enable global interrupts. */
    /* Enable CM4.  CY_CORTEX_M4_APPL_ADDR must be updated if CM4 memory layout is changed. */
    if(Cy_SysPm_GetIoFreezeStatus())
    {
        Cy_SysPm_IoUnfreeze();
    }
    Cy_BLE_Start(0);
    Cy_SysEnableCM4(CY_CORTEX_M4_APPL_ADDR); 
    for(;;)
    {
        Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
        // process ble controller events
         Cy_BLE_ProcessEvents();
    }
}

/* [] END OF FILE */
