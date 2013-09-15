#include <stdlib.h>

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "tvtext.h"
#include "common.h"
#include "scroll.h"
#include "joypad.h"
#include "slides1.h"


// Number of slides.
#define SLIDE_COUNT 4

// Title slide
void title(void) {
  tvtext_puts_P(PSTR(
		     " TITLE\n\r"
		     "\n\n"
		     "    A slideshow\n\r"
		     "    with a simple AVR\n\r"
		     "\n\n"
		     "          Damien Graux\n\r"
		     "          github.com/dgraux\n\r"
		     "\n"
		     ">\n\r"
		     ));  
}

// End slide
void end(void) {
  tvtext_puts_P(PSTR(
		     " End of \n\r"
		     "\n"
		     " slideshow\n\r"
		     "\n"
		     " ......\n\r"
		     "\n"
		     ">"
		     ));  
}

// Slide number 1
void plan(void) {
	tvtext_puts_P(PSTR(
		" PLAN                           \n\r"
		"________________________________\n\r"
		" - First Part                   \n\r"
		" - Then                         \n\r"
		" - After                        \n\r"
		" - Conclusion...                \n\r"
		"________________________________\n\r"
		" Damien Graux                  1\n\r"
	));
}

// Slide 2
void lorem(void) {
	tvtext_puts_P(PSTR(
		" LOREM IPSUM                    \n\r"
		"________________________________\n\r"
		"\n"
		"\n"
		" - Text here                    \n\r"
		"\n"
		"\n"
		" - Text here also               \n\r"
		"\n"
		"________________________________\n\r"
		" Damien Graux                   \n\r"
	));
}


void slides1(void) {

  //tvtext_init();

  // Wait a couple of seconds to allow the TV to get itself in gear.
  //tvtext_flags &= ~_BV(TVTEXT_VISIBLE);
  //delay_ms(2000);
  //tvtext_flags |= _BV(TVTEXT_VISIBLE);

  // Simple text display to test the program
  //test_basic();

  // Selected game index.
  uint8_t selected_slide = 0;

  // Clean the screen and launch the title slide.
  tvtext_clear();
  title();

  for (;;) {
    // Wait for all keys to be released.
    while (~JOY_PIN & JOY_ALL);

    // Preload the joypad input value.
    uint8_t joypad = ~JOY_PIN & JOY_ALL;
    uint8_t old_joypad = joypad;

    // Used to animate the swapping of game graphics.
    int8_t swap_slide_scroll = 0;

    // We're still choosing a slide...
    uint8_t choosing_slide = 1;

    
    while (choosing_slide) {
		
      // Wait for sync.
      tvtext_wait_vsync();

      // Poll the joypad.
      joypad = ~JOY_PIN & JOY_ALL;
      if ((joypad == old_joypad) || (swap_slide_scroll != 0)) {
	joypad = JOY_NONE;
      } else {
	old_joypad = joypad;
      }

      // Handle the joypad.
      if (joypad & JOY_UP) {
	// Handle start of slideshow: do nothing if start
	if (selected_slide!=0){
	  --selected_slide;
	  swap_slide_scroll = -16;
	  choosing_slide = 0;
	}
      }
      if (joypad & JOY_DOWN) {
	// Handle end of slideshow: do nothing if end 
	if (selected_slide<(SLIDE_COUNT-1)){
	  ++selected_slide;
	  swap_slide_scroll = +16;
	  choosing_slide = 0;
	}
      }
      if (joypad & JOY_FIRE1) {
	// Handle end of slideshow: escape if end 
	if (selected_slide<(SLIDE_COUNT-1)){
	  ++selected_slide;
	  swap_slide_scroll = +16;
	  choosing_slide = 0;
	}
	else{
	  tvtext_clear();
	  return;
	}
      }

    }

    // Scroll out the slide.
    scroll_out();
    tvtext_clear();
    tvtext_offset_y -= (128 + 8) * 2;
    
    // Run the selected slide.
    switch (selected_slide) {
    case 0:
      title();
      break;
    case 1:
      plan();
      break;
    case 2:
      lorem();
      break;
    case 3:
      end();
      break;
    }
  }
}
