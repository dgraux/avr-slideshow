#include <stdlib.h>

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "tvtext.h"
#include "common.h"
#include "demo.h"

// A BBC BASIC-like demo.
void demo_basic(void) {

	// Display a BBC Micro-like prompt.
	tvtext_clear();
	tvtext_puts_P(PSTR(
		"AVR Computer 1K\n\r"
		"\n"
		"ATmega 168\n\r"
		"\n"
		"BASIC\n\r"
		"\n"
		">"
	));
	delay_ms(2000);

	// Simulate typing in a BASIC program.
	type_string_P(PSTR("10 PRINT \"AVR rules! \";"));
	delay_ms(100);
	tvtext_puts_P(PSTR("\n\r>"));
	delay_ms(500);
	type_string_P(PSTR("20 GOTO 10"));
	delay_ms(100);
	tvtext_puts_P(PSTR("\n\r>"));
	delay_ms(500);
	type_string_P(PSTR("RUN"));
	delay_ms(1000);
	
	// "Run" the simulated BASIC program.
	tvtext_puts_P(PSTR("\n\r"));
	for(int i = 200; i >= 0; --i) {
		tvtext_puts_P(PSTR("AVR rules! "));
		tvtext_wait_vsync();
	}

	// "Break" out of the program to skip to the next part of the demo.
	tvtext_puts_P(PSTR("\n\rEscape at line 10\n\r>"));
	delay_ms(1000);
	type_string_P(PSTR("CHAIN \"DEMO\""));
	delay_ms(1000);

}

// Displays a screen of information about the library.
void demo_info(void) {

	// Hide the screen off the bottom of the display.
	tvtext_offset_y = 256 + 16;

	// We don't want the screen to automatically scroll, and we don't want a flashing cursor.
	tvtext_flags &= ~(_BV(TVTEXT_AUTO_SCROLL) | _BV(TVTEXT_CURSOR_ENABLED));

	// Border of blocks.
	tvtext_cleared = 0x7F;
	tvtext_clear();
	tvtext_cleared = ' ';
	
	// Middle hole.
	tvtext_set_viewport(1, 14, 30, 1);
	tvtext_clear();

	// Reduce viewport further for padding!
	tvtext_set_viewport(2, 13, 29, 2);

	// Main copy.
	tvtext_puts_P(PSTR(
		"tvText: B/W text on a PAL TV"
		"____________________________"
		"\n"
		"The display is made up of 16"
		"rows of 32 columns.  Each of"
		"the characters is 6\xD7""8 pixels"
		"which gives us a total pixel"
		"resolution of 192\xD7""128.\n\r"
		"\n"
		"The   output   is  interrupt"
		"driven,   leaving  some  CPU"
		"time  to the  user  program."
	));

	// Scroll the copy in.	
	while (tvtext_offset_y != 0) {
		tvtext_wait_vsync();
		tvtext_offset_y -= 2;
	}

	// Let that sink in...
	delay_ms(8000);

	// Reset the viewport.
	tvtext_reset_viewport_cursor_home();

	// Scroll the copy off the left of the screen.
	for (int i = 0; i < 32; ++i) {
		tvtext_scroll_left();
		delay_ms(40);
	}
	// Restore the cursor and auto-scrolling.
	tvtext_flags |= _BV(TVTEXT_AUTO_SCROLL) | _BV(TVTEXT_CURSOR_ENABLED);
}

void demo_viewport(void) {
	// Disable the cursor and auto-scrolling.
	tvtext_flags &= ~(_BV(TVTEXT_AUTO_SCROLL) | _BV(TVTEXT_CURSOR_ENABLED));
	// Draw the background.
	tvtext_clear();
	for (uint8_t i = 0; i < 73; ++i) {
		tvtext_puts_P(PSTR("tvText "));
	}
	tvtext_putc('t');
	delay_ms(1000);
	
	// Draw the viewport border.
	for (uint8_t i = 3; i <= 28; ++i) { // Top
		tvtext_buffer[2 * 32 + i] = 127;
		tvtext_wait_vsync();
	}
	for (uint8_t i = 3; i <= 12; ++i) { // Right
		tvtext_buffer[i * 32 + 28] = 127;
		tvtext_wait_vsync();
	}
	for (uint8_t i = 28; i >= 3; --i) { // Bottom
		tvtext_buffer[13 * 32 + i] = 127;
		tvtext_wait_vsync();
	}
	for (uint8_t i = 12; i >= 3; --i) { // Left
		tvtext_buffer[i * 32 + 3] = 127;
		tvtext_wait_vsync();
	}
	delay_ms(500);

	// Unzip the central portion.
	for (uint8_t i = 0; i < 12; ++i) {
		delay_ms(80);
		// Unzip left half.
		tvtext_set_viewport(4, 12, 15, 3);
		tvtext_scroll_left();
		// Unzip right half.
		tvtext_set_viewport(16, 12, 27, 3);
		tvtext_scroll_right();
	}

	// Set the new viewport.
	tvtext_set_viewport(5, 11, 26, 4);
	tvtext_clear();
	// Restore the cursor and auto-scrolling.
	tvtext_flags |= _BV(TVTEXT_CURSOR_ENABLED) | _BV(TVTEXT_AUTO_SCROLL);

	delay_ms(500);	
	type_string_P(PSTR(
		"User-defined viewports"
		"allow you to restrict "
		"the cursor to a region"
		"of the screen.\n\r"
		"\n"
		"Scrolling and clearing"
		"operations only affect"
		"the defined viewport."
	));

	delay_ms(4000);

	// Disable the cursor.
	tvtext_flags &= ~_BV(TVTEXT_CURSOR_ENABLED);

	// Scroll the message away.
	for (uint8_t i = 0; i < 8; ++i) {
		delay_ms(80);
		tvtext_scroll_up();
	}
	// Restore the full viewport.
	tvtext_reset_viewport_cursor_home();
	// Scroll the screen away.
	delay_ms(500);
	for (uint8_t i = 0; i < 16; ++i) {
		delay_ms(80);
		tvtext_scroll_down();
	}

	// Enable the cursor.
	tvtext_flags |= _BV(TVTEXT_CURSOR_ENABLED);
}

void demo_countdown(void) {
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

void demo_cursor(void) {
	tvtext_clear();
	tvtext_flags &= ~_BV(TVTEXT_CURSOR_ENABLED);
	for (uint8_t c = 0; c < 32; ++c) {
		tvtext_wait_vsync();
		tvtext_buffer[c] = 6; // Spades
		tvtext_buffer[c + 32] = 3; // Hearts
		tvtext_buffer[31 - c + 32 * 14] = 4; // Diamonds
		tvtext_buffer[31 - c + 32 * 15] = 5; // Clubs
	}

	// Enable the cursor.
	tvtext_flags |= _BV(TVTEXT_CURSOR_ENABLED);
	tvtext_cursor_move(0, 3);
	type_string_P(PSTR(
		"The library provides support for"
		"a flashing cursor."
	));
	delay_ms(2000);
	tvtext_flags &= ~_BV(TVTEXT_CURSOR_ENABLED);
	type_string_P(PSTR(
		"\n\r\nThe cursor can be switched off.\n\r\n"
	));
	delay_ms(2000);
	tvtext_flags |= _BV(TVTEXT_CURSOR_ENABLED);
	tvtext_cursor = 13;
	type_string_P(PSTR(
		"You can change its appearance to"
		"any of the other characters."
	));
	delay_ms(2000);
	tvtext_cursor = '_';
	type_string_P(PSTR(
		"\n\r\nYou can also change the rate at "
		"which it flashes. Slow..."
	));
	tvtext_cursor_flash_period = 50;
	tvtext_cursor_reset_flash();
	delay_ms(4000);
	type_string_P(PSTR("\x7F\x7F\x7F" " to fast!"));
	tvtext_cursor_flash_period = 4;
	tvtext_cursor_reset_flash();
	delay_ms(4000);
	tvtext_cursor_flash_period = 16;

	// Clear the screen with a wave.
	tvtext_flags &= ~_BV(TVTEXT_CURSOR_ENABLED);
	for (uint8_t c = 0; c < 54; ++c) {
		delay_ms(40);
		for (uint8_t wave = 0; wave < 5; ++wave) {
			char wave_char = ' ';
			switch (wave) {
				case 0:
					wave_char = 127; // 100%
					break;
				case 1:
					wave_char = 190; // 75%
					break;
				case 2:
					wave_char = 189; // 50%
					break;
				case 3:
					wave_char = 188; // 25%
					break;
			}
			int8_t wave_pos = c - wave;
			if (wave_pos >=0 && wave_pos < 48) {
				for (uint8_t i = 0; i <= wave_pos; ++i) {
					int8_t y = i;
					int8_t x = (wave_pos - i);
					if (x >= 0 && x < 32 && y >= 0 && y < 16) {
						tvtext_buffer[y * 32 + x] = wave_char;
					}
				}
			}
		}
	}

	// Enable the cursor.
	tvtext_flags |= _BV(TVTEXT_CURSOR_ENABLED);

}


void demo_invert_fade_up_blocks(void) {
	for (uint16_t c = 0; c < 32 * 16; ++c) {
		switch (tvtext_buffer[c]) {
			case 188: // 25%
			case 189: // 50%
				++tvtext_buffer[c];
				break;
			case 190: // 75%
				tvtext_buffer[c] = 127;
				break;
		}
	}
}

void demo_invert(void) {

	// Display the message.
	tvtext_set_viewport(8, 15, 23, 5);
	type_string_P(PSTR("If  you  dislike"));
	if (tvtext_flags & _BV(TVTEXT_INVERTED)) {
		type_string_P(PSTR("black  on white"));
	} else {
		type_string_P(PSTR("white  on black"));
	}
	type_string_P(PSTR(
	                  ","
		"you  can  invert"
		"the screen  with"
		"a  single  flag."
	));
	delay_ms(1000);

	// Disable the cursor.
	tvtext_flags &= ~_BV(TVTEXT_CURSOR_ENABLED);

	// Restore the full viewport.
	tvtext_reset_viewport_cursor_home();

	// Fill in the screen in a snake-like fashion.
	for (uint8_t row = 0; row < 16; row += 2) {
		for (uint8_t col = 0; col < 32; col += 2) {
			delay_ms(40);
			// "Fade up" any existing data on the screen.
			demo_invert_fade_up_blocks();
			// Add a new block to fade up.
			{
				uint8_t x_start = (row & 2) ? (30 - col) : col;
				for (uint8_t x = x_start; x < x_start + 2; ++x) {
					for (uint8_t y = row; y < row + 2; ++y) {
						tvtext_buffer[y * 32 + x] = 188; // 25%
					}
				}
			}
		}
	}
	// Fade up the remaining blocks.
	for (uint8_t i = 0; i < 3; ++i) {
		delay_ms(40);
		demo_invert_fade_up_blocks();
	}

	delay_ms(100);
	tvtext_flags ^= _BV(TVTEXT_INVERTED);
	tvtext_clear();
	delay_ms(500);

	// Enable the cursor.
	tvtext_flags |= _BV(TVTEXT_CURSOR_ENABLED);
}

void demo(void) {

  tvtext_init();

  // Wait a couple of seconds to allow the TV to get itself in gear.
  tvtext_flags &= ~_BV(TVTEXT_VISIBLE);
  delay_ms(2000);
  tvtext_flags |= _BV(TVTEXT_VISIBLE);

  //First loop in white on black !
  demo_basic();
  demo_countdown();
  demo_info();
  demo_viewport();
  demo_cursor();
  demo_invert();

  //Clean the business before leaving
  tvtext_clear();
  tvtext_reset_viewport_cursor_home();

}
