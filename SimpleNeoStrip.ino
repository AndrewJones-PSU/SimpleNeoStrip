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
#include "LSEffects.h"     // Header file for the effects functions
#include <FastLED.h>       // FastLED library which handles writing to the LED strip(s)
#include <LiquidCrystal.h> // LCD library which handles the LCD display

// =================================
// Global variables + defines
// =================================

/*
 * Definition of platform, this helps determine platform-specific code for timers
 */
#define PLATFORM_ARDUINO_UNO
// #define PLATFORM_ARDUINO_DUE

/*
 * Definition of lightstrip parameters (strip count + leds per strip + total LEDs)
 */
#define LEDS_PER_STRIP 300
#define LED_STRIP_COUNT 1
uint16_t totalLEDs = LEDS_PER_STRIP * LED_STRIP_COUNT;

// Definition of led values array
CRGB leds[LEDS_PER_STRIP * LED_STRIP_COUNT];

// Definitions for lightstrip pinouts (uno)
// Note: there are no definitions needed for lightstrip pinouts on the due,
// the due uses pins 25,26,27,28,14,15,29,11 for parallel output on port D.
#define LED_PIN_1 2

// Definitions for Liquid Crystal Pinouts
#define LCD_RS_PIN 8
#define LCD_ENABLE_PIN 9
#define LCD_D4_PIN 4
#define LCD_D5_PIN 5
#define LCD_D6_PIN 6
#define LCD_D7_PIN 7

// Definition of LCD button input + handlers
#define LCD_BUTTON_PIN A0
// indicies for arrays:
// 0 = select button
// 1 = up button
// 2 = down button
// 3 = left button
// 4 = right button
uint8_t buttonSR[5] = {};               // shift registers, used for debouncing
uint8_t buttonState[5] = {};            // button states
uint8_t buttonLastState[5] = {};        // last button states
uint8_t buttonHoldTrigger[3] = {};      // trigger for hold times
uint8_t buttonHoldTimerCounter[3] = {}; // counter for button hold triggering
uint8_t buttonHoldCounter[3] = {};      // number of times button has triggered during current hold

// Definition for update flag, used to determine if the lightstrip needs to be updated
uint8_t updateFlag = 0;

// enums for effects and menu items
enum effects
{
    effectSolidColor,
    effectSolidDrip,
    effectSolidCycle,
    effectRainbowSwirl,
    effectRainbowDrip,
    effectRainbowCycle
};

enum setting
{
    settingSolidColorR,
    settingSolidColorG,
    settingSolidColorB,
    settingSolidDripRotates,
    settingDripOnSpacing,
    settingDripOffSpacing,
    settingCycleWaves
};

// Definition of settings (TODO: make some of these savable to EEPROM)
uint8_t lightstripOn = 1;                  // Whether or not the lightstrip is on
uint8_t menuindex = 0;                     // Which menu are we on, 0 = effects, 1 = settings
effects effectindex = effectSolidColor;    // Which effect are we on
setting settingindex = settingSolidColorR; // Which setting are we on in the menu
uint8_t brightness = 64;                   // Brightness of the lightstrip
CRGB solidColorColor = CRGB::White;        // Color of the lightstrip
uint8_t dripOnSpacing = 9;                 // # of on pixels for solid color between segments
uint8_t dripOffSpacing = 0;                // # of off pixels for solid color between segments
boolean solidDripRotates = true;           // Whether or not the solid drip effect rotates
uint8_t cycleWaves = 10;                   // # of waves for cycle effect

// Definition of the LCD object
LiquidCrystal lcd(LCD_RS_PIN, LCD_ENABLE_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

// =================================
// setup and init functions
// =================================

void setup()
{
// initialize uno-specific setups
#ifdef PLATFORM_ARDUINO_UNO
    // initialize uno timers
    unotimerinit();
    // initialize lightstrips for uno
    FastLED.addLeds<WS2812, LED_PIN_1, GRB>(leds, LEDS_PER_STRIP * 0, LEDS_PER_STRIP);
#endif

// initialize due-specific setups
#ifdef PLATFORM_ARDUINO_DUE
    // initialize due timers
    duetimerinit();
    // initialize lightstrips for due
    FastLED.addLeds<WS2811_PORTD, LED_STRIP_COUNT>(leds, LEDS_PER_STRIP);
#endif

    // clear lightstrip values (sanity check)
    FastLED.clear();
    FastLED.show();

    // initialize settings
    FastLED.setBrightness(brightness);
    // Serial.begin(9600); for debug when I do dumb things

    // initialize LCD and display splash screen
    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F(" SimpleNeoStrip "));
    lcd.setCursor(0, 1);
    lcd.print(F("================"));

    // wait so we can see the splash screen
    delay(750);
    // initialize our effect
    initEffect();
    // show menu
    updateLCD();
}

// initialize timers + interrupt handler for arduino uno
#ifdef PLATFORM_ARDUINO_UNO
void unotimerinit()
{
    /* timer1 register bits (for arduino uno) are set accordingly:
     *
     * this comment is excessively verbose, may be removed/shortened in the future
     * See pages 108-113 of the ATmega328P datasheet for more information on timer1 registers, link here:
     * https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
     *
     * TCCR1A[7:4] = 0000 (don't output timer data to pins)
     * TCCR1A[3:2] are reserved, leave as zeros
     * TCCR1A[1:0] = 00 (we want CTC mode, clarified with TCCR1B[4:3]:TCCR1A[1:0] = 0100)
     *
     * TCCR1B[7] = 0 (don't need input capture noise canceler)
     * TCCR1B[6] = 0 (don't need input capture edge select)
     * TCCR1B[5] is reserved, leave as zero
     * TCCR1B[4:3] = 01 (we want CTC mode, clarified with TCCR1B[4:3]:TCCR1A[1:0] = 0100)
     * TCCR1B[2:0] = 010 (we want a prescaler of 8, so TCCR1B[2:0] = 010)
     *
     * TCNT1 = 0x0000 (init counter to zero)
     *
     * OCR1A = 33333 (init output compare register to desired count, in our case 16MHz/8/60Hz = 33333)
     *
     * TIMSK1[7:6] are reserved, leave as zeros
     * TIMSK1[5] = 0 (don't need input capture interrupts)
     * TIMSK1[4:3] are reserved, leave as zeros
     * TIMSK1[2] = 0 (don't need output compare interrupt for channel B)
     * TIMSK1[1] = 1 (enable output compare interrupt for channel A)
     * TIMSK1[0] = 0 (don't need overflow interrupt)
     *
     * All other registers can be left at their default values
     */
    TCCR1A = 0b00000000; // set timer1 control register A
    TCCR1B = 0b00001010; // set timer1 control register B
    TCNT1 = 0;           // initialize counter value to 0
    OCR1A = 33333;       // initialize compare value to 33333
    TIMSK1 = 0b00000010; // enable timer compare interrupt for channel A
}

ISR(TIMER1_COMPA_vect) // timer compare interrupt service routine
{
    updateFlag = 1; // tell main loop to update the lightstrip
}

#endif

//#ifdef PLATFORM_ARDUINO_DUE
void duetimerinit()
{
}

//#endif

// =================================
// loop and runtime functions
// =================================

void loop()
{
    if (updateFlag)
    {
        updateFlag = 0;
        universalUpdateHandler();
    }
}

void universalUpdateHandler()
{
    // update lightstrip effect values
    updateEffect();
    // write values to lightstrips
    FastLED.show();

    // update the button states
    updateButtonStates();

    // flag for updating the LCD if a button is pressed/held
    bool shouldUpdateLCD = false;

    // check if we have any button pushes to process, handle them if so
    for (int i = 0; i < 5; i++)
        if (buttonState[i] != buttonLastState[i])
        {
            handleButtonPress(i);
            shouldUpdateLCD = true;
            break; // only handle one button press per interrupt
        }

    // do the same for button holds
    for (int i = 0; i < 3; i++)
        if (buttonHoldTrigger[i] == 1)
        {
            handleButtonHold(i);
            shouldUpdateLCD = true;
            break; // only handle one button press per interrupt
        }

    // if a button hold or press happened, update the LCD
    if (shouldUpdateLCD)
        updateLCD();

    // end of interrupt routine
}

// after changing to a new effect, initialize it
// this assumes effectindex is already set to the new effect
void initEffect()
{
    noInterrupts(); // disable interrupts until end of function, as initializing effects can take a while
    switch (effectindex)
    {
    case effectSolidColor:
        initSolidColor(leds, totalLEDs, solidColorColor);
        break;
    case effectSolidDrip:
        initSolidDrip(leds, totalLEDs, solidColorColor, dripOnSpacing, dripOffSpacing);
        break;
    case effectSolidCycle:
        initSolidCycle(leds, totalLEDs, solidColorColor, cycleWaves);
        break;
    case effectRainbowSwirl:
        initRainbowSwirl(leds, totalLEDs);
        break;
    case effectRainbowDrip:
        initRainbowDrip(leds, totalLEDs, dripOnSpacing, dripOffSpacing);
        break;
    case effectRainbowCycle:
        initRainbowCycle(leds, totalLEDs, cycleWaves);
        break;
    default:
        // if effectindex is invalid, init with red solid color with alternating on/off spacing
        initSolidDrip(leds, totalLEDs, CRGB::Red, 1, 1);
        break;
    }
    interrupts(); // re-enable interrupts
}

// updates the current effect
void updateEffect()
{
    switch (effectindex)
    {
    case effectSolidColor:
        updateSolidColor(leds, totalLEDs);
        break;
    case effectSolidDrip:
        updateSolidDrip(leds, totalLEDs, solidDripRotates);
        break;
    case effectSolidCycle:
        updateSolidCycle(leds, totalLEDs);
        break;
    case effectRainbowSwirl:
        updateRainbowSwirl(leds, totalLEDs);
        break;
    case effectRainbowDrip:
        updateRainbowDrip(leds, totalLEDs);
        break;
    case effectRainbowCycle:
        updateRainbowCycle(leds, totalLEDs);
        break;
    default:
        // if effectindex is invalid, update with drip effect (force no rotation)
        updateSolidDrip(leds, totalLEDs, false);
        break;
    }
}

// update the states of the buttons
// button states are stored as shift registers, which enables debouncing
void updateButtonStates()
{
    // read value from input pin
    int read = analogRead(LCD_BUTTON_PIN);
    // get which button is currently pressed
    int pushedindex = -1;
    if (read < 60) // right button
        pushedindex = 4;
    else if (read < 200) // up button
        pushedindex = 1;
    else if (read < 400) // down button
        pushedindex = 2;
    else if (read < 600) // left button
        pushedindex = 3;
    else if (read < 800) // select button
        pushedindex = 0;

    // loop through all buttons
    for (int i = 0; i < 5; i++)
    {
        buttonSR[i] <<= 1;                   // shift register
        buttonSR[i] |= (pushedindex == i);   // set bit to 1 if button is pressed
        buttonSR[i] &= 0b00001111;           // clear top 4 bits of SR
        buttonLastState[i] = buttonState[i]; // store last state
        if (buttonSR[i] == 0b00001111)
            buttonState[i] = 1;
        else if (buttonSR[i] == 0b00000000)
        {
            buttonState[i] = 0;
        }

        if (i <= 2) // only handle buttons that support/need hold functionality
        {
            if (buttonState[i] == 1)
            {
                buttonHoldTimerCounter[i]++;
                if (buttonHoldTimerCounter[i] > 30 && buttonState[i] == 1)
                {
                    buttonHoldCounter[i]++; // increment hold counter
                    buttonHoldTrigger[i] = 1;
                    buttonHoldTimerCounter[i] = 0;
                }
                else
                {
                    buttonHoldTrigger[i] = 0;
                }
            }
            else
            {
                buttonHoldTimerCounter[i] = 0;
                if (buttonLastState[i] == 0)  // don't reset hold counter until next tick of button being released
                    buttonHoldCounter[i] = 0; // this allows us to track whether the button was held after a release
            }
        }
    }
}

void handleButtonPress(uint8_t buttonIndex)
{
    switch (buttonIndex)
    {
    case 0: // select button
        // if depressing + we haven't held select long enough for menu swap, toggle lights
        if (buttonState[buttonIndex] == 0 && buttonHoldCounter[buttonIndex] < 2)
        {
            lightstripOn = !lightstripOn;
            if (lightstripOn)
                FastLED.setBrightness(brightness);
            else
                FastLED.setBrightness(0);
        }
        break;
    case 1: // up button
        // if pressing, increment either brightness or value depending on menu state
        if (buttonState[buttonIndex] == 1)
        {
            if (menuindex == 0) // if on effects menu
            {
                brightness++;
                if (lightstripOn)
                    FastLED.setBrightness(brightness);
            }
            else if (menuindex == 1) // if on settings menu
            {
                updateSettingValue(1);
                initEffect();
            }
            break;
        }
    case 2: // down button
        // if pressing, decrement either brightness or value depending on menu state
        if (buttonState[buttonIndex] == 1)
        {
            if (menuindex == 0) // if on effects menu
            {
                brightness--;
                if (lightstripOn)
                    FastLED.setBrightness(brightness);
            }
            else if (menuindex == 1) // if on settings menu
            {
                updateSettingValue(-1);
                initEffect();
            }
            break;
        }
    case 3: // left button
        // if pressing, cycle through either effects or settings depending on menu state
        if (buttonState[buttonIndex] == 1)
        {
            if (menuindex == 0) // if on effects menu
            {
                effectindex = (effects)(effectindex - 1);
                initEffect();
            }
            else if (menuindex == 1) // if on settings menu
                settingindex = (setting)(settingindex - 1);
            break;
        }

    case 4: // right button
        // if pressing, cycle through either effects or settings depending on menu state
        if (buttonState[buttonIndex] == 1)
        {
            if (menuindex == 0) // if on effects menu
            {
                effectindex = (effects)(effectindex + 1);
                initEffect();
            }
            else if (menuindex == 1) // if on settings menu
                settingindex = (setting)(settingindex + 1);
            break;
        }
    }
}

void handleButtonHold(uint8_t buttonIndex)
{
    switch (buttonIndex)
    {
    case 0: // select button
        // if held, swap between effects and settings menus (but only once)
        if (buttonHoldTrigger[buttonIndex] == 1 && buttonHoldCounter[buttonIndex] == 2)
        {
            if (menuindex == 0)
                menuindex = 1;
            else
                menuindex = 0;
        }
        break;
    case 1: // up button
        // if held, increment either brightness or value depending on menu state
        if (buttonHoldTrigger[buttonIndex] == 1)
        {
            if (menuindex == 0) // if on effects menu
            {
                brightness += 10;
                if (lightstripOn)
                    FastLED.setBrightness(brightness);
            }
            else if (menuindex == 1) // if on settings menu
            {
                updateSettingValue(10);
                initEffect();
            }
        }
        break;
    case 2: // down button
        // if held, decrement either brightness or value depending on menu state
        if (buttonHoldTrigger[buttonIndex] == 1)
        {
            if (menuindex == 0) // if on effects menu
            {
                brightness -= 10;
                if (lightstripOn)
                    FastLED.setBrightness(brightness);
            }
            else if (menuindex == 1) // if on settings menu
            {
                updateSettingValue(-10);
                initEffect();
            }
        }
        break;
    }
}

void updateSettingValue(uint8_t value)
{
    switch (settingindex)
    {
    case setting::settingSolidColorR:
        solidColorColor.r += value;
        break;
    case setting::settingSolidColorG:
        solidColorColor.g += value;
        break;
    case setting::settingSolidColorB:
        solidColorColor.b += value;
        break;
    case setting::settingSolidDripRotates:
        solidDripRotates = !solidDripRotates;
        break;
    case setting::settingDripOnSpacing:
        dripOnSpacing += value;
        break;
    case setting::settingDripOffSpacing:
        dripOffSpacing += value;
        break;
    case setting::settingCycleWaves:
        cycleWaves += value;
        break;
    }
}

void updateLCD()
{
    lcd.setCursor(0, 0);
    if (menuindex == 0) // if on effects menu
    {
        switch (effectindex) // print top menu line based on effect name
        {
        case effectSolidColor:
            lcd.print(F("  Solid  Color  "));
            break;
        case effectSolidDrip:
            lcd.print(F("SolidColor  Drip"));
            break;
        case effectSolidCycle:
            lcd.print(F("SolidColor Cycle"));
            break;
        case effectRainbowSwirl:
            lcd.print(F(" Rainbow  Swirl "));
            break;
        case effectRainbowDrip:
            lcd.print(F("  Rainbow Drip  "));
            break;
        case effectRainbowCycle:
            lcd.print(F(" Rainbow  Cycle "));
            break;
        default:
            lcd.print(F(" Unknown Effect "));
            break;
        }
        // print bottom menu line based on whether lightstrip is on or off
        // also print spacing + overwrite brightness value
        lcd.setCursor(0, 1);
        if (lightstripOn == 0)
        {
            lcd.print(F("   Off --       "));
        }
        else
        {
            lcd.print(F("   On  --       "));
        }

        // set cursor back, print brightness
        lcd.setCursor(10, 1);
        lcd.print(brightness);
    }
    if (menuindex == 1) // if on settings menu
    {
        // print top and bottom menu lines based on setting name/value
        switch (settingindex)
        {
        case settingSolidColorR:
            lcd.print(F("Solid Color R   "));
            lcd.setCursor(0, 1);
            lcd.print(F("                ")); // clear old value
            lcd.setCursor(13, 1);
            lcd.print(solidColorColor.r);
            break;
        case settingSolidColorG:
            lcd.print(F("Solid Color G   "));
            lcd.setCursor(0, 1);
            lcd.print(F("                ")); // clear old value
            lcd.setCursor(13, 1);
            lcd.print(solidColorColor.g);
            break;
        case settingSolidColorB:
            lcd.print(F("Solid Color B   "));
            lcd.setCursor(0, 1);
            lcd.print(F("                ")); // clear old value
            lcd.setCursor(13, 1);
            lcd.print(solidColorColor.b);
            break;
        case settingSolidDripRotates:
            lcd.print(F("SolidDripRotates"));
            lcd.setCursor(0, 1);
            lcd.print(F("                ")); // clear old value
            lcd.setCursor(10, 1);
            if (solidDripRotates == false)
                lcd.print(F("Static"));
            else
                lcd.print(F("Rotate"));
            break;
        case settingDripOnSpacing:
            lcd.print(F("Drip  On-Spacing"));
            lcd.setCursor(0, 1);
            lcd.print(F("                ")); // clear old value
            lcd.setCursor(13, 1);
            lcd.print(dripOnSpacing);
            break;
        case settingDripOffSpacing:
            lcd.print(F("Drip Off-Spacing"));
            lcd.setCursor(0, 1);
            lcd.print(F("                ")); // clear old value
            lcd.setCursor(13, 1);
            lcd.print(dripOffSpacing);
            break;
        case settingCycleWaves:
            lcd.print(F("  Cycle  Waves  "));
            lcd.setCursor(0, 1);
            lcd.print(F("                ")); // clear old value
            lcd.setCursor(13, 1);
            lcd.print(cycleWaves);
            break;
        default:
            lcd.print(F("Unknown  Setting"));
            lcd.setCursor(0, 1);
            lcd.print(F("Unknown  Setting"));
            break;
        }
    }
}
