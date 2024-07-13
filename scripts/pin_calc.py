import sys


if len(sys.argv) != 2 or len(sys.argv[1]) != 4:
    print("USAGE: pin_calc.py 1234")
    sys.exit(1)

bs = sys.argv[1].encode("ascii")
bs = [x - ord('0') for x in bs]
bs = bytes([x ^ 0x65 for x in bs])
print(bs.hex(" "))