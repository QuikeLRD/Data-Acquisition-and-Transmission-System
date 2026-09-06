#include "comms.h"

/**
  * @brief  Transmit a buffer over UART, retrying on failure up to a fixed
  *         maximum (REQ-008: exactly 3 attempts per failed packet).
  *         Single job: the retry loop around the transmit call itself -
  *         no logging or error reporting is done here, that is the
  *         caller's concern.
  * @param  huart UART handle to transmit on
  * @param  data Buffer to transmit
  * @param  len Number of bytes to transmit
  * @param  timeout HAL timeout in milliseconds for each individual attempt
  * @retval HAL_OK if any attempt succeeds, otherwise the HAL error status
  *         from the final attempt
  */
HAL_StatusTypeDef Comms_TransmitWithRetry(UART_HandleTypeDef *huart, uint8_t *data,
                                           uint16_t len, uint32_t timeout) {
    HAL_StatusTypeDef status = HAL_ERROR;

    for (int attempt = 0; attempt < COMMS_MAX_RETRY_ATTEMPTS; attempt++) {
        status = HAL_UART_Transmit(huart, data, len, timeout);
        if (status == HAL_OK) {
            return HAL_OK;
        }
    }

    return status;
}
