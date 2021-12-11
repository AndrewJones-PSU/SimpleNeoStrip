/**
 * This file hold the lightstrip effect definitions. These are the functions that
 *      update the lightstrip values. The actual lightstrip is not written to in
 *      these functions.
 *
 * @author Andrew Jones (https://github.com/AndrewJones-PSU)
 * @version 1.0.0
 */

#ifndef _LS_EFFECTS_H_
#define _LS_EFFECTS_H_

#include <FastLED.h> // FastLED library which handles writing to the LED strip(s)

// =================================
// initialization functions
// =================================

// initialize solid color effect
int initSolidColor(CRGB *leds, int numLeds, CRGB color);

// initialize solid drip effect
int initSolidDrip(CRGB *leds, int numLeds, CRGB color, int onSpacing, int offSpacing);

// initialize solid cycle effect
int initSolidCycle(CRGB *leds, int numLeds, CRGB color, int cycleWaves);

// initialize rainbow swirl effect
int initRainbowSwirl(CRGB *leds, int numLeds);

// initialize rainbow drip effect
int initRainbowDrip(CRGB *leds, int numLeds, int onSpacing, int offSpacing);

// initialize rainbow cycle effect
int initRainbowCycle(CRGB *leds, int numLeds, int cycleWaves);

// =================================
// update functions
// =================================

// update solid color effect
int updateSolidColor(CRGB *leds, int numLeds);

// update solid drip effect
int updateSolidDrip(CRGB *leds, int numLeds, boolean rotate);

// update solid cycle effect
int updateSolidCycle(CRGB *leds, int numLeds);

// update rainbow swirl effect
int updateRainbowSwirl(CRGB *leds, int numLeds);

// update rainbow drip effect
int updateRainbowDrip(CRGB *leds, int numLeds);

// update rainbow cycle effect
int updateRainbowCycle(CRGB *leds, int numLeds);

#endif