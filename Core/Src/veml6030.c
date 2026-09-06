#include "veml6030.h"

/* Internal Command Registers */
#define VEML6030_REG_ALS_CONF   0x00
#define VEML6030_REG_ALS_DATA   0x04

/* Configuration Values */
#define VEML6030_CONF_POWER_ON  0x0000 // Setting bit 0 to '0' turns the sensor ON
#define VEML6030_CONF_SHUTDOWN  0x0001 // Setting bit 0 to '1' shuts it down

/* ---------------- PRIVATE HELPER FUNCTIONS ---------------- */

/**
  * @brief  Write the default configuration to power on the sensor.
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

/* ---------------- PUBLIC FUNCTIONS ---------------- */

/**
  * @brief  Power on and configure the sensor with default settings.
  * @param  hi2c I2C handle used to reach the sensor
  * @retval VEML_OK on success, VEML_ERR_I2C if the configuration write fails
  */
VEML6030_Status_t VEML6030_Init(I2C_HandleTypeDef *hi2c) {
    VEML6030_Status_t status;

    // 1. Setup Block: Power on and configure the sensor
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
  * @param  hi2c I2C handle used to reach the sensor
  * @param  data Output: ambient_light is filled on success
  * @retval VEML_OK on success, VEML_ERR_I2C if the I2C read fails
  */
VEML6030_Status_t VEML6030_ReadLight(I2C_HandleTypeDef *hi2c, VEML6030_Data_t *data) {
    uint8_t read_buffer[2];

    // 1. Read 2 bytes from the ALS Data Register (0x04)
    if (HAL_I2C_Mem_Read(hi2c, VEML6030_I2C_ADDR, VEML6030_REG_ALS_DATA,
                         I2C_MEMADD_SIZE_8BIT, read_buffer, 2, 100) != HAL_OK) {
        return VEML_ERR_I2C;
    }

    // 2. Process bytes into 16-bit integer (LSB is in buffer[0], MSB in buffer[1])
    data->ambient_light = (uint16_t)(read_buffer[1] << 8) | read_buffer[0];

    return VEML_OK;
}
