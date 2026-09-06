#include "sensor_data.h"
#include <stdio.h>

/**
  * @brief  Serialize a lux/pressure reading pair into a plaintext string,
  *         e.g. "Lux:123,hPa:1013". Pure formatting only - no padding or
  *         encryption is applied here, so this block can be reused by any
  *         transport (UART, BLE, ...) that needs the same plaintext.
  * @param  lux Ambient light reading in lux
  * @param  hpa Pressure reading in hPa
  * @param  output_buffer Destination buffer for the formatted string
  * @param  buffer_size Size of output_buffer, used to bound the write
  * @retval Number of bytes written, excluding the null terminator
  */
uint16_t SensorData_Serialize(int lux, int hpa, char *output_buffer, uint16_t buffer_size) {
    int len = snprintf(output_buffer, buffer_size, "Lux:%d,hPa:%d", lux, hpa);
    return (uint16_t)len;
}
