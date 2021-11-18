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
#include <FastLED.h>    // FastLED library which handles writing to the LED strip(s)
#include <LiquidMenu.h> // Library for making menus for the 16x2 LCD
#include <glyphs.h>     // part of liquidmenu, this might be unecessary

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

// Definition of settings (TODO: make some of these savable to EEPROM)
int on = 1;                    // Whether or not the lightstrip is on
int brightness = 64;           // Brightness of the lightstrip
CRGB solidColor = CRGB::White; // Color of the lightstrip

// Definition of the LCD object
LiquidCrystal lcd(LCD_RS_PIN, LCD_ENABLE_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

// Definition of menu strings/vars (for stuff that change)
const char *lightstrip_on_off = "on  -- ";

// Definition of the menu screens/lines
//

// 2nd Line that's always displayed on effect selection menus
LiquidLine LightstripStatusLine(3, 1, lightstrip_on_off, (int)FastLED.getBrightness());

// 1st Lines for effect names
LiquidLine SolidColorLine(2, 0, "Solid  Color");
LiquidLine SolidDripLine(3, 0, "Solid Drip");
LiquidLine SolidCycleLine(2, 0, "Solid  Cycle");
LiquidLine RainbowSwirlLine(1, 0, "Rainbow  Swirl");
LiquidLine RainbowDripLine(2, 0, "Rainbow Drip");
LiquidLine RainbowCycleLine(1, 0, "Rainbow  Cycle");

// Screens for effect selection
LiquidScreen SolidColorScreen(SolidColorLine, LightstripStatusLine);
LiquidScreen SolidDripScreen(SolidDripLine, LightstripStatusLine);
LiquidScreen SolidCycleScreen(SolidCycleLine, LightstripStatusLine);
LiquidScreen RainbowSwirlScreen(RainbowSwirlLine, LightstripStatusLine);
LiquidScreen RainbowDripScreen(RainbowDripLine, LightstripStatusLine);
LiquidScreen RainbowCycleScreen(RainbowCycleLine, LightstripStatusLine);

// Definition of the menu for effect selection, screens are added in setup()
LiquidMenu effectMenu(lcd);

// Lines and Screens for Settings Menu
//

// Brightness Setting
LiquidLine SettingBrightnessLine1(0, 0, "Brightness");
LiquidLine SettingBrightnessLine2(13, 1, brightness);
LiquidScreen SettingBrightnessScreen(SettingBrightnessLine1, SettingBrightnessLine2);

// Solid Color R Setting
LiquidLine SettingSolidColorRLine1(0, 0, "Solid Color R");
LiquidLine SettingSolidColorRLine2(13, 1, solidColor.r);
LiquidScreen SettingSolidColorRScreen(SettingSolidColorRLine1, SettingSolidColorRLine2);

// Solid Color G Setting
LiquidLine SettingSolidColorGLine1(0, 0, "Solid Color G");
LiquidLine SettingSolidColorGLine2(13, 1, solidColor.g);
LiquidScreen SettingSolidColorGScreen(SettingSolidColorGLine1, SettingSolidColorGLine2);

// Solid Color B Setting
LiquidLine SettingSolidColorBLine1(0, 0, "Solid Color B");
LiquidLine SettingSolidColorBLine2(13, 1, solidColor.b);
LiquidScreen SettingSolidColorBScreen(SettingSolidColorBLine1, SettingSolidColorBLine2);

// Definition of the settings menu, screens are added in setup()
LiquidMenu settingsMenu(lcd);

// Definition of the Menu System
LiquidSystem menuSystem(effectMenu, settingsMenu, 1);
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

    // clear lightstrip values (sanity check)
    FastLED.clear();
    FastLED.show();

    // initialize LCD Screens
    effectMenu.add_screen(SolidColorScreen);
    effectMenu.add_screen(SolidDripScreen);
    effectMenu.add_screen(SolidCycleScreen);
    effectMenu.add_screen(RainbowSwirlScreen);
    effectMenu.add_screen(RainbowDripScreen);
    effectMenu.add_screen(RainbowCycleScreen);

    settingsMenu.add_screen(SettingBrightnessScreen);
    settingsMenu.add_screen(SettingSolidColorRScreen);
    settingsMenu.add_screen(SettingSolidColorGScreen);
    settingsMenu.add_screen(SettingSolidColorBScreen);

    // initialize LCD
    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" SimpleNeoStrip ");
    lcd.setCursor(0, 1);
    lcd.print("================");

    // wait so we can see the splash screen
    delay(750);

    // switch over to menu system
    menuSystem.update();
}

void loop()
{
    // put your main code here, to run repeatedly:
}
