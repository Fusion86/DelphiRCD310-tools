from tqdm import tqdm
from pyBusPirateLite.I2C import I2C

i2c = I2C()
i2c.speed = "100kHz"

i2c.configure(power=False)
i2c.write_then_read(3, 0, [0xA0, 0, 0])

with open("temp.bin", "wb") as f:
    for i in tqdm(range(16)):
        # print(f"off = {i}")
        bs = i2c.write_then_read(1, 0x100, [0xA1])
        # print(bs.hex())
        f.write(bs)