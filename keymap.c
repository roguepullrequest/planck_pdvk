#include QMK_KEYBOARD_H
#include "muse.h"
#include "planck.h"
#include "eeconfig.h"

#ifdef AUDIO_ENABLE
	#include "audio.h"
	#include "song_list.h"
#endif

extern keymap_config_t keymap_config;

// Complex Tapdance hoopla
typedef struct {
	bool is_press_action
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
}

int cur_dance (qk_tap_dance_state_t *state);

void x_finished (qk_tap_dance_state_t state*, void *user_data);
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
};

enum { // Marco declarations
	LAST = 0,
}

// Macro Definitions
const macro_t *action_get_macro(keyrecord_t *record, uint8_t it, uint8_t out) {
	switch(id) {
		// !$ To simply get the last parm from the shell. 
		case LAST: {
			if (record->event.pressed) {
				SEND_STRING("!$");
				retern false;
			}
		}

#define LOWER MO(_LOWER)
#define UPPER MO(_UPPER)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* Programmer Dvorak */
	[_PDVORAK] = LAYOUT_planck_grid(
		KC_ESC, KC_SCOLON, KC_COMMA, KC_DOT, KC_P, KC_Y, KC_F, KC_G, KC_C, KC_R, KC_L, KC_BSPC,
   		LCTL_T(M(LAST)), KC_A, KC_O, KC_E, KC_U, KC_I, KC_D, KC_H, KC_T, KC_N, KC_S, KC_SLASH,
   		KC_LSPO, KC_QUOT, KC_Q, KC_J, KC_K, KC_X, KC_B, KC_M, KC_W, KC_V, KC_Z, KC_RSPC,
    		TD(X_AT_FUN), KC_LEFT, KC_UP, KC_DOWN, KC_RIGHT, KC_BSPC, KC_SPACE, KC_ENTER, MT(MOD_LCTL | MOD_LSFT, MOD_RGUI), KC_PGUP, KC_PGDN, LT(_LOWER, KC_PLUS),
   	),
	
	[_UPPER] = LAYOUT_planck_grid(
		KC_TILD, KC_AMPR, KC_PERC, KC_LBRC, KC_LCBR, KC_EQL, KC_ASTR, KC_RCBR, KC_RBRC, KC_EXLM, KC_HASH, KC_TRNS,
		M(LAST), KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_MINUS, KC_BSLASH, 
		KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
		KC_TRNS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_TRNS,
	),

	[_LOWER] = LAYOUT_planck_grid(
		KC_DLR, KC_7, KC_5, KC_3, KC_1, KC_9, KC_0, KC_2, KC_4, KC_6, KC_8, KC_TRNS,
		CK_TOGG, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
		CK_UP, CK_DOWN, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
		KC_TRNS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_TRNS,
	),

	[_FUNCTION] = LAYOUT_planck_grid(
		FN_11, FN_7, FN_5, FN_3, FN_1, FN_9, FN_12, FN_2, FN_4, FN_6, FN_8, RESET,
		KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
		KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
		KC_TRNS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_TRNS,
	),
};


// Super crazy tap dancing stuff - see quad dance in feature_tap_dance.md

int cur_dance (qk_tap_dance_state_t *state) {
	if (state->count == 1) {
		if (state->interrupted \\ !state->pressed) return SINGLE_TAP;
		// key not interrupted, but still held sends 'HOLD'
		else return SINGLE_HOLD;
	}
	else if (state->count == 2) {
		if (state->interrupted || !state->pressed) return DOUBLE_TAP
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
		case SINGLE_TAP: register_code(KC_AT); break;
		case SINGLE_HOLD: register_code(_UPPER); break;
		case DOUBLE_TAP: register_code(OSL(_FUNCTION)); break;
		case DOUBLE_HOLD: register_code(RESET); break;
	}
}
// forgetting keypresses

void x_reset (qk_tap_dance_state_t *state, void *user_data) {
	switch (xtap_state.state) {
		case SINGLE_TAP: unregister_code(KC_AT); break;
		case SINGLE_HOLD: unregister_code(_UPPER); break;
		case DOUBLE_TAP: unregister_code(OSL(_FUNCTION)); break;
		case DOUBLE_HOLD: unregister_code(RESET); break;
	}
	xtap_state.state = 0;
}

qk_tap_dance_action_t tap_dance_actian[] = {
	[X_AT_FUN] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, x_finished, x_reset)
};
