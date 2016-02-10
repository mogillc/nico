/**
 *  Lighting.cpp - lighting behavior definitions
 *
 *  Created by Sean Whitsitt on August 7th 2015
 *  Copyright 2015 Mogi LLC.  All rights reserved
 */

#include "Lighting.h"

using namespace Mogi;
using namespace Dynamixel;
using namespace Robot;

/// Externally required items
#define DEFAULT_DEBUG_LIGHTING_LINE                    5                               ///< The default line for debugging lighting
extern Hexapod hex;
extern double behavior_duration;
extern Handler dynamixelHandler;
extern void debug(std::string str, int line);

void callLighting(int id, double period) {
  switch (id) {
      
    case LIGHTING_ID_BLUE_SOLID:
      lighting_blue_solid(period);
      break;
    case LIGHTING_ID_BLUE_BLINK:
      lighting_blue_blink(period);
      break;
    case LIGHTING_ID_BLUE_STROBE:
      lighting_blue_strobe(period);
      break;
    case LIGHTING_ID_WHITE_SOLID:
      lighting_white_strobe(period);
      break;
    case LIGHTING_ID_WHITE_BLINK:
      lighting_white_blink(period);
      break;
    case LIGHTING_ID_WHITE_STROBE:
      lighting_white_strobe(period);
      break;
    case LIGHTING_ID_PINK_SOLID:
      lighting_pink_solid(period);
      break;
    case LIGHTING_ID_PINK_BLINK:
      lighting_pink_blink(period);
      break;
    case LIGHTING_ID_PINK_STROBE:
      lighting_pink_strobe(period);
      break;
    case LIGHTING_ID_YELLOW_SOLID:
      lighting_yellow_solid(period);
      break;
    case LIGHTING_ID_YELLOW_BLINK:
      lighting_yellow_blink(period);
      break;
    case LIGHTING_ID_YELLOW_STROBE:
      lighting_yellow_strobe(period);
      break;
    case LIGHTING_ID_RED_SOLID:
      lighting_red_solid(period);
      break;
    case LIGHTING_ID_RED_BLINK:
      lighting_red_blink(period);
      break;
    case LIGHTING_ID_RED_STROBE:
      lighting_red_strobe(period);
      break;
    case LIGHTING_ID_GREEN_SOLID:
      lighting_green_solid(period);
      break;
    case LIGHTING_ID_GREEN_BLINK:
      lighting_green_blink(period);
      break;
    case LIGHTING_ID_GREEN_STROBE:
      lighting_green_strobe(period);
      break;
    case LIGHTING_ID_CYAN_SOLID:
      lighting_cyan_solid(period);
      break;
    case LIGHTING_ID_CYAN_BLINK:
      lighting_cyan_blink(period);
      break;
    case LIGHTING_ID_CYAN_STROBE:
      lighting_cyan_strobe(period);
      break;
    case LIGHTING_ID_MULTI_STROBE_1:
      lighting_multicolor_strobe_1(period);
      break;
    case LIGHTING_ID_MULTI_STROBE_2:
      lighting_multicolor_strobe_2(period);
      break;
    case LIGHTING_ID_OFF:
      lighting_off(period);
      break;
    default:
      break;
  }
}


void lighting_blue_blink(double period) {
  std::vector<std::vector<int> > states;
  std::vector<int> color_state;
  color_state.push_back(LED_BLUE);
  states.push_back(color_state);
  std::vector<int> off_state;
  off_state.push_back(LED_OFF);
  states.push_back(off_state);
  
  lighting(period, states);
}

void lighting_blue_solid(double period) {
  std::vector<std::vector<int> > states;
  std::vector<int> color_state;
  color_state.push_back(LED_BLUE);
  states.push_back(color_state);
  lighting(period, states);
}

void lighting_blue_strobe(double period) {
  std::vector<std::vector<int> > states;
  std::vector<int> strobe;
  strobe.push_back(LED_BLUE);
  strobe.push_back(LED_OFF);
  states.push_back(strobe);
  
  lighting(period, states);
}


void lighting_white_blink(double period) {
  std::vector<std::vector<int> > states;
  std::vector<int> color_state;
  color_state.push_back(LED_WHITE);
  states.push_back(color_state);
  std::vector<int> off_state;
  off_state.push_back(LED_OFF);
  states.push_back(off_state);
  
  lighting(period, states);
}

void lighting_white_solid(double period) {
  std::vector<std::vector<int> > states;
  std::vector<int> color_state;
  color_state.push_back(LED_WHITE);
  states.push_back(color_state);
  lighting(period, states);
}

void lighting_white_strobe(double period) {
  std::vector<std::vector<int> > states;
  std::vector<int> strobe;
  strobe.push_back(LED_WHITE);
  strobe.push_back(LED_OFF);
  states.push_back(strobe);
  
  lighting(period, states);
}

void lighting_pink_blink(double period) {
  std::vector<std::vector<int> > states;
  std::vector<int> color_state;
  color_state.push_back(LED_PINK);
  states.push_back(color_state);
  std::vector<int> off_state;
  off_state.push_back(LED_OFF);
  states.push_back(off_state);
  
  lighting(period, states);
}

void lighting_pink_solid(double period) {
  std::vector<std::vector<int> > states;
  std::vector<int> color_state;
  color_state.push_back(LED_PINK);
  states.push_back(color_state);
  lighting(period, states);
}

void lighting_pink_strobe(double period) {
  std::vector<std::vector<int> > states;
  std::vector<int> strobe;
  strobe.push_back(LED_PINK);
  strobe.push_back(LED_OFF);
  states.push_back(strobe);
  
  lighting(period, states);
}

void lighting_yellow_blink(double period) {
  std::vector<std::vector<int> > states;
  std::vector<int> color_state;
  color_state.push_back(LED_YELLOW);
  states.push_back(color_state);
  std::vector<int> off_state;
  off_state.push_back(LED_OFF);
  states.push_back(off_state);
  
  lighting(period, states);
}

void lighting_yellow_solid(double period) {
  std::vector<std::vector<int> > states;
  std::vector<int> color_state;
  color_state.push_back(LED_YELLOW);
  states.push_back(color_state);
  lighting(period, states);
}

void lighting_yellow_strobe(double period) {
  std::vector<std::vector<int> > states;
  std::vector<int> strobe;
  strobe.push_back(LED_YELLOW);
  strobe.push_back(LED_OFF);
  states.push_back(strobe);
  
  lighting(period, states);
}

void lighting_red_blink(double period) {
  std::vector<std::vector<int> > states;
  std::vector<int> color_state;
  color_state.push_back(LED_RED);
  states.push_back(color_state);
  std::vector<int> off_state;
  off_state.push_back(LED_OFF);
  states.push_back(off_state);
  
  lighting(period, states);
}

void lighting_red_solid(double period) {
  std::vector<std::vector<int> > states;
  std::vector<int> color_state;
  color_state.push_back(LED_RED);
  states.push_back(color_state);
  lighting(period, states);
}

void lighting_red_strobe(double period) {
  std::vector<std::vector<int> > states;
  std::vector<int> strobe;
  strobe.push_back(LED_RED);
  strobe.push_back(LED_OFF);
  states.push_back(strobe);
  
  lighting(period, states);
}

void lighting_green_blink(double period) {
  std::vector<std::vector<int> > states;
  std::vector<int> color_state;
  color_state.push_back(LED_GREEN);
  states.push_back(color_state);
  std::vector<int> off_state;
  off_state.push_back(LED_OFF);
  states.push_back(off_state);
  
  lighting(period, states);
}

void lighting_green_solid(double period) {
  std::vector<std::vector<int> > states;
  std::vector<int> color_state;
  color_state.push_back(LED_GREEN);
  states.push_back(color_state);
  lighting(period, states);
}

void lighting_green_strobe(double period) {
  std::vector<std::vector<int> > states;
  std::vector<int> strobe;
  strobe.push_back(LED_GREEN);
  strobe.push_back(LED_OFF);
  states.push_back(strobe);
  
  lighting(period, states);
}

void lighting_cyan_strobe(double period) {
  std::vector<std::vector<int> > states;
  std::vector<int> strobe;
  strobe.push_back(LED_CYAN);
  strobe.push_back(LED_OFF);
  states.push_back(strobe);
  
  lighting(period, states);
}

void lighting_cyan_blink(double period) {
  std::vector<std::vector<int> > states;
  std::vector<int> color_state;
  color_state.push_back(LED_CYAN);
  states.push_back(color_state);
  std::vector<int> off_state;
  off_state.push_back(LED_OFF);
  states.push_back(off_state);
  
  lighting(period, states);
}

void lighting_cyan_solid(double period) {
  std::vector<std::vector<int> > states;
  std::vector<int> color_state;
  color_state.push_back(LED_CYAN);
  states.push_back(color_state);
  lighting(period, states);
}

void lighting_multicolor_strobe_1(double period) {
  std::vector<std::vector<int> > states;

  // yellow blue off state
  std::vector<int> yellow_blue;
  yellow_blue.push_back(LED_YELLOW);
  yellow_blue.push_back(LED_BLUE);
  yellow_blue.push_back(LED_OFF);
  states.push_back(yellow_blue);

  // blue pink off state
  std::vector<int> blue_pink;
  blue_pink.push_back(LED_BLUE);
  blue_pink.push_back(LED_OFF);
  blue_pink.push_back(LED_PINK);
  states.push_back(blue_pink);

  // pink red off state
  std::vector<int> pink_red;
  pink_red.push_back(LED_OFF);
  pink_red.push_back(LED_PINK);
  pink_red.push_back(LED_RED);
  states.push_back(pink_red);

  // red yellow off state
  std::vector<int> red_yellow;
  red_yellow.push_back(LED_RED);
  red_yellow.push_back(LED_YELLOW);
  red_yellow.push_back(LED_OFF);
  states.push_back(red_yellow);

  lighting(period, states);
}

void lighting_multicolor_strobe_2(double period) {
  std::vector<std::vector<int> > states;

  // green blue off state
  std::vector<int> green_blue;
  green_blue.push_back(LED_GREEN);
  green_blue.push_back(LED_BLUE);
  green_blue.push_back(LED_OFF);
  green_blue.push_back(LED_OFF);
  states.push_back(green_blue);

  // blue pink off state
  std::vector<int> blue_pink;
  blue_pink.push_back(LED_BLUE);
  blue_pink.push_back(LED_OFF);
  blue_pink.push_back(LED_OFF);
  blue_pink.push_back(LED_PINK);
  states.push_back(blue_pink);

  // pink cyan off state
  std::vector<int> pink_cyan;
  pink_cyan.push_back(LED_OFF);
  pink_cyan.push_back(LED_OFF);
  pink_cyan.push_back(LED_PINK);
  pink_cyan.push_back(LED_CYAN);
  states.push_back(pink_cyan);

  // cyan green off state
  std::vector<int> cyan_green;
  cyan_green.push_back(LED_CYAN);
  cyan_green.push_back(LED_GREEN);
  cyan_green.push_back(LED_OFF);
  cyan_green.push_back(LED_OFF);
  states.push_back(cyan_green);

  lighting(period, states);
}

void lighting_off(double period) {
  std::vector<std::vector<int> > states;
  std::vector<int> off;
  off.push_back(LED_OFF);
  states.push_back(off);

  lighting(period, states);
}

/// period is the length of time in seconds for a full rotation of all states
/// states are the lighting configurations, note that it will cycle through the values in each state until all dynamixels are addressed
void lighting(double period, std::vector<std::vector<int> > states) {
  std::map<unsigned char, Motor*> dynamixels = hex.dyn;                                   // Get the dynamixels
  int num_states = states.size();                                                   // Get the number of states that we have
  int id_state = ((int)(behavior_duration / period / num_states)) % num_states;     // Get the current state id based on the period and number of states (should iterate from 0 to num_states - 1)
  int offset = ((int)(behavior_duration / period)) % dynamixels.size();             // Get the offset for any rotation we might want to do
  std::vector<int> state = states.at(id_state);                                     // Get the curren state from the states
  int id_color = 0;                                                                 // Set up the color id
	for (std::map<unsigned char, Motor*>::iterator it = dynamixels.begin();
		 it != dynamixels.end();
		 ++it) {                                  // Loop iteration over each dynamixel
		unsigned char ii = it->first;
		Motor* dynamixel = it->second;
    int id_dynamixel = (ii + offset) % dynamixels.size();                           // Get the dynamixel to set based on the index and the offset
    dynamixel->setByte(REG_LED, state.at(id_color));                   // Set the dynamixel based on the current color id and the given state
    id_color++;                                                                     // Increment the color id
    if (id_color >= state.size()) {                                                 // Ensure that the color id is within the number of colors available in this state
      id_color = 0;                                                                 // Reset the color id if necessary
    }                                                                               // End if
  }                                                                                 // End for
  // update the dynamixels
  dynamixelHandler.pushInstruction(new Instruction(Instruction::SYNC_WRITE, DataRange(REG_LED, 1)));
}
