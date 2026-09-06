#include "crypto.h"

/**
  * @brief  Pad a buffer in place with PKCS#7 padding up to the next
  *         16-byte boundary. Pure memory operation - no encryption is
  *         performed here.
  * @param  buffer Buffer holding data_len bytes of plaintext; padding
  *         bytes are appended starting at buffer[data_len]
  * @param  data_len Number of plaintext bytes already in buffer
  * @param  buffer_capacity Total capacity of buffer, so padding never
  *         writes past its end
  * @retval Total length after padding, or the original data_len if there
  *         is not enough room in buffer to pad safely
  */
uint16_t AES_PadPKCS7(uint8_t *buffer, uint16_t data_len, uint16_t buffer_capacity) {
    uint8_t padding_val = 16 - (data_len % 16);
    uint16_t padded_len = data_len + padding_val;

    if (padded_len > buffer_capacity) {
        return data_len;
    }

    for (int i = 0; i < padding_val; i++) {
        buffer[data_len + i] = padding_val;
    }

    return padded_len;
}

/**
  * @brief  Encrypt a PKCS#7-padded plaintext block using the hardware
  *         AES-128 engine (SAES). Pure crypto operation - no formatting
  *         or padding is done here.
  * @param  hcryp CRYP handle for the hardware AES engine
  * @param  key 128-bit (16 byte) AES key
  * @param  input Padded plaintext; length must be a multiple of 16 bytes
  * @param  length Length of input/output in bytes
  * @param  output Destination buffer for the ciphertext, at least length bytes
  * @param  timeout HAL timeout in milliseconds for the encryption operation
  * @retval HAL_OK on success, a HAL error status otherwise
  */
HAL_StatusTypeDef AES_EncryptBlock(CRYP_HandleTypeDef *hcryp, const uint8_t *key,
                                    const uint8_t *input, uint16_t length,
                                    uint8_t *output, uint32_t timeout) {
    hcryp->Init.pKey = (uint32_t*)key;
    HAL_CRYP_Init(hcryp);

    return HAL_CRYP_Encrypt(hcryp, (uint32_t*)input, length, (uint32_t*)output, timeout);
}
