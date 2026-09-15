#include "platform_init.h"
#include "platform_uart.h"
#include "platform_crypto.h"
#include "platform_time.h"
#include "app_init.h"
#include "veml6030.h"
#include "lps22hh.h"
#include <stdbool.h>
#include <stdio.h>

#define APP_CYCLE_PERIOD_MS   1000    // 1 s for testing; REQ-003 requires 60000
#define APP_UART_TIMEOUT_MS   100
#define APP_AES_TIMEOUT_MS    1000
#define APP_LUX_PER_COUNT     0.0576f // VEML6030 resolution at its default gain/integration time

// 128-bit AES Key (16 bytes) - The Raspberry Pi gateway will need this exact key
static const uint8_t aes_key[PLATFORM_AES_KEY_SIZE] = {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
                                                       0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};

static char uart_buf[128];

static void App_Run(void);
static bool App_ReadSensors(VEML6030_Data_t *light, LPS22HH_Data_t *pressure);
static void App_TransmitReading(int lux, int hpa);
static uint16_t Format_And_Encrypt_Data(int lux, int hpa, uint8_t *output_buffer);
static void App_ReportSensorFailure(void);

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
        App_Run();
    }
}

/**
  * @brief  Run one acquisition/transmission cycle: read both sensors, send the
  *         encrypted reading (or a failure message), then wait for the next cycle.
  * @retval None
  */
static void App_Run(void) {
    VEML6030_Data_t light;
    LPS22HH_Data_t pressure;

    if (App_ReadSensors(&light, &pressure)) {
        int lux = (int)(light.ambient_light * APP_LUX_PER_COUNT);
        int hpa = (int)pressure.pressure_hPa;
        App_TransmitReading(lux, hpa);
    } else {
        App_ReportSensorFailure();
    }

    Platform_Delay_ms(APP_CYCLE_PERIOD_MS);
}

/**
  * @brief  Read both environmental sensors.
  * @param  light Output: latest ambient light reading
  * @param  pressure Output: latest pressure reading
  * @retval true if both sensors were read successfully, false otherwise
  */
static bool App_ReadSensors(VEML6030_Data_t *light, LPS22HH_Data_t *pressure) {
    // Both reads always run, so one failing sensor never skips the other.
    VEML6030_Status_t light_status = VEML6030_ReadLight(APP_SENSOR_BUS, light);
    LPS22HH_Status_t pressure_status = LPS22HH_ReadPressure(APP_SENSOR_BUS, pressure);

    return (light_status == VEML_OK) && (pressure_status == LPS_OK);
}

/**
  * @brief  Encrypt a reading and send it over UART (REQ-009).
  * @param  lux Ambient light reading in lux
  * @param  hpa Pressure reading in hPa
  * @retval None
  */
static void App_TransmitReading(int lux, int hpa) {
    uint16_t tx_len = Format_And_Encrypt_Data(lux, hpa, (uint8_t*)uart_buf);
    Platform_UART_Transmit((uint8_t*)uart_buf, tx_len, APP_UART_TIMEOUT_MS);
}

/**
  * @brief  Format a lux/pressure reading into a plaintext string, pad it
  *         to a 16-byte boundary with PKCS#7, and encrypt it with AES-128.
  * @param  lux Ambient light reading in lux
  * @param  hpa Pressure reading in hPa
  * @param  output_buffer Destination buffer for the ciphertext
  * @retval Length of the ciphertext in bytes (always a multiple of 16)
  */
static uint16_t Format_And_Encrypt_Data(int lux, int hpa, uint8_t *output_buffer) {
    char temp_str[64];

    // 1. Format the raw plaintext string
    int raw_len = sprintf(temp_str, "Lux:%d,hPa:%d", lux, hpa);

    // 2. PKCS#7 Padding to hit a 16-byte multiple
    uint8_t padding_val = PLATFORM_AES_BLOCK_SIZE - (raw_len % PLATFORM_AES_BLOCK_SIZE);
    uint16_t padded_len = raw_len + padding_val;

    for (int i = 0; i < padding_val; i++) {
        temp_str[raw_len + i] = padding_val;
    }

    // 3. AES-128 Execution via the platform layer
    Platform_AES_Encrypt(aes_key, (uint8_t*)temp_str, padded_len, output_buffer, APP_AES_TIMEOUT_MS);

    return padded_len;
}

/**
  * @brief  Send a plain-text sensor failure message over UART.
  * @retval None
  */
static void App_ReportSensorFailure(void) {
    int len = sprintf(uart_buf, "Error: Sensor Comm Failure\r\n");
    Platform_UART_Transmit((uint8_t*)uart_buf, len, APP_UART_TIMEOUT_MS);
}
