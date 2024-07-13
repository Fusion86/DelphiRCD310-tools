import os
import shutil
import matplotlib.pyplot as plt
import numpy as np
from bitstring import Bits

with open("Display code 0000 2.txt", "rt") as f:
    lines = f.readlines()

i = 0
strips = []

for line in lines:
    data = line.split(":")[-1].strip()
    data = bytes.fromhex(data)

    if len(data) > 32:
        strips.append(data)

print(f"Strips: {len(strips)}")

shutil.rmtree("imgs", ignore_errors=True)
os.makedirs("imgs", exist_ok=True)

for j in range(0, len(strips), 8):
    img_idx = j // 8
    img_strips = strips[j * 8 : j * 8 + 8]
    img_data = bytearray()
    [img_data.extend(x) for x in img_strips]

    width = 314
    img = np.zeros((324, 86))
    bits = Bits(img_data)

    for i, b in enumerate(bits):
        byte_offset = i // 8
        col = (byte_offset // width)
        # col = 8 - (byte_offset // width)
        y = byte_offset % width
        x = i % 8 + (col * 8)

        img[y, x] = b

    plt.imsave(os.path.join("imgs", f"{img_idx}.png"), img, cmap="gray")
