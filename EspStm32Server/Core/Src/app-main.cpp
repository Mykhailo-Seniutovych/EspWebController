#include "app-main.h"
#include "esp-command.h"
#include "stm32f1xx_hal.h"
#include <esp-server-config.h>
#include <esp-server.h>
#include <stdio.h>
#include <string.h>

#define STATUS_LED_PORT GPIOC
#define STATUS_LED_PIN GPIO_PIN_13
#define RED_LED_PORT GPIOB
#define RED_LED_PIN GPIO_PIN_11
#define GREEN_LED_PORT GPIOB
#define GREEN_LED_PIN GPIO_PIN_10

extern UART_HandleTypeDef huart2;

bool isGreenLedOn = false;
bool isRedLedOn = false;

static void executeCommand(EspServer &espServer, EspCommand command);
static const char *toBoolString(bool boolean);

int appMain() {
    auto config = EspServerConfig();
    config.port = 80;
    config.wifiSsid = "wifi";
    config.wifiPassword = "password";

    auto espServer = EspServer(&huart2, &config);
    auto initSuccess = espServer.initializeTcpServer();
    if (!initSuccess) {
        return -1;
    }

    HAL_GPIO_WritePin(STATUS_LED_PORT, STATUS_LED_PIN, GPIO_PIN_RESET);
    HAL_Delay(2000);
    HAL_GPIO_WritePin(STATUS_LED_PORT, STATUS_LED_PIN, GPIO_PIN_SET);

    while (1) {
        auto command = espServer.waitForCommand();
        executeCommand(espServer, command);
    }
    return 0;
}

static void executeCommand(EspServer &espServer, EspCommand command) {
    if (command == RequestLedState) {
        char buffer[40] = {0};

        sprintf(buffer, "{ \"greenLed\": %s, \"redLed\": %s }", toBoolString(isGreenLedOn), toBoolString(isRedLedOn));
        espServer.sendTcpResponse(buffer);
    } else if (command == GreenLedOn) {
        HAL_GPIO_WritePin(GREEN_LED_PORT, GREEN_LED_PIN, GPIO_PIN_SET);
        isGreenLedOn = true;
        espServer.sendTcpResponse(EspResponse::Ok);
    } else if (command == GreenLedOff) {
        HAL_GPIO_WritePin(GREEN_LED_PORT, GREEN_LED_PIN, GPIO_PIN_RESET);
        isGreenLedOn = false;
        espServer.sendTcpResponse(EspResponse::Ok);
    } else if (command == RedLedOn) {
        HAL_GPIO_WritePin(RED_LED_PORT, RED_LED_PIN, GPIO_PIN_SET);
        isRedLedOn = true;
        espServer.sendTcpResponse(EspResponse::Ok);
    } else if (command == RedLedOff) {
        HAL_GPIO_WritePin(RED_LED_PORT, RED_LED_PIN, GPIO_PIN_RESET);
        isRedLedOn = false;
        espServer.sendTcpResponse(EspResponse::Ok);
    }
}

static const char *toBoolString(bool boolean) {
    return boolean ? "true" : "false";
}