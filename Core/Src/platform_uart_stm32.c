#include "platform_uart.h"
#include "stm32u5xx_hal.h"

/* STM32-specific implementation of platform_uart.h. Porting to another
   vendor means writing platform_uart_<vendor>.c against the same header -
   no application code changes. */

extern UART_HandleTypeDef huart1;   /* ST-LINK virtual COM port (PA9/PA10) */
extern UART_HandleTypeDef huart3;   /* Arduino D1/D0 header (PD8/PD9)      */

static UART_HandleTypeDef* Platform_UART_GetHandle(Platform_UARTPort_t port) {
    switch (port) {
        case PLATFORM_UART_PC:      return &huart1;
        case PLATFORM_UART_GATEWAY: return &huart3;
        default:                    return NULL;
    }
}

Platform_Status_t Platform_UART_Transmit(Platform_UARTPort_t port, uint8_t *data, uint16_t len, uint32_t timeout) {
    UART_HandleTypeDef *handle = Platform_UART_GetHandle(port);
    if (handle == NULL) {
        return PLATFORM_ERROR;
    }
    if (HAL_UART_Transmit(handle, data, len, timeout) != HAL_OK) {
        return PLATFORM_ERROR;
    }
    return PLATFORM_OK;
}
