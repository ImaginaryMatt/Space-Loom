#ifndef SIGNAL_SEQUENCE_H
#define SIGNAL_SEQUENCE_H

#include "main.h"

/* pin assignments */

#define SIG1_GPIO_Port GPIOA
#define SIG1_Pin GPIO_PIN_0

#define SIG2_GPIO_Port GPIOA
#define SIG2_Pin GPIO_PIN_1

#define PULSE_GPIO_Port GPIOA
#define PULSE_Pin GPIO_PIN_2

/* function prototypes */

void SignalSequence_InitTimer(void);
void Delay_us(uint32_t us);
void Run_Signal_Sequence(void);

#endif
