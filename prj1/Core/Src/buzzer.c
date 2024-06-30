#include "buzzer.h"

void Buzzer_Init(BuzzerTypeDef *buzzer) {
    Buzzer_SetFrequency(buzzer, buzzer->frequency);
    Buzzer_SetVolume(buzzer, buzzer->volume);

    uint32_t period=0;
    if (buzzer->htim->Instance == TIM1 ) {
//    if (buzzer->htim->Instance == TIM1 || buzzer->htim->Instance == TIM8) {
    	 period = HAL_RCC_GetPCLK2Freq()/1000000-1;
    } else {
    	 period = HAL_RCC_GetPCLK1Freq()/1000000-1;
    }
    __HAL_TIM_SET_AUTORELOAD(buzzer->htim, period);

    HAL_TIM_PWM_Start(buzzer->htim, buzzer->channel);
}

void Buzzer_SetFrequency(BuzzerTypeDef *buzzer, uint32_t frequency) {
    buzzer->frequency = frequency;

    uint32_t prescaler = 1000000/frequency  - 1;
    __HAL_TIM_SET_PRESCALER(buzzer->htim, prescaler);
}

void Buzzer_SetVolume(BuzzerTypeDef *buzzer, uint8_t volume) {
    buzzer->volume = volume;
    uint32_t compare_value = volume;

    __HAL_TIM_SET_COMPARE(buzzer->htim, buzzer->channel, compare_value);
}
