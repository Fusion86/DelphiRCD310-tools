if __name__ == "__main__":
    with open(
        "/home/woutervisser/Nextcloud/Personal/Car & Bike/Data/Display code 0000 2.txt",
        "rt",
    ) as f:
        lines = f.readlines()

    i = 0

    for line in lines:
        data = line.split(":")[-1].strip()
        data = bytes.fromhex(data)

        if len(data) > 32:
            i += 1
            print(f"{len(data)}: {data.hex()}")

        if len(data) > 0 and data[0] == 0xAF:
            print()

    # print(i)
