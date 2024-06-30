#!/usr/bin/python3
import click
from tqdm import tqdm
from pyBusPirateLite.I2C import I2C

TOTAL_SIZE = 0x1000
BUFFER_SIZE = 32  # This is probably the limit.


@click.group()
def cli():
    pass


# TODO: Add verify option which reads back and compares.
@cli.command()
@click.option("-i", "--infile", type=click.Path())
def write(infile: str):
    with open(infile, "rb") as f:
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


@cli.command()
@click.option("-o", "--outfile", default="temp.bin", type=click.Path())
def read(outfile: str):
    i2c = I2C()
    i2c.speed = "100kHz"
    i2c.configure(power=False)

    i2c.write_then_read(3, 0, [0xA0, 0, 0])

    with open(outfile, "wb") as f:
        for i in tqdm(range(16)):
            # print(f"off = {i}")
            bs = i2c.write_then_read(1, 0x100, [0xA1])
            # print(bs.hex())
            f.write(bs)


if __name__ == "__main__":
    cli()
