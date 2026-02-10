#include "bsp_uart.h"
#define PRINTF_UART BSP_UART1

int _write(int fd, const void *buf, size_t count)
{
    (void)fd;
    BSP_UART_Write(PRINTF_UART, buf, count);
    return count;
}