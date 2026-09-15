#ifndef PLATFORM_CRYPTO_H
#define PLATFORM_CRYPTO_H

#include "platform_types.h"

/* AES-128 works on whole 16-byte blocks: the input length passed to
   Platform_AES_Encrypt must already be padded to a multiple of this. */
#define PLATFORM_AES_BLOCK_SIZE  16
#define PLATFORM_AES_KEY_SIZE    16

/**
  * @brief  Encrypt a padded plaintext buffer with AES-128.
  *         Whether this runs on a hardware crypto engine or in software is
  *         the platform implementation's concern, not the caller's.
  * @param  key 16-byte AES-128 key
  * @param  input Padded plaintext; len must be a multiple of PLATFORM_AES_BLOCK_SIZE
  * @param  len Length of input/output in bytes
  * @param  output Destination buffer for the ciphertext, at least len bytes
  * @param  timeout Timeout in milliseconds
  * @retval PLATFORM_OK on success, PLATFORM_ERROR otherwise
  */
Platform_Status_t Platform_AES_Encrypt(const uint8_t *key, uint8_t *input, uint16_t len,
                                        uint8_t *output, uint32_t timeout);

#endif /* PLATFORM_CRYPTO_H */
