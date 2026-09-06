#ifndef CRYPTO_H
#define CRYPTO_H

#include "stm32u5xx_hal.h"

/* Public Function Prototypes */
uint16_t AES_PadPKCS7(uint8_t *buffer, uint16_t data_len, uint16_t buffer_capacity);
HAL_StatusTypeDef AES_EncryptBlock(CRYP_HandleTypeDef *hcryp, const uint8_t *key,
                                    const uint8_t *input, uint16_t length,
                                    uint8_t *output, uint32_t timeout);

#endif /* CRYPTO_H */
