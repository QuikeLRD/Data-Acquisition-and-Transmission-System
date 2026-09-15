#include "platform_time.h"
#include "stm32u5xx_hal.h"

/* STM32-specific implementation of platform_time.h. HAL_Delay is driven by
   the SysTick interrupt configured during HAL_Init(). */

void Platform_Delay_ms(uint32_t ms) {
    HAL_Delay(ms);
}
