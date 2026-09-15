#ifndef PLATFORM_INIT_H
#define PLATFORM_INIT_H

/**
  * @brief  Bring up the hardware: core MCU init, power, clocks, pins and every
  *         peripheral the project uses. Must be the first call in main(),
  *         before any other platform_* function is used.
  * @retval None
  */
void Platform_Init(void);

#endif /* PLATFORM_INIT_H */
