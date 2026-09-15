#ifndef APP_INIT_H
#define APP_INIT_H

#include "platform_i2c.h"

/* The I2C bus both environmental sensors are wired to on this board. */
#define APP_SENSOR_BUS  PLATFORM_I2C_BUS_2

typedef enum {
    APP_INIT_OK = 0,
    APP_INIT_ERROR
} App_InitStatus_t;

/**
  * @brief  Initialize the application-level resources: the light and pressure
  *         sensors. Vendor-agnostic; call it after Platform_Init().
  * @retval APP_INIT_OK if every sensor initialized, APP_INIT_ERROR otherwise
  */
App_InitStatus_t App_Init(void);

#endif /* APP_INIT_H */
