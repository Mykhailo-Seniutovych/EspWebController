#include "esp-uart.h"
#include "stm32f1xx_hal.h"
#include <string.h>

EspUart::EspUart(UART_HandleTypeDef *huart) : huart_(huart) {
    readBuffer_ = new char[readBufferSize_];
    lastError_ = new char[errorBufferSize_];
}

EspUart::~EspUart() {
    delete[] readBuffer_;
    delete[] lastError_;
}

bool EspUart::sendCommand(const char *command, const char *expectedResponse) {
    uint8_t *cmd = reinterpret_cast<uint8_t *>(const_cast<char *>(command));

    auto buffer = reinterpret_cast<uint8_t *>(const_cast<char *>(readBuffer_));
    HAL_UARTEx_ReceiveToIdle_DMA(huart_, buffer, 1000);
    HAL_UART_Transmit(huart_, cmd, strlen(command), sendTimeout_);

    uint32_t approximateTime = 0;
    const uint8_t delay = 10;

    bool result = true;
    while (!strstr(const_cast<char *>(readBuffer_), expectedResponse)) {
        HAL_Delay(delay);
        approximateTime += delay;
        if (approximateTime > readTimeout_) {
            result = false;
            copyError();
            break;
        }
    }

    cleanBuffers();
    return result;
}

char *EspUart::getLastError() {
    return lastError_;
}

void EspUart::copyError() {
    for (uint32_t i = 0; readBuffer_[i] != 0 && i < readBufferSize_; i++) {
        lastError_[i] = readBuffer_[i];
    }
}

void EspUart::cleanBuffers() {
    for (uint32_t i = 0; i < readBufferSize_; i++) {
        readBuffer_[i] = 0;
    }

    for (uint32_t i = 0; i < errorBufferSize_; i++) {
        lastError_[i] = 0;
    }
}