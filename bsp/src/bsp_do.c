#include "bsp_do.h"

static const BSP_DO_Version_t version = {1, 0, 1};

/* ================== HW mapping ================== */
static const BSP_DO_Hw_t do_hw_map[DO_CH_MAX] = {
    /* GPIOA: DO_CH0 -> DO_CH15 */
    [DO_CH0] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_0},
    [DO_CH1] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_1},
    [DO_CH2] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_2},
    [DO_CH3] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_3},
    [DO_CH4] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_4},
    [DO_CH5] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_5},
    [DO_CH6] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_6},
    [DO_CH7] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_7},
    [DO_CH8] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_8},
    [DO_CH9] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_9},
    [DO_CH10] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_10},
    [DO_CH11] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_11},
    [DO_CH12] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_12},
    [DO_CH13] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_13},
    [DO_CH14] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_14},
    [DO_CH15] = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_15},

    /* GPIOB: DO_CH16 -> DO_CH31 */
    [DO_CH16] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_0},
    [DO_CH17] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_1},
    [DO_CH18] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_2},
    [DO_CH19] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_3},
    [DO_CH20] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_4},
    [DO_CH21] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_5},
    [DO_CH22] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_6},
    [DO_CH23] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_7},
    [DO_CH24] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_8},
    [DO_CH25] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_9},
    [DO_CH26] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_10},
    [DO_CH27] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_11},
    [DO_CH28] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_12},
    [DO_CH29] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_13},
    [DO_CH30] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_14},
    [DO_CH31] = {GPIOB, RCC_APB2Periph_GPIOB, GPIO_Pin_15},

    /* GPIOC: DO_CH32 -> DO_CH47 */
    [DO_CH32] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_0},
    [DO_CH33] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_1},
    [DO_CH34] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_2},
    [DO_CH35] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_3},
    [DO_CH36] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_4},
    [DO_CH37] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_5},
    [DO_CH38] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_6},
    [DO_CH39] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_7},
    [DO_CH40] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_8},
    [DO_CH41] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_9},
    [DO_CH42] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_10},
    [DO_CH43] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_11},
    [DO_CH44] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_12},
    [DO_CH45] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_13},
    [DO_CH46] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_14},
    [DO_CH47] = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_15},

    /* GPIOD: DO_CH48 -> DO_CH63 */
    [DO_CH48] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_0},
    [DO_CH49] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_1},
    [DO_CH50] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_2},
    [DO_CH51] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_3},
    [DO_CH52] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_4},
    [DO_CH53] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_5},
    [DO_CH54] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_6},
    [DO_CH55] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_7},
    [DO_CH56] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_8},
    [DO_CH57] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_9},
    [DO_CH58] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_10},
    [DO_CH59] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_11},
    [DO_CH60] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_12},
    [DO_CH61] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_13},
    [DO_CH62] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_14},
    [DO_CH63] = {GPIOD, RCC_APB2Periph_GPIOD, GPIO_Pin_15},
};

/* ================== DO state ================== */
static BSP_DO_State_t do_state[DO_CH_MAX];

/* ================== Public API ================= */
BSP_DO_Return_t BSP_DO_Init(BSP_DO_Channel_t channel)
{
    /* DO has been initialized */
    static uint8_t do_init = 0;
    if (do_init == 0)
    {
        for (int i = 0; i < DO_CH_MAX; i++)
        {
            do_state[i] = BSP_DO_STATE_UNINIT;
        }
        do_init = 1;
    }

    /* Parameter check */
    if (channel >= DO_CH_MAX || channel < DO_CH0)
    {
        return BSP_DO_ERROR_PARAM;
    }

    /* State check: DO has not been initialized */
    if (do_state[channel] != BSP_DO_STATE_UNINIT)
    {
        return BSP_DO_ERROR_ALREADY_INIT;
    }

    /* Enable clock */
    RCC_APB2PeriphClockCmd(do_hw_map[channel].rcc_clk, ENABLE);

    /* Initialize GPIO */
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin = do_hw_map[channel].pin;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(do_hw_map[channel].port, &gpio);

    /* Reset GPIO */
    GPIO_ResetBits(do_hw_map[channel].port, do_hw_map[channel].pin);
    /* Update DO state */
    do_state[channel] = BSP_DO_STATE_LOW;

    return BSP_DO_OK;
}

BSP_DO_Return_t BSP_DO_DeInit(BSP_DO_Channel_t channel)
{
    /* Parameter check */
    if (channel >= DO_CH_MAX || channel < DO_CH0)
    {
        return BSP_DO_ERROR_PARAM;
    }

    /* State check: DO has not been initialized */
    if (do_state[channel] == BSP_DO_STATE_UNINIT)
    {
        return BSP_DO_ERROR_UNINIT;
    }

    /* Reset GPIO struct */
    GPIO_InitTypeDef gpio;
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin = do_hw_map[channel].pin;
    GPIO_Init(do_hw_map[channel].port, &gpio);

    /* Update DO state */
    do_state[channel] = BSP_DO_STATE_UNINIT;

    return BSP_DO_OK;
}

BSP_DO_Return_t BSP_DO_Set_State(BSP_DO_Channel_t channel, BSP_DO_State_t state)
{
    /* Parameter check */
    if (channel >= DO_CH_MAX || channel < DO_CH0)
    {
        return BSP_DO_ERROR_PARAM;
    }

    if (state == BSP_DO_STATE_UNINIT)
    {
        return BSP_DO_ERROR_PARAM;
    }

    /* State check: DO has not been initialized */
    if (do_state[channel] == BSP_DO_STATE_UNINIT)
    {
        return BSP_DO_ERROR_UNINIT;
    }

    /* Set DO state */
    if (state == BSP_DO_STATE_HIGH)
    {
        GPIO_SetBits(do_hw_map[channel].port, do_hw_map[channel].pin);
    }
    else
    {
        GPIO_ResetBits(do_hw_map[channel].port, do_hw_map[channel].pin);
    }
    do_state[channel] = state;

    return BSP_DO_OK;
}

BSP_DO_Return_t BSP_DO_Get_State(BSP_DO_Channel_t channel, BSP_DO_State_t *state)
{
    /* Parameter check */
    if (channel >= DO_CH_MAX || channel < DO_CH0)
    {
        return BSP_DO_ERROR_PARAM;
    }

    if (state == NULL)
    {
        return BSP_DO_ERROR_PARAM;
    }

    /* Get DO state */
    *state = do_state[channel];

    return BSP_DO_OK;
}

BSP_DO_Return_t BSP_DO_Toggle(BSP_DO_Channel_t channel)
{
    /* Parameter check */
    if (channel >= DO_CH_MAX || channel < DO_CH0)
    {
        return BSP_DO_ERROR_PARAM;
    }

    /* State check: DO has not been initialized */
    if (do_state[channel] == BSP_DO_STATE_UNINIT)
    {
        return BSP_DO_ERROR_UNINIT;
    }

    /* Toggle DO state */
    if (do_state[channel] == BSP_DO_STATE_HIGH)
    {
        GPIO_ResetBits(do_hw_map[channel].port, do_hw_map[channel].pin);
        do_state[channel] = BSP_DO_STATE_LOW;
    }
    else
    {
        GPIO_SetBits(do_hw_map[channel].port, do_hw_map[channel].pin);
        do_state[channel] = BSP_DO_STATE_HIGH;
    }

    return BSP_DO_OK;
}

BSP_DO_Return_t BSP_DO_SetMask(BSP_DO_Type_t mask, BSP_DO_State_t state)
{
    /* Parameter check */
    if (state == BSP_DO_STATE_UNINIT)
    {
        return BSP_DO_ERROR_PARAM;
    }

    /* State check: DO has not been initialized */
    for (size_t i = 0; i < DO_CH_MAX; i++)
    {
        if (do_state[i] == BSP_DO_STATE_UNINIT && (mask & (1 << i)))
        {
            return BSP_DO_ERROR_UNINIT;
        }
    }

    /* Set DO state */
    for (int i = 0; i < DO_CH_MAX; i++)
    {
        if (mask & (1 << i))
        {
            GPIO_WriteBit(do_hw_map[i].port, do_hw_map[i].pin, state);
            do_state[i] = state;
        }
    }

    return BSP_DO_OK;
}

void BSP_DO_Get_Version(void)
{
    printf("BSP DO version: %d.%d.%d\n", version.major, version.minor, version.patch);
}
