#ifndef GATEWAY_FRAME_H
#define GATEWAY_FRAME_H

#include <stdint.h>

/* Wire format of one message to the gateway:
 *
 *   [ SOF ][ LEN ][ payload (LEN bytes) ]
 *
 * SOF marks where a frame starts so the receiver can resynchronize after a
 * lost byte; LEN tells it how many payload bytes follow. The payload is the
 * AES ciphertext, so it can contain any byte value, including SOF. The
 * receiver must therefore resync by discarding bytes until SOF, reading LEN,
 * and only trusting the frame if the payload decrypts and unpads correctly. */
#define GATEWAY_FRAME_SOF           0xA5
#define GATEWAY_FRAME_HEADER_SIZE   2
#define GATEWAY_FRAME_MAX_PAYLOAD   64
#define GATEWAY_FRAME_MAX_SIZE      (GATEWAY_FRAME_HEADER_SIZE + GATEWAY_FRAME_MAX_PAYLOAD)

/**
  * @brief  Wrap a payload in a gateway frame (header + payload).
  * @param  payload Bytes to send
  * @param  payload_len Number of payload bytes (1..GATEWAY_FRAME_MAX_PAYLOAD)
  * @param  frame_out Destination buffer, at least GATEWAY_FRAME_MAX_SIZE bytes
  * @retval Total frame length in bytes, or 0 if the payload is empty or too long
  */
uint16_t GatewayFrame_Build(const uint8_t *payload, uint16_t payload_len, uint8_t *frame_out);

#endif /* GATEWAY_FRAME_H */
