#ifndef LPS22HH_H
#define LPS22HH_H

#include "platform_i2c.h"
#include <stdint.h>

/* LPS22HH I2C Address (true 7-bit address, unshifted) */
#define LPS22HH_I2C_ADDR 0x5D

/* Sensor Data Structure */
typedef struct {
    float pressure_hPa;
} LPS22HH_Data_t;

/* Status Enumeration */
typedef enum {
    LPS_OK = 0,
    LPS_ERR_I2C,
    LPS_ERR_ID
} LPS22HH_Status_t;

/* Public Function Prototypes */
LPS22HH_Status_t LPS22HH_Init(Platform_I2CBus_t bus);
LPS22HH_Status_t LPS22HH_ReadPressure(Platform_I2CBus_t bus, LPS22HH_Data_t *data);

#endif /* LPS22HH_H */
