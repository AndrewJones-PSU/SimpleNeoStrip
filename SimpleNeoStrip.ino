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
#include <FastLED.h>       // FastLED library which handles writing to the LED strip(s)
#include <LiquidCrystal.h> // LCD library which handles the LCD display

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

// Definitions for Liquid Crystal Pinouts
#define LCD_RS_PIN 8
#define LCD_ENABLE_PIN 9
#define LCD_D4_PIN 4
#define LCD_D5_PIN 5
#define LCD_D6_PIN 6
#define LCD_D7_PIN 7

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
    settingSolidColorOnSpacing,
    settingSolidColorOffSpacing
};

// Definition of settings (TODO: make some of these savable to EEPROM)
uint8_t lightstripOn = 1;                  // Whether or not the lightstrip is on
uint8_t menuindex = 0;                     // Which menu are we on, 0 = effects, 1 = settings
effects effectindex = effectSolidColor;    // Which effect are we on
setting settingindex = settingSolidColorR; // Which setting are we on in the menu
uint8_t brightness = 64;                   // Brightness of the lightstrip
CRGB solidColorColor = CRGB::White;        // Color of the lightstrip
uint8_t SolidColorOnSpacing = 1;           // # of on pixels for solid color between segments
uint8_t SolidColorOffSpacing = 0;          // # of off pixels for solid color between segments

// Definition of the LCD object
LiquidCrystal lcd(LCD_RS_PIN, LCD_ENABLE_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

void setup()
{
    noInterrupts(); // disable all interrupts until we finish setup

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

    // initialize LCD
    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F(" SimpleNeoStrip "));
    lcd.setCursor(0, 1);
    lcd.print(F("================"));

    // wait so we can see the splash screen
    delay(750);

    interrupts(); // enable interrupts
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
    universalTimerInterruptHandler(); // call handler (allows uno+due to use same code)
}

#endif

//#ifdef PLATFORM_ARDUINO_DUE
void duetimerinit()
{
}

//#endif

void universalTimerInterruptHandler()
{
    // TODO: update lightstrip effect values

    FastLED.show(); // write values to lightstrips

    // TODO: check buttons + if we need to update LCD
}

void loop()
{
    // put your main code here, to run repeatedly:
}

void updateLCD()
{
    lcd.setCursor(0, 0);
    if (menuindex == 0) // if on effects menu
    {
        switch (effectindex) // print top menu line based on effect name
        {
        case effectSolidColor:
            lcd.println(F("  Solid  Color  "));
            break;
        case effectSolidDrip:
            lcd.println(F("   Solid Drip   "));
            break;
        case effectSolidCycle:
            lcd.println(F("  Solid  Cycle  "));
            break;
        case effectRainbowSwirl:
            lcd.println(F(" Rainbow  Swirl "));
            break;
        case effectRainbowDrip:
            lcd.println(F("  Rainbow Drip  "));
            break;
        case effectRainbowCycle:
            lcd.println(F(" Rainbow  Cycle "));
            break;
        default:
            lcd.println(F(" Unknown Effect "));
            break;
        }
        // print bottom menu line based on whether lightstrip is on or off
        // also print spacing + overwrite brightness value
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
            lcd.println(F("Solid Color R   "));
            lcd.print(F("                ")); // clear old value
            lcd.setCursor(13, 1);
            lcd.print(solidColorColor.r);
            break;
        case settingSolidColorG:
            lcd.println(F("Solid Color G   "));
            lcd.print(F("                ")); // clear old value
            lcd.setCursor(13, 1);
            lcd.print(solidColorColor.g);
            break;
        case settingSolidColorB:
            lcd.println(F("Solid Color B   "));
            lcd.print(F("                ")); // clear old value
            lcd.setCursor(13, 1);
            lcd.print(solidColorColor.b);
            break;
        case settingSolidColorOnSpacing:
            lcd.println(F("SolClrOnSpacing "));
            lcd.print(F("                ")); // clear old value
            lcd.setCursor(13, 1);
            lcd.print(SolidColorOnSpacing);
            break;
        case settingSolidColorOffSpacing:
            lcd.println(F("SolClrOffSpacing"));
            lcd.print(F("                ")); // clear old value
            lcd.setCursor(13, 1);
            lcd.print(SolidColorOffSpacing);
            break;
        default:
            lcd.println(F("Unknown  Setting"));
            lcd.print(F("Unknown  Setting"));
            break;
        }
    }
}
