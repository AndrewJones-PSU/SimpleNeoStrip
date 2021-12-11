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
int initSolidColor(CRGB *leds, int numLeds, CRGB color)
{
    for (int i = 0; i < numLeds; i++) // loop through all LEDs
    {
        leds[i] = color;
    }
    return 0;
}

// initialize solid drip effect
int initSolidDrip(CRGB *leds, int numLeds, CRGB color, int onSpacing, int offSpacing)
{
    // this has the same init as solid color, so we'll just call that
    int counter = 0;
    bool on = true;
    for (int i = 0; i < numLeds; i++) // loop through all LEDs
    {
        if (on) // handle on and off spacing
        {
            leds[i] = color;
            counter++;
            if (counter == onSpacing && offSpacing != 0) // switch to off if onSpacing reached
            {
                on = false;
                counter = 0;
            }
        }
        else
        {
            leds[i] = CRGB::Black;
            counter++;
            if (counter == offSpacing && onSpacing != 0) // switch to on if offSpacing reached
            {
                on = true;
                counter = 0;
            }
        }
    }
    return 0;
}

// initialize solid cycle effect
int initSolidCycle(CRGB *leds, int numLeds, CRGB color, int cycleWaves)
{
    double waveScalar = 256.0 / numLeds * cycleWaves;
    uint8_t sineValue = 0;
    for (int i = 0; i < numLeds; i++) // loop through all LEDs
    {
        if (((int)((i * waveScalar) / 128) % 2) == 0) // any negative outputs of sin() should be zero
        {
            sineValue = sin8(i * waveScalar);
            leds[i] = CRGB(sineValue * (color.r / 256.0), sineValue * (color.g / 256.0), sineValue * (color.b / 256.0));
        }
        else
            leds[i] = CRGB::Black;
    }
    return 0;
}

// initialize rainbow swirl effect
int initRainbowSwirl(CRGB *leds, int numLeds)
{
    double hueScalar = 256.0 / numLeds;
    for (int i = 0; i < numLeds; i++) // loop through all LEDs
    {
        hsv2rgb_raw(CHSV(i * hueScalar, 255, 255), leds[i]);
    }
    return 0;
}

// initialize rainbow drip effect
int initRainbowDrip(CRGB *leds, int numLeds, int onSpacing, int offSpacing)
{
    double hueScalar = 256.0 / numLeds;
    int counter = 0;
    bool on = true;
    for (int i = 0; i < numLeds; i++) // loop through all LEDs
    {
        if (on) // handle on and off spacing
        {
            hsv2rgb_raw(CHSV(i * hueScalar, 255, 255), leds[i]);
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

// initialize rainbow cycle effect
int initRainbowCycle(CRGB *leds, int numLeds, int cycleWaves)
{
    double hueScalar = 256.0 / numLeds;
    double waveScalar = 256.0 / numLeds * cycleWaves;
    for (int i = 0; i < numLeds; i++) // loop through all LEDs
    {
        if (((int)((i * waveScalar) / 128) % 2) == 0) // any negative outputs of sin() should be zero
        {
            hsv2rgb_raw(CHSV(i * hueScalar, 255, sin8(i * waveScalar)), leds[i]);
        }
        else
        {
            leds[i] = CRGB::Black;
        }
    }
    return 0;
}

// update solid color effect
int updateSolidColor(CRGB *leds, int numLeds)
{
    // don't actually need to do anything here :)
    // in the event the way SolidColor functions is changed, this will be here
    return 0;
}

// update solid drip effect
int updateSolidDrip(CRGB *leds, int numLeds, boolean rotate)
{
    // if we are rotating, we need to shift the array
    if (rotate)
    {
        CRGB temp = leds[0];
        for (int i = 0; i < numLeds - 1; i++) // loop through all LEDs
        {
            leds[i] = leds[i + 1];
        }
        leds[numLeds - 1] = temp;
    }
    return 0;
}

// update solid cycle effect
int updateSolidCycle(CRGB *leds, int numLeds)
{
    // same as solid drip, but we always need to shift the array
    CRGB temp = leds[0];
    for (int i = 0; i < numLeds - 1; i++) // loop through all LEDs
    {
        leds[i] = leds[i + 1];
    }
    leds[numLeds - 1] = temp;
    return 0;
}

// update rainbow swirl effect
int updateRainbowSwirl(CRGB *leds, int numLeds)
{
    // same as solid cycle, shift array
    CRGB temp = leds[0];
    for (int i = 0; i < numLeds - 1; i++) // loop through all LEDs
    {
        leds[i] = leds[i + 1];
    }
    leds[numLeds - 1] = temp;
    return 0;
}

// update rainbow drip effect
int updateRainbowDrip(CRGB *leds, int numLeds)
{
    // same as solid cycle, shift array
    CRGB temp = leds[0];
    for (int i = 0; i < numLeds - 1; i++) // loop through all LEDs
    {
        leds[i] = leds[i + 1];
    }
    leds[numLeds - 1] = temp;
    return 0;
}

// update rainbow cycle effect
int updateRainbowCycle(CRGB *leds, int numLeds)
{
    // same as solid cycle, shift array
    CRGB temp = leds[0];
    for (int i = 0; i < numLeds - 1; i++) // loop through all LEDs
    {
        leds[i] = leds[i + 1];
    }
    leds[numLeds - 1] = temp;
    return 0;
}
