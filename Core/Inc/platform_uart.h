#ifndef PLATFORM_UART_H
#define PLATFORM_UART_H

#include "platform_types.h"

/**
  * @brief  Transmit a buffer over the project's data/debug serial port.
  *         The application does not choose which physical UART this is -
  *         that mapping lives in the platform implementation.
  * @param  data Bytes to transmit
  * @param  len Number of bytes to transmit
  * @param  timeout Timeout in milliseconds
  * @retval PLATFORM_OK on success, PLATFORM_ERROR otherwise
  */
Platform_Status_t Platform_UART_Transmit(uint8_t *data, uint16_t len, uint32_t timeout);

#endif /* PLATFORM_UART_H */
