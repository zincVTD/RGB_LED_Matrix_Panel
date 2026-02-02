#ifndef BSP_DO_H
#define BSP_DO_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

typedef enum
{
    DO_CH0 = 0, /* DO channel 0 */
    DO_CH1,     /* DO channel 1 */
    DO_CH2,     /* DO channel 2 */
    DO_CH3,     /* DO channel 3 */
    DO_CH4,     /* DO channel 4 */
    DO_CH5,     /* DO channel 5 */
    DO_CH6,     /* DO channel 6 */
    DO_CH7,     /* DO channel 7 */
    DO_CH8,     /* DO channel 8 */
    DO_CH9,     /* DO channel 9 */
    DO_CH10,    /* DO channel 10 */
    DO_CH11,    /* DO channel 11 */
    DO_CH12,    /* DO channel 12 */
    DO_CH13,    /* DO channel 13 */
    DO_CH14,    /* DO channel 14 */
    DO_CH15,    /* DO channel 15 */
    DO_CH16,    /* DO channel 16 */
    DO_CH17,    /* DO channel 17 */
    DO_CH18,    /* DO channel 18 */
    DO_CH19,    /* DO channel 19 */
    DO_CH20,    /* DO channel 20 */
    DO_CH21,    /* DO channel 21 */
    DO_CH22,    /* DO channel 22 */
    DO_CH23,    /* DO channel 23 */
    DO_CH24,    /* DO channel 24 */
    DO_CH25,    /* DO channel 25 */
    DO_CH26,    /* DO channel 26 */
    DO_CH27,    /* DO channel 27 */
    DO_CH28,    /* DO channel 28 */
    DO_CH29,    /* DO channel 29 */
    DO_CH30,    /* DO channel 30 */
    DO_CH31,    /* DO channel 31 */
    DO_CH32,    /* DO channel 32 */
    DO_CH33,    /* DO channel 33 */
    DO_CH34,    /* DO channel 34 */
    DO_CH35,    /* DO channel 35 */
    DO_CH36,    /* DO channel 36 */
    DO_CH37,    /* DO channel 37 */
    DO_CH38,    /* DO channel 38 */
    DO_CH39,    /* DO channel 39 */
    DO_CH40,    /* DO channel 40 */
    DO_CH41,    /* DO channel 41 */
    DO_CH42,    /* DO channel 42 */
    DO_CH43,    /* DO channel 43 */
    DO_CH44,    /* DO channel 44 */
    DO_CH45,    /* DO channel 45 */
    DO_CH46,    /* DO channel 46 */
    DO_CH47,    /* DO channel 47 */
    DO_CH48,    /* DO channel 48 */
    DO_CH49,    /* DO channel 49 */
    DO_CH50,    /* DO channel 50 */
    DO_CH51,    /* DO channel 51 */
    DO_CH52,    /* DO channel 52 */
    DO_CH53,    /* DO channel 53 */
    DO_CH54,    /* DO channel 54 */
    DO_CH55,    /* DO channel 55 */
    DO_CH56,    /* DO channel 56 */
    DO_CH57,    /* DO channel 57 */
    DO_CH58,    /* DO channel 58 */
    DO_CH59,    /* DO channel 59 */
    DO_CH60,    /* DO channel 60 */
    DO_CH61,    /* DO channel 61 */
    DO_CH62,    /* DO channel 62 */
    DO_CH63,    /* DO channel 63 */
    DO_CH_MAX   /* Total number of DO channels */
} BSP_DO_Channel_t;

typedef enum
{
    BSP_DO_STATE_UNINIT = -1,
    BSP_DO_STATE_LOW = 0,
    BSP_DO_STATE_HIGH = 1,
} BSP_DO_State_t;

typedef enum
{
    BSP_DO_OK = 0,

    BSP_DO_ERROR_PARAM = -1,

    BSP_DO_ERROR_HW,
    BSP_DO_ERROR_UNINIT = -5,
    BSP_DO_ERROR_ALREADY_INIT = -6,
    BSP_DO_ERROR_STATE,

} BSP_DO_Return_t;

typedef struct
{
    GPIO_TypeDef *port;
    uint32_t rcc_clk;
    uint16_t pin;
} BSP_DO_Hw_t;

typedef uint64_t BSP_DO_Type_t;

typedef struct
{
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
} BSP_DO_Version_t;

/* ===== Lifecycle ===== */
BSP_DO_Return_t BSP_DO_Init(BSP_DO_Channel_t channel);
BSP_DO_Return_t BSP_DO_DeInit(BSP_DO_Channel_t channel);

/* ===== Single channel control ===== */
BSP_DO_Return_t BSP_DO_Set_State(BSP_DO_Channel_t channel, BSP_DO_State_t state);
BSP_DO_Return_t BSP_DO_Get_State(BSP_DO_Channel_t channel, BSP_DO_State_t *state);
BSP_DO_Return_t BSP_DO_Toggle(BSP_DO_Channel_t channel);

/* ===== Multi-channel control ===== */
BSP_DO_Return_t BSP_DO_SetMask(BSP_DO_Type_t mask, BSP_DO_State_t state);

/* ===== Version ===== */
void BSP_DO_Get_Version(void);

#endif /* BSP_DO_H */