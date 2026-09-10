#ifndef VEML6030_H
#define VEML6030_H

#include "platform_i2c.h"
#include <stdint.h>

/* VEML6030 I2C Address (true 7-bit address, unshifted) */
#define VEML6030_I2C_ADDR  0x10

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
VEML6030_Status_t VEML6030_Init(Platform_I2CBus_t bus);
VEML6030_Status_t VEML6030_ReadLight(Platform_I2CBus_t bus, VEML6030_Data_t *data);

#endif /* VEML6030_H */
