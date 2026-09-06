#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include "stm32u5xx_hal.h"
#include <stdint.h>

/* Known Failure Codes */
typedef enum {
    ERR_SENSOR_COMM_FAILURE = 0,
    ERR_UART_TX_FAILURE,
} ErrorCode_t;

/* Public Function Prototypes */
void ErrorHandler_Report(UART_HandleTypeDef *huart, ErrorCode_t code);
uint32_t ErrorHandler_GetFailureCount(void);

#endif /* ERROR_HANDLER_H */
