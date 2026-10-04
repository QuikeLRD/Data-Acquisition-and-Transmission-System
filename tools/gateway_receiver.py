#!/usr/bin/env python3
"""Gateway receiver for TheBlackData (runs on the Raspberry Pi).

Reads framed, AES-128-ECB encrypted sensor readings from the STM32 over UART:

    [ 0xA5 ][ LEN ][ ciphertext (LEN bytes) ]

and prints the decoded reading. Keep the constants below in sync with
Core/Inc/gateway_frame.h and the AES key in Core/Src/main.c.

Dependencies (Raspberry Pi OS / Debian):
    sudo apt install -y python3-serial python3-pycryptodome
"""

import re
import sys

import serial

try:
    from Crypto.Cipher import AES  # pycryptodome (pip)
except ImportError:
    from Cryptodome.Cipher import AES  # name used by some distro packages

PORT = "/dev/serial0"
BAUD = 115200

FRAME_SOF = 0xA5
BLOCK_SIZE = 16
MAX_PAYLOAD = 64  # GATEWAY_FRAME_MAX_PAYLOAD

AES_KEY = bytes([0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
                 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C])

READING_RE = re.compile(rb"^Lux:(-?\d+),hPa:(-?\d+)$")


def decrypt_payload(payload):
    """Decrypt and unpad one payload. Returns the plaintext bytes, or None if
    the padding is invalid (which means this was not a real frame)."""
    plain = AES.new(AES_KEY, AES.MODE_ECB).decrypt(payload)
    pad = plain[-1]
    if pad < 1 or pad > BLOCK_SIZE or plain[-pad:] != bytes([pad]) * pad:
        return None
    return plain[:-pad]


def extract_frame(buf):
    """Try to pull one valid frame out of the front of buf (a bytearray).

    Returns the plaintext of the frame, or None if more bytes are needed.
    Bytes that cannot start a valid frame are discarded from buf, one at a
    time, so the receiver resynchronizes after any lost or corrupted byte.
    """
    while True:
        # Drop everything before the next start byte.
        start = buf.find(bytes([FRAME_SOF]))
        if start < 0:
            buf.clear()
            return None
        del buf[:start]

        if len(buf) < 2:
            return None
        length = buf[1]

        # The payload is whole AES blocks; anything else is a false SOF
        # (0xA5 can legitimately appear inside the ciphertext).
        if length == 0 or length > MAX_PAYLOAD or length % BLOCK_SIZE != 0:
            del buf[:1]
            continue

        if len(buf) < 2 + length:
            return None

        plain = decrypt_payload(bytes(buf[2:2 + length]))
        if plain is None:
            del buf[:1]  # padding check failed: not a real frame
            continue

        del buf[:2 + length]
        return plain


def main():
    port = sys.argv[1] if len(sys.argv) > 1 else PORT
    buf = bytearray()
    with serial.Serial(port, BAUD, timeout=1) as ser:
        print(f"Listening on {port} at {BAUD} baud (Ctrl+C to stop)")
        while True:
            buf.extend(ser.read(ser.in_waiting or 1))
            while True:
                plain = extract_frame(buf)
                if plain is None:
                    break
                match = READING_RE.match(plain)
                if match:
                    lux, hpa = (int(g) for g in match.groups())
                    print(f"Lux: {lux}  Pressure: {hpa} hPa")
                else:
                    print(f"Unexpected payload: {plain!r}")


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
