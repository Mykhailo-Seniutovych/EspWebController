#include "app-main.h"
#include "stm32f1xx_hal.h"
#include <esp-uart.h>
#include <string.h>

extern UART_HandleTypeDef huart2;

int appMain() {
    auto espUart = EspUart(&huart2);
    espUart.sendCommand("AT\r\n", "OK");
    espUart.sendCommand("AT\r\n", "OK");
    espUart.sendCommand("AT+CWMODE_CUR=1\r\n", "OK");
    espUart.sendCommand("AT+CWJAP_CUR=\"Fake Agent\",\"\"\r\n", "WIFI GOT IP");
    espUart.sendCommand("AT+CIPMUX=1\r\n", "OK");
    espUart.sendCommand("AT+CIPSERVER=1,80\r\n", "OK");
    espUart.sendCommand("AT+CIFSR\r\n", "OK");


    HAL_Delay(2000);

    while (1) {
    }
    return 0;
}