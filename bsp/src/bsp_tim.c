/**
 * @file    bsp_timer.c
 * @brief   BSP Timer implementation using SPL
 */

#include "bsp_tim.h"

/*============================================================================
 *                          PRIVATE DEFINITIONS
 *===========================================================================*/

/**
 * @brief Hardware mapping table: Timer instance → TIM_TypeDef*
 */
static TIM_TypeDef *const TIMER_MAP[BSP_TIMER_COUNT] = {
    [BSP_TIMER_1] = TIM1,
    [BSP_TIMER_2] = TIM2,
    [BSP_TIMER_3] = TIM3,
    [BSP_TIMER_4] = TIM4};

/**
 * @brief Hardware mapping table: Timer instance → IRQn
 */
static const IRQn_Type TIMER_IRQ_MAP[BSP_TIMER_COUNT] = {
    [BSP_TIMER_1] = TIM1_UP_IRQn,
    [BSP_TIMER_2] = TIM2_IRQn,
    [BSP_TIMER_3] = TIM3_IRQn,
    [BSP_TIMER_4] = TIM4_IRQn};

/**
 * @brief PWM GPIO mapping: [timer][channel] → {GPIO_Port, GPIO_Pin}
 * @note  Default pin mapping (no remap)
 *
 * TIM1: CH1=PA8,  CH2=PA9,  CH3=PA10, CH4=PA11
 * TIM2: CH1=PA0,  CH2=PA1,  CH3=PA2,  CH4=PA3
 * TIM3: CH1=PA6,  CH2=PA7,  CH3=PB0,  CH4=PB1
 * TIM4: CH1=PB6,  CH2=PB7,  CH3=PB8,  CH4=PB9
 */
typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
} GPIO_PinMap_t;

static const GPIO_PinMap_t PWM_PIN_MAP[BSP_TIMER_COUNT][BSP_TIMER_CH_COUNT] = {
    /* TIM1 */ {{GPIOA, GPIO_Pin_8}, {GPIOA, GPIO_Pin_9}, {GPIOA, GPIO_Pin_10}, {GPIOA, GPIO_Pin_11}},
    /* TIM2 */ {{GPIOA, GPIO_Pin_0}, {GPIOA, GPIO_Pin_1}, {GPIOA, GPIO_Pin_2}, {GPIOA, GPIO_Pin_3}},
    /* TIM3 */ {{GPIOA, GPIO_Pin_6}, {GPIOA, GPIO_Pin_7}, {GPIOB, GPIO_Pin_0}, {GPIOB, GPIO_Pin_1}},
    /* TIM4 */ {{GPIOB, GPIO_Pin_6}, {GPIOB, GPIO_Pin_7}, {GPIOB, GPIO_Pin_8}, {GPIOB, GPIO_Pin_9}},
};

/**
 * @brief SPL DMA source mapping
 */
static const uint16_t DMA_SOURCE_MAP[] = {
    [BSP_TIMER_DMA_UPDATE] = TIM_DMA_Update,
    [BSP_TIMER_DMA_CC1] = TIM_DMA_CC1,
    [BSP_TIMER_DMA_CC2] = TIM_DMA_CC2,
    [BSP_TIMER_DMA_CC3] = TIM_DMA_CC3,
    [BSP_TIMER_DMA_CC4] = TIM_DMA_CC4,
};

/*============================================================================
 *                          PRIVATE VARIABLES
 *===========================================================================*/

/** Callback storage for each timer */
static BSP_Timer_Callback_t s_callbacks[BSP_TIMER_COUNT] = {NULL};

/** Init state tracking */
static bool s_initialized[BSP_TIMER_COUNT] = {false};

/** Store ARR value for duty % calculation */
static uint16_t s_arr_values[BSP_TIMER_COUNT] = {0};

/** SysTick millisecond counter */
static volatile uint32_t s_systick_ms = 0;

/*============================================================================
 *                          PRIVATE FUNCTIONS
 *===========================================================================*/

/**
 * @brief  Enable RCC clock for specified timer
 */
static void Timer_EnableClock(BSP_Timer_Instance_t instance)
{
    switch (instance)
    {
    case BSP_TIMER_1:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
        break;
    case BSP_TIMER_2:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
        break;
    case BSP_TIMER_3:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
        break;
    case BSP_TIMER_4:
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
        break;
    default:
        break;
    }
}

/**
 * @brief  Enable RCC clock for GPIO port used by PWM pin
 */
static void Timer_EnableGPIOClock(GPIO_TypeDef *port)
{
    if (port == GPIOA)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    else if (port == GPIOB)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    else if (port == GPIOC)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}

/**
 * @brief  Configure GPIO pin for PWM output (AF Push-Pull)
 */
static void Timer_ConfigPWM_GPIO(BSP_Timer_Instance_t instance,
                                 BSP_Timer_Channel_t channel)
{
    const GPIO_PinMap_t *pin = &PWM_PIN_MAP[instance][channel];

    Timer_EnableGPIOClock(pin->port);

    /* AFIO clock needed for alternate functions */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef gpio_cfg;
    gpio_cfg.GPIO_Pin = pin->pin;
    gpio_cfg.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_cfg.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(pin->port, &gpio_cfg);
}

/**
 * @brief  Validate timer instance
 */
static bool Timer_IsValidInstance(BSP_Timer_Instance_t instance)
{
    return (instance < BSP_TIMER_COUNT);
}

/**
 * @brief  Validate channel
 */
static bool Timer_IsValidChannel(BSP_Timer_Channel_t channel)
{
    return (channel < BSP_TIMER_CH_COUNT);
}

/*============================================================================
 *                     PUBLIC API - BASE TIMER
 *===========================================================================*/

BSP_Timer_Status_t BSP_Timer_Base_Init(BSP_Timer_Instance_t instance,
                                       const BSP_Timer_Base_Config_t *config)
{
    if (!Timer_IsValidInstance(instance))
        return BSP_TIMER_ERROR_INVALID_INSTANCE;

    if (config == NULL)
        return BSP_TIMER_ERROR_INVALID_INSTANCE;

    /* Enable timer clock */
    Timer_EnableClock(instance);

    TIM_TypeDef *tim = TIMER_MAP[instance];

    /* Configure time base */
    TIM_TimeBaseInitTypeDef tb_cfg;
    TIM_TimeBaseStructInit(&tb_cfg); /* Fill defaults first */

    tb_cfg.TIM_Prescaler = config->prescaler;
    tb_cfg.TIM_Period = config->period;
    tb_cfg.TIM_CounterMode = TIM_CounterMode_Up;
    tb_cfg.TIM_ClockDivision = TIM_CKD_DIV1;
    tb_cfg.TIM_RepetitionCounter = 0; /* Only used by TIM1 */

    TIM_TimeBaseInit(tim, &tb_cfg);

    /* Enable/disable auto-reload preload */
    if (config->auto_reload)
        TIM_ARRPreloadConfig(tim, ENABLE);
    else
        TIM_ARRPreloadConfig(tim, DISABLE);

    /* Store ARR for duty calculation */
    s_arr_values[instance] = config->period;
    s_initialized[instance] = true;

    return BSP_TIMER_OK;
}

BSP_Timer_Status_t BSP_Timer_Start(BSP_Timer_Instance_t instance)
{
    if (!Timer_IsValidInstance(instance))
        return BSP_TIMER_ERROR_INVALID_INSTANCE;
    if (!s_initialized[instance])
        return BSP_TIMER_ERROR_NOT_INITIALIZED;

    TIM_Cmd(TIMER_MAP[instance], ENABLE);
    return BSP_TIMER_OK;
}

BSP_Timer_Status_t BSP_Timer_Stop(BSP_Timer_Instance_t instance)
{
    if (!Timer_IsValidInstance(instance))
        return BSP_TIMER_ERROR_INVALID_INSTANCE;

    TIM_Cmd(TIMER_MAP[instance], DISABLE);
    return BSP_TIMER_OK;
}

/*============================================================================
 *                     PUBLIC API - INTERRUPT
 *===========================================================================*/

BSP_Timer_Status_t BSP_Timer_EnableInterrupt(BSP_Timer_Instance_t instance,
                                             BSP_Timer_Callback_t callback,
                                             uint8_t priority)
{
    if (!Timer_IsValidInstance(instance))
        return BSP_TIMER_ERROR_INVALID_INSTANCE;
    if (!s_initialized[instance])
        return BSP_TIMER_ERROR_NOT_INITIALIZED;

    /* Store callback */
    s_callbacks[instance] = callback;

    /* Configure NVIC */
    NVIC_InitTypeDef nvic_cfg;
    nvic_cfg.NVIC_IRQChannel = TIMER_IRQ_MAP[instance];
    nvic_cfg.NVIC_IRQChannelPreemptionPriority = priority;
    nvic_cfg.NVIC_IRQChannelSubPriority = 0;
    nvic_cfg.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_cfg);

    /* Enable Update interrupt */
    TIM_ITConfig(TIMER_MAP[instance], TIM_IT_Update, ENABLE);

    return BSP_TIMER_OK;
}

BSP_Timer_Status_t BSP_Timer_DisableInterrupt(BSP_Timer_Instance_t instance)
{
    if (!Timer_IsValidInstance(instance))
        return BSP_TIMER_ERROR_INVALID_INSTANCE;

    TIM_ITConfig(TIMER_MAP[instance], TIM_IT_Update, DISABLE);
    s_callbacks[instance] = NULL;

    return BSP_TIMER_OK;
}

/*============================================================================
 *                     PUBLIC API - PWM
 *===========================================================================*/

BSP_Timer_Status_t BSP_Timer_PWM_Init(BSP_Timer_Instance_t instance,
                                      const BSP_Timer_PWM_Config_t *pwm_config)
{
    if (!Timer_IsValidInstance(instance))
        return BSP_TIMER_ERROR_INVALID_INSTANCE;
    if (!s_initialized[instance])
        return BSP_TIMER_ERROR_NOT_INITIALIZED;
    if (pwm_config == NULL || !Timer_IsValidChannel(pwm_config->channel))
        return BSP_TIMER_ERROR_INVALID_CHANNEL;

    TIM_TypeDef *tim = TIMER_MAP[instance];

    /* Configure GPIO for PWM output */
    Timer_ConfigPWM_GPIO(instance, pwm_config->channel);

    /* Configure Output Compare for PWM Mode 1 */
    TIM_OCInitTypeDef oc_cfg;
    TIM_OCStructInit(&oc_cfg);

    oc_cfg.TIM_OCMode = TIM_OCMode_PWM1;
    oc_cfg.TIM_OutputState = TIM_OutputState_Enable;
    oc_cfg.TIM_Pulse = pwm_config->pulse;
    oc_cfg.TIM_OCPolarity = TIM_OCPolarity_High;

    /* Initialize the correct channel */
    switch (pwm_config->channel)
    {
    case BSP_TIMER_CH1:
        TIM_OC1Init(tim, &oc_cfg);
        if (pwm_config->preload)
            TIM_OC1PreloadConfig(tim, TIM_OCPreload_Enable);
        break;
    case BSP_TIMER_CH2:
        TIM_OC2Init(tim, &oc_cfg);
        if (pwm_config->preload)
            TIM_OC2PreloadConfig(tim, TIM_OCPreload_Enable);
        break;
    case BSP_TIMER_CH3:
        TIM_OC3Init(tim, &oc_cfg);
        if (pwm_config->preload)
            TIM_OC3PreloadConfig(tim, TIM_OCPreload_Enable);
        break;
    case BSP_TIMER_CH4:
        TIM_OC4Init(tim, &oc_cfg);
        if (pwm_config->preload)
            TIM_OC4PreloadConfig(tim, TIM_OCPreload_Enable);
        break;
    default:
        return BSP_TIMER_ERROR_INVALID_CHANNEL;
    }

    /**
     * CRITICAL: TIM1 is Advanced Timer
     * Must enable Main Output Enable (MOE) bit
     * Without this, PWM output will NOT appear on pin!
     */
    if (instance == BSP_TIMER_1)
    {
        TIM_CtrlPWMOutputs(TIM1, ENABLE);
    }

    return BSP_TIMER_OK;
}

BSP_Timer_Status_t BSP_Timer_PWM_SetPulse(BSP_Timer_Instance_t instance,
                                          BSP_Timer_Channel_t channel,
                                          uint16_t pulse)
{
    if (!Timer_IsValidInstance(instance))
        return BSP_TIMER_ERROR_INVALID_INSTANCE;
    if (!Timer_IsValidChannel(channel))
        return BSP_TIMER_ERROR_INVALID_CHANNEL;

    TIM_TypeDef *tim = TIMER_MAP[instance];

    switch (channel)
    {
    case BSP_TIMER_CH1:
        TIM_SetCompare1(tim, pulse);
        break;
    case BSP_TIMER_CH2:
        TIM_SetCompare2(tim, pulse);
        break;
    case BSP_TIMER_CH3:
        TIM_SetCompare3(tim, pulse);
        break;
    case BSP_TIMER_CH4:
        TIM_SetCompare4(tim, pulse);
        break;
    default:
        return BSP_TIMER_ERROR_INVALID_CHANNEL;
    }

    return BSP_TIMER_OK;
}

BSP_Timer_Status_t BSP_Timer_PWM_SetDutyPercent(BSP_Timer_Instance_t instance,
                                                BSP_Timer_Channel_t channel,
                                                uint8_t percent)
{
    if (percent > 100)
        percent = 100;

    uint16_t pulse = BSP_TIMER_CALC_CCR(s_arr_values[instance], percent);
    return BSP_Timer_PWM_SetPulse(instance, channel, pulse);
}

/*============================================================================
 *                     PUBLIC API - DMA SUPPORT
 *===========================================================================*/

BSP_Timer_Status_t BSP_Timer_DMA_Enable(BSP_Timer_Instance_t instance,
                                        BSP_Timer_DMA_Source_t source)
{
    if (!Timer_IsValidInstance(instance))
        return BSP_TIMER_ERROR_INVALID_INSTANCE;

    TIM_DMACmd(TIMER_MAP[instance], DMA_SOURCE_MAP[source], ENABLE);
    return BSP_TIMER_OK;
}

BSP_Timer_Status_t BSP_Timer_DMA_Disable(BSP_Timer_Instance_t instance,
                                         BSP_Timer_DMA_Source_t source)
{
    if (!Timer_IsValidInstance(instance))
        return BSP_TIMER_ERROR_INVALID_INSTANCE;

    TIM_DMACmd(TIMER_MAP[instance], DMA_SOURCE_MAP[source], DISABLE);
    return BSP_TIMER_OK;
}

uint32_t BSP_Timer_GetCCR_Address(BSP_Timer_Instance_t instance,
                                  BSP_Timer_Channel_t channel)
{
    if (!Timer_IsValidInstance(instance) || !Timer_IsValidChannel(channel))
        return 0;

    TIM_TypeDef *tim = TIMER_MAP[instance];

    /**
     * CCR register addresses:
     * CCR1 = TIM base + 0x34
     * CCR2 = TIM base + 0x38
     * CCR3 = TIM base + 0x3C
     * CCR4 = TIM base + 0x40
     */
    switch (channel)
    {
    case BSP_TIMER_CH1:
        return (uint32_t)&tim->CCR1;
    case BSP_TIMER_CH2:
        return (uint32_t)&tim->CCR2;
    case BSP_TIMER_CH3:
        return (uint32_t)&tim->CCR3;
    case BSP_TIMER_CH4:
        return (uint32_t)&tim->CCR4;
    default:
        return 0;
    }
}

/*============================================================================
 *                     PUBLIC API - DELAY (SysTick)
 *===========================================================================*/

void BSP_Delay_Init(void)
{
    /* Configure SysTick for 1ms interrupt */
    SysTick_Config(BSP_TIMER_SYSTEM_CLOCK_HZ / 1000U);
}

void BSP_Delay_Ms(uint32_t ms)
{
    uint32_t start = s_systick_ms;
    while ((s_systick_ms - start) < ms)
    {
        /* Wait — can add __WFI() for power saving */
    }
}

void BSP_Delay_Us(uint32_t us)
{
    /**
     * Simple busy-wait using SysTick current value
     * At 72MHz: 1µs = 72 cycles
     * Max delay with this method: ~233ms (24-bit counter)
     */
    uint32_t ticks = us * (BSP_TIMER_SYSTEM_CLOCK_HZ / 1000000U);

    uint32_t start = SysTick->VAL;
    uint32_t reload = SysTick->LOAD + 1;

    while (1)
    {
        uint32_t now = SysTick->VAL;
        uint32_t elapsed;

        /* SysTick counts DOWN */
        if (now <= start)
            elapsed = start - now;
        else
            elapsed = start + (reload - now);

        if (elapsed >= ticks)
            break;
    }
}

uint32_t BSP_Delay_GetTick(void)
{
    return s_systick_ms;
}

/*============================================================================
 *                     INTERRUPT HANDLERS
 *===========================================================================*/

/**
 * @brief SysTick interrupt handler — called every 1ms
 */
void SysTick_Handler(void)
{
    s_systick_ms++;
}

/**
 * @brief Common timer ISR logic
 */
static inline void Timer_IRQ_Common(BSP_Timer_Instance_t instance)
{
    TIM_TypeDef *tim = TIMER_MAP[instance];

    if (TIM_GetITStatus(tim, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(tim, TIM_IT_Update);

        if (s_callbacks[instance] != NULL)
        {
            s_callbacks[instance]();
        }
    }
}

/* Individual IRQ handlers — these are called by hardware */
void TIM1_UP_IRQHandler(void)
{
    Timer_IRQ_Common(BSP_TIMER_1);
}

void TIM2_IRQHandler(void)
{
    Timer_IRQ_Common(BSP_TIMER_2);
}

void TIM3_IRQHandler(void)
{
    Timer_IRQ_Common(BSP_TIMER_3);
}

void TIM4_IRQHandler(void)
{
    Timer_IRQ_Common(BSP_TIMER_4);
}