/**
 * @file    bsp_timer.h
 * @brief   BSP Timer module for STM32F103C8T6
 * @details Supports: Base timer, PWM output, DMA request, Delay
 *          Timers:   TIM1 (Advanced), TIM2, TIM3, TIM4 (General Purpose)
 */

#ifndef BSP_TIMER_H
#define BSP_TIMER_H

#include "stm32f10x.h"
#include <stdint.h>
#include <stdbool.h>

/*============================================================================
 *                          TYPE DEFINITIONS
 *===========================================================================*/

/**
 * @brief Timer instance selection
 * @note  TIM1 is Advanced Timer (APB2), TIM2-4 are General Purpose (APB1)
 */
typedef enum
{
    BSP_TIMER_1 = 0, /**< TIM1 - Advanced timer (APB2, 72MHz) */
    BSP_TIMER_2,     /**< TIM2 - General purpose (APB1, 72MHz) */
    BSP_TIMER_3,     /**< TIM3 - General purpose (APB1, 72MHz) */
    BSP_TIMER_4,     /**< TIM4 - General purpose (APB1, 72MHz) */
    BSP_TIMER_COUNT  /**< Number of available timers */
} BSP_Timer_Instance_t;

/**
 * @brief PWM channel selection
 */
typedef enum
{
    BSP_TIMER_CH1 = 0,
    BSP_TIMER_CH2,
    BSP_TIMER_CH3,
    BSP_TIMER_CH4,
    BSP_TIMER_CH_COUNT
} BSP_Timer_Channel_t;

/**
 * @brief Timer DMA request source
 */
typedef enum
{
    BSP_TIMER_DMA_UPDATE = 0, /**< DMA request on Update event */
    BSP_TIMER_DMA_CC1,        /**< DMA request on CC1 event */
    BSP_TIMER_DMA_CC2,        /**< DMA request on CC2 event */
    BSP_TIMER_DMA_CC3,        /**< DMA request on CC3 event */
    BSP_TIMER_DMA_CC4         /**< DMA request on CC4 event */
} BSP_Timer_DMA_Source_t;

/**
 * @brief Timer status/error codes
 */
typedef enum
{
    BSP_TIMER_OK = 0,
    BSP_TIMER_ERROR_INVALID_INSTANCE,
    BSP_TIMER_ERROR_INVALID_CHANNEL,
    BSP_TIMER_ERROR_NOT_INITIALIZED,
    BSP_TIMER_ERROR_ALREADY_INITIALIZED
} BSP_Timer_Status_t;

/**
 * @brief Callback function type for timer interrupt
 */
typedef void (*BSP_Timer_Callback_t)(void);

/**
 * @brief Timer base configuration
 */
typedef struct
{
    uint16_t prescaler; /**< Prescaler value (0 to 65535) */
    uint16_t period;    /**< Auto-reload value / ARR (0 to 65535) */
    bool auto_reload;   /**< Enable auto-reload preload */
} BSP_Timer_Base_Config_t;

/**
 * @brief PWM configuration for a specific channel
 */
typedef struct
{
    BSP_Timer_Channel_t channel; /**< PWM channel (CH1-CH4) */
    uint16_t pulse;              /**< Initial CCR value (duty) */
    bool preload;                /**< Enable CCR preload (recommended for DMA) */
} BSP_Timer_PWM_Config_t;

/*============================================================================
 *                     HELPER MACROS FOR CALCULATION
 *===========================================================================*/

/** System clock frequency (verify with your setup) */
#define BSP_TIMER_SYSTEM_CLOCK_HZ 72000000UL

/**
 * @brief Calculate ARR value for desired timer frequency
 * @param psc   Prescaler value (actual, not register value)
 *              Register value = psc - 1
 * @param freq  Desired frequency in Hz
 * @return      ARR value (period - 1)
 *
 * Example: 800kHz with PSC=1 → ARR = 72000000/1/800000 - 1 = 89
 */
#define BSP_TIMER_CALC_ARR(psc, freq) \
    ((uint16_t)((BSP_TIMER_SYSTEM_CLOCK_HZ / (psc) / (freq)) - 1U))

/**
 * @brief Calculate CCR value for desired duty cycle percentage
 * @param arr   Auto-reload value
 * @param pct   Duty cycle in percent (0-100)
 * @return      CCR value
 *
 * Example: ARR=89, 66% duty → CCR = (89+1)*66/100 - 1 ≈ 58
 */
#define BSP_TIMER_CALC_CCR(arr, pct) \
    ((uint16_t)(((uint32_t)((arr) + 1U) * (pct)) / 100U))

/*============================================================================
 *                      PUBLIC API - BASE TIMER
 *===========================================================================*/

/**
 * @brief  Initialize timer time base
 * @param  instance  Timer to initialize (BSP_TIMER_1 to BSP_TIMER_4)
 * @param  config    Pointer to base configuration
 * @retval BSP_TIMER_OK on success
 *
 * @note   Automatically enables RCC clock for the timer
 * @note   Does NOT start the timer — call BSP_Timer_Start() separately
 *
 * Example:
 *   BSP_Timer_Base_Config_t cfg = {
 *       .prescaler   = 72 - 1,    // 72MHz / 72 = 1MHz tick
 *       .period      = 1000 - 1,  // 1MHz / 1000 = 1kHz overflow
 *       .auto_reload = true
 *   };
 *   BSP_Timer_Base_Init(BSP_TIMER_3, &cfg);
 */
BSP_Timer_Status_t BSP_Timer_Base_Init(BSP_Timer_Instance_t instance,
                                       const BSP_Timer_Base_Config_t *config);

/**
 * @brief  Start timer counter
 */
BSP_Timer_Status_t BSP_Timer_Start(BSP_Timer_Instance_t instance);

/**
 * @brief  Stop timer counter
 */
BSP_Timer_Status_t BSP_Timer_Stop(BSP_Timer_Instance_t instance);

/*============================================================================
 *                      PUBLIC API - INTERRUPT
 *===========================================================================*/

/**
 * @brief  Enable timer Update interrupt and register callback
 * @param  instance  Timer instance
 * @param  callback  Function to call on Update event (NULL to disable)
 * @param  priority  NVIC priority (0 = highest, 15 = lowest)
 *
 * Example:
 *   void on_timer3_tick(void) {
 *       frame_counter++;
 *   }
 *   BSP_Timer_EnableInterrupt(BSP_TIMER_3, on_timer3_tick, 2);
 */
BSP_Timer_Status_t BSP_Timer_EnableInterrupt(BSP_Timer_Instance_t instance,
                                             BSP_Timer_Callback_t callback,
                                             uint8_t priority);

/**
 * @brief  Disable timer Update interrupt
 */
BSP_Timer_Status_t BSP_Timer_DisableInterrupt(BSP_Timer_Instance_t instance);

/*============================================================================
 *                      PUBLIC API - PWM
 *===========================================================================*/

/**
 * @brief  Initialize PWM output on a specific channel
 * @param  instance   Timer instance
 * @param  pwm_config Pointer to PWM channel configuration
 * @retval BSP_TIMER_OK on success
 *
 * @note   Automatically configures GPIO pin as AF_PP
 * @note   Timer base MUST be initialized first via BSP_Timer_Base_Init()
 * @note   For TIM1: automatically calls TIM_CtrlPWMOutputs(TIM1, ENABLE)
 *
 * Example:
 *   // 800kHz PWM on TIM1_CH1 (PA8) for WS2812
 *   BSP_Timer_Base_Config_t base = {
 *       .prescaler   = 1 - 1,     // No prescaler → 72MHz
 *       .period      = 90 - 1,    // 72MHz / 90 = 800kHz
 *       .auto_reload = true
 *   };
 *   BSP_Timer_Base_Init(BSP_TIMER_1, &base);
 *
 *   BSP_Timer_PWM_Config_t pwm = {
 *       .channel = BSP_TIMER_CH1,
 *       .pulse   = 0,             // Start with 0% duty
 *       .preload = true           // Required for DMA mode
 *   };
 *   BSP_Timer_PWM_Init(BSP_TIMER_1, &pwm);
 */
BSP_Timer_Status_t BSP_Timer_PWM_Init(BSP_Timer_Instance_t instance,
                                      const BSP_Timer_PWM_Config_t *pwm_config);

/**
 * @brief  Set PWM duty cycle (raw CCR value)
 * @param  instance  Timer instance
 * @param  channel   PWM channel
 * @param  pulse     CCR value (0 to ARR)
 */
BSP_Timer_Status_t BSP_Timer_PWM_SetPulse(BSP_Timer_Instance_t instance,
                                          BSP_Timer_Channel_t channel,
                                          uint16_t pulse);

/**
 * @brief  Set PWM duty cycle (percentage)
 * @param  instance  Timer instance
 * @param  channel   PWM channel
 * @param  percent   Duty cycle 0-100
 */
BSP_Timer_Status_t BSP_Timer_PWM_SetDutyPercent(BSP_Timer_Instance_t instance,
                                                BSP_Timer_Channel_t channel,
                                                uint8_t percent);

/*============================================================================
 *                      PUBLIC API - DMA SUPPORT
 *===========================================================================*/

/**
 * @brief  Enable DMA request from timer
 * @param  instance  Timer instance
 * @param  source    DMA request source (Update or CC event)
 *
 * @note   This only enables the DMA request flag in the timer.
 *         Actual DMA channel configuration is done in bsp_dma module.
 *
 * Example (for WS2812):
 *   // Enable DMA request on TIM1_CH1 CC event
 *   BSP_Timer_DMA_Enable(BSP_TIMER_1, BSP_TIMER_DMA_CC1);
 */
BSP_Timer_Status_t BSP_Timer_DMA_Enable(BSP_Timer_Instance_t instance,
                                        BSP_Timer_DMA_Source_t source);

/**
 * @brief  Disable DMA request from timer
 */
BSP_Timer_Status_t BSP_Timer_DMA_Disable(BSP_Timer_Instance_t instance,
                                         BSP_Timer_DMA_Source_t source);

/**
 * @brief  Get CCR register address (needed for DMA destination)
 * @param  instance  Timer instance
 * @param  channel   PWM channel
 * @return Register address as uint32_t, 0 if invalid
 *
 * Example:
 *   uint32_t ccr_addr = BSP_Timer_GetCCR_Address(BSP_TIMER_1, BSP_TIMER_CH1);
 *   // Use this as DMA peripheral address:
 *   // DMA_InitStruct.DMA_PeripheralBaseAddr = ccr_addr;
 */
uint32_t BSP_Timer_GetCCR_Address(BSP_Timer_Instance_t instance,
                                  BSP_Timer_Channel_t channel);

/*============================================================================
 *                      PUBLIC API - DELAY (SysTick)
 *===========================================================================*/

/**
 * @brief  Initialize SysTick for delay functions
 * @note   Call once at startup
 */
void BSP_Delay_Init(void);

/**
 * @brief  Blocking delay in milliseconds
 */
void BSP_Delay_Ms(uint32_t ms);

/**
 * @brief  Blocking delay in microseconds
 * @note   Accuracy depends on system clock. At 72MHz, min resolution ~1µs
 */
void BSP_Delay_Us(uint32_t us);

/**
 * @brief  Get system tick count (ms since startup)
 * @return Milliseconds elapsed
 */
uint32_t BSP_Delay_GetTick(void);

#endif /* BSP_TIMER_H */