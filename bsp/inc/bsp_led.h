#ifndef BSP_LED_H
#define BSP_LED_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

typedef enum
{
    LED_CH0 = 0, /* LED channel 0 */
    LED_CH1,     /* LED channel 1 */
    LED_CH2,     /* LED channel 2 */
    LED_CH3,     /* LED channel 3 */
    LED_CH4,     /* LED channel 4 */
    LED_CH5,     /* LED channel 5 */
    LED_CH6,     /* LED channel 6 */
    LED_CH7,     /* LED channel 7 */
    LED_CH8,     /* LED channel 8 */
    LED_CH9,     /* LED channel 9 */
    LED_CH10,    /* LED channel 10 */
    LED_CH11,    /* LED channel 11 */
    LED_CH12,    /* LED channel 12 */
    LED_CH13,    /* LED channel 13 */
    LED_CH14,    /* LED channel 14 */
    LED_CH15,    /* LED channel 15 */
    LED_CH16,    /* LED channel 16 */
    LED_CH17,    /* LED channel 17 */
    LED_CH18,    /* LED channel 18 */
    LED_CH19,    /* LED channel 19 */
    LED_CH20,    /* LED channel 20 */
    LED_CH21,    /* LED channel 21 */
    LED_CH22,    /* LED channel 22 */
    LED_CH23,    /* LED channel 23 */
    LED_CH24,    /* LED channel 24 */
    LED_CH25,    /* LED channel 25 */
    LED_CH26,    /* LED channel 26 */
    LED_CH27,    /* LED channel 27 */
    LED_CH28,    /* LED channel 28 */
    LED_CH29,    /* LED channel 29 */
    LED_CH30,    /* LED channel 30 */
    LED_CH31,    /* LED channel 31 */
    LED_CH32,    /* LED channel 32 */
    LED_CH33,    /* LED channel 33 */
    LED_CH34,    /* LED channel 34 */
    LED_CH35,    /* LED channel 35 */
    LED_CH36,    /* LED channel 36 */
    LED_CH37,    /* LED channel 37 */
    LED_CH38,    /* LED channel 38 */
    LED_CH39,    /* LED channel 39 */
    LED_CH40,    /* LED channel 40 */
    LED_CH41,    /* LED channel 41 */
    LED_CH42,    /* LED channel 42 */
    LED_CH43,    /* LED channel 43 */
    LED_CH44,    /* LED channel 44 */
    LED_CH45,    /* LED channel 45 */
    LED_CH46,    /* LED channel 46 */
    LED_CH47,    /* LED channel 47 */
    LED_CH48,    /* LED channel 48 */
    LED_CH49,    /* LED channel 49 */
    LED_CH50,    /* LED channel 50 */
    LED_CH51,    /* LED channel 51 */
    LED_CH52,    /* LED channel 52 */
    LED_CH53,    /* LED channel 53 */
    LED_CH54,    /* LED channel 54 */
    LED_CH55,    /* LED channel 55 */
    LED_CH56,    /* LED channel 56 */
    LED_CH57,    /* LED channel 57 */
    LED_CH58,    /* LED channel 58 */
    LED_CH59,    /* LED channel 59 */
    LED_CH60,    /* LED channel 60 */
    LED_CH61,    /* LED channel 61 */
    LED_CH62,    /* LED channel 62 */
    LED_CH63,    /* LED channel 63 */
    LED_CH_MAX   /* Total number of LED channels */
} BSP_LED_Channel_t;

typedef enum
{
    BSP_LED_STATE_UNINIT = -1,
    BSP_LED_STATE_LOW = 0,
    BSP_LED_STATE_HIGH = 1,
} BSP_LED_State_t;

typedef enum
{
    BSP_LED_OK = 0,

    BSP_LED_ERROR_PARAM = -1,

    BSP_LED_ERROR_HW,
    BSP_LED_ERROR_UNINIT = -5,
    BSP_LED_ERROR_ALREADY_INIT = -6,
    BSP_LED_ERROR_STATE,

} BSP_LED_Return_t;

typedef struct
{
    GPIO_TypeDef *port;
    uint32_t rcc_clk;
    uint16_t pin;
} BSP_LED_Hw_t;

typedef uint64_t BSP_LED_Type_t;

typedef struct
{
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
} BSP_LED_Version_t;

/* ===== Lifecycle ===== */
BSP_LED_Return_t BSP_LED_Init(BSP_LED_Channel_t channel);
BSP_LED_Return_t BSP_LED_DeInit(BSP_LED_Channel_t channel);

/* ===== Single channel control ===== */
BSP_LED_Return_t BSP_LED_Set_State(BSP_LED_Channel_t channel, BSP_LED_State_t state);
BSP_LED_Return_t BSP_LED_Get_State(BSP_LED_Channel_t channel, BSP_LED_State_t *state);
BSP_LED_Return_t BSP_LED_Toggle(BSP_LED_Channel_t channel);

/* ===== Multi-channel control ===== */
BSP_LED_Return_t BSP_LED_SetMask(BSP_LED_Type_t mask, BSP_LED_State_t state);

/* ===== Version ===== */
void BSP_LED_Get_Version(void);

#endif /* BSP_LED_H */