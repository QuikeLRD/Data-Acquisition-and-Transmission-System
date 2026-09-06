#include "error_handler.h"
#include <string.h>

/* Local diagnostic log: fixed-size circular buffer of recent failure codes */
#define ERROR_LOG_CAPACITY 16

static ErrorCode_t error_log[ERROR_LOG_CAPACITY];
static uint16_t error_log_head = 0;
static uint32_t error_failure_count = 0;

/* ---------------- PRIVATE HELPER FUNCTIONS ---------------- */

/**
  * @brief  Map a failure code to its human-readable UART message.
  *         Pure lookup, no I/O and no logging performed here.
  * @param  code Failure code to describe
  * @retval Null-terminated message text, including trailing "\r\n"
  */
static const char* ErrorHandler_MessageForCode(ErrorCode_t code) {
    switch (code) {
        case ERR_SENSOR_COMM_FAILURE:
            return "Error: Sensor Comm Failure\r\n";
        case ERR_UART_TX_FAILURE:
            return "Error: UART Transmission Failure\r\n";
        default:
            return "Error: Unknown Failure\r\n";
    }
}

/**
  * @brief  Transmit a failure message over UART. Pure I/O, no logging.
  * @param  huart UART handle to transmit on
  * @param  message Null-terminated message text to send
  * @retval None
  */
static void ErrorHandler_Send(UART_HandleTypeDef *huart, const char *message) {
    HAL_UART_Transmit(huart, (uint8_t*)message, (uint16_t)strlen(message), 100);
}

/**
  * @brief  Record a failure code into the local diagnostic log.
  *         Pure bookkeeping, no I/O performed here.
  * @param  code Failure code to record
  * @retval None
  */
static void ErrorHandler_Log(ErrorCode_t code) {
    error_log[error_log_head] = code;
    error_log_head = (error_log_head + 1) % ERROR_LOG_CAPACITY;
    error_failure_count++;
}

/* ---------------- PUBLIC FUNCTIONS ---------------- */

/**
  * @brief  Report a failure: send it over UART and record it in the local
  *         diagnostic log. Single entry point joining the two small blocks
  *         above so callers only need one call for both effects.
  * @param  huart UART handle to send the message on
  * @param  code Failure code being reported
  * @retval None
  */
void ErrorHandler_Report(UART_HandleTypeDef *huart, ErrorCode_t code) {
    const char *message = ErrorHandler_MessageForCode(code);

    ErrorHandler_Send(huart, message);
    ErrorHandler_Log(code);
}

/**
  * @brief  Get the total number of failures recorded since boot.
  * @retval Total failure count
  */
uint32_t ErrorHandler_GetFailureCount(void) {
    return error_failure_count;
}
