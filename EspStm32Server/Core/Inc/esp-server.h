#ifndef __ESP_SERVER_H
#define __ESP_SERVER_H

#include "esp-command.h"
#include "esp-response.h"
#include "esp-server-config.h"
#include "stm32f1xx_hal.h"

class EspServer {
  public:
    EspServer(UART_HandleTypeDef *huart, EspServerConfig *config);
    ~EspServer();

    bool initializeTcpServer();
    EspCommand waitForCommand();
    void sendTcpResponse(EspResponse response);
    void sendTcpResponse(const char *response);
    char *getLastError();

  private:
    const uint32_t sendTimeout_ = 1000;
    const uint32_t readTimeout_ = 10000;

    const uint32_t readBufferSize_ = 1000;
    volatile char *readBuffer_;

    const uint32_t errorBufferSize_ = 100;
    char *lastError_;
    char currentConnectionId_;

    UART_HandleTypeDef *huart_;
    EspServerConfig *config_;

    bool sendCommand(const char *command, const char *expectedResponse);
    bool waitForUartMsg(const char *msg);
    void cleanBuffers();
    void copyError();

    inline char *readBufferChar() const {
        return const_cast<char *>(readBuffer_);
    }

    inline uint8_t *readBufferInt() const {
        return reinterpret_cast<uint8_t *>(const_cast<char *>(readBuffer_));
    }
};

#endif