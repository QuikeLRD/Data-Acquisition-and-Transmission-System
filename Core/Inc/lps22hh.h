#ifndef LPS22HH_H
#define LPS22HH_H

#include "stm32u5xx_hal.h"
#include <stdint.h>

/* LPS22HH I2C Address: 0x5D shifted left by 1 */
#define LPS22HH_I2C_ADDR (0x5D << 1)

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
LPS22HH_Status_t LPS22HH_Init(I2C_HandleTypeDef *hi2c);
LPS22HH_Status_t LPS22HH_ReadPressure(I2C_HandleTypeDef *hi2c, LPS22HH_Data_t *data);

#endif /* LPS22HH_H */
