#ifndef __ESP_COMMAND_H
#define __ESP_COMMAND_H

#include "stm32f1xx_hal.h"

enum EspCommand {
    RequestLedState = 0,
    GreenLedOn = 1,
    GreenLedOff = 2,
    RedLedOn = 3,
    RedLedOff = 4,
};

#endif