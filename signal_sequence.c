#include "signal_sequence.h"

extern TIM_HandleTypeDef htim2;

/* timing values (microseconds) */

#define SIG1_DELAY_US 20
#define SIG2_DELAY_US 50
#define PULSE_DELAY_US 10
#define PULSE_WIDTH_US 5

void SignalSequence_InitTimer(void)
{
    HAL_TIM_Base_Start(&htim2);
}

void Delay_us(uint32_t us)
{
    __HAL_TIM_SET_COUNTER(&htim2, 0);

    while(__HAL_TIM_GET_COUNTER(&htim2) < us)
    {
    }
}

void Run_Signal_Sequence(void)
{
    uint32_t t = 0;

    uint32_t end_time = PULSE_DELAY_US + PULSE_WIDTH_US;

    if(SIG1_DELAY_US > end_time) end_time = SIG1_DELAY_US;
    if(SIG2_DELAY_US > end_time) end_time = SIG2_DELAY_US;

    HAL_GPIO_WritePin(SIG1_GPIO_Port, SIG1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SIG2_GPIO_Port, SIG2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PULSE_GPIO_Port, PULSE_Pin, GPIO_PIN_RESET);

    while(1)
    {
        if(t == SIG1_DELAY_US)
        {
            HAL_GPIO_WritePin(SIG1_GPIO_Port, SIG1_Pin, GPIO_PIN_SET);
        }

        if(t == SIG2_DELAY_US)
        {
            HAL_GPIO_WritePin(SIG2_GPIO_Port, SIG2_Pin, GPIO_PIN_SET);
        }

        if(t == PULSE_DELAY_US)
        {
            HAL_GPIO_WritePin(PULSE_GPIO_Port, PULSE_Pin, GPIO_PIN_SET);
        }

        if(t == (PULSE_DELAY_US + PULSE_WIDTH_US))
        {
            HAL_GPIO_WritePin(PULSE_GPIO_Port, PULSE_Pin, GPIO_PIN_RESET);
        }

        if(t > end_time)
        {
            break;
        }

        Delay_us(1);
        t++;
    }
}
