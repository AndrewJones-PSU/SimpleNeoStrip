/**
 * This file is the main file for the SimpleNeoStrip program. It handles the
 *      initial setup and most of the user interaction that occurs within the program.
 *      Assigning the values of LEDs will be handled in seperate files, although when
 *      these values should be written to the lightstrip is handled here.
 * 
 * @author Andrew Jones (https://github.com/AndrewJones-PSU)
 * @version 0.0.0
 */

// Include the FastLED and LCD library
#include <FastLED.h>	// FastLED library which handles writing to the LED strip(s)
#include <glyphs.h>		// part of liquidmenu, this might be unecessary
#include <LiquidMenu.h> // Library for making menus for the 16x2 LCD

/* 
 * Definition of platform, this helps determine platform-specific code for timers
 */
#define PLATFORM_ARDUINO_UNO
// #define PLATFORM_ARDUINO_DUE

/* 
 * Definition of lightstrip parameters (strip count + leds per strip)
 */
#define LEDS_PER_STRIP 300
#define LED_STRIP_COUNT 1

// Definition of led values array
CRGB leds[LEDS_PER_STRIP * LED_STRIP_COUNT];

// Definitions for lightstrip pinouts (uno)
// Note: there are no definitions needed for lightstrip pinouts on the due,
// the due uses pins 25,26,27,28,14,15,29,11 for parallel output on port D.
#define LED_PIN_1 2

void setup()
{
// initialize lightstrips (uno)
#ifdef PLATFORM_ARDUINO_UNO
	FastLED.addLeds<WS2812, LED_PIN_1, GRB>(leds, LEDS_PER_STRIP * 0, LEDS_PER_STRIP);
#endif

// initialize lightstrips (due)
#ifdef PLATFORM_ARDUINO_DUE
	FastLED.addLeds<WS2811_PORTD, LED_STRIP_COUNT>(leds, LEDS_PER_STRIP);
#endif
}

void loop()
{
	// put your main code here, to run repeatedly:
}
