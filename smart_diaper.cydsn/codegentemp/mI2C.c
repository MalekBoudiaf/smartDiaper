/***************************************************************************//**
* \file mI2C.c
* \version 2.0
*
*  This file provides the source code to the API for the I2C Component.
*
********************************************************************************
* \copyright
* Copyright 2016-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "mI2C.h"
#include "sysint/cy_sysint.h"
#include "cyfitter_sysint.h"
#include "cyfitter_sysint_cfg.h"


#if defined(__cplusplus)
extern "C" {
#endif

/***************************************
*     Global variables
***************************************/

/** mI2C_initVar indicates whether the mI2C
*  component has been initialized. The variable is initialized to 0
*  and set to 1 the first time mI2C_Start() is called.
*  This allows  the component to restart without reinitialization
*  after the first call to the mI2C_Start() routine.
*
*  If re-initialization of the component is required, then the
*  mI2C_Init() function can be called before the
*  mI2C_Start() or mI2C_Enable() function.
*/
uint8_t mI2C_initVar = 0U;

/** The instance-specific configuration structure.
* The pointer to this structure should be passed to Cy_SCB_I2C_Init function
* to initialize component with GUI selected settings.
*/
cy_stc_scb_i2c_config_t const mI2C_config =
{
    .i2cMode    = CY_SCB_I2C_MASTER,

    .useRxFifo = false,
    .useTxFifo = false,

    .slaveAddress        = 0U,
    .slaveAddressMask    = 0U,
    .acceptAddrInFifo    = false,
    .ackGeneralAddr      = false,

    .enableWakeFromSleep = false
};

/** The instance-specific context structure.
* It is used while the driver operation for internal configuration and
* data keeping for the I2C. The user should not modify anything in this
* structure.
*/
cy_stc_scb_i2c_context_t mI2C_context;


/*******************************************************************************
* Function Name: mI2C_Start
****************************************************************************//**
*
* Invokes mI2C_Init() and mI2C_Enable().
* Also configures interrupt and low and high oversampling phases.
* After this function call the component is enabled and ready for operation.
* This is the preferred method to begin component operation.
*
* \globalvars
* \ref mI2C_initVar - used to check initial configuration,
* modified  on first function call.
*
*******************************************************************************/
void mI2C_Start(void)
{
    if (0U == mI2C_initVar)
    {
        /* Configure component */
        (void) Cy_SCB_I2C_Init(mI2C_HW, &mI2C_config, &mI2C_context);

    #if (mI2C_ENABLE_MASTER)
        /* Configure desired data rate */
        (void) Cy_SCB_I2C_SetDataRate(mI2C_HW, mI2C_DATA_RATE_HZ, mI2C_CLK_FREQ_HZ);

        #if (mI2C_MANUAL_SCL_CONTROL)
            Cy_SCB_I2C_MasterSetLowPhaseDutyCycle (mI2C_HW, mI2C_LOW_PHASE_DUTY_CYCLE);
            Cy_SCB_I2C_MasterSetHighPhaseDutyCycle(mI2C_HW, mI2C_HIGH_PHASE_DUTY_CYCLE);
        #endif /* (mI2C_MANUAL_SCL_CONTROL) */
    #endif /* (mI2C_ENABLE_MASTER) */

        /* Hook interrupt service routine */
    #if defined(mI2C_SCB_IRQ__INTC_ASSIGNED)
        (void) Cy_SysInt_Init(&mI2C_SCB_IRQ_cfg, &mI2C_Interrupt);
    #endif /* (mI2C_SCB_IRQ__INTC_ASSIGNED) */

        mI2C_initVar = 1U;
    }

    /* Enable interrupt in NVIC */
#if defined(mI2C_SCB_IRQ__INTC_ASSIGNED)
    NVIC_EnableIRQ((IRQn_Type) mI2C_SCB_IRQ_cfg.intrSrc);
#endif /* (mI2C_SCB_IRQ__INTC_ASSIGNED) */

    Cy_SCB_I2C_Enable(mI2C_HW);
}

#if defined(__cplusplus)
}
#endif


/* [] END OF FILE */
