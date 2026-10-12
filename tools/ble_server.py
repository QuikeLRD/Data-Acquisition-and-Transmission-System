#!/usr/bin/env python3
"""BLE server for TheBlackData (runs on the Raspberry Pi).

Reads the framed, encrypted sensor readings that the STM32 sends over UART
(see gateway_receiver.py) and publishes them as a BLE peripheral:

    Environmental Sensing Service (0x181A)   "Environmental Sensor Node"
      |- Illuminance (0x2AFB)  Read, Notify   ambient light
      '- Pressure    (0x2A6D)  Read, Notify   atmospheric pressure

These are Bluetooth SIG standard UUIDs, so generic apps (nRF Connect, LightBlue)
decode the values without a custom profile. Value encodings follow the SIG
characteristic definitions:
    Illuminance: uint24, little-endian, resolution 0.01 lux
    Pressure:    uint32, little-endian, resolution 0.1 Pa

Only one program can own the serial port, so do not run gateway_receiver.py at
the same time as this script.

Setup on the Pi (the venv keeps the apt-installed serial/crypto packages):
    sudo apt install -y python3-serial python3-pycryptodome
    python3 -m venv --system-site-packages ~/gw
    ~/gw/bin/pip install bless
Run:
    ~/gw/bin/python ble_server.py
"""

import asyncio
import sys

import serial

# Reuse the frame parser and the reading format from the UART receiver.
from gateway_receiver import BAUD, PORT, READING_RE, extract_frame

DEVICE_NAME = "TheBlackData"

SERVICE_UUID = "0000181a-0000-1000-8000-00805f9b34fb"      # Environmental Sensing
ILLUMINANCE_UUID = "00002afb-0000-1000-8000-00805f9b34fb"  # Illuminance
PRESSURE_UUID = "00002a6d-0000-1000-8000-00805f9b34fb"     # Pressure


def encode_illuminance(lux):
    """Illuminance characteristic: uint24 LE, 0.01 lux per unit."""
    units = max(0, min(int(round(lux * 100)), 0xFFFFFF))
    return bytearray(units.to_bytes(3, "little"))


def encode_pressure(hpa):
    """Pressure characteristic: uint32 LE, 0.1 Pa per unit (1 hPa = 100 Pa)."""
    units = max(0, min(int(round(hpa * 100 * 10)), 0xFFFFFFFF))
    return bytearray(units.to_bytes(4, "little"))


async def read_readings(port, queue):
    """Read UART frames in a worker thread and put (lux, hpa) tuples on queue."""
    loop = asyncio.get_running_loop()
    buf = bytearray()
    with serial.Serial(port, BAUD, timeout=1) as ser:
        while True:
            chunk = await loop.run_in_executor(None, ser.read, ser.in_waiting or 1)
            buf.extend(chunk)
            while True:
                plain = extract_frame(buf)
                if plain is None:
                    break
                match = READING_RE.match(plain)
                if match:
                    lux, hpa = (int(g) for g in match.groups())
                    await queue.put((lux, hpa))


async def run_server(port):
    # Imported here so the encoders above can be tested without BlueZ/bless.
    from bless import (BlessServer, GATTAttributePermissions,
                       GATTCharacteristicProperties)

    loop = asyncio.get_running_loop()
    server = BlessServer(name=DEVICE_NAME, loop=loop)

    def on_read(characteristic, **kwargs):
        return characteristic.value

    server.read_request_func = on_read

    props = (GATTCharacteristicProperties.read |
             GATTCharacteristicProperties.notify)
    perms = GATTAttributePermissions.readable

    await server.add_new_service(SERVICE_UUID)
    await server.add_new_characteristic(
        SERVICE_UUID, ILLUMINANCE_UUID, props, encode_illuminance(0), perms)
    await server.add_new_characteristic(
        SERVICE_UUID, PRESSURE_UUID, props, encode_pressure(0), perms)
    await server.start()
    print(f"Advertising as '{DEVICE_NAME}' (service {SERVICE_UUID})")
    print(f"Reading sensor data from {port}. Ctrl+C to stop.")

    queue = asyncio.Queue()
    reader = asyncio.create_task(read_readings(port, queue))
    try:
        while True:
            lux, hpa = await queue.get()
            server.get_characteristic(ILLUMINANCE_UUID).value = encode_illuminance(lux)
            server.update_value(SERVICE_UUID, ILLUMINANCE_UUID)
            server.get_characteristic(PRESSURE_UUID).value = encode_pressure(hpa)
            server.update_value(SERVICE_UUID, PRESSURE_UUID)
            print(f"Lux: {lux}  Pressure: {hpa} hPa  -> notified")
    finally:
        reader.cancel()
        await server.stop()


def main():
    port = sys.argv[1] if len(sys.argv) > 1 else PORT
    try:
        asyncio.run(run_server(port))
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()
