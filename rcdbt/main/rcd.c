#include <stdbool.h>

#include "rcd.h"

const char *keyboard_event_to_string(keyboard_event evt)
{
    switch (evt)
    {
    case KEY_DOWN:
        return "KEY_DOWN";
    case KEY_HELD_SHORT:
        return "KEY_HELD_SHORT";
    case KEY_HELD_REPEAT:
        return "KEY_HELD_REPEAT";
    case KEY_UP:
        return "KEY_UP";
    case KEY_UP_PREVIEW:
        return "KEY_UP_PREVIEW";
    default:
        return "<invalid>";
    }
}

const char *keyboard_btn_to_string(keyboard_btn btn)
{
    switch (btn)
    {
    case BTN_POWER_KNOB:
        return "BTN_POWER_KNOB";
    case BTN_EJECT:
        return "BTN_EJECT";
    case BTN_RIGHT_KNOB:
        return "BTN_RIGHT_KNOB";
    case BTN_TP:
        return "BTN_TP";
    case BTN_1:
        return "BTN_1";
    case BTN_2:
        return "BTN_2";
    case BTN_3:
        return "BTN_3";
    case BTN_4:
        return "BTN_4";
    case BTN_5:
        return "BTN_5";
    case BTN_6:
        return "BTN_6";
    case BTN_AUDIO:
        return "BTN_AUDIO";
    case BTN_RADIO:
        return "BTN_RADIO";
    case BTN_TRACK_PREV:
        return "BTN_TRACK_PREV";
    case BTN_TRACK_NEXT:
        return "BTN_TRACK_NEXT";
    case BTN_SOUND:
        return "BTN_SOUND";
    case BTN_FOLDER_PREV:
        return "BTN_FOLDER_PREV";
    case BTN_FOLDER_NEXT:
        return "BTN_FOLDER_NEXT";
    case BTN_MENU:
        return "BTN_MENU";
    default:
        return "<unknown>";
    }
}

bool is_valid_keyboard_input(int evt, int btn)
{
    bool valid_evt = evt == KEY_DOWN || evt == KEY_HELD_SHORT || evt == KEY_HELD_REPEAT || evt == KEY_UP;
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