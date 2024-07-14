#include <stdbool.h>

#include "rcd.h"

ENUM_DEFINE(rcd_event, RCD_EVENT_ENUM)
ENUM_DEFINE(keyboard_btn, KEYBOARD_BTN_ENUM)

bool is_keyboard_event(int evt, int btn)
{
    bool valid_evt =
        evt == EVT_KEY_DOWN ||
        evt == EVT_KEY_HELD_SHORT ||
        evt == EVT_KEY_HELD_REPEAT ||
        evt == EVT_KEY_UP ||
        evt == EVT_KEY_UP_PREVIEW;

    // bool valid_key =
    //     btn == BTN_POWER_KNOB ||
    //     btn == BTN_EJECT ||
    //     btn == BTN_RIGHT_KNOB ||
    //     btn == BTN_TP ||
    //     btn == BTN_1 ||
    //     btn == BTN_2 ||
    //     btn == BTN_3 ||
    //     btn == BTN_4 ||
    //     btn == BTN_5 ||
    //     btn == BTN_6 ||
    //     btn == BTN_AUDIO ||
    //     btn == BTN_RADIO ||
    //     btn == BTN_TRACK_PREV ||
    //     btn == BTN_TRACK_NEXT ||
    //     btn == BTN_SOUND ||
    //     btn == BTN_FOLDER_PREV ||
    //     btn == BTN_FOLDER_NEXT ||
    //     btn == BTN_MENU;

    return valid_evt;
}