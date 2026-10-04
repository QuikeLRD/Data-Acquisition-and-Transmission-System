#ifndef PLATFORM_UART_H
#define PLATFORM_UART_H

#include "platform_types.h"

/* The serial links the application can talk to. The application names the
   purpose of the link; which physical UART and pins implement it lives in the
   platform implementation. */
typedef enum {
    PLATFORM_UART_PC = 0,   /* Debug / monitor link to the host PC */
    PLATFORM_UART_GATEWAY   /* Link to the gateway (Raspberry Pi) */
} Platform_UARTPort_t;

/**
  * @brief  Transmit a buffer over one of the project's serial ports.
  * @param  port Which link to send on (PLATFORM_UART_PC or PLATFORM_UART_GATEWAY)
  * @param  data Bytes to transmit
  * @param  len Number of bytes to transmit
  * @param  timeout Timeout in milliseconds
  * @retval PLATFORM_OK on success, PLATFORM_ERROR otherwise (including an
  *         unknown port)
  */
Platform_Status_t Platform_UART_Transmit(Platform_UARTPort_t port, uint8_t *data, uint16_t len, uint32_t timeout);

#endif /* PLATFORM_UART_H */
