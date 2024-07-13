with open("dump.txt", "r") as f:
    txt = f.read()
    parts = [x for x in txt.split(" ") if x != "" and x != "ACK"]
    xs = "".join(x[2:] for x in parts)
    bs = bytes.fromhex(xs)

with open("dump.bin", "wb") as f:
    f.write(bs)