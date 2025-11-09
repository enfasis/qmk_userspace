/**
 * Copyright 2021 Charly Delay <charly@codesink.dev> (@0xcharly)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

#ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
#    include "timer.h"
#endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

enum charybdis_keymap_layers {
    LAYER_BASE = 0,
    LAYER_LOWER,
    LAYER_RAISE,
    LAYER_POINTER,
};

enum custom_key_codes {
    CCKC_GRAVE = 0x0035,
};

typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_HOLD,
} td_state_t;

typedef struct {
    bool       is_press_action;
    td_state_t state;
} td_tap_t;

enum tap_dance_codes {
    TDC,
    TDG,
};

td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 2 && state->pressed) return TD_DOUBLE_HOLD;
    return TD_SINGLE_TAP;
}

// Initialize tap structure associated with example tap dance key
static td_tap_t ql_tap_state = {.is_press_action = true, .state = TD_NONE};

void ql_finished(tap_dance_state_t *state, void *user_data) {
    ql_tap_state.state = cur_dance(state);
    switch (ql_tap_state.state) {
        case TD_DOUBLE_HOLD:
            // Check to see if the layer is already set
            if (layer_state_is(LAYER_LOWER)) {
                // If already set, then switch it off
                layer_off(LAYER_LOWER);
            } else {
                // If not already set, then switch the layer on
                layer_on(LAYER_LOWER);
            }
            break;
        default:
            register_code(KC_LGUI);
    }
}

void ql_reset(tap_dance_state_t *state, void *user_data) {
    // If the key was held down and now is released then switch off the layer
    if (ql_tap_state.state == TD_DOUBLE_HOLD) {
        layer_off(LAYER_LOWER);
    } else {
        unregister_code(KC_LGUI);
    }
    ql_tap_state.state = TD_NONE;
}

tap_dance_action_t tap_dance_actions[] = {
    [TDC] = ACTION_TAP_DANCE_DOUBLE(KC_LCTL, KC_PSCR),
    [TDG] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, ql_finished, ql_reset),
};

/** \brief Automatically enable sniping-mode on the pointer layer. */
#define CHARYBDIS_AUTO_SNIPING_ON_LAYER LAYER_POINTER

#ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
static uint16_t auto_pointer_layer_timer = 0;

#    ifndef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS
#        define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS 1000
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS

#    ifndef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD
#        define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD 8
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD
#endif     // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

#define LOWER MO(LAYER_LOWER)
#define RAISE MO(LAYER_RAISE)
#define RT_L LT(LAYER_RAISE, CCKC_GRAVE)
#define PT_Z LT(LAYER_POINTER, KC_Z)
#define PT_SLSH LT(LAYER_POINTER, KC_SLSH)

#ifndef POINTING_DEVICE_ENABLE
#    define DRGSCRL KC_NO
#    define DPI_MOD KC_NO
#    define S_D_MOD KC_NO
#    define SNIPING KC_NO
#endif // !POINTING_DEVICE_ENABLE

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
   CCKC_GRAVE,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,       KC_6,    KC_7,    KC_8,    KC_9,    KC_0, KC_MINS,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,       KC_Y,    KC_U,    KC_I,    KC_O,    KC_P, KC_EQL,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_ESC,    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,       KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN, KC_QUOT,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
      KC_LSFT,    PT_Z,    KC_X,    KC_C,    KC_V,    KC_B,       KC_N,    KC_M, KC_COMM,  KC_DOT, PT_SLSH, KC_LSFT,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                 TD(TDG), TD(TDC), KC_BSPC,     KC_ENT,  KC_SPC,
                                          KC_LALT,  KC_DEL,      RAISE
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_LOWER] = LAYOUT(
  // ╭───────────────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       KC_TILD,  KC_EXLM,    KC_AT,    KC_HASH,    KC_DLR,   KC_PERC,    KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_UNDS,
  // ├───────────────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, C(KC_18), C(KC_F19), C(KC_F15),   XXXXXXX,   KC_LCBR,    KC_RCBR, KC_PIPE, XXXXXXX, XXXXXXX, KC_RBRC, XXXXXXX,
  // ├───────────────────────────────────────────────────────────────┤ ├ ──────────────────────────────────────────────────────┤
       XXXXXXX,  XXXXXXX, C(KC_F16),   XXXXXXX, C(KC_F14), C(KC_F13),     KC_F16,  KC_F15,  KC_F14,  KC_F13, KC_PMNS, KC_PEQL,
  // ├───────────────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_LSFT,  XXXXXXX,   XXXXXXX,   XXXXXXX,   XXXXXXX,   KC_LBRC,    KC_RBRC, KC_BSLS, XXXXXXX, XXXXXXX, KC_PSLS, KC_PDOT,
  // ╰───────────────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                       XXXXXXX,   XXXXXXX,   _______,    XXXXXXX, _______,
                                                  XXXXXXX,   XXXXXXX,      KC_P0
  //                            ╰────────────────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_RAISE] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       XXXXXXX,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,      KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10, _______,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       _______, _______, _______, _______, _______, KC_LBRC,    KC_RBRC, _______, _______, _______,  KC_F11, _______,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       _______, KC_MPRV, KC_VOLD, KC_VOLU, KC_MNXT, _______,    KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT,  KC_F12, _______,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
        _______, KC_MUTE, KC_STOP, KC_MPLY, _______, _______,    KC_F24,  KC_F23, _______, _______, _______, _______,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  _______, _______, _______,    _______, _______,
                                           _______, _______,    XXXXXXX
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_POINTER] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       QK_BOOT,  EE_CLR, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, QK_BOOT,  EE_CLR,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, DPI_MOD, S_D_MOD,    S_D_MOD, DPI_MOD, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, XXXXXXX,    XXXXXXX, KC_RSFT, KC_RCTL, KC_RALT, KC_RGUI, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, _______, DRGSCRL, SNIPING, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, SNIPING, DRGSCRL, _______, XXXXXXX,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  KC_BTN2, KC_BTN1, KC_BTN3,    KC_BTN3, KC_BTN1,
                                           XXXXXXX, KC_BTN2,    KC_BTN2
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),
};
// clang-format on

#ifdef POINTING_DEVICE_ENABLE
#    ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (abs(mouse_report.x) > CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD || abs(mouse_report.y) > CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD) {
        if (auto_pointer_layer_timer == 0) {
            layer_on(LAYER_POINTER);
#        ifdef RGB_MATRIX_ENABLE
            rgb_matrix_mode_noeeprom(RGB_MATRIX_NONE);
            rgb_matrix_sethsv_noeeprom(HSV_GREEN);
#        endif // RGB_MATRIX_ENABLE
        }
        auto_pointer_layer_timer = timer_read();
    }
    return mouse_report;
}

void matrix_scan_user(void) {
    if (auto_pointer_layer_timer != 0 && TIMER_DIFF_16(timer_read(), auto_pointer_layer_timer) >= CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS) {
        auto_pointer_layer_timer = 0;
        layer_off(LAYER_POINTER);
#        ifdef RGB_MATRIX_ENABLE
        rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
#        endif // RGB_MATRIX_ENABLE
    }
}
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

#    ifdef CHARYBDIS_AUTO_SNIPING_ON_LAYER
layer_state_t layer_state_set_user(layer_state_t state) {
    charybdis_set_pointer_sniping_enabled(layer_state_cmp(state, CHARYBDIS_AUTO_SNIPING_ON_LAYER));
    return state;
}
#    endif // CHARYBDIS_AUTO_SNIPING_ON_LAYER
#endif     // POINTING_DEVICE_ENABLE

#ifdef RGB_MATRIX_ENABLE
// Forward-declare this helper function since it is defined in rgb_matrix.c.
void rgb_matrix_update_pwm_buffers(void);
#endif
