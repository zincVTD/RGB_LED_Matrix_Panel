#ifndef BSP_DMA_H
#define BSP_DMA_H

#include <stdint.h>
#include <stddef.h>

#include "stm32f10x_rcc.h"
#include "stm32f10x_dma.h"

/* ================================================================
 * BSP DMA — STM32F103C8T6
 * Abstraction layer for DMA1 (channels 1–7).
 * DMA2 is not available on this device.
 *
 * Primary use-case: Memory → Peripheral (M2P) for TIM PWM + DMA
 * to drive WS2812B without CPU intervention.
 * ================================================================ */

/* ===== Channel IDs ===== */
typedef enum
{
    BSP_DMA_CH1 = 0,
    BSP_DMA_CH2,
    BSP_DMA_CH3,
    BSP_DMA_CH4,
    BSP_DMA_CH5,
    BSP_DMA_CH6,
    BSP_DMA_CH7,
    BSP_DMA_CH_MAX
} BSP_DMA_Channel_t;

/* ===== Transfer direction ===== */
typedef enum
{
    BSP_DMA_DIR_M2P = 0, /* Memory → Peripheral (e.g. TIM CCR) */
    BSP_DMA_DIR_P2M,     /* Peripheral → Memory  (e.g. ADC)    */
    BSP_DMA_DIR_M2M,     /* Memory → Memory                     */
} BSP_DMA_Dir_t;

/* ===== Data width ===== */
typedef enum
{
    BSP_DMA_WIDTH_8BIT = 0,
    BSP_DMA_WIDTH_16BIT,
    BSP_DMA_WIDTH_32BIT,
} BSP_DMA_Width_t;

/* ===== Transfer mode ===== */
typedef enum
{
    BSP_DMA_MODE_NORMAL = 0, /* One-shot, stops after buf_size transfers  */
    BSP_DMA_MODE_CIRCULAR,   /* Auto-reload, loops indefinitely            */
} BSP_DMA_Mode_t;

/* ===== Channel priority ===== */
typedef enum
{
    BSP_DMA_PRIO_LOW = 0,
    BSP_DMA_PRIO_MEDIUM,
    BSP_DMA_PRIO_HIGH,
    BSP_DMA_PRIO_VERY_HIGH,
} BSP_DMA_Prio_t;

/* ===== Configuration struct ===== */
typedef struct
{
    uint32_t periph_addr; /* Destination: peripheral register address    */
    uint32_t mem_addr;    /* Source: memory buffer address               */
    uint16_t buf_size;    /* Number of data units to transfer            */
    BSP_DMA_Dir_t dir;
    BSP_DMA_Width_t periph_width;
    BSP_DMA_Width_t mem_width;
    uint8_t periph_inc; /* 1 = increment peripheral addr after each xfer */
    uint8_t mem_inc;    /* 1 = increment memory addr after each xfer     */
    BSP_DMA_Mode_t mode;
    BSP_DMA_Prio_t priority;
    uint8_t tc_irq_en; /* 1 = enable Transfer Complete interrupt        */
} BSP_DMA_Config_t;

/* ===== Callback type ===== */
typedef void (*BSP_DMA_Callback_t)(BSP_DMA_Channel_t channel);

/* ===== Return codes ===== */
typedef enum
{
    BSP_DMA_OK = 0,
    BSP_DMA_ERROR_PARAM = -1,
    BSP_DMA_ERROR_HW = -2,
    BSP_DMA_ERROR_UNINIT = -5,
    BSP_DMA_ERROR_ALREADY_INIT = -6,
    BSP_DMA_ERROR_STATE = -7,
    BSP_DMA_ERROR_BUSY = -8,
} BSP_DMA_Return_t;

/* ===== State ===== */
typedef enum
{
    BSP_DMA_STATE_UNINIT = -1,
    BSP_DMA_STATE_READY = 0,
    BSP_DMA_STATE_BUSY = 1,
    BSP_DMA_STATE_COMPLETE = 2,
    BSP_DMA_STATE_ERROR = 3,
} BSP_DMA_State_t;

/* ===== Version ===== */
typedef struct
{
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
} BSP_DMA_Version_t;

/* ================================================================
 * Public API
 * ================================================================ */

/* --- Lifecycle --- */
BSP_DMA_Return_t BSP_DMA_Init(BSP_DMA_Channel_t channel, const BSP_DMA_Config_t *cfg);
BSP_DMA_Return_t BSP_DMA_DeInit(BSP_DMA_Channel_t channel);

/* --- Transfer control --- */
BSP_DMA_Return_t BSP_DMA_Start(BSP_DMA_Channel_t channel);
BSP_DMA_Return_t BSP_DMA_Stop(BSP_DMA_Channel_t channel);

/* --- Reconfigure source/size at runtime (no full re-init) --- */
BSP_DMA_Return_t BSP_DMA_Reload(BSP_DMA_Channel_t channel,
                                uint32_t mem_addr,
                                uint16_t buf_size);

/* --- Status --- */
BSP_DMA_Return_t BSP_DMA_GetState(BSP_DMA_Channel_t channel, BSP_DMA_State_t *state);
uint16_t BSP_DMA_GetRemaining(BSP_DMA_Channel_t channel);

/* --- Interrupt callback --- */
BSP_DMA_Return_t BSP_DMA_SetCallback(BSP_DMA_Channel_t channel, BSP_DMA_Callback_t cb);

/* --- IRQ handlers — call from DMA1_ChannelX_IRQHandler --- */
void BSP_DMA_IRQHandler(BSP_DMA_Channel_t channel);

/* --- Version --- */
void BSP_DMA_GetVersion(void);

#endif /* BSP_DMA_H */