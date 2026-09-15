#include "platform_init.h"
#include "platform_uart.h"
#include "platform_crypto.h"
#include "platform_time.h"
#include "app_init.h"
#include "veml6030.h"
#include "lps22hh.h"
#include <stdio.h>

VEML6030_Data_t light_data;
VEML6030_Status_t light_status;

LPS22HH_Data_t pressure_data;
LPS22HH_Status_t pressure_status;

char uart_buf[128];

uint16_t Format_And_Encrypt_Data(int lux, int hpa, uint8_t *output_buffer);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    Platform_Init();
    App_Init();

    while (1)
    {
        light_status = VEML6030_ReadLight(APP_SENSOR_BUS, &light_data);
        pressure_status = LPS22HH_ReadPressure(APP_SENSOR_BUS, &pressure_data);

        if (light_status == VEML_OK && pressure_status == LPS_OK) {
            int lux = (int)(light_data.ambient_light * 0.0576);
            int hpa = (int)pressure_data.pressure_hPa;

            // 1. Format and Encrypt (REQ-009)
            uint16_t tx_len = Format_And_Encrypt_Data(lux, hpa, (uint8_t*)uart_buf);

            // 2. Transmit the cipher text
            Platform_UART_Transmit((uint8_t*)uart_buf, tx_len, 100);
        } else {
            int len = sprintf(uart_buf, "Error: Sensor Comm Failure\r\n");
            Platform_UART_Transmit((uint8_t*)uart_buf, len, 100);
        }

        Platform_Delay_ms(1000); // 1-second delay for testing (change to 60000 for REQ-003 later)
    }
}

// 128-bit AES Key (16 bytes) - The Raspberry Pi gateway will need this exact key
static const uint8_t aes_key[16] = {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
                                    0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};

/**
  * @brief  Format a lux/pressure reading into a plaintext string, pad it
  *         to a 16-byte boundary with PKCS#7, and encrypt it in place
  *         using the hardware AES-128 engine (SAES).
  * @param  lux Ambient light reading in lux
  * @param  hpa Pressure reading in hPa
  * @param  output_buffer Destination buffer for the ciphertext
  * @retval Length of the ciphertext in bytes (always a multiple of 16)
  */
uint16_t Format_And_Encrypt_Data(int lux, int hpa, uint8_t *output_buffer) {
    char temp_str[64];

    // 1. Format the raw plaintext string
    int raw_len = sprintf(temp_str, "Lux:%d,hPa:%d", lux, hpa);

    // 2. PKCS#7 Padding to hit a 16-byte multiple
    uint8_t padding_val = 16 - (raw_len % 16);
    uint16_t padded_len = raw_len + padding_val;

    for (int i = 0; i < padding_val; i++) {
        temp_str[raw_len + i] = padding_val;
    }

    // 3. AES-128 Execution via the platform layer
    Platform_AES_Encrypt(aes_key, (uint8_t*)temp_str, padded_len, output_buffer, 1000);

    return padded_len;
}
