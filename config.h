#pragma once

#define AUDIO_CLICKY

#ifdef AUDIO_ENABLE
	#define STARTUP_SONG SONG(TO_BOLDLY_GO)
	#define DEFAULT_LAYER_SONGS { \
		SONG(ZELDA_PUZZLE),   \
	       	SONG(MARIO_MUSHROOM), \
		SONG(MARIO_GAMEOVER), \
		SONG(RICK_ROLL)       \
	}
#endif
