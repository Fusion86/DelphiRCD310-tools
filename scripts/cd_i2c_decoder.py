# Usage:
# sigrok-cli -c samplerate=8m -d fx2lafw --channels D0=SDA,D1=SCL -P i2c:scl=SCL:sda=SDA -A i2c=address-read:address-write:data-read:data-write:stop --continuous | python3 cd_i2c_decoder.py

import sys

mode = None
cur_addr = None
data_buffer = []

# pat = "Address/Data:"
pat = "i2c-1:"


for line in iter(sys.stdin.readline, b""):
    line = line.strip()

    off = line.index(pat)
    data = line[off + len(pat) :].strip()

    if data == "Stop":
        bs = bytes.fromhex("".join(data_buffer))
        data_buffer.clear()

        bs_len = bs[0]
        assert bs_len == len(bs) - 1, "Len is wrong"

        f = 0
        for b in bs:
            f += b

        assert f % 256 == 255, "Checksum failed"

        msg = bs[1:-1]
        assert len(msg) == len(bs) - 2
        print(mode, cur_addr, msg.hex())

        # print(mode, cur_addr, bs.hex())
    elif "Address" in data or "Data" in data:
        parts = data.split(":")
        rw = "READ" if "read" in parts[0] else "WRITE"
        b = parts[1].strip()

        if "Data" in parts[0]:
            data_buffer.append(b)
            assert mode == rw
        elif "Address" in parts[0]:
            cur_addr = b
            mode = rw
        else:
            assert False
