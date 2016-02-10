/**
 *  Lighting.h - Header file for including lighting behaviors
 *
 *  Created by Sean Whitsitt on August 7th 2015
 *  Copyright 2015 Mogi LLC.  All rights reserved
 */

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <curses.h>
#include <sstream>
#include <signal.h>
#include <ctime>
#include <sys/time.h>
#include <math.h>
#include <mogi/robot/hexapod.h>

#define LED_OFF                                     (0x00)                              ///< Turn the LED off
#define LED_RED                                     (0x01)                              ///< Turn the LED red
#define LED_GREEN                                   (0x02)                              ///< Turn the LED green
#define LED_BLUE                                    (0x04)                              ///< Turn the LED blue
#define LED_PINK                                    (LED_RED | LED_BLUE)                ///< Turn the LED pink
#define LED_CYAN                                    (LED_GREEN | LED_BLUE)              ///< Turn the LED cyan
#define LED_YELLOW                                  (LED_RED | LED_GREEN)               ///< Turn the LED yellow
#define LED_WHITE                                   (LED_RED | LED_GREEN | LED_BLUE)    ///< Turn the LED white

#define LIGHTING_ID_BLUE_SOLID                      0                                   ///< The id for the blue solid behavior
#define LIGHTING_ID_BLUE_BLINK                      1                                   ///< The id for the blue blinking behavior
#define LIGHTING_ID_BLUE_STROBE                     2                                   ///< The id for the blue strobing behavior
#define LIGHTING_ID_WHITE_SOLID                     3                                   ///< The id for the white solid behavior
#define LIGHTING_ID_WHITE_BLINK                     4                                   ///< The id for the white blinking behavior
#define LIGHTING_ID_WHITE_STROBE                    5                                   ///< The id for the white strobing behavior
#define LIGHTING_ID_PINK_SOLID                      6                                   ///< The id for the pink solid behavior
#define LIGHTING_ID_PINK_BLINK                      7                                   ///< The id for the pink blinking behavior
#define LIGHTING_ID_PINK_STROBE                     8                                   ///< The id for the pink strobing behavior
#define LIGHTING_ID_YELLOW_SOLID                    9                                   ///< The id for the yellow solid behavior
#define LIGHTING_ID_YELLOW_BLINK                    10                                   ///< The id for the yellow blinking behavior
#define LIGHTING_ID_YELLOW_STROBE                   11                                   ///< The id for the yellow strobing behavior
#define LIGHTING_ID_RED_SOLID                       12                                   ///< The id for the red solid behavior
#define LIGHTING_ID_RED_BLINK                       13                                   ///< The id for the red blinking behavior
#define LIGHTING_ID_RED_STROBE                      14                                   ///< The id for the red strobing behavior
#define LIGHTING_ID_GREEN_SOLID                     15                                  ///< The id for the green solid behavior
#define LIGHTING_ID_GREEN_BLINK                     16                                  ///< The id for the green blinking behavior
#define LIGHTING_ID_GREEN_STROBE                    17                                  ///< The id for the green strobing behavior
#define LIGHTING_ID_CYAN_SOLID                      18                                  ///< The id for the cyan solid behavior
#define LIGHTING_ID_CYAN_BLINK                      19                                  ///< The id for the cyan blinking behavior
#define LIGHTING_ID_CYAN_STROBE                     20                                  ///< The id for the cyan strobing behavior
#define LIGHTING_ID_MULTI_STROBE_1                  21                                  ///< The id for the first multicolor strobing behavior
#define LIGHTING_ID_MULTI_STROBE_2                  22                                  ///< The id for the second multicolor strobing behavior
#define LIGHTING_ID_OFF                             23                                  ///< The id for turing lighting off

#define LIGHTING_KEY_BLUE_SOLID                     'B'                                 ///< The key in manual mode for the blue solid behavior
#define LIGHTING_KEY_BLUE_BLINK                     'b'                                 ///< The key in manual mode for the blue blinking behavior
#define LIGHTING_KEY_BLUE_STROBE                    'h'                                 ///< The key in manual mode for the blue strobing behavior
#define LIGHTING_KEY_WHITE_SOLID                    'N'                                 ///< The key in manual mode for the white solid behavior
#define LIGHTING_KEY_WHITE_BLINK                    'n'                                 ///< The key in manual mode for the white blinking behavior
#define LIGHTING_KEY_WHITE_STROBE                   'j'                                 ///< The key in manual mode for the white strobing behavior
#define LIGHTING_KEY_PINK_SOLID                     'M'                                 ///< The key in manual mode for the pink solid behavior
#define LIGHTING_KEY_PINK_BLINK                     'm'                                 ///< The key in manual mode for the pink blinking behavior
#define LIGHTING_KEY_PINK_STROBE                    'k'                                 ///< The key in manual mode for the pink strobing behavior
#define LIGHTING_KEY_YELLOW_SOLID                   '<'                                 ///< The key in manual mode for the yellow solid behavior
#define LIGHTING_KEY_YELLOW_BLINK                   ','                                 ///< The key in manual mode for the yellow blinking behavior
#define LIGHTING_KEY_YELLOW_STROBE                  'l'                                 ///< The key in manual mode for the yellow strobing behavior
#define LIGHTING_KEY_RED_SOLID                      '>'                                 ///< The key in manual mode for the red solid behavior
#define LIGHTING_KEY_RED_BLINK                      '.'                                 ///< The key in manual mode for the red blinking behavior
#define LIGHTING_KEY_RED_STROBE                     ';'                                 ///< The key in manual mode for the red strobing behavior
#define LIGHTING_KEY_GREEN_SOLID                    '?'                                 ///< The key in manual mode for the green solid behavior
#define LIGHTING_KEY_GREEN_BLINK                    '/'                                 ///< The key in manual mode for the green blinking behavior
#define LIGHTING_KEY_GREEN_STROBE                   '\''                                ///< The key in manual mode for the green strobing behavior
#define LIGHTING_KEY_CYAN_SOLID                     'V'                                 ///< The key in manual mode for the cyan solid behavior
#define LIGHTING_KEY_CYAN_BLINK                     'v'                                 ///< The key in manual mode for the cyan blinking behavior
#define LIGHTING_KEY_CYAN_STROBE                    'g'                                 ///< The key in manual mode for the cyan strobing behavior
#define LIGHTING_KEY_MULTI_STROBE_1                 '['                                 ///< The key in manual mode for the first multicolor strobing behavior
#define LIGHTING_KEY_MULTI_STROBE_2                 ']'                                 ///< The key in manual mode for the second multicolor strobing behavior
#define LIGHTING_KEY_OFF                            'o'                                 ///< The key in manual mode for turing lighting off

#define LIGHTING_NAME_BLUE_SOLID                    "blue solid"                        ///< The name for the blue solid behavior
#define LIGHTING_NAME_BLUE_BLINK                    "blue blink"                        ///< The name for the blue blinking behavior
#define LIGHTING_NAME_BLUE_STROBE                   "blue strobe"                       ///< The name for the blue strobing behavior
#define LIGHTING_NAME_WHITE_SOLID                   "white solid"                       ///< The name for the white solid behavior
#define LIGHTING_NAME_WHITE_BLINK                   "white blink"                       ///< The name for the white blinking behavior
#define LIGHTING_NAME_WHITE_STROBE                  "white strobe"                      ///< The name for the white strobing behavior
#define LIGHTING_NAME_PINK_SOLID                    "pink solid"                        ///< The name for the pink solid behavior
#define LIGHTING_NAME_PINK_BLINK                    "pink blink"                        ///< The name for the pink blinking behavior
#define LIGHTING_NAME_PINK_STROBE                   "pink strobe"                       ///< The name for the pink strobing behavior
#define LIGHTING_NAME_YELLOW_SOLID                  "yellow solid"                      ///< The name for the yellow solid behavior
#define LIGHTING_NAME_YELLOW_BLINK                  "yellow blink"                      ///< The name for the yellow blinking behavior
#define LIGHTING_NAME_YELLOW_STROBE                 "yellow strobe"                     ///< The name for the yellow strobing behavior
#define LIGHTING_NAME_RED_SOLID                     "red solid"                         ///< The name for the red solid behavior
#define LIGHTING_NAME_RED_BLINK                     "red blink"                         ///< The name for the red blinking behavior
#define LIGHTING_NAME_RED_STROBE                    "red strobe"                        ///< The name for the red strobing behavior
#define LIGHTING_NAME_GREEN_SOLID                   "green solid"                       ///< The name for the green solid behavior
#define LIGHTING_NAME_GREEN_BLINK                   "green blink"                       ///< The name for the green blinking behavior
#define LIGHTING_NAME_GREEN_STROBE                  "green strobe"                      ///< The name for the green strobing behavior
#define LIGHTING_NAME_CYAN_SOLID                    "cyan solid"                        ///< The name for the cyan solid behavior
#define LIGHTING_NAME_CYAN_BLINK                    "cyan blink"                        ///< The name for the cyan blinking behavior
#define LIGHTING_NAME_CYAN_STROBE                   "cyan strobe"                       ///< The name for the cyan strobing behavior
#define LIGHTING_NAME_MULTI_STROBE_1                "multi1 strobe"                     ///< The name for the first multicolor strobing behavior
#define LIGHTING_NAME_MULTI_STROBE_2                "multi2 strobe"                     ///< The name for the second multicolor strobing behavior
#define LIGHTING_NAME_OFF                           "off"                               ///< The name for turing lighting off

void callLighting(int id, double period);

void lighting_blue_solid(double period);
void lighting_blue_blink(double period);
void lighting_blue_strobe(double period);
void lighting_white_solid(double period);
void lighting_white_blink(double period);
void lighting_white_strobe(double period);
void lighting_pink_solid(double period);
void lighting_pink_blink(double period);
void lighting_pink_strobe(double period);
void lighting_yellow_solid(double period);
void lighting_yellow_blink(double period);
void lighting_yellow_strobe(double period);
void lighting_red_solid(double period);
void lighting_red_blink(double period);
void lighting_red_strobe(double period);
void lighting_green_solid(double period);
void lighting_green_blink(double period);
void lighting_green_strobe(double period);
void lighting_cyan_solid(double period);
void lighting_cyan_blink(double period);
void lighting_cyan_strobe(double period);

void lighting_multicolor_strobe_1(double period);
void lighting_multicolor_strobe_2(double period);

void lighting_off(double period);

/// period is the length of time in seconds for a full rotation of all states
/// states are the lighting configurations, note that it will cycle through the values in each state until all dynamixels are addressed
void lighting(double period, std::vector<std::vector<int> > states);
