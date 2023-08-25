# Dump Delphi RCD310 EEPROM (24LC32)

Getting the code for a Delphi RCD310 using generic 'hackerman' tools.
No soldering, fully offline and no stupid signups required.

It is probably easier to use a universal programmer, which I don't have.
You could also use pretty much any microcontroller, but I just wanted to use my buspirate for something.

All scripts frankly suck, but it only has to work once.

## Used tools

- Bus Pirate v3.6a
- [Crappy aliexpress IC clamps](https://www.aliexpress.com/item/1005003324309581.html)
- Some random power supply
- Hacked together cable to power the radio

## How to actually power and start the car radio

Todo...

## How to dump the EEPROM

### Wiring guide

Todo...

### Commands

1. Start picocom and tee output to logfile: `picocom -b 115200 /dev/buspirate | tee log.txt`
2. Set mode to i2c and set speed using `m` command.
   1. Pullup and power is not needed because the board has pullups (I think).
   2. I used 100khz, but anything should work.
3. Set read address to 0x0: `[0xa0 0 0]`
4. Read 0x1000 bytes (4k), the website requires exactly 0x1000: `[0xa1 r:0x1000]`
   1. Actually we only need 0x100 bytes to get the code and validate the version number, but it's good practice to dump the whole thing.
5. Copy all bytes read (including all the ACK spam) from log.txt to dump.txt
6. Run `python3 dump2bin.py`
7. Run `python3 eeprom_info.py dump.bin`

## Random notes

Alternative is to use https://www.weboctopus.nl/rcd310-rcd510-navi-radio-code-calculator.php#calculator

```
S/N:
VWZ4Z2K1423726
Type:
RCD310 EU
Part number:
1K0035186AA HW: H14, SW: 5043
PIN: 0384
```
