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

static LPS22HH_Status_t LPS22HH_CheckID(I2C_HandleTypeDef *hi2c) {
    uint8_t id = 0;
    if (HAL_I2C_Mem_Read(hi2c, LPS22HH_I2C_ADDR, LPS22HH_REG_WHO_AM_I,
                         I2C_MEMADD_SIZE_8BIT, &id, 1, 100) != HAL_OK) {
        return LPS_ERR_I2C;
    }
    if (id != LPS22HH_EXPECTED_ID) return LPS_ERR_ID;
    return LPS_OK;
}

/* ---------------- PUBLIC FUNCTIONS ---------------- */

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

LPS22HH_Status_t LPS22HH_ReadPressure(I2C_HandleTypeDef *hi2c, LPS22HH_Data_t *data) {
    uint8_t buffer[3];

    // Read 3 consecutive bytes starting from PRESS_OUT_XL (0x28)
    if (HAL_I2C_Mem_Read(hi2c, LPS22HH_I2C_ADDR, LPS22HH_REG_PRESS_OUT,
                         I2C_MEMADD_SIZE_8BIT, buffer, 3, 100) != HAL_OK) {
        return LPS_ERR_I2C;
    }

    // Process 24-bit data: LSB first, then Mid, then MSB
    int32_t raw_pressure = (int32_t)((buffer[2] << 16) | (buffer[1] << 8) | buffer[0]);

    // Sign extension: If the 24th bit is a '1', it is a negative number, fill top 8 bits with 1s
    if (raw_pressure & 0x00800000) {
        raw_pressure |= 0xFF000000;
    }

    // Divide by 4096.0 to get the exact value in hPa
    data->pressure_hPa = (float)raw_pressure / 4096.0f;

    return LPS_OK;
}
