#ifndef AT_WIFI_H
#define AT_WIFI_H

#include "stm32f1xx_hal.h"
#include <string.h>
#include "main.h"
#include <stdio.h>

extern UART_HandleTypeDef *AT_huart;

// Structure to hold Wi-Fi information
typedef struct {
    uint8_t ssid[32];   // SSID (up to 32 characters)
    uint8_t bssid[18];  // BSSID (MAC address, formatted like "8c:53:c3:81:0f:d6")
    uint8_t channel;     // Channel number
    uint8_t rssi;        // Signal strength in dBm
    int8_t encryption;  // Encryption type (0 for open, 1 for WEP, 2 for WPA_PSK, 3 for WPA2_PSK, 4 for WPA_WPA2_PSK)
} WiFiInfoTypeDef;


// Function prototypes for sending command and receiving response
void ESP_SendCommand(const char *command);

// Function prototypes for sending data with multiple variables
void ESP_SendTCP(uint8_t con_num, char *data);

// Function prototype for initializing UART with specified handle
void ESP_UART_Init(UART_HandleTypeDef *huart);

// Function prototypes for AT commands with modifiable parameters
void ESP_Reset();
void ESP_SetModeAP();
void ESP_SetIP(const char *ip);
void ESP_SetModeStation();
void ESP_SetSoftAP(const char *ssid, const char *password);
void ESP_ConnectWiFi(const char *ssid, const char *password);
WiFiInfoTypeDef ESP_CheckWiFi(void);
void ESP_GetIP();
void ESP_EnableMUX();
void ESP_StartServer(uint16_t port);
void ESP_StopServer();
void ESP_ListAPs();
void ESP_RestoreDefaults();

#endif // AT_WIFI_H
