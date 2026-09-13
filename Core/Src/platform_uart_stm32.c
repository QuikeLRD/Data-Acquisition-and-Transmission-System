#include "platform_uart.h"
#include "stm32u5xx_hal.h"

/* STM32-specific implementation of platform_uart.h. Porting to another
   vendor means writing platform_uart_<vendor>.c against the same header -
   no application code changes. */

extern UART_HandleTypeDef huart1;

Platform_Status_t Platform_UART_Transmit(uint8_t *data, uint16_t len, uint32_t timeout) {
    if (HAL_UART_Transmit(&huart1, data, len, timeout) != HAL_OK) {
        return PLATFORM_ERROR;
    }
    return PLATFORM_OK;
}
