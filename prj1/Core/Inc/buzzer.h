#ifndef BUZZER_H
#define BUZZER_H

#include "stm32f1xx_hal.h" // 适用于你的具体 MCU 系列

typedef struct {
    TIM_HandleTypeDef *htim;
    uint32_t channel;
    uint32_t frequency;
    uint8_t volume;
} BuzzerTypeDef;

void Buzzer_Init(BuzzerTypeDef *buzzer);
void Buzzer_SetFrequency(BuzzerTypeDef *buzzer, uint32_t frequency);
void Buzzer_SetVolume(BuzzerTypeDef *buzzer, uint8_t volume);

#endif // BUZZER_H
