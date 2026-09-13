#include "platform_crypto.h"
#include "stm32u5xx_hal.h"

/* STM32-specific implementation of platform_crypto.h, backed by the U5's
   hardware AES (SAES) engine. A vendor without a crypto peripheral would
   implement the same header in software instead. */

extern CRYP_HandleTypeDef hcryp;

Platform_Status_t Platform_AES_Encrypt(const uint8_t *key, uint8_t *input, uint16_t len,
                                        uint8_t *output, uint32_t timeout) {
    // The key is loaded per call, so callers are free to rotate keys without
    // the platform layer holding any key material of its own.
    hcryp.Init.pKey = (uint32_t*)key;

    if (HAL_CRYP_Init(&hcryp) != HAL_OK) {
        return PLATFORM_ERROR;
    }

    if (HAL_CRYP_Encrypt(&hcryp, (uint32_t*)input, len, (uint32_t*)output, timeout) != HAL_OK) {
        return PLATFORM_ERROR;
    }

    return PLATFORM_OK;
}
