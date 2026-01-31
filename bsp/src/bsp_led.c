#include "bsp_led.h"

static const BSP_LED_Version_t version = {1, 0, 0};

/* ================== HW mapping ================== */
static const BSP_LED_Hw_t led_hw_map[LED_CH_MAX] = {
    /* GPIOA: LED_CH0 -> LED_CH15 */
    [LED_CH0] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_0},
    [LED_CH1] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_1},
    [LED_CH2] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_2},
    [LED_CH3] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_3},
    [LED_CH4] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_4},
    [LED_CH5] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_5},
    [LED_CH6] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_6},
    [LED_CH7] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_7},
    [LED_CH8] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_8},
    [LED_CH9] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_9},
    [LED_CH10] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_10},
    [LED_CH11] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_11},
    [LED_CH12] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_12},
    [LED_CH13] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_13},
    [LED_CH14] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_14},
    [LED_CH15] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_15},

    /* GPIOB: LED_CH16 -> LED_CH31 */
    [LED_CH16] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_0},
    [LED_CH17] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_1},
    [LED_CH18] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_2},
    [LED_CH19] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_3},
    [LED_CH20] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_4},
    [LED_CH21] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_5},
    [LED_CH22] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_6},
    [LED_CH23] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_7},
    [LED_CH24] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_8},
    [LED_CH25] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_9},
    [LED_CH26] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_10},
    [LED_CH27] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_11},
    [LED_CH28] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_12},
    [LED_CH29] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_13},
    [LED_CH30] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_14},
    [LED_CH31] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_15},

    /* GPIOC: LED_CH32 -> LED_CH47 */
    [LED_CH32] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_0},
    [LED_CH33] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_1},
    [LED_CH34] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_2},
    [LED_CH35] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_3},
    [LED_CH36] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_4},
    [LED_CH37] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_5},
    [LED_CH38] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_6},
    [LED_CH39] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_7},
    [LED_CH40] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_8},
    [LED_CH41] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_9},
    [LED_CH42] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_10},
    [LED_CH43] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_11},
    [LED_CH44] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_12},
    [LED_CH45] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_13},
    [LED_CH46] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_14},
    [LED_CH47] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_15},

    /* GPIOD: LED_CH48 -> LED_CH63 */
    [LED_CH48] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_0},
    [LED_CH49] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_1},
    [LED_CH50] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_2},
    [LED_CH51] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_3},
    [LED_CH52] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_4},
    [LED_CH53] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_5},
    [LED_CH54] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_6},
    [LED_CH55] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_7},
    [LED_CH56] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_8},
    [LED_CH57] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_9},
    [LED_CH58] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_10},
    [LED_CH59] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_11},
    [LED_CH60] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_12},
    [LED_CH61] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_13},
    [LED_CH62] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_14},
    [LED_CH63] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_15},
};

/* ================== LED state ================== */
static BSP_LED_State_t led_state[LED_CH_MAX] = {BSP_LED_STATE_UNINIT};

/* ================== Public API ================= */
BSP_LED_Return_t BSP_LED_Init(BSP_LED_Channel_t channel)
{
    /* Parameter check */
    if (channel >= LED_CH_MAX || channel < LED_CH0)
    {
        return BSP_LED_ERROR_PARAM;
    }

    /* State check: LED has not been initialized */
    if (led_state[channel] != BSP_LED_STATE_UNINIT)
    {
        return BSP_LED_ERROR_ALREADY_INIT;
    }

    /* Enable clock */
    RCC_APB2PeriphClockCmd(led_hw_map[channel].rcc_clk, ENABLE);

    /* Initialize GPIO */
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin = led_hw_map[channel].pin;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(led_hw_map[channel].port, &gpio);

    /* Reset GPIO */
    GPIO_ResetBits(led_hw_map[channel].port, led_hw_map[channel].pin);
    /* Update LED state */
    led_state[channel] = BSP_LED_STATE_LOW;

    return BSP_LED_OK;
}

BSP_LED_Return_t BSP_LED_DeInit(BSP_LED_Channel_t channel)
{
    /* Parameter check */
    if (channel >= LED_CH_MAX || channel < LED_CH0)
    {
        return BSP_LED_ERROR_PARAM;
    }

    /* State check: LED has not been initialized */
    if (led_state[channel] == BSP_LED_STATE_UNINIT)
    {
        return BSP_LED_ERROR_UNINIT;
    }

    /* Reset GPIO struct */
    GPIO_InitTypeDef gpio;
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin = led_hw_map[channel].pin;
    GPIO_Init(led_hw_map[channel].port, &gpio);

    /* Update LED state */
    led_state[channel] = BSP_LED_STATE_UNINIT;

    return BSP_LED_OK;
}

BSP_LED_Return_t BSP_LED_Set_State(BSP_LED_Channel_t channel, BSP_LED_State_t state)
{
    /* Parameter check */
    if (channel >= LED_CH_MAX || channel < LED_CH0)
    {
        return BSP_LED_ERROR_PARAM;
    }

    if (state == BSP_LED_STATE_UNINIT)
    {
        return BSP_LED_ERROR_PARAM;
    }

    /* State check: LED has not been initialized */
    if (led_state[channel] == BSP_LED_STATE_UNINIT)
    {
        return BSP_LED_ERROR_UNINIT;
    }

    /* Set LED state */
    if (state == BSP_LED_STATE_HIGH)
    {
        GPIO_SetBits(led_hw_map[channel].port, led_hw_map[channel].pin);
    }
    else
    {
        GPIO_ResetBits(led_hw_map[channel].port, led_hw_map[channel].pin);
    }
    led_state[channel] = state;

    return BSP_LED_OK;
}

BSP_LED_Return_t BSP_LED_Get_State(BSP_LED_Channel_t channel, BSP_LED_State_t *state)
{
    /* Parameter check */
    if (channel >= LED_CH_MAX || channel < LED_CH0)
    {
        return BSP_LED_ERROR_PARAM;
    }

    if (state == NULL)
    {
        return BSP_LED_ERROR_PARAM;
    }

    /* Get LED state */
    *state = led_state[channel];

    return BSP_LED_OK;
}

BSP_LED_Return_t BSP_LED_Toggle(BSP_LED_Channel_t channel)
{
    /* Parameter check */
    if (channel >= LED_CH_MAX || channel < LED_CH0)
    {
        return BSP_LED_ERROR_PARAM;
    }

    /* State check: LED has not been initialized */
    if (led_state[channel] == BSP_LED_STATE_UNINIT)
    {
        return BSP_LED_ERROR_UNINIT;
    }

    /* Toggle LED state */
    if (led_state[channel] == BSP_LED_STATE_HIGH)
    {
        GPIO_ResetBits(led_hw_map[channel].port, led_hw_map[channel].pin);
        led_state[channel] = BSP_LED_STATE_LOW;
    }
    else
    {
        GPIO_SetBits(led_hw_map[channel].port, led_hw_map[channel].pin);
        led_state[channel] = BSP_LED_STATE_HIGH;
    }

    return BSP_LED_OK;
}

BSP_LED_Return_t BSP_LED_SetMask(BSP_LED_Type_t mask, BSP_LED_State_t state)
{
    /* Parameter check */
    if (state == BSP_LED_STATE_UNINIT)
    {
        return BSP_LED_ERROR_PARAM;
    }

    /* State check: LED has not been initialized */
    for (size_t i = 0; i < LED_CH_MAX; i++)
    {
        if (led_state[i] == BSP_LED_STATE_UNINIT && (mask & (1 << i)))
        {
            return BSP_LED_ERROR_UNINIT;
        }
    }

    /* Set LED state */
    for (int i = 0; i < LED_CH_MAX; i++)
    {
        if (mask & (1 << i))
        {
            GPIO_WriteBit(led_hw_map[i].port, led_hw_map[i].pin, state);
            led_state[i] = state;
        }
    }

    return BSP_LED_OK;
}

void BSP_LED_Get_Version(void)
{
    printf("BSP LED version: %d.%d.%d\n", version.major, version.minor, version.patch);
}
