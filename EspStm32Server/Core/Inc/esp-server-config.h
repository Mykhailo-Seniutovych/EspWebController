#ifndef __ESP_SERVER_CONFIG_H
#define __ESP_SERVER_CONFIG_H

#include "stm32f1xx_hal.h"

struct EspServerConfig {
    uint16_t port;
    char* wifiSsid;
    char* wifiPassword;
};

#endif

