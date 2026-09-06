#ifndef COMMS_H
#define COMMS_H

#include "stm32u5xx_hal.h"

/* REQ-008: maximum threshold of exactly 3 retry attempts per failed packet */
#define COMMS_MAX_RETRY_ATTEMPTS 3

/* Public Function Prototypes */
HAL_StatusTypeDef Comms_TransmitWithRetry(UART_HandleTypeDef *huart, uint8_t *data,
                                           uint16_t len, uint32_t timeout);

#endif /* COMMS_H */
