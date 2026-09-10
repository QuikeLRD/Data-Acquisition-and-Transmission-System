#include "platform_i2c.h"
#include "stm32u5xx_hal.h"

/* This is the ONLY file in the sensor layer allowed to know this project
   is running on an STM32. Porting to a different vendor (e.g. NXP) means
   writing a new platform_i2c_<vendor>.c that implements the same
   platform_i2c.h interface - nothing above this file (veml6030.c,
   lps22hh.c, main.c) ever needs to change. */

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;

/**
  * @brief  Resolve a vendor-agnostic bus identifier to the STM32 HAL
  *         handle that actually implements it.
  * @param  bus Which I2C bus was requested
  * @retval Pointer to the matching HAL I2C handle
  */
static I2C_HandleTypeDef* Platform_I2C_GetHandle(Platform_I2CBus_t bus) {
    return (bus == PLATFORM_I2C_BUS_1) ? &hi2c1 : &hi2c2;
}

Platform_Status_t Platform_I2C_WriteRegister(Platform_I2CBus_t bus, uint8_t devAddr,
                                              uint8_t regAddr, uint8_t *data, uint16_t len) {
    I2C_HandleTypeDef *hi2c = Platform_I2C_GetHandle(bus);

    // The platform interface takes the true 7-bit I2C address; STM32's
    // HAL expects it pre-shifted left by 1 (bit 0 is reserved for the
    // read/write flag), so that translation is this file's job.
    uint16_t shiftedAddr = (uint16_t)(devAddr << 1);

    if (HAL_I2C_Mem_Write(hi2c, shiftedAddr, regAddr, I2C_MEMADD_SIZE_8BIT,
                          data, len, 100) != HAL_OK) {
        return PLATFORM_ERROR;
    }
    return PLATFORM_OK;
}

Platform_Status_t Platform_I2C_ReadRegister(Platform_I2CBus_t bus, uint8_t devAddr,
                                             uint8_t regAddr, uint8_t *data, uint16_t len) {
    I2C_HandleTypeDef *hi2c = Platform_I2C_GetHandle(bus);
    uint16_t shiftedAddr = (uint16_t)(devAddr << 1);

    if (HAL_I2C_Mem_Read(hi2c, shiftedAddr, regAddr, I2C_MEMADD_SIZE_8BIT,
                         data, len, 100) != HAL_OK) {
        return PLATFORM_ERROR;
    }
    return PLATFORM_OK;
}
