#include QMK_KEYBOARD_H

extern bool is_drag_scroll;

// Tap Dance keycodes
enum td_keycodes {
    MSE_BTN5_LAYR_1,  // Our example key: Forward mouse button when held switch to layer 1,  forward mouse button when tapped.
    MSE_BTN4_DRAG     // Hold mse btn4 toggles dragscroll
};

// Define a state for when we're holding down button 4
// this enters precison mode but also allows us to switch to another layer
bool btn4_held = false;
bool precision_mode = false;

// Define a type containing as many tapdance states as you need
typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_SINGLE_TAP,
    TD_DOUBLE_HOLD,
    TD_DOUBLE_TAP
} td_state_t;

// Create a global instance of the tapdance state type
static td_state_t td_btn4_state;
static td_state_t td_btn5_state;

// Declare your tapdance functions:

// Function to determine the current tapdance state
td_state_t cur_dance(tap_dance_state_t *state);

// `finished` and `reset` functions for each tapdance keycode
void mseBtn4_finished(tap_dance_state_t *state, void *user_data);
void mseBtn4_reset(tap_dance_state_t *state, void *user_data);
void mseBtn5_finished(tap_dance_state_t *state, void *user_data);
void mseBtn5_reset(tap_dance_state_t *state, void *user_data);

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT( /* Base */
        KC_BTN1, KC_BTN3, KC_BTN2,
          TD(MSE_BTN4_DRAG), TD(MSE_BTN5_LAYR_1)
    ),
    [1] = LAYOUT(
        DRAG_SCROLL, _______, DPI_CONFIG,
          _______, _______
    ),
    [2] = LAYOUT(
        KC_BTN1, KC_BTN3, TD(MSE_BTN4_DRAG),
          KC_BTN2, TD(MSE_BTN5_LAYR_1)
    )
};


// Determine the tapdance state to return
td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        // Interrupted means some other button was pressed in the tapping term
        if (state->interrupted || !state->pressed) {
            return TD_SINGLE_TAP;
        } else {
            return TD_SINGLE_HOLD;
        }
    }

    if (state->count == 2) {
        if (state->interrupted) {
            return TD_DOUBLE_SINGLE_TAP;
        } else if (state->pressed) {
            return TD_DOUBLE_HOLD;
        } else {
            return TD_DOUBLE_TAP;
        }

    } else {
        return TD_UNKNOWN; // Any number higher than the maximum state value you return above
    }
}
void mseBtn4_finished(tap_dance_state_t *state, void *user_data) {
    td_btn4_state = cur_dance(state);
    switch (td_btn4_state) {

        // hold for button 4 finished
        case TD_SINGLE_HOLD:
            is_drag_scroll = true;
            btn4_held = true;
        break;

        case TD_DOUBLE_TAP:
            if (!precision_mode) {
                pmw33xx_set_cpi(0, (dpi_array[keyboard_config.dpi_config] / 2) );
                precision_mode = true;
            } else {
                pmw33xx_set_cpi(0, dpi_array[keyboard_config.dpi_config] );
                precision_mode = false;
            }
        break;

        default:
        break;
    }
}

void mseBtn4_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_btn4_state) {

        // reset button 4 sending tap code
        case TD_SINGLE_TAP:
            tap_code16(KC_BTN4);
        break;

        // hold for button 4 reset
        case TD_SINGLE_HOLD:
            is_drag_scroll = false;
            btn4_held = false;
        break;

        default:
        break;
    }
    td_btn4_state = TD_NONE;
}

// Handle the possible states for each tapdance keycode you define:
void mseBtn5_finished(tap_dance_state_t *state, void *user_data) {
    td_btn5_state = cur_dance(state);
    switch (td_btn5_state) {

        // turning on layer 1 for button 5
        case TD_SINGLE_HOLD:
            layer_on(1);
        break;

        default:
        break;
    }
}

void mseBtn5_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_btn5_state) {

        case TD_SINGLE_TAP:
            tap_code16(KC_BTN5);
        break;

        case TD_SINGLE_HOLD:
            layer_off(1);
        break;

        // button 5 double tapped
        case TD_DOUBLE_TAP:
            if (btn4_held) {
                // If button 4 is held we're in drag scroll, so come out of that mode
                is_drag_scroll = false;
                if (layer_state_is(0))  {
                    // layer state was 0, switching to 2
                    layer_on(2);
                }
                else {
                    // layer state was 2, switching to 0
                    layer_clear();
                }
            }
        break;

        default:
        break;
    }
    td_btn5_state = TD_NONE;
}

// Define `ACTION_TAP_DANCE_FN_ADVANCED()` for each tapdance keycode, passing in `finished` and `reset` functions
tap_dance_action_t tap_dance_actions[] = {
    [MSE_BTN5_LAYR_1]  = ACTION_TAP_DANCE_FN_ADVANCED(NULL, mseBtn5_finished, mseBtn5_reset),
    [MSE_BTN4_DRAG]   = ACTION_TAP_DANCE_FN_ADVANCED(NULL, mseBtn4_finished, mseBtn4_reset)
};

// Set a long-ish tapping term for tap-dance keys
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case QK_TAP_DANCE ... QK_TAP_DANCE_MAX:
            return 275;
        default:
            return TAPPING_TERM;
    }
}
