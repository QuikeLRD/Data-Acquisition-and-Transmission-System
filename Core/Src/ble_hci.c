#include "ble_hci.h"

void BLE_Hardware_Reset(void) {
    // 1. Ensure Chip Select is HIGH (Inactive) before reset
    HAL_GPIO_WritePin(BLE_CS_PORT, BLE_CS_PIN, GPIO_PIN_SET);

    // 2. Pull Reset LOW to turn off the BLE module
    HAL_GPIO_WritePin(BLE_RST_PORT, BLE_RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(10); // Hold it in reset for 10ms

    // 3. Pull Reset HIGH to boot the BLE module
    HAL_GPIO_WritePin(BLE_RST_PORT, BLE_RST_PIN, GPIO_PIN_SET);

    // 4. Wait for the module's internal firmware to initialize
    HAL_Delay(50);
}

uint8_t BLE_Send_HCI_Reset(void) {
    // Construct the HCI Reset Packet: [Type], [Opcode LSB], [Opcode MSB], [Length]
    uint8_t reset_cmd[4] = {0x01, 0x03, 0x0C, 0x00};

    // 1. Pull Chip Select LOW to wake the module and start SPI communication
    HAL_GPIO_WritePin(BLE_CS_PORT, BLE_CS_PIN, GPIO_PIN_RESET);
    HAL_Delay(2); // Short delay to allow the module to wake up

    // 2. Transmit the 4 bytes over SPI2
    HAL_SPI_Transmit(&hspi2, reset_cmd, 4, 100);

    // 3. Pull Chip Select HIGH to end the transmission
    HAL_GPIO_WritePin(BLE_CS_PORT, BLE_CS_PIN, GPIO_PIN_SET);

    // 4. Wait for the BLE module to pull the INT pin HIGH (indicating it replied)
    for (int i = 0; i < 100; i++) {
        if (HAL_GPIO_ReadPin(BLE_INT_PORT, BLE_INT_PIN) == GPIO_PIN_SET) {
            return 1; // Success! The BLE module acknowledged the command
        }
        HAL_Delay(1);
    }

    return 0; // Failure/Timeout: No response from the module
}
