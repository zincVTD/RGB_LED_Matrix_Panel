#ifndef BSP_UART_H
#define BSP_UART_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"

#define BSP_UART_DEFAULT_CONFIG           \
    {                                     \
        .baudrate = 115200,               \
        .word_len = BSP_UART_WORDLEN_8B,  \
        .parity = BSP_UART_PARITY_NONE,   \
        .stop_bits = BSP_UART_STOPBITS_1, \
    }

typedef enum
{
    BSP_UART_OK = 0,

    BSP_UART_ERROR_PARAM = -1,

    BSP_UART_ERROR_HW,
    BSP_UART_ERROR_UNINIT = -5,
    BSP_UART_ERROR_ALREADY_INIT = -6,
    BSP_UART_ERROR_STATE,

} BSP_UART_Return_t;

typedef enum
{
    BSP_UART1 = 0,
    BSP_UART2,
    BSP_UART3,
    BSP_UART_MAX
} BSP_UART_Id_t;

typedef struct
{
    USART_TypeDef *usart;
    GPIO_TypeDef *port;
    uint32_t gpio_rcc_clk;
    uint32_t uart_rcc_clk;
    uint16_t pin_tx;
    uint16_t pin_rx;
} BSP_UART_Hw_t;

typedef enum
{
    BSP_UART_STATE_UNINIT = -1,
    BSP_UART_STATE_READY = 0,
    BSP_UART_STATE_BUSY = -2,
} BSP_UART_State_t;

typedef enum
{
    BSP_UART_BAUDRATE_600,
    BSP_UART_BAUDRATE_1200,
    BSP_UART_BAUDRATE_2400,
    BSP_UART_BAUDRATE_4800,
    BSP_UART_BAUDRATE_9600,
    BSP_UART_BAUDRATE_19200,
    BSP_UART_BAUDRATE_38400,
    BSP_UART_BAUDRATE_56000,
    BSP_UART_BAUDRATE_57600,
    BSP_UART_BAUDRATE_115200,
} BSP_UART_Baudrate_t;

typedef enum
{
    BSP_UART_PARITY_NONE,
    BSP_UART_PARITY_EVEN,
    BSP_UART_PARITY_ODD,
} BSP_UART_Parity_t;

typedef enum
{
    BSP_UART_STOPBITS_1,
    BSP_UART_STOPBITS_2,
} BSP_UART_StopBits_t;

typedef enum
{
    BSP_UART_WORDLEN_8B,
    BSP_UART_WORDLEN_9B,
} BSP_UART_WordLen_t;

typedef struct
{
    uint32_t baudrate;
    BSP_UART_WordLen_t word_len;
    BSP_UART_Parity_t parity;
    BSP_UART_StopBits_t stop_bits;
} BSP_UART_Config_t;

BSP_UART_Return_t BSP_UART_Init(BSP_UART_Id_t uart, BSP_UART_Config_t *cfg);
BSP_UART_Return_t BSP_UART_Write(BSP_UART_Id_t uart, const char *data, uint16_t len);

#endif /* BSP_UART_H */
