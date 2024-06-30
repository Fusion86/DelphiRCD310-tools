import logging
from dataclasses import dataclass
from enum import IntEnum

import serial
from pyparsing import Char, Opt, Word, alphanums, alphas, hexnums, nums


@dataclass
class Uart0Event:
    ts: int  # timestamp since start in milliseconds
    level: str  # Values I have seen: INFO
    id: int  # 'ID:'
    param: int  # 'P:'
    thrd: int  # 'Thrd:' meaning thread?
    module: str  # 'Mod:'
    file: str  # 'File:' no clue. Code offset? Or some magic number?


class EventId(IntEnum):
    KEY_DOWN = 2490369
    KEY_HELD_SHORT = 2490373
    KEY_HELD_REPEAT = 2490374
    KEY_UP = 2490370
    KEY_UP_PREVIEW = 2490371  # Always fired before a KEY_UP event.


class KeyId(IntEnum):
    BTN_POWER_KNOW = 1
    BTN_EJECT = 2
    BTN_RIGHT_KNOB = 3
    BTN_TP = 5
    BTN_1 = 6
    BTN_2 = 7
    BTN_3 = 8
    BTN_4 = 9
    BTN_5 = 10
    BTN_6 = 11
    BTN_AUDIO = 12
    BTN_RADIO = 13
    BTN_TRACK_PREV = 14
    BTN_TRACK_NEXT = 15
    BTN_SOUND = 17
    BTN_FOLDER_PREV = 20
    BTN_FOLDER_NEXT = 21
    BTN_MENU = 22


KNOWN_EVENT_IDS = list(EventId)
KNOWN_KEY_IDS = list(KeyId)

VALID_MODULES = [
    "Audio SRC Manager",
    "Keyboard",
    "BAP MDI",
    "Customer Diagnostics",
    "Audio",
    "BAP",
]

LINE_PARSER = (
    Word(nums).set_results_name("ts")
    + Word(alphas).set_results_name("level")
    + Char(alphas)
    + "ID: 0x"
    + Word(hexnums).set_results_name("id")
    + "P: 0x"
    + Word(hexnums).set_results_name("p")
    + "Thrd:"
    + Word(nums).set_results_name("thrd")
    + "Mod:"
    + Word(alphanums).set_results_name("mod")
    + Opt("File:" + Word(alphanums).set_results_name("file"))
)


def parse_line(line: str):
    res = LINE_PARSER.parse_string(line)

    mod = res["mod"]

    if mod not in VALID_MODULES:
        logging.warning("Unknown or invalid module: '%s'", mod)

    return Uart0Event(
        ts=res["ts"],
        level=res["level"],
        id=int(res["id"], 16),
        param=int(res["p"], 16),
        thrd=int(res["thrd"]),
        module=mod,
        file=res.get("file"),
    )


if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG)

    with serial.Serial("/dev/ttyUSB0", 115_200, timeout=0.1) as ser:
        while True:
            try:
                line = ser.readline().strip().decode("utf-8")

                if len(line) == 0:
                    continue

                data = parse_line(line)

                if data.module.startswith("Keyb"):
                    event = EventId(data.id) if data.id in KNOWN_EVENT_IDS else None
                    key = KeyId(data.param) if data.param in KNOWN_KEY_IDS else None

                    if event is not None and key is not None:
                        print(event.name, key.name)
                    else:
                        if event is None:
                            logging.warning("Unknown event id: '%s'", data.id)
                        if key is None:
                            logging.warning("Unknown key id: '%s'", data.param)
                else:
                    logging.debug(line)
            except Exception as ex:
                logging.error(ex)
