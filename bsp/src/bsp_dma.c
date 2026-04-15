#include "bsp_dma.h"
#include <stdio.h>

static const BSP_DMA_Version_t s_version = {1, 0, 0};

/* ================================================================
 * Hardware mapping
 *
 * DMA1 channel registers are at fixed offsets from DMA1 base:
 *   Ch1: 0x4002 0008, Ch2: +20, Ch3: +40, ..., Ch7: +120
 * SPL provides DMA1_Channel1 ... DMA1_Channel7 pointers directly.
 * ================================================================ */
static DMA_Channel_TypeDef *const s_dma_ch[BSP_DMA_CH_MAX] = {
    DMA1_Channel1, /* BSP_DMA_CH1 */
    DMA1_Channel2, /* BSP_DMA_CH2 */
    DMA1_Channel3, /* BSP_DMA_CH3 */
    DMA1_Channel4, /* BSP_DMA_CH4 */
    DMA1_Channel5, /* BSP_DMA_CH5 */
    DMA1_Channel6, /* BSP_DMA_CH6 */
    DMA1_Channel7, /* BSP_DMA_CH7 */
};

/* TC flag per channel in DMA1->ISR */
static const uint32_t s_tc_flag[BSP_DMA_CH_MAX] = {
    DMA1_FLAG_TC1,
    DMA1_FLAG_TC2,
    DMA1_FLAG_TC3,
    DMA1_FLAG_TC4,
    DMA1_FLAG_TC5,
    DMA1_FLAG_TC6,
    DMA1_FLAG_TC7,
};

/* Global TC clear flag */
static const uint32_t s_gl_flag[BSP_DMA_CH_MAX] = {
    DMA1_FLAG_GL1,
    DMA1_FLAG_GL2,
    DMA1_FLAG_GL3,
    DMA1_FLAG_GL4,
    DMA1_FLAG_GL5,
    DMA1_FLAG_GL6,
    DMA1_FLAG_GL7,
};

/* ================================================================
 * Module state
 * ================================================================ */
static BSP_DMA_State_t s_state[BSP_DMA_CH_MAX];
static BSP_DMA_Callback_t s_tc_cb[BSP_DMA_CH_MAX];
static BSP_DMA_Config_t s_cfg[BSP_DMA_CH_MAX]; /* cached for Reload() */

/* ================================================================
 * Internal helpers
 * ================================================================ */
static uint32_t _dir_to_spl(BSP_DMA_Dir_t dir)
{
    switch (dir)
    {
    case BSP_DMA_DIR_M2P:
        return DMA_DIR_PeripheralDST;
    case BSP_DMA_DIR_P2M:
        return DMA_DIR_PeripheralSRC;
    case BSP_DMA_DIR_M2M:
        return DMA_DIR_PeripheralSRC; /* M2M uses mem-to-mem bit separately */
    default:
        return DMA_DIR_PeripheralSRC;
    }
}

static uint32_t _width_to_periph_spl(BSP_DMA_Width_t w)
{
    switch (w)
    {
    case BSP_DMA_WIDTH_8BIT:
        return DMA_PeripheralDataSize_Byte;
    case BSP_DMA_WIDTH_16BIT:
        return DMA_PeripheralDataSize_HalfWord;
    case BSP_DMA_WIDTH_32BIT:
        return DMA_PeripheralDataSize_Word;
    default:
        return DMA_PeripheralDataSize_Byte;
    }
}

static uint32_t _width_to_mem_spl(BSP_DMA_Width_t w)
{
    switch (w)
    {
    case BSP_DMA_WIDTH_8BIT:
        return DMA_MemoryDataSize_Byte;
    case BSP_DMA_WIDTH_16BIT:
        return DMA_MemoryDataSize_HalfWord;
    case BSP_DMA_WIDTH_32BIT:
        return DMA_MemoryDataSize_Word;
    default:
        return DMA_MemoryDataSize_Byte;
    }
}

static uint32_t _prio_to_spl(BSP_DMA_Prio_t p)
{
    switch (p)
    {
    case BSP_DMA_PRIO_LOW:
        return DMA_Priority_Low;
    case BSP_DMA_PRIO_MEDIUM:
        return DMA_Priority_Medium;
    case BSP_DMA_PRIO_HIGH:
        return DMA_Priority_High;
    case BSP_DMA_PRIO_VERY_HIGH:
        return DMA_Priority_VeryHigh;
    default:
        return DMA_Priority_Medium;
    }
}

/* ================================================================
 * BSP_DMA_Init
 * ================================================================ */
BSP_DMA_Return_t BSP_DMA_Init(BSP_DMA_Channel_t channel, const BSP_DMA_Config_t *cfg)
{
    /* One-time module init */
    static uint8_t s_module_init = 0;
    if (s_module_init == 0)
    {
        for (int i = 0; i < BSP_DMA_CH_MAX; i++)
        {
            s_state[i] = BSP_DMA_STATE_UNINIT;
            s_tc_cb[i] = NULL;
        }
        /* Enable DMA1 clock once */
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
        s_module_init = 1;
    }

    /* Param check */
    if (channel >= BSP_DMA_CH_MAX || cfg == NULL)
    {
        return BSP_DMA_ERROR_PARAM;
    }

    if (cfg->buf_size == 0 || cfg->mem_addr == 0 ||
        (cfg->dir != BSP_DMA_DIR_M2M && cfg->periph_addr == 0))
    {
        return BSP_DMA_ERROR_PARAM;
    }

    if (s_state[channel] != BSP_DMA_STATE_UNINIT)
    {
        return BSP_DMA_ERROR_ALREADY_INIT;
    }

    /* Cache config for Reload() */
    s_cfg[channel] = *cfg;

    /* Configure DMA channel via SPL */
    DMA_InitTypeDef dma;
    dma.DMA_PeripheralBaseAddr = cfg->periph_addr;
    dma.DMA_MemoryBaseAddr = cfg->mem_addr;
    dma.DMA_BufferSize = cfg->buf_size;
    dma.DMA_DIR = _dir_to_spl(cfg->dir);
    dma.DMA_PeripheralDataSize = _width_to_periph_spl(cfg->periph_width);
    dma.DMA_MemoryDataSize = _width_to_mem_spl(cfg->mem_width);
    dma.DMA_PeripheralInc = cfg->periph_inc ? DMA_PeripheralInc_Enable
                                            : DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc = cfg->mem_inc ? DMA_MemoryInc_Enable
                                     : DMA_MemoryInc_Disable;
    dma.DMA_Mode = (cfg->mode == BSP_DMA_MODE_CIRCULAR)
                       ? DMA_Mode_Circular
                       : DMA_Mode_Normal;
    dma.DMA_Priority = _prio_to_spl(cfg->priority);
    /* M2M transfers require this bit; for M2P/P2M it must be Disable */
    dma.DMA_M2M = (cfg->dir == BSP_DMA_DIR_M2M)
                      ? DMA_M2M_Enable
                      : DMA_M2M_Disable;

    DMA_Init(s_dma_ch[channel], &dma);

    /* TC interrupt */
    if (cfg->tc_irq_en)
    {
        DMA_ITConfig(s_dma_ch[channel], DMA_IT_TC, ENABLE);
    }

    s_state[channel] = BSP_DMA_STATE_READY;

    return BSP_DMA_OK;
}

/* ================================================================
 * BSP_DMA_DeInit
 * ================================================================ */
BSP_DMA_Return_t BSP_DMA_DeInit(BSP_DMA_Channel_t channel)
{
    if (channel >= BSP_DMA_CH_MAX)
    {
        return BSP_DMA_ERROR_PARAM;
    }

    if (s_state[channel] == BSP_DMA_STATE_UNINIT)
    {
        return BSP_DMA_ERROR_UNINIT;
    }

    /* Stop first if running */
    DMA_Cmd(s_dma_ch[channel], DISABLE);
    DMA_ITConfig(s_dma_ch[channel], DMA_IT_TC, DISABLE);
    DMA_DeInit(s_dma_ch[channel]);

    s_state[channel] = BSP_DMA_STATE_UNINIT;
    s_tc_cb[channel] = NULL;

    return BSP_DMA_OK;
}

/* ================================================================
 * BSP_DMA_Start
 * ================================================================ */
BSP_DMA_Return_t BSP_DMA_Start(BSP_DMA_Channel_t channel)
{
    if (channel >= BSP_DMA_CH_MAX)
    {
        return BSP_DMA_ERROR_PARAM;
    }

    if (s_state[channel] == BSP_DMA_STATE_UNINIT)
    {
        return BSP_DMA_ERROR_UNINIT;
    }

    if (s_state[channel] == BSP_DMA_STATE_BUSY)
    {
        return BSP_DMA_ERROR_BUSY;
    }

    /* Clear previous TC flag before enabling */
    DMA_ClearFlag(s_gl_flag[channel]);

    DMA_Cmd(s_dma_ch[channel], ENABLE);
    s_state[channel] = BSP_DMA_STATE_BUSY;

    return BSP_DMA_OK;
}

/* ================================================================
 * BSP_DMA_Stop
 * ================================================================ */
BSP_DMA_Return_t BSP_DMA_Stop(BSP_DMA_Channel_t channel)
{
    if (channel >= BSP_DMA_CH_MAX)
    {
        return BSP_DMA_ERROR_PARAM;
    }

    if (s_state[channel] == BSP_DMA_STATE_UNINIT)
    {
        return BSP_DMA_ERROR_UNINIT;
    }

    DMA_Cmd(s_dma_ch[channel], DISABLE);

    /* In circular mode the channel is never "complete"; treat Stop as ready */
    s_state[channel] = BSP_DMA_STATE_READY;

    return BSP_DMA_OK;
}

/* ================================================================
 * BSP_DMA_Reload
 *
 * Update source address and/or size without full re-init.
 * Channel MUST be stopped before calling.
 * Useful for double-buffering or chaining different frame buffers.
 * ================================================================ */
BSP_DMA_Return_t BSP_DMA_Reload(BSP_DMA_Channel_t channel,
                                uint32_t mem_addr,
                                uint16_t buf_size)
{
    if (channel >= BSP_DMA_CH_MAX)
    {
        return BSP_DMA_ERROR_PARAM;
    }

    if (s_state[channel] == BSP_DMA_STATE_UNINIT)
    {
        return BSP_DMA_ERROR_UNINIT;
    }

    if (s_state[channel] == BSP_DMA_STATE_BUSY)
    {
        return BSP_DMA_ERROR_BUSY; /* Must stop first */
    }

    if (buf_size == 0)
    {
        return BSP_DMA_ERROR_PARAM;
    }

    /* DMA CMAR/CNDTR can only be written while channel is disabled */
    DMA_Cmd(s_dma_ch[channel], DISABLE);

    s_dma_ch[channel]->CMAR = mem_addr;
    s_dma_ch[channel]->CNDTR = buf_size;

    /* Update cached config */
    s_cfg[channel].mem_addr = mem_addr;
    s_cfg[channel].buf_size = buf_size;

    s_state[channel] = BSP_DMA_STATE_READY;

    return BSP_DMA_OK;
}

/* ================================================================
 * BSP_DMA_GetState
 * ================================================================ */
BSP_DMA_Return_t BSP_DMA_GetState(BSP_DMA_Channel_t channel, BSP_DMA_State_t *state)
{
    if (channel >= BSP_DMA_CH_MAX || state == NULL)
    {
        return BSP_DMA_ERROR_PARAM;
    }

    *state = s_state[channel];
    return BSP_DMA_OK;
}

/* ================================================================
 * BSP_DMA_GetRemaining
 *
 * Returns number of data units NOT yet transferred.
 * Returns 0 if channel is uninit or idle.
 * ================================================================ */
uint16_t BSP_DMA_GetRemaining(BSP_DMA_Channel_t channel)
{
    if (channel >= BSP_DMA_CH_MAX || s_state[channel] == BSP_DMA_STATE_UNINIT)
    {
        return 0;
    }

    return (uint16_t)(s_dma_ch[channel]->CNDTR);
}

/* ================================================================
 * BSP_DMA_SetCallback
 * ================================================================ */
BSP_DMA_Return_t BSP_DMA_SetCallback(BSP_DMA_Channel_t channel, BSP_DMA_Callback_t cb)
{
    if (channel >= BSP_DMA_CH_MAX)
    {
        return BSP_DMA_ERROR_PARAM;
    }

    if (s_state[channel] == BSP_DMA_STATE_UNINIT)
    {
        return BSP_DMA_ERROR_UNINIT;
    }

    s_tc_cb[channel] = cb; /* cb == NULL → disable callback */
    return BSP_DMA_OK;
}

/* ================================================================
 * BSP_DMA_IRQHandler
 *
 * Call this from the NVIC handler:
 *   void DMA1_Channel1_IRQHandler(void) { BSP_DMA_IRQHandler(BSP_DMA_CH1); }
 * ================================================================ */
void BSP_DMA_IRQHandler(BSP_DMA_Channel_t channel)
{
    if (channel >= BSP_DMA_CH_MAX)
    {
        return;
    }

    if (DMA_GetITStatus(s_tc_flag[channel]) == RESET)
    {
        return;
    }

    /* Clear TC flag */
    DMA_ClearITPendingBit(s_tc_flag[channel]);

    /* In normal mode: channel auto-disables, update state */
    if (s_cfg[channel].mode == BSP_DMA_MODE_NORMAL)
    {
        s_state[channel] = BSP_DMA_STATE_COMPLETE;
    }
    /* Circular mode stays BUSY — hardware auto-reloads CNDTR */

    /* Fire user callback */
    if (s_tc_cb[channel] != NULL)
    {
        s_tc_cb[channel](channel);
    }
}

/* ================================================================
 * BSP_DMA_GetVersion
 * ================================================================ */
void BSP_DMA_GetVersion(void)
{
    printf("BSP DMA version: %d.%d.%d\n",
           s_version.major, s_version.minor, s_version.patch);
}