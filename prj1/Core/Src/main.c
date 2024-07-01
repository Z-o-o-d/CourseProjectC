/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "usbd_cdc_if.h"
#include "ssd1306_Views.h"
#include "buzzer.h"

#include "AT_WIFI.h"
#include "DHT.h"
#include <stdio.h>
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define CDC_BUFFER_SIZE 30

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_tx;
DMA_HandleTypeDef hdma_usart3_rx;

/* USER CODE BEGIN PV */

//extern USBD_HandleTypeDef hUsbDeviceFS;




/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_ADC2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */


extern ESP_Config esp_config;

uint8_t CDC_BUFFER[CDC_BUFFER_SIZE]={0};

uint16_t ADC_BUFFER = 0;

uint16_t Period_=100;

uint8_t FLAG_MUTE = 0;

uint16_t PUMP[2]={0,0};

WiFiInfoTypeDef WiFiInfo={0};
IPInfoTypeDef IPInfo={0};
DHT_data DHT11_Info={0};
DHT_data DHT11_Alarm_H={40,70};
DHT_data DHT11_Alarm_L={20,70};

DHT_sensor DHT11_Sensor = {GPIOB, GPIO_PIN_0, DHT11, GPIO_NOPULL};
BuzzerTypeDef buzzer = {&htim1, TIM_CHANNEL_3, 100, 36};

uint8_t CurrentView=1;

char *settings[] = {"TempH", "TempL", "Hum H", "Hum L"};
uint8_t FLAG_NowSettingVal = 2;
uint8_t FLAG_CheckWifi = 1;
uint8_t FLAG_CheckDHT = 1;
uint8_t FLAG_SentTCP = 1;

volatile uint32_t CNT_TIMER2 = 0;

volatile uint8_t FLAG_SentKEY0 = 0;
volatile uint8_t FLAG_SentKEY1 = 0;
volatile uint8_t FLAG_SentKEY2 = 0;
volatile uint8_t FLAG_SentKEY3 = 0;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void ssd1306_WelcomeView(){
	uint8_t msg[100];
	ssd1306_Fill(Black);
	ssd1306_SetCursor(0, 0);
	sprintf(msg, "BOOTING");
	ssd1306_WriteString(msg, Font_11x18, White);
	ssd1306_SetCursor(0, 25);
	sprintf(msg, "Course PrjC");
	ssd1306_WriteString(msg, Font_11x18, White);
	ssd1306_SetCursor(0, 50);
	sprintf(msg, "Initialing...");
	ssd1306_WriteString(msg, Font_7x10, White);
	ssd1306_UpdateScreen();
}

void ssd1306_IndexView(){
	uint8_t msg[100];
	WiFiInfo=ESP_CheckWiFi();
	IPInfo=ESP_GetIPInfo();
	ssd1306_Fill(Black);
	ssd1306_SetCursor(0, 0);
	sprintf(msg, "Index");
	ssd1306_WriteString(msg, Font_11x18, White);



	ssd1306_SetCursor(0 , 56);
	sprintf(msg, "DHT");
	ssd1306_WriteString(msg, Font_6x8, White);
	ssd1306_SetCursor(32, 56);
	sprintf(msg, "PUMP");
	ssd1306_WriteString(msg, Font_6x8, White);
	ssd1306_SetCursor(64, 56);
	sprintf(msg, "BEEP");
	ssd1306_WriteString(msg, Font_6x8, White);
	ssd1306_SetCursor(96, 56);
	sprintf(msg, "NET");
	ssd1306_WriteString(msg, Font_6x8, White);
	ssd1306_UpdateScreen();
}

void ssd1306_NetWorkView(){
	uint8_t msg[100];
	ssd1306_Fill(Black);
	ssd1306_SetCursor(0, 0);
	sprintf(msg, "NETWORK %d",WiFiInfo.rssi);
	ssd1306_WriteString(msg, Font_11x18, White);
	ssd1306_SetCursor(0, 18);
	sprintf(msg, "%s",esp_config.ap_ssid);
	ssd1306_WriteString(msg, Font_7x10, White);
	ssd1306_SetCursor(0, 31);
	sprintf(msg, "AP :%s",IPInfo.AP_IP);
	ssd1306_WriteString(msg, Font_7x10, White);
	ssd1306_SetCursor(0, 43);
	sprintf(msg, "%s",WiFiInfo.ssid);
	ssd1306_WriteString(msg, Font_7x10, White);
	ssd1306_SetCursor(0, 54);
	sprintf(msg, "STA:%s",IPInfo.STA_IP);
	ssd1306_WriteString(msg, Font_7x10, White);
	ssd1306_UpdateScreen();
}

void ssd1306_SensorView(){
	uint8_t msg[100];
	ssd1306_Fill(Black);
	ssd1306_SetCursor(0, 0);
	sprintf(msg, "SENSOR");
	ssd1306_WriteString(msg, Font_11x18, White);
	ssd1306_SetCursor(0, 18);
	sprintf(msg, "T:%dC",(uint8_t)DHT11_Info.temp);
	ssd1306_WriteString(msg, Font_11x18, White);
	ssd1306_SetCursor(64, 18);
	sprintf(msg, "H:%d%%",(uint8_t)DHT11_Info.hum);
	ssd1306_WriteString(msg, Font_11x18, White);
	ssd1306_SetCursor(0, 36);
	sprintf(msg, "%d",(uint8_t)DHT11_Alarm_L.temp);
	ssd1306_WriteString(msg, Font_7x10, White);
	ssd1306_SetCursor(32, 36);
	sprintf(msg, "%d",(uint8_t)DHT11_Alarm_H.temp);
	ssd1306_WriteString(msg, Font_7x10, White);
	ssd1306_SetCursor(96, 36);
	sprintf(msg, "%d",(uint8_t)DHT11_Alarm_L.hum);
	ssd1306_WriteString(msg, Font_7x10, White);
	ssd1306_SetCursor(64, 36);
	sprintf(msg, "%d",(uint8_t)DHT11_Alarm_H.hum);
	ssd1306_WriteString(msg, Font_7x10, White);
	ssd1306_SetCursor(0, 46);
	sprintf(msg, "SOIL");
	ssd1306_WriteString(msg, Font_7x10, White);
	ssd1306_SetCursor(32, 46);
	sprintf(msg, "1:%d",(uint8_t)DHT11_Alarm_H.temp);
	ssd1306_WriteString(msg, Font_7x10, White);
	ssd1306_SetCursor(96, 46);
	sprintf(msg, "2:%d",(uint8_t)DHT11_Alarm_H.temp);
	ssd1306_WriteString(msg, Font_7x10, White);
	ssd1306_SetCursor(64, 46);
	sprintf(msg, "3:%d",(uint8_t)DHT11_Alarm_H.temp);
	ssd1306_WriteString(msg, Font_7x10, White);
	ssd1306_SetCursor(0 , 56);
	sprintf(msg, "<");
	ssd1306_WriteString(msg, Font_6x8, White);
	ssd1306_SetCursor(16, 56);
	sprintf(msg, "%s", settings[FLAG_NowSettingVal]);
	ssd1306_WriteString(msg, Font_6x8, White);
	ssd1306_SetCursor(64, 56);
	sprintf(msg, " \\/");
	ssd1306_WriteString(msg, Font_6x8, White);
	ssd1306_SetCursor(96, 56);
	sprintf(msg, " /\\");
	ssd1306_WriteString(msg, Font_6x8, White);
	ssd1306_UpdateScreen();
}

void ssd1306_PumpView(){
	uint8_t msg[100];
	ssd1306_Fill(Black);
	ssd1306_SetCursor(0, 0);
	sprintf(msg, "PUMP");
	ssd1306_WriteString(msg, Font_11x18, White);
	ssd1306_SetCursor(0, 18);
	sprintf(msg, "U:%d\r\n", PUMP[0]);
	ssd1306_WriteString(msg, Font_11x18, White);
	ssd1306_SetCursor(64, 18);
	sprintf(msg, "I:%d\r\n", PUMP[1]);
	ssd1306_WriteString(msg, Font_11x18, White);
	ssd1306_SetCursor(0, 36);
	sprintf(msg, "Duty:%d", Period_);
	ssd1306_WriteString(msg, Font_11x18, White);
	ssd1306_SetCursor(110, 36);
	sprintf(msg, "MAX");
	ssd1306_WriteString(msg, Font_6x8, White);
	ssd1306_SetCursor(110, 45);
	sprintf(msg, "720");
	ssd1306_WriteString(msg, Font_6x8, White);
	ssd1306_SetCursor(0 , 56);
	sprintf(msg, "<");
	ssd1306_WriteString(msg, Font_6x8, White);
	ssd1306_SetCursor(16, 56);
	sprintf(msg, "TimeSet");
	ssd1306_WriteString(msg, Font_6x8, White);
	ssd1306_SetCursor(64, 56);
	sprintf(msg, " \\/");
	ssd1306_WriteString(msg, Font_6x8, White);
	ssd1306_SetCursor(96, 56);
	sprintf(msg, " /\\");
	ssd1306_WriteString(msg, Font_6x8, White);
	ssd1306_UpdateScreen();
}

void ssd1306_BuzzerView(){
	uint8_t msg[100];
	ssd1306_Fill(Black);
	ssd1306_SetCursor(0, 0);
	sprintf(msg, "BUZZER");
	ssd1306_WriteString(msg, Font_11x18, White);
	ssd1306_SetCursor(0, 18);
	sprintf(msg, "FREQ:%d\r\n",buzzer.frequency );
	ssd1306_WriteString(msg, Font_11x18, White);
	ssd1306_SetCursor(0, 36);
    sprintf(msg, "MUTE: %s\r\n", FLAG_MUTE ? "ON" : "OFF");
    ssd1306_WriteString(msg, Font_11x18, White);
	ssd1306_SetCursor(0 , 56);
	sprintf(msg, "<");
	ssd1306_WriteString(msg, Font_6x8, White);
	ssd1306_SetCursor(24, 56);
	sprintf(msg, "MUTE");
	ssd1306_WriteString(msg, Font_6x8, White);
	ssd1306_SetCursor(64, 56);
	sprintf(msg, " \\/");
	ssd1306_WriteString(msg, Font_6x8, White);
	ssd1306_SetCursor(96, 56);
	sprintf(msg, " /\\");
	ssd1306_WriteString(msg, Font_6x8, White);
	ssd1306_UpdateScreen();
}

void KeyHandeler_WelcomeView(){

}
void KeyHandeler_IndexView(){
	if (FLAG_SentKEY0) {
		FLAG_SentKEY0=0;
		CurrentView=V_NETWORK;
	}else
	if (FLAG_SentKEY1) {
		FLAG_SentKEY1=0;
		CurrentView=V_BUZZER;
	}else
	if (FLAG_SentKEY2) {
		FLAG_SentKEY2=0;
		CurrentView=V_PUMP;
	}else
	if (FLAG_SentKEY3) {
		FLAG_SentKEY3=0;
		CurrentView=V_INDEX;
	}
}

void KeyHandeler_NetWorkView(){
	if (FLAG_SentKEY0) {
		FLAG_SentKEY0=0;
		CurrentView=V_NETWORK;
	}else
	if (FLAG_SentKEY1) {
		FLAG_SentKEY1=0;
		CurrentView=V_NETWORK;
	}else
	if (FLAG_SentKEY2) {
		FLAG_SentKEY2=0;
		CurrentView=V_NETWORK;
	}else
	if (FLAG_SentKEY3) {
		FLAG_SentKEY3=0;
		CurrentView=V_INDEX;
	}
}
void KeyHandeler_SensorView(){
	if (FLAG_SentKEY0) {
		FLAG_SentKEY0=0;
		CurrentView=V_NETWORK;
	}else
	if (FLAG_SentKEY1) {
		FLAG_SentKEY1=0;
		CurrentView=V_NETWORK;
	}else
	if (FLAG_SentKEY2) {
		FLAG_SentKEY2=0;
		CurrentView=V_NETWORK;
	}else
	if (FLAG_SentKEY3) {
		FLAG_SentKEY3=0;
		CurrentView=V_INDEX;
	}
}
void KeyHandeler_PumpView(){
	if (FLAG_SentKEY0) {
		FLAG_SentKEY0=0;
		CurrentView=V_NETWORK;
	}else
	if (FLAG_SentKEY1) {
		FLAG_SentKEY1=0;
		CurrentView=V_NETWORK;
	}else
	if (FLAG_SentKEY2) {
		FLAG_SentKEY2=0;
		CurrentView=V_NETWORK;
	}else
	if (FLAG_SentKEY3) {
		FLAG_SentKEY3=0;
		CurrentView=V_INDEX;
	}
}
void KeyHandeler_BuzzerView(){
	if (FLAG_SentKEY0) {
		FLAG_SentKEY0=0;
		CurrentView=V_NETWORK;
	}else
	if (FLAG_SentKEY1) {
		FLAG_SentKEY1=0;
		CurrentView=V_NETWORK;
	}else
	if (FLAG_SentKEY2) {
		FLAG_SentKEY2=0;
		CurrentView=V_NETWORK;
	}else
	if (FLAG_SentKEY3) {
		FLAG_SentKEY3=0;
		CurrentView=V_INDEX;
	}
}


void KeyHandeler(uint8_t SWView) {
	if (SWView == V_WELCOME) {
	    KeyHandeler_WelcomeView();
	} else if (SWView == V_INDEX) {
	    KeyHandeler_IndexView();
	} else if (SWView == V_NETWORK) {
	    KeyHandeler_NetWorkView();
	} else if (SWView == V_SENSOR) {
	    KeyHandeler_SensorView();
	} else if (SWView == V_PUMP) {
	    KeyHandeler_PumpView();
	} else if (SWView == V_BUZZER) {
	    KeyHandeler_BuzzerView();
	}
//
//			if (FLAG_SentKEY0) {
//				CurrentView=V_BUZZER;
//				FLAG_SentKEY0=0;
//			}else
//			if (FLAG_SentKEY1) {
//				CurrentView=V_NETWORK;
//				FLAG_SentKEY1=0;
//			}else
//			if (FLAG_SentKEY2) {
//				CurrentView=V_PUMP;
//				FLAG_SentKEY2=0;
//			}else
//			if (FLAG_SentKEY3) {
//				CurrentView=V_SENSOR;
//				FLAG_SentKEY3=0;
//			}
}

void ShowView(uint8_t SWView) {
    switch (SWView) {
        case V_WELCOME:
            ssd1306_WelcomeView();
            break;
        case V_INDEX:
            ssd1306_IndexView();
            break;
        case V_NETWORK:
            ssd1306_NetWorkView();
            break;
        case V_SENSOR:
            ssd1306_SensorView();
            break;
        case V_PUMP:
            ssd1306_PumpView();
            break;
        case V_BUZZER:
            ssd1306_BuzzerView();
            break;
        default:
            // Handle invalid view case
            break;
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM1_Init();
  MX_ADC2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADC_Start(&hadc1);
  ssd1306_Init();
  ssd1306_WelcomeView();

  HAL_TIM_Base_Start_IT(&htim2);

  HAL_TIM_Base_Start(&htim1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
//  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

  HAL_TIM_Base_Start(&htim3);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_Base_Start(&htim4);

  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)PUMP, 2);



  ESP_UART_Init(&huart3);
//  ESP_INIT_FULL();
  ESP_INIT_BASE();
  HAL_Delay(100);

	WiFiInfo=ESP_CheckWiFi();
	  HAL_Delay(100);
	IPInfo=ESP_GetIPInfo();
  ssd1306_NetWorkView();


  Buzzer_Init(&buzzer);
  Buzzer_SetVolume(&buzzer, 36);

  HAL_Delay(100);
  Buzzer_SetFrequency(&buzzer, 200);
  HAL_Delay(100);
  Buzzer_SetFrequency(&buzzer, 300);
  HAL_Delay(100);
  Buzzer_SetFrequency(&buzzer, 400);
  HAL_Delay(100);
  Buzzer_SetFrequency(&buzzer, 500);
  HAL_Delay(100);

//  playMelody(&buzzer);
  Buzzer_SetVolume(&buzzer, 2);

  Buzzer_SetVolume(&buzzer, 0);



  HAL_Delay(100);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  if (FLAG_CheckDHT) {
			DHT11_Info = DHT_getData(&DHT11_Sensor);
			FLAG_CheckDHT=0;
		}

		if (FLAG_SentTCP) {
			uint8_t msg[1000];
			sprintf(msg, "DATA:T:%lf,H:%lf,Duty:%d,U:%d,I:%d\r\n", DHT11_Info.temp,DHT11_Info.hum,Period_,PUMP[0],PUMP[1]);
			ESP_SendTCP(0,msg);
			  HAL_Delay(100);
			  FLAG_SentTCP=0;
		}

		if (FLAG_CheckWifi) {
			WiFiInfo=ESP_CheckWiFi();
			HAL_Delay(10);
			IPInfo=ESP_GetIPInfo();
			FLAG_CheckWifi=0;
		}


		KeyHandeler(CurrentView);
//	  CurrentView=V_NETWORK;
		ShowView(CurrentView);



    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T4_CC4;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 2;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc2.Init.ContinuousConvMode = ENABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 3;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM2;
  sConfigOC.Pulse = 2;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.Pulse = 36;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 720-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 10000-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 720;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 72-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
  sConfigOC.Pulse = 36;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin : DHT11_Pin */
  GPIO_InitStruct.Pin = DHT11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB12 PB13 PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
