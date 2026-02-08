#include "bsp_do.h"
#include "bsp_uart.h"

int main()
{
    setvbuf(stdout, NULL, _IONBF, 0);
    BSP_UART_Config_t cfg = BSP_UART_DEFAULT_CONFIG;
    BSP_UART_Init(BSP_UART1, &cfg);

    BSP_DO_Return_t ret = BSP_DO_Init(DO_CH45);

    if (ret != BSP_DO_OK)
    {
        printf("BSP DO init error: %d\n", ret);
    }

    ret = BSP_DO_Set_State(DO_CH45, BSP_DO_STATE_LOW);
    if (ret != BSP_DO_OK)
    {
        printf("BSP DO set state error: %d\n", ret);
    }

    while (1)
    {
    }
}