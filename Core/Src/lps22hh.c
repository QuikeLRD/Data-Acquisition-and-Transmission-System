/*
 * lps22hh.c
 *
 *  Created on: Aug 17, 2026
 *      Author: enriq
 */

#include "lps22hh.h"

/* Internal Registers */
#define LPS22HH_REG_WHO_AM_I   0x0F
#define LPS22HH_REG_CTRL1      0x10
#define LPS22HH_REG_CTRL2      0x11
#define LPS22HH_REG_PRESS_OUT  0x28  // Start of 3-byte pressure data

#define LPS22HH_EXPECTED_ID    0xB3

/* ---------------- PRIVATE HELPER FUNCTIONS ---------------- */

/**
  * @brief  Validate that the device on the bus is really an LPS22HH by
  *         reading its WHO_AM_I register and comparing against the
  *         expected fixed ID.
  * @param  hi2c I2C handle used to reach the sensor
  * @retval LPS_OK on match, LPS_ERR_I2C on bus failure, LPS_ERR_ID on mismatch
  */
static LPS22HH_Status_t LPS22HH_CheckID(I2C_HandleTypeDef *hi2c) {
    uint8_t id = 0;
    if (HAL_I2C_Mem_Read(hi2c, LPS22HH_I2C_ADDR, LPS22HH_REG_WHO_AM_I,
                         I2C_MEMADD_SIZE_8BIT, &id, 1, 100) != HAL_OK) {
        return LPS_ERR_I2C;
    }
    if (id != LPS22HH_EXPECTED_ID) return LPS_ERR_ID;
    return LPS_OK;
}

/**
  * @brief  Read the raw 24-bit pressure sample from the sensor over I2C.
  *         Performs only the bus transaction, sign extension included;
  *         no engineering-unit conversion is done here.
  * @param  hi2c I2C handle used to reach the sensor
  * @param  raw_pressure Output: sign-extended 24-bit raw reading
  * @retval LPS_OK on success, LPS_ERR_I2C if the I2C read fails
  */
static LPS22HH_Status_t LPS22HH_ReadRaw(I2C_HandleTypeDef *hi2c, int32_t *raw_pressure) {
    uint8_t buffer[3];

    // Read 3 consecutive bytes starting from PRESS_OUT_XL (0x28)
    if (HAL_I2C_Mem_Read(hi2c, LPS22HH_I2C_ADDR, LPS22HH_REG_PRESS_OUT,
                         I2C_MEMADD_SIZE_8BIT, buffer, 3, 100) != HAL_OK) {
        return LPS_ERR_I2C;
    }

    // Assemble 24-bit data: LSB first, then Mid, then MSB
    int32_t value = (int32_t)((buffer[2] << 16) | (buffer[1] << 8) | buffer[0]);

    // Sign extension: If the 24th bit is a '1', it is a negative number, fill top 8 bits with 1s
    if (value & 0x00800000) {
        value |= 0xFF000000;
    }

    *raw_pressure = value;
    return LPS_OK;
}

/**
  * @brief  Convert a raw 24-bit sensor sample into engineering units.
  *         Pure math, no I2C access.
  * @param  raw_pressure Sign-extended 24-bit raw reading from LPS22HH_ReadRaw()
  * @retval Pressure in hPa
  */
static float LPS22HH_ConvertToHPa(int32_t raw_pressure) {
    // Divide by 4096.0 to get the exact value in hPa
    return (float)raw_pressure / 4096.0f;
}

/* ---------------- PUBLIC FUNCTIONS ---------------- */

/**
  * @brief  Validate the sensor is present, then configure it for continuous
  *         pressure sampling: 10Hz output data rate with block data update,
  *         and register auto-increment for multi-byte reads.
  * @param  hi2c I2C handle used to reach the sensor
  * @retval LPS_OK on success, LPS_ERR_ID if device validation fails,
  *         LPS_ERR_I2C if a configuration write fails
  */
LPS22HH_Status_t LPS22HH_Init(I2C_HandleTypeDef *hi2c) {
    if (LPS22HH_CheckID(hi2c) != LPS_OK) {
        return LPS_ERR_ID;
    }

    // CTRL_REG1: Set Output Data Rate to 10Hz (0x20) and enable Block Data Update (0x02)
    uint8_t ctrl1 = 0x22;
    if (HAL_I2C_Mem_Write(hi2c, LPS22HH_I2C_ADDR, LPS22HH_REG_CTRL1,
                          I2C_MEMADD_SIZE_8BIT, &ctrl1, 1, 100) != HAL_OK) return LPS_ERR_I2C;

    // CTRL_REG2: Enable Auto-Increment (0x10) for multi-byte reads
    uint8_t ctrl2 = 0x10;
    if (HAL_I2C_Mem_Write(hi2c, LPS22HH_I2C_ADDR, LPS22HH_REG_CTRL2,
                          I2C_MEMADD_SIZE_8BIT, &ctrl2, 1, 100) != HAL_OK) return LPS_ERR_I2C;

    return LPS_OK;
}

/**
  * @brief  Read the current pressure measurement, converted to hPa.
  *         Orchestrates the raw I2C read and the unit conversion as two
  *         separate steps.
  * @param  hi2c I2C handle used to reach the sensor
  * @param  data Output: pressure_hPa is filled on success
  * @retval LPS_OK on success, LPS_ERR_I2C if the I2C read fails
  */
LPS22HH_Status_t LPS22HH_ReadPressure(I2C_HandleTypeDef *hi2c, LPS22HH_Data_t *data) {
    int32_t raw_pressure;

    LPS22HH_Status_t status = LPS22HH_ReadRaw(hi2c, &raw_pressure);
    if (status != LPS_OK) {
        return status;
    }

    data->pressure_hPa = LPS22HH_ConvertToHPa(raw_pressure);

    return LPS_OK;
}
