#include QMK_KEYBOARD_H
#include "muse.h"
#include "planck.h"
#include "eeconfig.h"

#ifdef AUDIO_ENABLE
	#include "audio.h"
	#include "song_list.h"
	float mushroom[][2] = SONG(MARIO_MUSHROOM);
#endif

extern keymap_config_t keymap_config;

// Complex Tapdance hoopla
typedef struct {
	bool is_press_action;
	int state;
} tap;

enum {
	SINGLE_TAP = 1,
	SINGLE_HOLD = 2,
	DOUBLE_TAP = 3,
	DOUBLE_HOLD = 4,
};

// Tap dance enums
enum {
	X_AT_FUN = 0,
};

int cur_dance (qk_tap_dance_state_t *state);

void x_finished (qk_tap_dance_state_t *state, void *user_data);
void x_reset (qk_tap_dance_state_t *state, void *user_data);


// Modified Programmer Dvorak enums

enum planck_layers {
	_PDVORAK,
	_UPPER,
	_LOWER,
	_FUNCTION,
};

enum custom_keycodes {
	PDVK = SAFE_RANGE,
	KC_LAST
};

// For getting the last arg in shell line (SUPER THANKS to Drashna over on Discord)

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  static uint16_t macro_timer;

  switch (keycode){
    case KC_LAST:
      if(record->event.pressed){
        macro_timer = timer_read();
        register_mods(MOD_BIT(KC_LCTRL));
      } else {
        unregister_mods(MOD_BIT(KC_LCTRL));
        if (timer_elapsed(macro_timer) < 150) {
          SEND_STRING("!$");
        }
      }
    return false;
  }
  return true;
}

#define PDVORAK MO(_PDVORAK)
#define LOWER MO(_LOWER)
#define UPPER MO(_UPPER)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* Programmer Dvorak */
	[_PDVORAK] = LAYOUT_planck_grid(
		KC_ESC, KC_SCOLON, KC_COMMA, KC_DOT, KC_P, KC_Y, KC_F, KC_G, KC_C, KC_R, KC_L, KC_BSPC,
   		KC_LAST, KC_A, KC_O, KC_E, KC_U, KC_I, KC_D, KC_H, KC_T, KC_N, KC_S, KC_SLASH,
   		KC_LSPO, KC_QUOT, KC_Q, KC_J, KC_K, KC_X, KC_B, KC_M, KC_W, KC_V, KC_Z, KC_RSPC,
    		TD(X_AT_FUN), KC_LEFT, KC_DOWN, KC_UP, KC_RIGHT, KC_LALT, KC_SPACE, KC_ENTER, MT(MOD_LCTL | MOD_LSFT, KC_LGUI), KC_PGUP, KC_PGDN, LT(_LOWER, KC_PLUS)
   	),
	
	[_UPPER] = LAYOUT_planck_grid(
		KC_TILD, KC_AMPR, KC_PERC, KC_LBRC, KC_LCBR, KC_EQL, KC_ASTR, KC_RCBR, KC_RBRC, KC_EXLM, KC_HASH, KC_TRNS,
		KC_PLUS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_MINUS, KC_BSLASH, 
		KC_TRNS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
		KC_TRNS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_TRNS
	),

	[_LOWER] = LAYOUT_planck_grid(
		KC_DLR, KC_7, KC_5, KC_3, KC_1, KC_9, KC_0, KC_2, KC_4, KC_6, KC_8, KC_TRNS,
		KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
		KC_NO, CK_DOWN, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
		KC_TRNS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_TRNS
	),

	[_FUNCTION] = LAYOUT_planck_grid(
		KC_F11, KC_F7, KC_F5, KC_F3, KC_F1, KC_F9, KC_F12, KC_F2, KC_F4, KC_F6, KC_F8, RESET,
		KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
		KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
		KC_TRNS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_TRNS
	),
};


// Super crazy tap dancing stuff - see quad dance in feature_tap_dance.md

int cur_dance (qk_tap_dance_state_t *state) {
	if (state->count == 1) {
		if (state->interrupted || !state->pressed) return SINGLE_TAP;
		// key not interrupted, but still held sends 'HOLD'
		else return SINGLE_HOLD;
	}
	else if (state->count == 2) {
		if (state->interrupted || !state->pressed) return DOUBLE_TAP;
		else return DOUBLE_HOLD;
	}
	else return 8; // magic number that i dont believe works. 
}

// instance of tap for the 'X' dance.
static tap xtap_state = {
	.is_press_action = true,
	.state = 0
};

// registering keypresses
void x_finished (qk_tap_dance_state_t *state, void *user_data) {
	xtap_state.state = cur_dance(state);
	switch (xtap_state.state) {
		case SINGLE_TAP: register_code16(KC_AT); break;
		case SINGLE_HOLD: set_oneshot_layer(_UPPER, ONESHOT_START); break;
		case DOUBLE_TAP: set_oneshot_layer(_FUNCTION, ONESHOT_START); break;
		case DOUBLE_HOLD: reset_keyboard(); break;
	}
}
// forgetting keypresses

void x_reset (qk_tap_dance_state_t *state, void *user_data) {
	switch (xtap_state.state) {
		case SINGLE_TAP: unregister_code16(KC_AT); break;
		case SINGLE_HOLD: clear_oneshot_layer_state(ONESHOT_PRESSED); break;
		case DOUBLE_TAP: clear_oneshot_layer_state(ONESHOT_PRESSED); break;
		case DOUBLE_HOLD: reset_keyboard(); break;
	}
	xtap_state.state = 0;
}

qk_tap_dance_action_t tap_dance_actions[] = {
	[X_AT_FUN] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, x_finished, x_reset)
};
