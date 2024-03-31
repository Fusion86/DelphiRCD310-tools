# For some reason the dumping method demonstrated in the README.md didn't work for me anymore.
# Idk why, but this seems to work. So whatever.
# You need to setup the buspirate before running this script.
# E.g.
# sudo minicom -D /dev/ttyUSB0 -b 115200
# m
# 4
# 2
# Ctrl-a x

from datetime import datetime
import serial
from tqdm import tqdm

ser = serial.Serial("/dev/ttyUSB0", 115200, timeout=0.001)

ser.write(b"[0xa0 0 0]\n")

while True:
    data = ser.readline()
    if data == b"I2C>":
        break
    else:
        print(data.decode("utf-8"), end="")

bs = bytearray()

with open(f"dump-{datetime.utcnow().isoformat()}.bin", "wb") as f:
    try:
        for _ in tqdm(range(4096)):
            ser.write(b"[0xa1 r]\n")
            while True:
                data = ser.readline()
                # TODO: Ensure we are getting ACKS
                if data == b"I2C>":
                    break
                else:
                    ds = data.decode("utf-8")
                    if ds.startswith("READ: "):
                        hx = ds.split(":")[1].replace("0x", "")
                        b = bytes.fromhex(hx)
                        f.write(b)
                    # print(ds, end="")
    except Exception as e:
        print(e)

# with open("dump3.bin", "wb") as f:
#     f.write(bs)
