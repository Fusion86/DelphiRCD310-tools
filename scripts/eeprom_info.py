# Based on https://www.youtube.com/watch?v=y7doseFQ93A
# This code is tested on exaclty one dump.

import sys

if len(sys.argv) != 2:
    print(f"USAGE: ./{sys.argv[0]} eeprom_dump.bin")
    sys.exit(1)

with open(sys.argv[1], "rb") as f:
    f.seek(0xA0)
    bs = f.read(4)

    code = ""
    for b in bs:
        # b ^ 0x65 would also work.
        digit = b & 0xF ^ 0x5
        code += str(digit)

    print("Code: " + code)

    # Bonus
    try:
        f.seek(0xF0)
        name_and_ver = f.read(0x10).decode("UTF-8")
        print("Name and HW: " + name_and_ver)
    except:
        pass