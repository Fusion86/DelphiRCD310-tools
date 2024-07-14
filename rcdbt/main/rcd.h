#pragma once

#include "enum.h"

#define RCD_EVENT_ENUM(X)                     \
    ENUM_COMMENT("MOD: Audio SRC Manager")    \
    X(EVT_SWITCH_MEDIA_SRC, 0x22800)          \
    X(EVT_VOLUME, 0x21801)                    \
    X(EVT_POWEROFF_UNK4, 0x14801)             \
    ENUM_COMMENT("Mod: Keyboard")             \
    X(EVT_KEY_DOWN, 0x260001)                 \
    X(EVT_KEY_HELD_SHORT, 0x260005)           \
    X(EVT_KEY_HELD_REPEAT, 0x260006)          \
    X(EVT_KEY_UP, 0x260002)                   \
    X(EVT_KEY_UP_PREVIEW, 0x260003)           \
    ENUM_COMMENT("Mod: Customer Diagnostics") \
    X(EVT_POWEROFF_UNK1, 0x2A3802)            \
    X(EVT_POWEROFF_UNK3, 0x11)                \
    ENUM_COMMENT("BAP MDI")                   \
    X(EVT_POWEROFF_UNK2, 0x1E1805)

ENUM_DECLARE(rcd_event, RCD_EVENT_ENUM)

#define KEYBOARD_BTN_ENUM(X) \
    X(BTN_POWER_KNOB, 1)     \
    X(BTN_EJECT, 2)          \
    X(BTN_RIGHT_KNOB, 3)     \
    X(BTN_TP, 5)             \
    X(BTN_1, 6)              \
    X(BTN_2, 7)              \
    X(BTN_3, 8)              \
    X(BTN_4, 9)              \
    X(BTN_5, 10)             \
    X(BTN_6, 11)             \
    X(BTN_AUDIO, 12)         \
    X(BTN_RADIO, 13)         \
    X(BTN_TRACK_PREV, 14)    \
    X(BTN_TRACK_NEXT, 15)    \
    X(BTN_SOUND, 17)         \
    X(BTN_FOLDER_PREV, 20)   \
    X(BTN_FOLDER_NEXT, 21)   \
    X(BTN_MENU, 22)

ENUM_DECLARE(keyboard_btn, KEYBOARD_BTN_ENUM)

typedef enum
{
    VOLUME_ON_0 = 0x0, // Fired before the source is enabled/unmuted. Also fired after switching to a new source.
    VOLUME_ON_3 = 0x3,
    VOLUME_OFF = 0x10003,
} audio_src_manager_event_0x21801;

// Can this one be merged with audio_src_manager_event_0x21801?
typedef enum audio_src_flags_t
{
    SRC_RADIO_FM = 0x1,
    SRC_RADIO_AM = 0x2,
    SRC_AUX = 0x7,

    SRC_SOUND_ON = 0x10000,  // Fired after VOLUME_ON_PREVIEW and before VOLUME_ON_3.
    SRC_SOUND_OFF = 0x20000, // Fired after VOLUME_OFF
} audio_src_flags;

bool is_keyboard_event(int evt, int btn);
