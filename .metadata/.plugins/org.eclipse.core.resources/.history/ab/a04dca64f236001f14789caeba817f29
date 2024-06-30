#include "buzzer.h"

void Buzzer_Init(BuzzerConfigTypeDef* buzzer) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 初始化GPIO
    GPIO_InitStruct.Pin = buzzer->GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(buzzer->GPIOx, &GPIO_InitStruct);

    // 如果是有源蜂鸣器，直接将GPIO设置为低电平
    if (buzzer->type == BUZZER_ACTIVE) {
        HAL_GPIO_WritePin(buzzer->GPIOx, buzzer->GPIO_Pin, GPIO_PIN_RESET);
    }
}

void Buzzer_On(BuzzerConfigTypeDef* buzzer, uint32_t frequency) {
    if (buzzer->mode == BUZZER_PWM) {
        if (buzzer->type == BUZZER_PASSIVE) {
            // 配置定时器PWM频率
            uint32_t period = HAL_RCC_GetHCLKFreq() / frequency;
            buzzer->htim->Instance->ARR = period - 1;
            buzzer->htim->Instance->CCR1 = period / 2;
            HAL_TIM_PWM_Start(buzzer->htim, buzzer->tim_channel);
        } else {
            // 有源蜂鸣器直接设置GPIO高电平
            HAL_GPIO_WritePin(buzzer->GPIOx, buzzer->GPIO_Pin, GPIO_PIN_SET);
        }
    } else if (buzzer->mode == BUZZER_BLOCKING) {
        if (buzzer->type == BUZZER_PASSIVE) {
            uint32_t delay = 500000 / frequency;  // 半周期延时，单位为微秒
            while (1) {
                HAL_GPIO_TogglePin(buzzer->GPIOx, buzzer->GPIO_Pin);
                HAL_Delay(delay);
            }
        } else {
            // 有源蜂鸣器直接设置GPIO高电平
            HAL_GPIO_WritePin(buzzer->GPIOx, buzzer->GPIO_Pin, GPIO_PIN_SET);
        }
    }
}

void Buzzer_Off(BuzzerConfigTypeDef* buzzer) {
    if (buzzer->mode == BUZZER_PWM) {
        if (buzzer->type == BUZZER_PASSIVE) {
            HAL_TIM_PWM_Stop(buzzer->htim, buzzer->tim_channel);
        }
        // 不论有源还是无源，都设置GPIO为低电平
        HAL_GPIO_WritePin(buzzer->GPIOx, buzzer->GPIO_Pin, GPIO_PIN_RESET);
    } else if (buzzer->mode == BUZZER_BLOCKING) {
        // 直接设置GPIO为低电平
        HAL_GPIO_WritePin(buzzer->GPIOx, buzzer->GPIO_Pin, GPIO_PIN_RESET);
    }
}
