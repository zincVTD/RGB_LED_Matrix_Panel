#include "bsp_uart.h"

/* ================== HW mapping ================== */
static const BSP_UART_Hw_t uart_hw_map[BSP_UART_MAX] = {
    {USART1, GPIOA, RCC_APB2Periph_GPIOA, RCC_APB2Periph_USART1, GPIO_Pin_9, GPIO_Pin_10},
    {USART2, GPIOA, RCC_APB2Periph_GPIOA, RCC_APB1Periph_USART2, GPIO_Pin_2, GPIO_Pin_3},
    {USART3, GPIOB, RCC_APB2Periph_GPIOB, RCC_APB1Periph_USART3, GPIO_Pin_10, GPIO_Pin_11},
};

static BSP_UART_State_t uart_state[BSP_UART_MAX];

/* ================== Public API ================= */
BSP_UART_Return_t BSP_UART_Init(BSP_UART_Id_t uart_id, BSP_UART_Config_t *cfg)
{
    /* Initialize UART state */
    static uint8_t uart_init = 0;
    if (uart_init == 0)
    {
        for (int i = 0; i < BSP_UART_MAX; i++)
        {
            uart_state[i] = BSP_UART_STATE_UNINIT;
        }
        uart_init = 1;
    }

    /* parameter check */
    if (uart_id >= BSP_UART_MAX || uart_id < BSP_UART1)
    {
        return BSP_UART_ERROR_PARAM;
    }

    if (cfg == NULL)
    {
        return BSP_UART_ERROR_PARAM;
    }

    RCC_APB2PeriphClockCmd(uart_hw_map[uart_id].gpio_rcc_clk, ENABLE);
    if (uart_id == BSP_UART1)
    {
        RCC_APB2PeriphClockCmd(uart_hw_map[uart_id].uart_rcc_clk, ENABLE);
    }
    else
    {
        RCC_APB1PeriphClockCmd(uart_hw_map[uart_id].uart_rcc_clk, ENABLE);
    }

    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin = uart_hw_map[uart_id].pin_tx;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(uart_hw_map[uart_id].port, &gpio);

    gpio.GPIO_Pin = uart_hw_map[uart_id].pin_rx;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(uart_hw_map[uart_id].port, &gpio);

    USART_InitTypeDef usart;
    usart.USART_BaudRate = cfg->baudrate;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart.USART_Parity = (cfg->parity == BSP_UART_PARITY_EVEN)  ? USART_Parity_Even
                         : (cfg->parity == BSP_UART_PARITY_ODD) ? USART_Parity_Odd
                                                                : USART_Parity_No;
    usart.USART_StopBits = (cfg->stop_bits == BSP_UART_STOPBITS_2) ? USART_StopBits_2 : USART_StopBits_1;
    usart.USART_WordLength = (cfg->word_len == BSP_UART_WORDLEN_9B) ? USART_WordLength_9b : USART_WordLength_8b;
    USART_Init(uart_hw_map[uart_id].usart, &usart);
    USART_Cmd(uart_hw_map[uart_id].usart, ENABLE);

    uart_state[uart_id] = BSP_UART_STATE_READY;
    return BSP_UART_OK;
}

BSP_UART_Return_t BSP_UART_Write(BSP_UART_Id_t uart_id, const char *data, uint16_t len)
{
    /* parameter check */
    if (uart_id >= BSP_UART_MAX || uart_id < BSP_UART1)
    {
        return BSP_UART_ERROR_PARAM;
    }

    if (data == NULL || len == 0)
    {
        return BSP_UART_ERROR_PARAM;
    }

    if (uart_state[uart_id] != BSP_UART_STATE_READY)
    {
        return BSP_UART_ERROR_STATE;
    }

    uart_state[uart_id] = BSP_UART_STATE_BUSY;

    for (int i = 0; i < len; i++)
    {
        while (USART_GetFlagStatus(uart_hw_map[uart_id].usart, USART_FLAG_TXE) == RESET)
            ;
        USART_SendData(uart_hw_map[uart_id].usart, data[i]);
        while (USART_GetFlagStatus(uart_hw_map[uart_id].usart, USART_FLAG_TC) == RESET)
            ;
    }

    uart_state[uart_id] = BSP_UART_STATE_READY;
    return BSP_UART_OK;
}
