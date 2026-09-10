#ifndef PLATFORM_I2C_H
#define PLATFORM_I2C_H

#include <stdint.h>

/* I2C bus identifiers. Each one maps to a real hardware
   peripheral inside the platform-specific implementation (see
   platform_i2c_stm32.c) - no sensor driver should ever need to know
   which physical peripheral, or which vendor's silicon, sits behind it. */
typedef enum {
    PLATFORM_I2C_BUS_1,
    PLATFORM_I2C_BUS_2
} Platform_I2CBus_t;

typedef enum {
    PLATFORM_OK = 0,
    PLATFORM_ERROR
} Platform_Status_t;

/**
  * @brief  Write to a register on an I2C device.
  * @param  bus Which I2C bus to use
  * @param  devAddr 7-bit I2C device address (unshifted)
  * @param  regAddr Register address on the device
  * @param  data Bytes to write
  * @param  len Number of bytes to write
  * @retval PLATFORM_OK on success, PLATFORM_ERROR otherwise
  */
Platform_Status_t Platform_I2C_WriteRegister(Platform_I2CBus_t bus, uint8_t devAddr,
                                              uint8_t regAddr, uint8_t *data, uint16_t len);

/**
  * @brief  Read from a register on an I2C device.
  * @param  bus Which I2C bus to use
  * @param  devAddr 7-bit I2C device address (unshifted)
  * @param  regAddr Register address on the device
  * @param  data Output buffer for the bytes read
  * @param  len Number of bytes to read
  * @retval PLATFORM_OK on success, PLATFORM_ERROR otherwise
  */
Platform_Status_t Platform_I2C_ReadRegister(Platform_I2CBus_t bus, uint8_t devAddr,
                                             uint8_t regAddr, uint8_t *data, uint16_t len);

#endif /* PLATFORM_I2C_H */
