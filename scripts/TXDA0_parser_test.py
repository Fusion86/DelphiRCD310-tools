import pytest

from TXDA0_parser import parse_line


@pytest.mark.parametrize(
    "line",
    [
        b"363822 INFO M ID: 0x260003 P: 0xB Thrd: 17  Mod: Keyboard File: 0x260:283",
        b"363822 INFO M ID: 0x260002 P: 0xB Thrd: 17  Mod: Keyboard",
        b"358606 INFO M ID: 0x260003 P: 0x15 Thrd: 17  Mod: Keyboard File: 0x260:283",
        b"358606 INFO M ID: 0x260002 P: 0x15 Thrd: 17  Mod: Keyboar",
        b"370286 INFO M ID: 0x260002 P: 0xE Thrd: 17  Mod: Keyboard"
        b"450734 INFO M ID: 0x260001 P: 0x1 Thrd: 17  Mod: Keyboard File: 0x260:283",
        b"450738 INFO H ID: 0x1E1805 P: 0x0 Thrd: 15  Mod: BAP MDI File: 0x1E1:619",
        b"450739 INFO H ID: 0x1D1800 P: 0x35 Thrd",
        b"450741 INFO M ID4  Mod: Audio SRC Manager File: 0x21:914\r: 0x21807 P: 0x0 Thrd: 4  Mod: Audio SRC",
        b"45074C Manager File: 0x21:914",
        b"450752 I2 INFO M ID: 0x21801 P: 0x10001 Thrd: 8  Mod: Audio SRC Manager File: 0x21:95NFO M ID: 0x21801 P: 0x10001 Thrd: 8  Mod: Audio SRC Manager File: 0x21:951",
        b"450830 INFO M ID: 0x260003 P: 0x1 Thrd: 17  Mod: Keyboard File: 0x260:283",
        b"450831 INFO M ID: 0x260002 P: 0x1 Thrd: 17  Mod: Keyboa",
        b"450949 INFO M ID: 0x2A3802 P: 0x0 Thrd: 12  Mod: Customer Diagnostics File: 0x2A3:1098",
        b"451768 INFO M ID: 0x22800 P: 0x7 Thrd: 8  Mod: Audio SRC Manager File: 0x22:1730",
        b"451768 INFO M ID: 0x22800 P: 0x7 Thrd: 8  Mod: Audio SRC Manager File: 0x22:1730",
        b"452774 INFO H ID: 0x21805 P: 0x7A Thrd: 8  Mod: Audio SRC Manager File: 0x21:523",
        b"456382 INFO H ID: 0x1E1805 P: 0x0 Thrd: 15  Mod: BAP MDI File: 0x1E1:619",
        b"510755 INFO M ID: 0x11 P: 0x51 Thrd: 12  Mod: Customer Diagnostics File: 0x2A2:436",
        b"510756 INFO M ID: 0x14801 P: 0x1 Thrd: 9  Mod: A",
        b"51udio SRC Manager File0756 INFO M ID: 0x21802 P: 0x1 Thrd: 8  Mod: Audio SRC Manager File: 0x21:594",
    ],
)
def test_line_parser(line: bytes):
    line = line.decode("utf-8")
    parse_line(line)
