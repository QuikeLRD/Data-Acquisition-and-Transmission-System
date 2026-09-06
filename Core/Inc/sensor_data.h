#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <stdint.h>

/* Public Function Prototypes */
uint16_t SensorData_Serialize(int lux, int hpa, char *output_buffer, uint16_t buffer_size);

#endif /* SENSOR_DATA_H */
