#include "AT_WIFI.h"

// Default UART handle
UART_HandleTypeDef *AT_huart;
extern UART_HandleTypeDef huart3;

ESP_Config esp_config = {
    .server_port = 9999,
    .wifi_ssid = "ChinaNet-2.4G-4924",
    .wifi_pswd = "",
    .ap_ssid = "ESP_WIFI",
    .ap_pswd = "12345678",
    .ap_ip = "192.168.15.1"
};

void SetESPConfig(uint16_t server_port, const char* wifi_ssid, const char* wifi_pswd, const char* ap_ssid, const char* ap_pswd, const char* ap_ip) {
    esp_config.server_port = server_port;
    strncpy(esp_config.wifi_ssid, wifi_ssid, sizeof(esp_config.wifi_ssid));
    strncpy(esp_config.wifi_pswd, wifi_pswd, sizeof(esp_config.wifi_pswd));
    strncpy(esp_config.ap_ssid, ap_ssid, sizeof(esp_config.ap_ssid));
    strncpy(esp_config.ap_pswd, ap_pswd, sizeof(esp_config.ap_pswd));
    strncpy(esp_config.ap_ip, ap_ip, sizeof(esp_config.ap_ip));
}

void ESP_SendCommand(const char *command) {
    // Transmit the command using the default UART
    HAL_UART_Transmit(AT_huart, (uint8_t*)command, strlen(command), 1000);
}

void ESP_SendTCP(uint8_t con_num, char *data) {
    // Calculate the total length
    uint8_t cmd[50];
    sprintf((char*)cmd, "AT+CIPSEND=%d,%d\r\n", con_num, strlen(data));
    HAL_UART_Transmit(AT_huart, cmd, strlen((char*)cmd), 100);
    HAL_Delay(1); // Adjust this delay as needed
    HAL_UART_Transmit(AT_huart, (uint8_t *)data, strlen(data), 1000);
}

void ESP_UART_Init(UART_HandleTypeDef *huart){
    AT_huart = huart;
}

// Function definitions for AT commands with modifiable parameters

void ESP_Reset() {
#ifdef ENABLE_GPIO_RESET_CONTROL
	ESP_Reset_GPIO();
#else
    ESP_SendCommand("AT+RST\r\n");
#endif
}

void ESP_SetMode_AP() {
    ESP_SendCommand("AT+CWMODE=2\r\n");
}

void ESP_SetIP(const char *ip) {
    char cmd[50];
    sprintf(cmd, "AT+CIPAP=\"%s\"\r\n", ip);
    ESP_SendCommand(cmd);
}

void ESP_SetMode_AP_STD() {
    ESP_SendCommand("AT+CWMODE=3\r\n");
}

void ESP_SetSoftAP(const char *ssid, const char *password) {
    char cmd[100];

    #ifdef USE_PASSWORD
        // Use password if USE_PASSWORD is defined
        sprintf(cmd, "AT+CWSAP=\"%s\",\"%s\",1,3\r\n", ssid, password);
    #else
        // No password if USE_PASSWORD is not defined
        sprintf(cmd, "AT+CWSAP=\"%s\",\"\",1,0\r\n", ssid);
    #endif

    ESP_SendCommand(cmd);
}

void ESP_ConnectWiFi(const char *ssid, const char *password) {
    char cmd[100];
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
    ESP_SendCommand(cmd);
}

WiFiInfoTypeDef ESP_CheckWiFi(void) {
    WiFiInfoTypeDef info = {0};
    char response[100] = {0};
    char *token;
    huart3.RxXferCount = 0;
    ESP_SendCommand("AT+CWJAP?\r\n");
    HAL_UART_Receive(AT_huart, (uint8_t *)response, sizeof(response), 1000);

    // Parse the response and populate the WiFiInfo struct

    // I don know why it can't be     token = strtok(response, "+CWJAP:,\"");
    // sscanf scanf ALSO can't used  so strange

    token = strtok(response, ":,\"");

    token = strtok(NULL, "\",\""); // Get SSID
    strcpy(info.ssid, token);

    token = strtok(NULL, "\","); // Get MAC address
    strcpy(info.bssid, token);

    token = strtok(NULL, ","); // Get security mode
    info.channel = atoi(token);

    token = strtok(NULL, ","); // Get RSSI
    info.rssi = atoi(token);

    token = strtok(NULL, "\r\n"); // Get connected status
    info.encryption = atoi(token);

    return info;
}

IPInfoTypeDef ESP_GetIPInfo(void) {
    IPInfoTypeDef ipInfo = {0};
    char response[200] = {0};
    char *token;

    ESP_SendCommand("AT+CIFSR\r\n");
    HAL_UART_Receive(AT_huart, (uint8_t *)response, sizeof(response), 1000);

    // Parse AP IP
    token = strstr(response, "+CIFSR:APIP,\"");
    if (token) {
        token += strlen("+CIFSR:APIP,\"");
        strncpy(ipInfo.AP_IP, token, strcspn(token, "\""));
    }

    // Parse AP MAC
    token = strstr(response, "+CIFSR:APMAC,\"");
    if (token) {
        token += strlen("+CIFSR:APMAC,\"");
        strncpy(ipInfo.AP_MAC, token, strcspn(token, "\""));
    }

    // Parse STA IP
    token = strstr(response, "+CIFSR:STAIP,\"");
    if (token) {
        token += strlen("+CIFSR:STAIP,\"");
        strncpy(ipInfo.STA_IP, token, strcspn(token, "\""));
    }

    // Parse STA MAC
    token = strstr(response, "+CIFSR:STAMAC,\"");
    if (token) {
        token += strlen("+CIFSR:STAMAC,\"");
        strncpy(ipInfo.STA_MAC, token, strcspn(token, "\""));
    }
    return ipInfo;
}

void ESP_EnableMUX() {
    ESP_SendCommand("AT+CIPMUX=1\r\n");
}

void ESP_StartServer(uint16_t port) {
    char cmd[50];
    sprintf(cmd, "AT+CIPSERVER=1,%d\r\n", port);
    ESP_SendCommand(cmd);
}

void ESP_StopServer() {
    ESP_SendCommand("AT+CIPSERVER=0\r\n");
}

void ESP_ListAPs() {
    ESP_SendCommand("AT+CWLAP\r\n");
}

void ESP_RestoreDefaults() {
    ESP_SendCommand("AT+RESTORE\r\n");
}


void ESP_RESET_SERVER() {
	ESP_StopServer();
    HAL_Delay(1000);
    ESP_StartServer(9999);
    HAL_Delay(1000);
}

void ESP_INIT_BASE() {
    ESP_Reset();
    HAL_Delay(1000);
    ESP_EnableMUX();
    HAL_Delay(1000);
    ESP_StartServer(9999);
    HAL_Delay(1000);
}

void ESP_INIT_FULL() {
    ESP_RestoreDefaults();
    HAL_Delay(2000);

    ESP_Reset();
    HAL_Delay(2000);

    ESP_SetMode_AP();
    HAL_Delay(1000);

    ESP_SetIP(esp_config.ap_ip);
    HAL_Delay(1000);

    ESP_SetMode_AP_STD();
    HAL_Delay(1000);

    ESP_SetSoftAP(esp_config.ap_ssid, esp_config.ap_pswd);
    HAL_Delay(2000);

    HAL_Delay(2000);
    ESP_ConnectWiFi(esp_config.wifi_ssid, esp_config.wifi_pswd);
    HAL_Delay(20000);

    ESP_EnableMUX();
    HAL_Delay(1000);

    ESP_StartServer(esp_config.server_port);
}

#ifdef ENABLE_GPIO_RESET_CONTROL
static GPIO_TypeDef *ESP_Reset_GPIO_Port = ESP_RESET_GPIO_Port;
static uint16_t ESP_Reset_Pin = ESP_RESET_Pin;

void ESP_Reset_GPIO() {
    // Set the reset pin low to reset the ESP module
    HAL_GPIO_WritePin(ESP_Reset_GPIO_Port, ESP_Reset_Pin, GPIO_PIN_RESET);
    HAL_Delay(100); // Hold low for 1000 ms
    // Set the reset pin high to bring the ESP module out of reset
    HAL_GPIO_WritePin(ESP_Reset_GPIO_Port, ESP_Reset_Pin, GPIO_PIN_SET);
    HAL_Delay(1000); // Wait for the ESP module to initialize
}
#endif // ENABLE_GPIO_RESET_CONTROL




