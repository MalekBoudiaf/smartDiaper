/***************************************************************************//**
* \file mUART.h
* \version 2.0
*
*  This file provides constants and parameter values for the UART component.
*
********************************************************************************
* \copyright
* Copyright 2016-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(mUART_CY_SCB_UART_PDL_H)
#define mUART_CY_SCB_UART_PDL_H

#include "cyfitter.h"
#include "scb/cy_scb_uart.h"

#if defined(__cplusplus)
extern "C" {
#endif

/***************************************
*   Initial Parameter Constants
****************************************/

#define mUART_DIRECTION  (3U)
#define mUART_ENABLE_RTS (0U)
#define mUART_ENABLE_CTS (0U)

/* UART direction enum */
#define mUART_RX    (0x1U)
#define mUART_TX    (0x2U)

#define mUART_ENABLE_RX  (0UL != (mUART_DIRECTION & mUART_RX))
#define mUART_ENABLE_TX  (0UL != (mUART_DIRECTION & mUART_TX))


/***************************************
*        Function Prototypes
***************************************/
/**
* \addtogroup group_general
* @{
*/
/* Component specific functions. */
void mUART_Start(void);

/* Basic functions */
__STATIC_INLINE cy_en_scb_uart_status_t mUART_Init(cy_stc_scb_uart_config_t const *config);
__STATIC_INLINE void mUART_DeInit(void);
__STATIC_INLINE void mUART_Enable(void);
__STATIC_INLINE void mUART_Disable(void);

/* Register callback. */
__STATIC_INLINE void mUART_RegisterCallback(cy_cb_scb_uart_handle_events_t callback);

/* Configuration change. */
#if (mUART_ENABLE_CTS)
__STATIC_INLINE void mUART_EnableCts(void);
__STATIC_INLINE void mUART_DisableCts(void);
#endif /* (mUART_ENABLE_CTS) */

#if (mUART_ENABLE_RTS)
__STATIC_INLINE void     mUART_SetRtsFifoLevel(uint32_t level);
__STATIC_INLINE uint32_t mUART_GetRtsFifoLevel(void);
#endif /* (mUART_ENABLE_RTS) */

__STATIC_INLINE void mUART_EnableSkipStart(void);
__STATIC_INLINE void mUART_DisableSkipStart(void);

#if (mUART_ENABLE_RX)
/* Low level: Receive direction. */
__STATIC_INLINE uint32_t mUART_Get(void);
__STATIC_INLINE uint32_t mUART_GetArray(void *buffer, uint32_t size);
__STATIC_INLINE void     mUART_GetArrayBlocking(void *buffer, uint32_t size);
__STATIC_INLINE uint32_t mUART_GetRxFifoStatus(void);
__STATIC_INLINE void     mUART_ClearRxFifoStatus(uint32_t clearMask);
__STATIC_INLINE uint32_t mUART_GetNumInRxFifo(void);
__STATIC_INLINE void     mUART_ClearRxFifo(void);
#endif /* (mUART_ENABLE_RX) */

#if (mUART_ENABLE_TX)
/* Low level: Transmit direction. */
__STATIC_INLINE uint32_t mUART_Put(uint32_t data);
__STATIC_INLINE uint32_t mUART_PutArray(void *buffer, uint32_t size);
__STATIC_INLINE void     mUART_PutArrayBlocking(void *buffer, uint32_t size);
__STATIC_INLINE void     mUART_PutString(char_t const string[]);
__STATIC_INLINE void     mUART_SendBreakBlocking(uint32_t breakWidth);
__STATIC_INLINE uint32_t mUART_GetTxFifoStatus(void);
__STATIC_INLINE void     mUART_ClearTxFifoStatus(uint32_t clearMask);
__STATIC_INLINE uint32_t mUART_GetNumInTxFifo(void);
__STATIC_INLINE bool     mUART_IsTxComplete(void);
__STATIC_INLINE void     mUART_ClearTxFifo(void);
#endif /* (mUART_ENABLE_TX) */

#if (mUART_ENABLE_RX)
/* High level: Ring buffer functions. */
__STATIC_INLINE void     mUART_StartRingBuffer(void *buffer, uint32_t size);
__STATIC_INLINE void     mUART_StopRingBuffer(void);
__STATIC_INLINE void     mUART_ClearRingBuffer(void);
__STATIC_INLINE uint32_t mUART_GetNumInRingBuffer(void);

/* High level: Receive direction functions. */
__STATIC_INLINE cy_en_scb_uart_status_t mUART_Receive(void *buffer, uint32_t size);
__STATIC_INLINE void     mUART_AbortReceive(void);
__STATIC_INLINE uint32_t mUART_GetReceiveStatus(void);
__STATIC_INLINE uint32_t mUART_GetNumReceived(void);
#endif /* (mUART_ENABLE_RX) */

#if (mUART_ENABLE_TX)
/* High level: Transmit direction functions. */
__STATIC_INLINE cy_en_scb_uart_status_t mUART_Transmit(void *buffer, uint32_t size);
__STATIC_INLINE void     mUART_AbortTransmit(void);
__STATIC_INLINE uint32_t mUART_GetTransmitStatus(void);
__STATIC_INLINE uint32_t mUART_GetNumLeftToTransmit(void);
#endif /* (mUART_ENABLE_TX) */

/* Interrupt handler */
__STATIC_INLINE void mUART_Interrupt(void);
/** @} group_general */


/***************************************
*    Variables with External Linkage
***************************************/
/**
* \addtogroup group_globals
* @{
*/
extern uint8_t mUART_initVar;
extern cy_stc_scb_uart_config_t const mUART_config;
extern cy_stc_scb_uart_context_t mUART_context;
/** @} group_globals */


/***************************************
*         Preprocessor Macros
***************************************/
/**
* \addtogroup group_macros
* @{
*/
/** The pointer to the base address of the hardware */
#define mUART_HW     ((CySCB_Type *) mUART_SCB__HW)
/** @} group_macros */


/***************************************
*    In-line Function Implementation
***************************************/

/*******************************************************************************
* Function Name: mUART_Init
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Init() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_scb_uart_status_t mUART_Init(cy_stc_scb_uart_config_t const *config)
{
   return Cy_SCB_UART_Init(mUART_HW, config, &mUART_context);
}


/*******************************************************************************
* Function Name: mUART_DeInit
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_DeInit() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_DeInit(void)
{
    Cy_SCB_UART_DeInit(mUART_HW);
}


/*******************************************************************************
* Function Name: mUART_Enable
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Enable() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_Enable(void)
{
    Cy_SCB_UART_Enable(mUART_HW);
}


/*******************************************************************************
* Function Name: mUART_Disable
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Disable() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_Disable(void)
{
    Cy_SCB_UART_Disable(mUART_HW, &mUART_context);
}


/*******************************************************************************
* Function Name: mUART_RegisterCallback
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_RegisterCallback() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_RegisterCallback(cy_cb_scb_uart_handle_events_t callback)
{
    Cy_SCB_UART_RegisterCallback(mUART_HW, callback, &mUART_context);
}


#if (mUART_ENABLE_CTS)
/*******************************************************************************
* Function Name: mUART_EnableCts
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_EnableCts() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_EnableCts(void)
{
    Cy_SCB_UART_EnableCts(mUART_HW);
}


/*******************************************************************************
* Function Name: Cy_SCB_UART_DisableCts
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_DisableCts() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_DisableCts(void)
{
    Cy_SCB_UART_DisableCts(mUART_HW);
}
#endif /* (mUART_ENABLE_CTS) */


#if (mUART_ENABLE_RTS)
/*******************************************************************************
* Function Name: mUART_SetRtsFifoLevel
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_SetRtsFifoLevel() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_SetRtsFifoLevel(uint32_t level)
{
    Cy_SCB_UART_SetRtsFifoLevel(mUART_HW, level);
}


/*******************************************************************************
* Function Name: mUART_GetRtsFifoLevel
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetRtsFifoLevel() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t mUART_GetRtsFifoLevel(void)
{
    return Cy_SCB_UART_GetRtsFifoLevel(mUART_HW);
}
#endif /* (mUART_ENABLE_RTS) */


/*******************************************************************************
* Function Name: mUART_EnableSkipStart
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_EnableSkipStart() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_EnableSkipStart(void)
{
    Cy_SCB_UART_EnableSkipStart(mUART_HW);
}


/*******************************************************************************
* Function Name: mUART_DisableSkipStart
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_DisableSkipStart() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_DisableSkipStart(void)
{
    Cy_SCB_UART_DisableSkipStart(mUART_HW);
}


#if (mUART_ENABLE_RX)
/*******************************************************************************
* Function Name: mUART_Get
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Get() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t mUART_Get(void)
{
    return Cy_SCB_UART_Get(mUART_HW);
}


/*******************************************************************************
* Function Name: mUART_GetArray
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetArray() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t mUART_GetArray(void *buffer, uint32_t size)
{
    return Cy_SCB_UART_GetArray(mUART_HW, buffer, size);
}


/*******************************************************************************
* Function Name: mUART_GetArrayBlocking
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetArrayBlocking() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_GetArrayBlocking(void *buffer, uint32_t size)
{
    Cy_SCB_UART_GetArrayBlocking(mUART_HW, buffer, size);
}


/*******************************************************************************
* Function Name: mUART_GetRxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetRxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t mUART_GetRxFifoStatus(void)
{
    return Cy_SCB_UART_GetRxFifoStatus(mUART_HW);
}


/*******************************************************************************
* Function Name: mUART_ClearRxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_ClearRxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_ClearRxFifoStatus(uint32_t clearMask)
{
    Cy_SCB_UART_ClearRxFifoStatus(mUART_HW, clearMask);
}


/*******************************************************************************
* Function Name: mUART_GetNumInRxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetNumInRxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t mUART_GetNumInRxFifo(void)
{
    return Cy_SCB_UART_GetNumInRxFifo(mUART_HW);
}


/*******************************************************************************
* Function Name: mUART_ClearRxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_ClearRxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_ClearRxFifo(void)
{
    Cy_SCB_UART_ClearRxFifo(mUART_HW);
}
#endif /* (mUART_ENABLE_RX) */


#if (mUART_ENABLE_TX)
/*******************************************************************************
* Function Name: mUART_Put
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Put() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t mUART_Put(uint32_t data)
{
    return Cy_SCB_UART_Put(mUART_HW,data);
}


/*******************************************************************************
* Function Name: mUART_PutArray
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_PutArray() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t mUART_PutArray(void *buffer, uint32_t size)
{
    return Cy_SCB_UART_PutArray(mUART_HW, buffer, size);
}


/*******************************************************************************
* Function Name: mUART_PutArrayBlocking
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_PutArrayBlocking() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_PutArrayBlocking(void *buffer, uint32_t size)
{
    Cy_SCB_UART_PutArrayBlocking(mUART_HW, buffer, size);
}


/*******************************************************************************
* Function Name: mUART_PutString
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_PutString() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_PutString(char_t const string[])
{
    Cy_SCB_UART_PutString(mUART_HW, string);
}


/*******************************************************************************
* Function Name: mUART_SendBreakBlocking
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_SendBreakBlocking() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_SendBreakBlocking(uint32_t breakWidth)
{
    Cy_SCB_UART_SendBreakBlocking(mUART_HW, breakWidth);
}


/*******************************************************************************
* Function Name: mUART_GetTxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetTxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t mUART_GetTxFifoStatus(void)
{
    return Cy_SCB_UART_GetTxFifoStatus(mUART_HW);
}


/*******************************************************************************
* Function Name: mUART_ClearTxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_ClearTxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_ClearTxFifoStatus(uint32_t clearMask)
{
    Cy_SCB_UART_ClearTxFifoStatus(mUART_HW, clearMask);
}


/*******************************************************************************
* Function Name: mUART_GetNumInTxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetNumInTxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t mUART_GetNumInTxFifo(void)
{
    return Cy_SCB_UART_GetNumInTxFifo(mUART_HW);
}


/*******************************************************************************
* Function Name: mUART_IsTxComplete
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_IsTxComplete() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE bool mUART_IsTxComplete(void)
{
    return Cy_SCB_UART_IsTxComplete(mUART_HW);
}


/*******************************************************************************
* Function Name: mUART_ClearTxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_ClearTxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_ClearTxFifo(void)
{
    Cy_SCB_UART_ClearTxFifo(mUART_HW);
}
#endif /* (mUART_ENABLE_TX) */


#if (mUART_ENABLE_RX)
/*******************************************************************************
* Function Name: mUART_StartRingBuffer
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_StartRingBuffer() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_StartRingBuffer(void *buffer, uint32_t size)
{
    Cy_SCB_UART_StartRingBuffer(mUART_HW, buffer, size, &mUART_context);
}


/*******************************************************************************
* Function Name: mUART_StopRingBuffer
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_StopRingBuffer() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_StopRingBuffer(void)
{
    Cy_SCB_UART_StopRingBuffer(mUART_HW, &mUART_context);
}


/*******************************************************************************
* Function Name: mUART_ClearRingBuffer
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_ClearRingBuffer() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_ClearRingBuffer(void)
{
    Cy_SCB_UART_ClearRingBuffer(mUART_HW, &mUART_context);
}


/*******************************************************************************
* Function Name: mUART_GetNumInRingBuffer
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetNumInRingBuffer() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t mUART_GetNumInRingBuffer(void)
{
    return Cy_SCB_UART_GetNumInRingBuffer(mUART_HW, &mUART_context);
}


/*******************************************************************************
* Function Name: mUART_Receive
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Receive() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_scb_uart_status_t mUART_Receive(void *buffer, uint32_t size)
{
    return Cy_SCB_UART_Receive(mUART_HW, buffer, size, &mUART_context);
}


/*******************************************************************************
* Function Name: mUART_GetReceiveStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetReceiveStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t mUART_GetReceiveStatus(void)
{
    return Cy_SCB_UART_GetReceiveStatus(mUART_HW, &mUART_context);
}


/*******************************************************************************
* Function Name: mUART_AbortReceive
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_AbortReceive() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_AbortReceive(void)
{
    Cy_SCB_UART_AbortReceive(mUART_HW, &mUART_context);
}


/*******************************************************************************
* Function Name: mUART_GetNumReceived
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetNumReceived() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t mUART_GetNumReceived(void)
{
    return Cy_SCB_UART_GetNumReceived(mUART_HW, &mUART_context);
}
#endif /* (mUART_ENABLE_RX) */


#if (mUART_ENABLE_TX)
/*******************************************************************************
* Function Name: mUART_Transmit
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Transmit() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_scb_uart_status_t mUART_Transmit(void *buffer, uint32_t size)
{
    return Cy_SCB_UART_Transmit(mUART_HW, buffer, size, &mUART_context);
}


/*******************************************************************************
* Function Name: mUART_GetTransmitStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetTransmitStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t mUART_GetTransmitStatus(void)
{
    return Cy_SCB_UART_GetTransmitStatus(mUART_HW, &mUART_context);
}


/*******************************************************************************
* Function Name: mUART_AbortTransmit
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_AbortTransmit() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_AbortTransmit(void)
{
    Cy_SCB_UART_AbortTransmit(mUART_HW, &mUART_context);
}


/*******************************************************************************
* Function Name: mUART_GetNumLeftToTransmit
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetNumLeftToTransmit() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t mUART_GetNumLeftToTransmit(void)
{
    return Cy_SCB_UART_GetNumLeftToTransmit(mUART_HW, &mUART_context);
}
#endif /* (mUART_ENABLE_TX) */


/*******************************************************************************
* Function Name: mUART_Interrupt
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Interrupt() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void mUART_Interrupt(void)
{
    Cy_SCB_UART_Interrupt(mUART_HW, &mUART_context);
}

#if defined(__cplusplus)
}
#endif

#endif /* mUART_CY_SCB_UART_PDL_H */


/* [] END OF FILE */
