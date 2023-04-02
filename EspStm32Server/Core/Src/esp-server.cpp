#include "esp-server.h"
#include "stm32f1xx_hal.h"
#include <array>
#include <regex>
#include <stdio.h>
#include <string.h>

EspServer::EspServer(UART_HandleTypeDef *huart, EspServerConfig *config) : huart_(huart), config_(config) {
    readBuffer_ = new char[readBufferSize_];
    lastError_ = new char[errorBufferSize_];
}

EspServer::~EspServer() {
    delete[] readBuffer_;
    delete[] lastError_;
}

bool EspServer::initializeTcpServer() {
    char wifiConnect[100];
    sprintf(wifiConnect, "AT+CWJAP_CUR=\"%s\",\"%s\"\r\n", config_->wifiSsid, config_->wifiPassword);
    char serverStart[30];
    sprintf(serverStart, "AT+CIPSERVER=1,%d\r\n", config_->port);

    return sendCommand("AT\r\n", "OK")                                        //
           && sendCommand("AT+CWMODE_CUR=1\r\n", "OK")                        //
           && sendCommand(wifiConnect, "WIFI GOT IP") && waitForUartMsg("OK") //
           && sendCommand("AT+CIPMUX=1\r\n", "OK")                            //
           && sendCommand("AT+CIFSR\r\n", "OK")                               //
           && sendCommand(serverStart, "OK");                                 //
}

EspCommand EspServer::waitForCommand() {
    HAL_UARTEx_ReceiveToIdle_DMA(huart_, readBufferInt(), 1000);

    const char *requestPattern = "+IPD,";
    volatile char *responseStart = NULL;
    while ((responseStart = strstr(readBufferChar(), requestPattern)) == NULL) {
        HAL_UARTEx_ReceiveToIdle_DMA(huart_, readBufferInt(), readBufferSize_);
    }

    // A short delay to make sure we already have the request payload in UART buffer
    HAL_Delay(100);

    const uint8_t conIdPosition = 5;
    const uint8_t cmdPosition = 9;
    currentConnectionId_ = responseStart[conIdPosition];
    auto cmd = static_cast<EspCommand>(responseStart[cmdPosition]);

    cleanBuffers();
    return cmd;
}

void EspServer::sendTcpResponse(EspResponse response) {
    char *sendResponseCommand = "AT+CIPSEND=0,1\r\n";
    sendCommand(sendResponseCommand, "OK");

    HAL_UART_Transmit(huart_, reinterpret_cast<uint8_t *>(&response), 1, sendTimeout_);
    HAL_Delay(500);
    currentConnectionId_ = 0;
}

void EspServer::sendTcpResponse(const char *response) {
    auto length = strlen(response);

    char sendResponseCommand[20];
    sprintf(sendResponseCommand, "AT+CIPSEND=0,%d\r\n", length);
    sendCommand(sendResponseCommand, "OK");

    HAL_UART_Transmit(huart_, (uint8_t *)response, length, sendTimeout_);
    HAL_Delay(500);
    currentConnectionId_ = 0;
}

bool EspServer::sendCommand(const char *command, const char *expectedResponse) {
    uint8_t *cmd = reinterpret_cast<uint8_t *>(const_cast<char *>(command));
    HAL_UARTEx_ReceiveToIdle_DMA(huart_, readBufferInt(), readBufferSize_);
    HAL_UART_Transmit(huart_, cmd, strlen(command), sendTimeout_);

    bool result = true;
    if (expectedResponse) {
        bool result = waitForUartMsg(expectedResponse);
    }

    cleanBuffers();
    return result;
}

bool EspServer::waitForUartMsg(const char *msg) {
    HAL_UARTEx_ReceiveToIdle_DMA(huart_, readBufferInt(), readBufferSize_);

    uint32_t approximateTime = 0;
    const uint8_t delay = 10;

    bool result = true;
    while (!strstr(readBufferChar(), msg)) {
        HAL_Delay(delay);
        approximateTime += delay;
        if (approximateTime > readTimeout_) {
            result = false;
            copyError();
            break;
        }

        auto wasUartDataReceived = readBuffer_[0] != 0;
        if (wasUartDataReceived) {
            HAL_UARTEx_ReceiveToIdle_DMA(huart_, readBufferInt(), readBufferSize_);
        }
    }
    return result;
}

char *EspServer::getLastError() { return lastError_; }

void EspServer::copyError() {
    for (uint32_t i = 0; readBuffer_[i] != 0 && i < readBufferSize_; i++) {
        lastError_[i] = readBuffer_[i];
    }
}

void EspServer::cleanBuffers() {
    for (uint32_t i = 0; readBuffer_[i] != 0 && i < readBufferSize_; i++) {
        readBuffer_[i] = 0;
    }

    for (uint32_t i = 0; lastError_[i] != 0 && i < errorBufferSize_; i++) {
        lastError_[i] = 0;
    }
}