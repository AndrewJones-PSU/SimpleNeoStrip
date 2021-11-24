/**
 * This file hold the lightstrip effect implementations. These are the functions that
 *      update the lightstrip values. The actual lightstrip is not written to in
 *      these functions.
 *
 * @author Andrew Jones (https://github.com/AndrewJones-PSU)
 * @version 0.0.0
 */

#include "LSEffects.h"

// initialize solid color effect
int initSolidColor(CRGB *leds, int numLeds, CRGB color, int onSpacing, int offSpacing)
{
    int counter = 0;
    bool on = true;
    for (int i = 0; i < numLeds; i++) // loop through all LEDs
    {
        if (on) // handle on and off spacing
        {
            leds[i] = color;
            counter++;
            if (counter == onSpacing) // switch to off if onSpacing reached
            {
                on = false;
                counter = 0;
            }
        }
        else
        {
            leds[i] = CRGB::Black;
            counter++;
            if (counter == offSpacing) // switch to on if offSpacing reached
            {
                on = true;
                counter = 0;
            }
        }
    }
    return 0;
}

// initialize solid drip effect
int initSolidDrip(CRGB *leds, int numLeds, CRGB color, int onSpacing, int offSpacing)
{
    // this has the same init as solid color, so we'll just call that
    return initSolidColor(leds, numLeds, color, onSpacing, offSpacing);
}

// initialize solid cycle effect
int initSolidCycle(CRGB *leds, int numLeds, CRGB color, int cycleWaves)
{
    double scalar = 256.0 / numLeds * cycleWaves;
    for (int i = 0; i < numLeds; i++) // loop through all LEDs
    {
        if (((int)((i * scalar) / 128) % 2) == 0) // any negative outputs of sin() should be zero
            leds[i] = CRGB(sin8_C(i * scalar) * (color.r / 256.0), sin8_C(i * scalar) * (color.g / 256.0),
                           sin8_C(i * scalar) * (color.b / 256.0));
        else
            leds[i] = CRGB::Black;
    }
    return 0;
}

// initialize rainbow swirl effect
int initRainbowSwirl(CRGB *leds, int numLeds)
{

    return 0;
}

// initialize rainbow drip effect
int initRainbowDrip(CRGB *leds, int numLeds)
{
    return 0;
}

// initialize rainbow cycle effect
int initRainbowCycle(CRGB *leds, int numLeds, int cycleWaves)
{
    return 0;
}

// update solid color effect
int updateSolidColor(CRGB *leds, int numLeds)
{
    return 0;
}

// update solid drip effect
int updateSolidDrip(CRGB *leds, int numLeds)
{
    return 0;
}

// update solid cycle effect
int updateSolidCycle(CRGB *leds, int numLeds)
{
    return 0;
}

// update rainbow swirl effect
int updateRainbowSwirl(CRGB *leds, int numLeds)
{
    return 0;
}

// update rainbow drip effect
int updateRainbowDrip(CRGB *leds, int numLeds)
{
    return 0;
}

// update rainbow cycle effect
int updateRainbowCycle(CRGB *leds, int numLeds)
{
    return 0;
}
