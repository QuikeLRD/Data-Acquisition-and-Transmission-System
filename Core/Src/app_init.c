#include "app_init.h"
#include "veml6030.h"
#include "lps22hh.h"

App_InitStatus_t App_Init(void) {
    // Both sensors are initialized unconditionally, so one failing does not
    // leave the other unconfigured.
    VEML6030_Status_t light_status = VEML6030_Init(APP_SENSOR_BUS);
    LPS22HH_Status_t pressure_status = LPS22HH_Init(APP_SENSOR_BUS);

    if (light_status != VEML_OK || pressure_status != LPS_OK) {
        return APP_INIT_ERROR;
    }
    return APP_INIT_OK;
}
