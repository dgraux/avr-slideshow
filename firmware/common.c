#include <avr/pgmspace.h>

#include "tvtext.h"
#include "common.h"

// Wait for a particular number of ms.
void delay_ms(int ms) {
	ms /= 20; // At 50 frames per second, there are 20ms per frame.
	while (ms-- > 0) tvtext_wait_vsync();
}

// "Type" a string onto the screen (delays a little after each character).
void type_string_P(const char* s) {
	char c;
	while ((c = pgm_read_byte(s++))) {
		tvtext_putc(c);
		for (uint8_t delay = 0; delay < 3; ++delay) {
			tvtext_wait_vsync();
		}
	}
}
