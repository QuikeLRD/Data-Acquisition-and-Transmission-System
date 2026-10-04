#include "gateway_frame.h"
#include <string.h>

uint16_t GatewayFrame_Build(const uint8_t *payload, uint16_t payload_len, uint8_t *frame_out) {
    if (payload_len == 0 || payload_len > GATEWAY_FRAME_MAX_PAYLOAD) {
        return 0;
    }

    frame_out[0] = GATEWAY_FRAME_SOF;
    frame_out[1] = (uint8_t)payload_len;
    memcpy(&frame_out[GATEWAY_FRAME_HEADER_SIZE], payload, payload_len);

    return (uint16_t)(GATEWAY_FRAME_HEADER_SIZE + payload_len);
}
