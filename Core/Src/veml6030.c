#include "veml6030.h"

/* Internal Command Registers */
#define VEML6030_REG_ALS_CONF   0x00
#define VEML6030_REG_ALS_DATA   0x04

/* Configuration Values */
#define VEML6030_CONF_POWER_ON  0x0000 // Setting bit 0 to '0' turns the sensor ON
#define VEML6030_CONF_SHUTDOWN  0x0001 // Setting bit 0 to '1' shuts it down

/* ---------------- PRIVATE HELPER FUNCTIONS ---------------- */

/**
  * @brief  Validate that a device is present and responding at the
  *         VEML6030 I2C address before any configuration is attempted.
  *         The VEML6030 has no WHO_AM_I register, so presence is checked
  *         with an I2C address probe instead of an ID comparison.
  * @param  hi2c I2C handle used to reach the sensor
  * @retval VEML_OK if the device acknowledges, VEML_ERR_I2C otherwise
  */
static VEML6030_Status_t VEML6030_ValidateDevice(I2C_HandleTypeDef *hi2c) {
    if (HAL_I2C_IsDeviceReady(hi2c, VEML6030_I2C_ADDR, 3, 100) != HAL_OK) {
        return VEML_ERR_I2C;
    }
    return VEML_OK;
}

/**
  * @brief  Write the default configuration to power on the sensor.
  *         Pure I2C write, no validation performed here.
  * @param  hi2c I2C handle used to reach the sensor
  * @retval VEML_OK on success, VEML_ERR_I2C if the I2C write fails
  */
static VEML6030_Status_t VEML6030_PowerOn(I2C_HandleTypeDef *hi2c) {
    uint8_t config_buffer[2];

    // VEML6030 expects LSB first, then MSB
    config_buffer[0] = (uint8_t)(VEML6030_CONF_POWER_ON & 0xFF);        // LSB
    config_buffer[1] = (uint8_t)((VEML6030_CONF_POWER_ON >> 8) & 0xFF); // MSB

    // Write 2 bytes to the Configuration Register (0x00)
    if (HAL_I2C_Mem_Write(hi2c, VEML6030_I2C_ADDR, VEML6030_REG_ALS_CONF,
                          I2C_MEMADD_SIZE_8BIT, config_buffer, 2, 100) != HAL_OK) {
        return VEML_ERR_I2C;
    }

    return VEML_OK;
}

/**
  * @brief  Read the raw 2-byte ambient light sample over I2C.
  *         Pure bus transaction, no byte assembly performed here.
  * @param  hi2c I2C handle used to reach the sensor
  * @param  raw_buffer Output: 2 raw bytes, LSB first then MSB
  * @retval VEML_OK on success, VEML_ERR_I2C if the I2C read fails
  */
static VEML6030_Status_t VEML6030_ReadRaw(I2C_HandleTypeDef *hi2c, uint8_t *raw_buffer) {
    if (HAL_I2C_Mem_Read(hi2c, VEML6030_I2C_ADDR, VEML6030_REG_ALS_DATA,
                         I2C_MEMADD_SIZE_8BIT, raw_buffer, 2, 100) != HAL_OK) {
        return VEML_ERR_I2C;
    }
    return VEML_OK;
}

/**
  * @brief  Assemble the two raw bytes from VEML6030_ReadRaw() into the
  *         16-bit ambient light count. Pure bit manipulation, no I2C access.
  * @param  raw_buffer 2 raw bytes, LSB first then MSB
  * @retval 16-bit ambient light raw count
  */
static uint16_t VEML6030_AssembleRawValue(const uint8_t *raw_buffer) {
    return (uint16_t)(raw_buffer[1] << 8) | raw_buffer[0];
}

/* ---------------- PUBLIC FUNCTIONS ---------------- */

/**
  * @brief  Validate the sensor is present, then power it on with the
  *         default configuration.
  * @param  hi2c I2C handle used to reach the sensor
  * @retval VEML_OK on success, VEML_ERR_I2C if validation or the
  *         configuration write fails
  */
VEML6030_Status_t VEML6030_Init(I2C_HandleTypeDef *hi2c) {
    VEML6030_Status_t status;

    // 1. Hardware Validation Block: confirm the sensor answers on the bus
    status = VEML6030_ValidateDevice(hi2c);
    if (status != VEML_OK) {
        // Future Step: Call ErrorLogger_HandleFault() here
        return status;
    }

    // 2. Setup Block: Power on and configure the sensor
    status = VEML6030_PowerOn(hi2c);
    if (status != VEML_OK) {
        // Future Step: Call ErrorLogger_HandleFault() here
        return status;
    }

    // Optional: Add a small HAL_Delay() here if the sensor needs boot time before first read

    return VEML_OK;
}

/**
  * @brief  Read the current ambient light measurement.
  *         Orchestrates the raw I2C read and the byte assembly as two
  *         separate steps.
  * @param  hi2c I2C handle used to reach the sensor
  * @param  data Output: ambient_light is filled on success
  * @retval VEML_OK on success, VEML_ERR_I2C if the I2C read fails
  */
VEML6030_Status_t VEML6030_ReadLight(I2C_HandleTypeDef *hi2c, VEML6030_Data_t *data) {
    uint8_t raw_buffer[2];

    VEML6030_Status_t status = VEML6030_ReadRaw(hi2c, raw_buffer);
    if (status != VEML_OK) {
        return status;
    }

    data->ambient_light = VEML6030_AssembleRawValue(raw_buffer);

    return VEML_OK;
}
