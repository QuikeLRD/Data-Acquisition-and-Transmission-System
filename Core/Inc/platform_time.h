#ifndef PLATFORM_TIME_H
#define PLATFORM_TIME_H

#include <stdint.h>

/**
  * @brief  Block for the given number of milliseconds.
  * @param  ms Milliseconds to wait
  * @retval None
  */
void Platform_Delay_ms(uint32_t ms);

#endif /* PLATFORM_TIME_H */
