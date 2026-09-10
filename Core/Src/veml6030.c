#include <veml6030.h>

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