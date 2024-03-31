# import typer
from tqdm import tqdm
from pyBusPirateLite.I2C import I2C

TOTAL_SIZE = 0x1000
BUFFER_SIZE = 32 # This is probably the limit.

def main(bin_file: str):
    with open(bin_file, "rb") as f:
        i2c = I2C()
        i2c.speed = "100kHz"

        i2c.configure(power=False)
        for i in tqdm(range(TOTAL_SIZE // BUFFER_SIZE)):
            addr = i * BUFFER_SIZE
            addr_high = addr >> 8
            addr_low = addr & 0xFF
            data = f.read(BUFFER_SIZE)
            cmd = bytes([0xA0, addr_high, addr_low, *data])
            i2c.write_then_read(len(cmd), 0, cmd)

if __name__ == "__main__":
    # typer.run(main)
    main("/home/woutervisser/Repos/DelphiRCD310-tools/dump.bin")