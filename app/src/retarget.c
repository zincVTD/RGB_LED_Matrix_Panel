#include "bsp_uart.h"

int _write(int fd, const void *buf, size_t count)
{
    (void)fd;
    BSP_UART_Write(BSP_UART1, buf, count);
    return count;
}