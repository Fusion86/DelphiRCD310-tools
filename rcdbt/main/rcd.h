#pragma once

typedef enum keyboard_event_t
{
    KEY_DOWN = 2490369,
    KEY_HELD_SHORT = 2490373,
    KEY_HELD_REPEAT = 2490374,
    KEY_UP = 2490370,
    KEY_UP_PREVIEW = 2490371,
} keyboard_event;

typedef enum keyboard_btn_t
{
    BTN_POWER_KNOB = 1,
    BTN_EJECT = 2,
    BTN_RIGHT_KNOB = 3,
    BTN_TP = 5,
    BTN_1 = 6,
    BTN_2 = 7,
    BTN_3 = 8,
    BTN_4 = 9,
    BTN_5 = 10,
    BTN_6 = 11,
    BTN_AUDIO = 12,
    BTN_RADIO = 13,
    BTN_TRACK_PREV = 14,
    BTN_TRACK_NEXT = 15,
    BTN_SOUND = 17,
    BTN_FOLDER_PREV = 20,
    BTN_FOLDER_NEXT = 21,
    BTN_MENU = 22,
} keyboard_btn;

const char *keyboard_event_to_string(keyboard_event evt);
const char *keyboard_btn_to_string(keyboard_btn btn);
bool is_valid_keyboard_input(int evt, int btn);