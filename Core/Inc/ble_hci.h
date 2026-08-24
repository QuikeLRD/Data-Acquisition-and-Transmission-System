#ifndef BLE_HCI_H
#define BLE_HCI_H

#include "stm32u5xx_hal.h"

/* Reference to the SPI2 handle generated in main.c */
extern SPI_HandleTypeDef hspi2;

/* BLE Hardware Control Pins */
#define BLE_CS_PORT  GPIOI
#define BLE_CS_PIN   GPIO_PIN_0

#define BLE_RST_PORT GPIOF
#define BLE_RST_PIN  GPIO_PIN_14

#define BLE_INT_PORT GPIOE
#define BLE_INT_PIN  GPIO_PIN_10

/* Public Function Prototypes */
void BLE_Hardware_Reset(void);
uint8_t BLE_Send_HCI_Reset(void);

#endif /* BLE_HCI_H */
