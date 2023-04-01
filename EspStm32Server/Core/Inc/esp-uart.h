#ifndef __ESP_UART_H
#define __ESP_UART_H

#include "stm32f1xx_hal.h"

class EspUart {
  public:
    EspUart(UART_HandleTypeDef *huart);
    ~EspUart();

    bool sendCommand(const char *command, const char *expectedResponse);
    char *getLastError();

  private:
    const uint32_t sendTimeout_ = 1000;
    const uint32_t readTimeout_ = 10000;

    const uint32_t readBufferSize_ = 1000;
    volatile char *readBuffer_;

    const uint32_t errorBufferSize_ = 100;
    char *lastError_;

    UART_HandleTypeDef *huart_;

    void cleanBuffers();
    void copyError();
};

#endif