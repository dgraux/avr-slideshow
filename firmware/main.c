#include <stdlib.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "tvtext.h"
#include "common.h"
#include "joypad.h"
#include "rectangle.h"
#include "scroll.h"

#include "slides1.h"
#include "demo.h"
#include "setup.h"

#include "gamepreviews.h"

// Position of the game selection rectangle.
#define GAME_SELECTION_LEFT   11
#define GAME_SELECTION_TOP    2
#define GAME_SELECTION_BOTTOM (GAME_SELECTION_TOP + 10)
#define GAME_SELECTION_RIGHT  (GAME_SELECTION_LEFT + 10)

// Position of the game preview graphic.
#define GAME_PREVIEW_LEFT     4
#define GAME_PREVIEW_TOP      4

// Number of games.
#define GAME_COUNT            4

// Draws a preview box for a particular game.
void draw_game_preview(uint8_t game, int8_t y_offset) {
  uint8_t i = game * 5 * 7;
  int8_t y = y_offset + GAME_PREVIEW_TOP - 1;
  for (int8_t r = -1; r < 8; ++r) {
    for (uint8_t c = 0; c < 5; ++c) {
      char preview_c = (r >= 0 && r < 7) ? pgm_read_byte(game_previews + i++) : ' ';
      if (y >= 0 && y < 16) {
	tvtext_buffer[y * TVTEXT_BUFFER_WIDTH + c + GAME_PREVIEW_LEFT] = preview_c;
      }
    }
    ++y;
  }
}

void countdown(void) {
	// Disable the cursor.
	tvtext_flags &= ~_BV(TVTEXT_CURSOR_ENABLED);
	// Count down from 5 to 0.
	tvtext_wait_vsync();
	for (int8_t countdown = 5; countdown >= 0; --countdown) {
		tvtext_clear();
		for (uint8_t r = 0; r < 7; ++r) {
			uint8_t pixels = tvtext_get_font_row(countdown + '0', r);
			for (uint8_t c = 0; c < 6; ++c) {
				if (pixels & 0x80) {
					for (uint8_t y = 0; y < 2; ++y) {
						for (uint8_t x = 0; x < 3; ++x) {
							tvtext_buffer[(r * 2 + y + 1) * 32 + (c * 3) + x + 8] = 127;
						}
						tvtext_buffer[(r * 2 + y + 2) * 32 + (c * 3) + 3 + 8] = 188;
					}
					for (uint8_t x = 0; x < 3; ++x) {
						tvtext_buffer[(r * 2 + 3) * 32 + (c * 3) + x + 9] = 188;
					}
				}
				pixels <<= 1;
			}
		}
		delay_ms(500);
	}
	// Enable the cursor.
	tvtext_flags |= _BV(TVTEXT_CURSOR_ENABLED);
}


int main(void) {
  // Initialise the joypad.
  JOY_PORT |= JOY_ALL;

  // Initialise the tvText library.
  tvtext_init();
  tvtext_flags = _BV(TVTEXT_VISIBLE) | _BV(TVTEXT_INVERTED);

  // Selected game index.
  uint8_t selected_game = 0;

  // To be patient
  countdown();
  tvtext_clear();

  for (;;) {
    // Wait for all keys to be released.
    while (~JOY_PIN & JOY_ALL);

    // Draw the selection menu.
    tvtext_set_viewport(GAME_SELECTION_LEFT + 2, 15, GAME_SELECTION_LEFT + 2 + 6, GAME_SELECTION_TOP + 2);
    tvtext_clear();
    tvtext_puts_P(PSTR(
		       "Profile"
		       "\n"
		       "Demo   "
		       "\n"
		       "Options"
		       "\n"
		       "Credits"
		       ));
    tvtext_reset_viewport_cursor_home();
    draw_rounded_rectangle(GAME_SELECTION_LEFT, GAME_SELECTION_TOP, GAME_SELECTION_RIGHT, GAME_SELECTION_BOTTOM);

    // Preload the joypad input value.
    uint8_t joypad = ~JOY_PIN & JOY_ALL;
    uint8_t old_joypad = joypad;

    // Used to animate the swapping of game graphics.
    int8_t swap_game_scroll = 0;

    // We're still choosing a game...
    uint8_t choosing_game = 1;
    while (choosing_game) {
		
      // Wait for sync.
      tvtext_wait_vsync();

      // Clear the current game rectangle.
      clear_rectangle(GAME_SELECTION_LEFT + 1, GAME_SELECTION_TOP + 1 + 2 * selected_game, GAME_SELECTION_RIGHT - 1, GAME_SELECTION_TOP + 3 + 2 * selected_game);

      // Poll the joypad.
      joypad = ~JOY_PIN & JOY_ALL;
      if ((joypad == old_joypad) || (swap_game_scroll != 0)) {
	joypad = JOY_NONE;
      } else {
	old_joypad = joypad;
      }
      // Handle the joypad.
      if (joypad & JOY_UP) {
	selected_game += (GAME_COUNT - 1);
	swap_game_scroll = -16;
      }
      if (joypad & JOY_DOWN) {
	++selected_game;
	swap_game_scroll = +16;
      }
      selected_game %= GAME_COUNT;

      // Have we selected a game?
      choosing_game = !(joypad & JOY_FIRE1);
		
      // Draw the new selected game rectangle.
      draw_animated_rectangle(GAME_SELECTION_LEFT + 1, GAME_SELECTION_TOP + 1 + 2 * selected_game, GAME_SELECTION_RIGHT - 1, GAME_SELECTION_TOP + 3 + 2 * selected_game);

      // Draw the game preview graphic.
      draw_game_preview(selected_game, swap_game_scroll);
      uint8_t other_game = selected_game;
      uint8_t other_game_scroll = swap_game_scroll;
      if (swap_game_scroll > 0) {
	--swap_game_scroll;
	other_game += (GAME_COUNT - 1);
	other_game_scroll -= 16;
      } else if (swap_game_scroll < 0) {
	++swap_game_scroll;
	other_game += 1;
	other_game_scroll += 16;
      }
      other_game %= GAME_COUNT;
      draw_game_preview(other_game, other_game_scroll);

    }

    // Scroll out the menu.
    scroll_out();
    tvtext_clear();
    tvtext_offset_y -= (128 + 8) * 2;

    // Run the selected game.
    switch (selected_game) {
    case 0:
      slides1();
      break;
    case 1:
      demo();
      break;
    case 2:
      setup();
      break;
    case 3:
      credit();
      break;
    }
  }
}
