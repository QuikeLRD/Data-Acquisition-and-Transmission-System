#ifndef VEML6030_H
#define VEML6030_H

#include "stm32u5xx_hal.h"
#include <stdint.h>

/* VEML6030 I2C Address (8-bit shifted format for STM32 HAL) */
#define VEML6030_I2C_ADDR  0x20

/* Sensor Data Structure */
typedef struct {
    uint16_t ambient_light;

} VEML6030_Data_t;

/* Status Enumeration */
typedef enum {
    VEML_OK = 0,
    VEML_ERR_I2C,
    VEML_ERR_CONFIG
} VEML6030_Status_t;

/* Public Function Prototypes */
VEML6030_Status_t VEML6030_Init(I2C_HandleTypeDef *hi2c);
VEML6030_Status_t VEML6030_ReadLight(I2C_HandleTypeDef *hi2c, VEML6030_Data_t *data);

#endif /* VEML6030_H */
