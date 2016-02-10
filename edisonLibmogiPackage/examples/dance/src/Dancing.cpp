
/**
 *  Dancing.cpp - program for following dance moves from a script or from command line input
 *
 *  Created by Sean Whitsitt on July 20th 2015
 *  Copyright 2015 Mogi LLC.  All rights reserved
 */

// includes
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
#include "danceState.h"
#include "Lighting.h"
#include "danceParameters.h"
#include <mogi/robot/hexapod.h>
#include <mogi.h>


// OSC includes
#include <pthread.h>
#include <algorithm>
#include "../osc/OscReceivedElements.h"
#include "../osc/OscPacketListener.h"
#include "../ip/UdpSocket.h"

using namespace Mogi;
using namespace Robot;
using namespace Dynamixel;

// OSC defines
#define DEBUG false
#define PORT 7000   // todo group should be in config , env, or arg
#define ALL_BOT_GROUP "all"
#define WRIST_BOT_GROUP "wrist"
#define LOCK_WRIST_BOT_GROUP "lock"
#define DEFAULT_BOT_GROUP "n1"

//#define FLYING_ANOUK	// Enables a different behavior for the flying anouk hexapods
//#define DANGER_ZONE		// Disables the motor shutdown!  Be careful!



// Exit modes
#define EXIT_SUCCESS                              0                                 ///< The exit status for success
#define EXIT_NCURSES_FAILED                       1                                 ///< The exit status that indicates that curses failed
#define EXIT_USAGE                                2                                 ///< The exit status that indicates a command line argument issue

// behavior ids
#define BEHAVIOR_ID_STAND                         0                                 ///< The id for the standing behavior, i.e., put feet on ground
#define BEHAVIOR_ID_FIST_BUMP_RIGHT               1                                 ///< The id for the right fist bump behavior
#define BEHAVIOR_ID_FIST_BUMP_LEFT                2                                 ///< The id for the left fist bump behavior
#define BEHAVIOR_ID_FIST_BUMP_DOUBLE              3                                 ///< The id for the double fist bump behavior
#define BEHAVIOR_ID_LEG_WAVE_CLOCKWISE            4                                 ///< The id for the leg wave clockwise behavior
#define BEHAVIOR_ID_LEG_WAVE_COUNTER_CLOCKWISE    5                                 ///< The id for the leg wave counter clockwise behavior
#define BEHAVIOR_ID_BODY_WAVES                    6                                 ///< The id for the body waves behavior
#define BEHAVIOR_ID_SHUFFLE_RIGHT                 7                                 ///< The id for the shuffle right behavior
#define BEHAVIOR_ID_SHUFFLE_LEFT                  8                                 ///< The id for the shuffle left behavior
#define BEHAVIOR_ID_SHUFFLE_FORWARD               9                                 ///< The id for the shuffle forward behavior
#define BEHAVIOR_ID_SHUFFLE_BACK                  10                                ///< The id for the shuffle back behavior
#define BEHAVIOR_ID_CORPSE                        11                                ///< The id for the corpse behavior
#define BEHAVIOR_ID_RESURRECTION                  12                                ///< The id for the resurrection behavior
#define BEHAVIOR_ID_CIRCLE_RIGHT                  13                                ///< The id for the circle right behavior
#define BEHAVIOR_ID_CIRCLE_LEFT                   14                                ///< The id for the circle left behavior
#define BEHAVIOR_ID_FLY                           15                                ///< The id for the circle left behavior
#define BEHAVIOR_ID_KILL                        16                                ///< The id for the circle left behavior
#define BEHAVIOR_ID_HIP_SWING                   17                                ///< The id for the circle left behavior
#define BEHAVIOR_ID_BODY_WAVE_BUMP                   18                                ///< The id for the circle left behavior


// behavior cli input
#define BEHAVIOR_KEY_STAND                         'p'                              ///< The key in manual mode for the standing behavior
#define BEHAVIOR_KEY_FIST_BUMP_RIGHT               'e'                              ///< The key in manual mode for the right fist bump
#define BEHAVIOR_KEY_FIST_BUMP_LEFT                'q'                              ///< The key in manual mode for the left fist bump
#define BEHAVIOR_KEY_FIST_BUMP_DOUBLE              '1'                              ///< The key in manual mode for the double first bump
#define BEHAVIOR_KEY_LEG_WAVE_CLOCKWISE            'z'                              ///< The key in manual mode for the clockwise leg wave
#define BEHAVIOR_KEY_LEG_WAVE_COUNTER_CLOCKWISE    'c'                              ///< The key in manual mode for the counter clockwise leg wave
#define BEHAVIOR_KEY_BODY_WAVES                    'x'                              ///< The key in manual mode for the body waves
#define BEHAVIOR_KEY_SHUFFLE_RIGHT                 'd'                              ///< The key in manual mode for shuffling right
#define BEHAVIOR_KEY_SHUFFLE_LEFT                  'a'                              ///< The key in manual mode for shuffling left
#define BEHAVIOR_KEY_SHUFFLE_FORWARD               'w'                              ///< The key in manual mode for shuffling forward
#define BEHAVIOR_KEY_SHUFFLE_BACK                  's'                              ///< The key in manual mode for shuffling backwards
#define BEHAVIOR_KEY_CORPSE                        '2'                              ///< The key in manual mode for the corpse
#define BEHAVIOR_KEY_RESURRECTION                  '3'                              ///< The key in manual mode for the resurrection
#define BEHAVIOR_KEY_CIRCLE_RIGHT                  '5'                              ///< The key in manual mode for circling right
#define BEHAVIOR_KEY_CIRCLE_LEFT                   '4'                              ///< The key in manual mode for circling left
#define BEHAVIOR_KEY_FLY                           '6'                              ///< The key in manual mode for flying
#define BEHAVIOR_KEY_KILL                          27                               ///< The key in manual mode for shutting down
#define BEHAVIOR_KEY_HIP_SWING                     '7'                              ///< The key in manual mode for hip swing (incomplete)
#define BEHAVIOR_KEY_BODY_WAVE_BUMP                '8'                              ///< The key in manual mode for body wave bump

// behavior names
#define BEHAVIOR_NAME_STAND                         "stand"                         ///< Stand still name (for script)
#define BEHAVIOR_NAME_FIST_BUMP_RIGHT               "fist_bump_right"               ///< Right fist bump behavior name (for script)
#define BEHAVIOR_NAME_FIST_BUMP_LEFT                "fist_bump_left"                ///< Left fist bump behavior name (for script)
#define BEHAVIOR_NAME_FIST_BUMP_DOUBLE              "fist_bump_double"              ///< Double fist bump behavior name (for script)
#define BEHAVIOR_NAME_LEG_WAVE_CLOCKWISE            "leg_wave_clockwise"            ///< Clockwise leg wave behavior name (for script)
#define BEHAVIOR_NAME_LEG_WAVE_COUNTER_CLOCKWISE    "leg_wave_counter_clockwise"    ///< Counter clockwise leg wave behavior name (for script)
#define BEHAVIOR_NAME_BODY_WAVES                    "body_waves"                    ///< Body waves behavior name (for script)
#define BEHAVIOR_NAME_SHUFFLE_RIGHT                 "shuffle_right"                 ///< Shuffle right behavior name (for script)
#define BEHAVIOR_NAME_SHUFFLE_LEFT                  "shuffle_left"                  ///< Shuffle left behavior name (for script)
#define BEHAVIOR_NAME_SHUFFLE_FORWARD               "shuffle_forward"               ///< Shuffle forward behavior name (for script)
#define BEHAVIOR_NAME_SHUFFLE_BACK                  "shuffle_back"                  ///< Shuffle back behavior name (for script)
#define BEHAVIOR_NAME_CORPSE                        "corpse"                        ///< Corpse behavior name (for script)
#define BEHAVIOR_NAME_RESURRECTION                  "resurrection"                  ///< Resurrection behavior name (for script)
#define BEHAVIOR_NAME_CIRCLE_RIGHT                  "circle_right"                  ///< Right circle behavior name (for script)
#define BEHAVIOR_NAME_CIRCLE_LEFT                   "circle_left"                   ///< Left circle behavior name (for script)
#define BEHAVIOR_NAME_FLY                   "flying"                   ///< Left circle behavior name (for script)
#define BEHAVIOR_NAME_KILL                   "kill"                   ///< Left circle behavior name (for script)
#define BEHAVIOR_NAME_HIP_SWING                   "hip_swing"                   ///< Left circle behavior name (for script)
#define BEHAVIOR_NAME_BODY_WAVE_BUMP                   "body_wave_bump"                   ///< Left circle behavior name (for script)

// behavior duration
#define BEHAVIOR_DURATION_STAND                         0.25                         ///< Stand still DURATION (for script)
#define BEHAVIOR_DURATION_FIST_BUMP_RIGHT               0.5               ///< Right fist bump behavior DURATION (for script)
#define BEHAVIOR_DURATION_FIST_BUMP_LEFT                0.5                ///< Left fist bump behavior DURATION (for script)
#define BEHAVIOR_DURATION_FIST_BUMP_DOUBLE              1.5              ///< Double fist bump behavior DURATION (for script)
#define BEHAVIOR_DURATION_LEG_WAVE_CLOCKWISE            1.0            ///< Clockwise leg wave behavior DURATION (for script)
#define BEHAVIOR_DURATION_LEG_WAVE_COUNTER_CLOCKWISE    1.0    ///< Counter clockwise leg wave behavior DURATION (for script)
#define BEHAVIOR_DURATION_BODY_WAVES                    0.5                    ///< Body waves behavior DURATION (for script)
#define BEHAVIOR_DURATION_SHUFFLE_RIGHT                 1.0                 ///< Shuffle right behavior DURATION (for script)
#define BEHAVIOR_DURATION_SHUFFLE_LEFT                  1.0                  ///< Shuffle left behavior DURATION (for script)
#define BEHAVIOR_DURATION_SHUFFLE_FORWARD               1.0               ///< Shuffle forward behavior DURATION (for script)
#define BEHAVIOR_DURATION_SHUFFLE_BACK                  1.0                  ///< Shuffle back behavior DURATION (for script)
#define BEHAVIOR_DURATION_CORPSE                        0.5                        ///< Corpse behavior DURATION (for script)
#define BEHAVIOR_DURATION_RESURRECTION                  0.5                  ///< Resurrection behavior DURATION (for script)
#define BEHAVIOR_DURATION_CIRCLE_RIGHT                  0.5                  ///< Right circle behavior DURATION (for script)
#define BEHAVIOR_DURATION_CIRCLE_LEFT                   0.5                   ///< Left circle behavior DURATION (for script)
#define BEHAVIOR_DURATION_FLY                   1.0                   ///< Left circle behavior DURATION (for script)
#define BEHAVIOR_DURATION_KILL                   0.5                   ///< Left circle behavior DURATION (for script)
#define BEHAVIOR_DURATION_HIP_SWING                   0.25                   ///< Left circle behavior DURATION (for script)
#define BEHAVIOR_DURATION_BODY_WAVE_BUMP                   0.25                   ///< Left circle behavior DURATION (for script)

// defaults
#define DEFAULT_BLEND_DURATION                      0.1                             ///< Blend duration in seconds
#define DEFAULT_UPDATE_PERIOD                       100000                           ///< Update frequency in microseconds
#define DEFAULT_LIGHTING_UPDATE                     10                              ///< Number of iterations of normal updates per lighting update
#define DEFAULT_HEADER_LINE                         0                               ///< The default line to print the manual mode header
#define DEFAULT_REMAINING_TIME_LINE                 1                               ///< the default line for placing the remaining time
#define DEFAULT_CURRENT_BEHAVIOR_LINE               2                               ///< The current behavior print line
#define DEFAULT_PREVIOUS_BEHAVIOR_LINE              3                               ///< The previous behavior print line
#define DEFAULT_DEBUG_START_LINE                    6                               ///< The default line for debugging information
#define DEFAULT_MAX_DEBUG_LINES                     20                              ///< the default number of lines to use for debugging
#define DEFAULT_STRING_SPLIT_DELIMITER              ' '                             ///< The default delimiter for splitting strings



// extra class for hexapod, may not need?
class JsonStringObserver : public App::JsonValueObserver {
private:
  DanceState *dancestate;
  void update( const Json::Value& newValue ) {
    if (newValue.isString()) {
      std::string value = newValue.asString();
      std::cout << "Cool! Got the string: " << value << std::endl;
      if (value == "R") {
        dancestate->setLightState(RED);
      } else if (value == "A") {
        dancestate->setLightState(ALL);
      } else if (value == "B") {
        dancestate->setLightState(BLUE);
      } else if (value == "X") {
        dancestate->setLightState(XMAS);
      } else if (value == "G") {
        dancestate->setLightState(GREEN);
      } else if (value == "Off") {
        dancestate->setLightState(OFF);
      }
    }
  }
public:
  JsonStringObserver( DanceState* danceStateToModify ) {
    dancestate = danceStateToModify;
  }
};

class DynamixelVoltageObserver : public Observer {
private:
	std::vector<Motor*> measurements;
	double voltage;
public:
	DynamixelVoltageObserver()
	:voltage(100) {};

	void update( Motor* dynamixel, const Instruction* instruction) {
		if (instruction->type == Instruction::READ) {
			if (instruction->range.reg == REG_Present_Voltage) {
				bool needToAddMeasuerement = true;
				for (std::vector<Motor*>::iterator it = measurements.begin(); it!=measurements.end(); it++) {
					if (dynamixel == *it) {
						needToAddMeasuerement = false;
						break;
					}
				}
				if (needToAddMeasuerement) {
					measurements.push_back(dynamixel);
				}

				double sum = 0;
				for (std::vector<Motor*>::iterator it = measurements.begin(); it!=measurements.end(); it++) {
					sum += (*it)->getByte(REG_Present_Voltage);
				}
				voltage = sum/(10.0 * (double)measurements.size());
			}
		}

	}

	double getVoltage() {
		return voltage;
	}
};

DynamixelVoltageObserver voltageObserver;

// helper functions
void init();                                                                        ///< Initialize the state variables
void initHex();                                                                     ///< Initialize the hexapod
void initAppInterface();                                                            ///< Initialize the iOS simulator
void initCurses();                                                                  ///< Initialize the curses variables
void cli(int argc, char *argv[]);                                                   ///< read in command line arguments
bool isArg(std::string arg, char id);                                               ///< check if a cli argument is the given id (must have a dash (-) then the id)
void usage(char *program);                                                          ///< print usage information
void usage(std::string program);                                                    ///< print usage information
void load(char *filename);                                                          ///< load a set of behaviors from a script
void load(std::string filename);                                                    ///< load a set of behaviors from a script
Vector fromVectorToVector(Vector from, Vector to, double magnitude);          ///< go from one vector (magnitude 0.0) to another vector (magnitude 1.0)
double fromValueToValue(double from, double to, double magnitude);                  ///< go from one value (magnitude 0.0) to another value (magnitude 1.0)
std::vector<std::string> split(char *str);                                          ///< split a string on the detault character
std::vector<std::string> split(std::string str);                                    ///< split a string on the detault character
std::vector<std::string> split(char *str, char delim);                              ///< split a string on the given character
std::vector<std::string> split(std::string str, char delim);                        ///< split a string on the given character

// interrupt functions
void setTimer();                                                                    ///< set up the timers that call SIGALRM
void stopTimer(void);
void setSignals();                                                                  ///< set up the signals (SIGALRM, SIGTERM and SIGINT)
void handler(int signum);                                                           ///< handle the timer calls (i.e., processing behaviors) and close ncurses on SIGTERM and SIGINT
void *stateUpdate(void*);

// curses variables
WINDOW *mainwin;                                                                    ///< ncurses window
int oldcur;                                                                         ///< the old cursor position (SOW: I think that is what this is, I am uncertain)

// debugging
void debug(const char *str);                                                        ///< the function for debugging information
void debug(std::string str);                                                        ///< the function for debugging information
void debug(const char *str, int line);                                              ///< the function for putting a debug message at a given line
void debug(std::string str, int line);                                              ///< the function for putting a debug message at a given line
int debug_line;                                                                     ///< the line for displaying debug information
double time_marker;                                                                 ///< time_marker

// state variables
std::string jsonFileName = "nico.json";
char * botGroupParam = DEFAULT_BOT_GROUP;
bool manual;                                                                        ///< determines if we are in manual mode or automatic script mode
bool use_osc;                                                                       ///< determines if we are in osc mode
bool daemonMode;                                                                       ///< determines if we are in osc mode

bool curses;                                                                        ///< determines if curses is used or not
int lighting_counter;                                                               ///< the counter for ensuring proper lighting frequency
double current_lighting_period;                                                     ///< the period for the current lighting behavior
int current_behavior;                                                               ///< the id for the current behavior
int previous_behavior;                                                              ///< the id for the previous behavior
int current_lighting;                                                               ///< the id of the current lighting behavior
double current_magnitude = 1.0;                                                           ///< the magnitude of the current behavior
double previous_magnitude;                                                          ///< the magnitude of the previous behavior
std::string current_behavior_name;                                                  ///< the name of the current behavior
std::string previous_behavior_name;                                                 ///< the name of the previous behavior
std::string current_lighting_name;                                                  ///< the name of the current lighting behavior
int behavior_index;                                                                 ///< the current behavior index in the script
double current_blend_duration;                                                      ///< the current blend duration
bool first_behavior;                                                                ///< the script hasn't started running yet if this is true
double remaining_time;                                                              ///< the amount of time remaining for the current behavior
bool script_finished;                                                               ///< flag for if the script is finished or not
double behavior_duration;                                                           ///< The duration for the current behavior
Time timer_iteration;
double timer_iteration_offset = 0;	// Bunting: Use this later for time syncing
Time timer_behavior;
double behaviorTime = 0;// Bunting: this will be = timer_iteration.runningTime() + timer_iteration_offset;
Vector bodyVelocity(3); // Bunting: making this global for more control
double bodyAngularVelocity = 0;
bool newBehavior = false;	//	for better behavior transistioning.
bool exitingABehavior = false;
int previousKey = 0;
double previous_blend_duration = 0;
HEXAPOD_TYPE hexapodType;
DanceParameters mDanceParameters;
bool terminate, keepThreadAlive;
pthread_mutex_t commandMutex = PTHREAD_MUTEX_INITIALIZER;

// saved state variables
Vector saved_hexapod_position(3);
Quaternion saved_hexapod_orientation;
Vector saved_right_front(3);
Vector saved_left_front(3);
Vector saved_body_position(3);
Vector saved_corpse_foot_0(3);
Vector saved_corpse_foot_1(3);
Vector saved_corpse_foot_2(3);
Vector saved_corpse_foot_3(3);
Vector saved_corpse_foot_4(3);
Vector saved_corpse_foot_5(3);

// hexapod variables
Node rootNode;                                                                    ///< the root node of the hexapod
Hexapod hex( &rootNode );                                                         ///< the hexapod main class
App::AppInterface appInterface;
DanceState danceState;
Handler dynamixelHandler;
MotionControlAngular headingControl;
MotionControl<Vector> locationControl;
Vector zeroVector(3);
Vector headAngles(3);
Vector bodyOffset(3);
Vector bodyAngleAdjust(3);

double voltageShutdownThreshold = 12;

// variables for dance moves
std::vector<int>       behaviors;                                                   ///< the list of behaviors read in from the script
std::vector<double>    durations;                                                   ///< the list of durations read in from the script
std::vector<double>    blend_durations;                                             ///< the list of blend durations read in from the script

// base hexapod movement
void hexMovement();

// prototypes for dance moves
void behavior_stand(int id, double magnitude);                                      ///< run the standing behavior
void behavior_fist_bump_right(int id, double magnitude);                            ///< run the fist bump right behavior
void behavior_fist_bump_left(int id, double magnitude);                             ///< run the fist bump left behavior
void behavior_fist_bump_double(int id, double magnitude);                           ///< run the fist bump double behavior
void behavior_leg_wave_clockwise(int id, double magnitude);                         ///< run the leg wave clockwise behavior
void behavior_leg_wave_counter_clockwise(int id, double magnitude);                 ///< run the leg wave counter clockwise behavior
void behavior_body_waves(int id, double magnitude);                                 ///< run the body waves behavior
void behavior_shuffle_right(int id, double magnitude);                              ///< run the shuffle right behavior
void behavior_shuffle_left(int id, double magnitude);                               ///< run the shuffle left behavior
void behavior_shuffle_forward(int id, double magnitude);                            ///< run the shuffle forward behavior
void behavior_shuffle_back(int id, double magnitude);                               ///< run the shuffle back behavior
void behavior_corpse(int id, double magnitude);                                     ///< run the corpse behavior
void behavior_resurrection(int id, double magnitude);                               ///< run the resurrection behavior
void behavior_circle_right(int id, double magnitude);                               ///< run the circle right behavior
void behavior_circle_left(int id, double magnitude);                                ///< run the circle left behavior
void behavior_fly(int id, double magnitude);										///< run the circle left behavior
void behavior_kill(int id, double magnitude);                                ///< run the circle left behavior
void behavior_hip_swing(int id, double magnitude);                                ///< run the circle left behavior
void behavior_body_wave_bump(int id, double magnitude);                                ///< run the circle left behavior

// update the dance state
void updateStateWithCommand(int command);




void oscRun(UdpListeningReceiveSocket *s) {
  s->Run();
}

void *oscVoidRun(void *arg) {
  UdpListeningReceiveSocket *s= (UdpListeningReceiveSocket *)arg;
  s->Run();
  pthread_exit(NULL);
  return NULL;
}


class OscBehaviorListener : public osc::OscPacketListener {
  
  
  
private:
  DanceState *danceState;
  char * botGroup;
  Hexapod *hexapod;
  bool wristUnlocked;
  
public:
  OscBehaviorListener(Hexapod *hexModify, DanceState* danceStateToModify, char * botGroupModify ) {
    hexapod = hexModify;
    danceState = danceStateToModify;
    botGroup = botGroupModify;
    wristUnlocked = true;
  }
  
  OscBehaviorListener(char * botGroupModify) {
    botGroup = botGroupModify;
    wristUnlocked = true;
  }
  
protected:
  OscBehaviorListener() {} // not used
  
  void handleCommand(char command) {
    
    updateStateWithCommand(command);
    
    return;
    
  }
  
  virtual void ProcessMessage( const osc::ReceivedMessage& m,
                              const IpEndpointName& remoteEndpoint )
  {
    (void) remoteEndpoint; // suppress unused parameter warning
    
    try{
      
      char * address = (char *)m.AddressPattern();
      
      if(curses) {
        mvaddstr(DEFAULT_DEBUG_START_LINE, 10, "                ");
        mvaddstr(DEFAULT_DEBUG_START_LINE, 10, address);
        mvaddstr(DEFAULT_DEBUG_START_LINE+2, 10, "                ");
        mvaddstr(DEFAULT_DEBUG_START_LINE+2, 10, botGroup);
      }
      
      if( strstr(m.AddressPattern(), (char *)ALL_BOT_GROUP ) != NULL ) {
        
        // get command char
        char command = address[strlen(address)-1];
        updateStateWithCommand(command);
        if(curses) {
          mvaddstr(DEFAULT_DEBUG_START_LINE+1, 10, "                ");
          mvaddstr(DEFAULT_DEBUG_START_LINE+1, 10, "all");
        }
        
      }
      
      if( strstr(m.AddressPattern(), (char *)WRIST_BOT_GROUP ) != NULL ) {
       
        if( strstr(m.AddressPattern(), (char *)LOCK_WRIST_BOT_GROUP ) != NULL ) {
        
          osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
          float f3;
          args >> f3 >> osc::EndMessage;
          
          if (f3 > 0) {
            if(curses) mvaddstr(DEFAULT_DEBUG_START_LINE+2, 10, "wrist LOCKED");
            wristUnlocked = false;
          } else {
            if(curses) mvaddstr(DEFAULT_DEBUG_START_LINE+2, 10, "wrist UNlocked");
            wristUnlocked = true;
          }
          
        } else if (wristUnlocked) {
          
          // get command char
          char command = address[strlen(address)-1];
          updateStateWithCommand(command);
          if(curses) {
            mvaddstr(DEFAULT_DEBUG_START_LINE+1, 10, "                ");
            mvaddstr(DEFAULT_DEBUG_START_LINE+1, 10, "wrist");
          }
          
        }
        
      }
      
      
      // is MESSAGE FOR MY GROUP ??
      if( strstr(m.AddressPattern(), (char *)botGroup ) != NULL ){
        
        // get command char
        char command = address[strlen(address)-1];
        updateStateWithCommand(command);
        
        if(curses) {
          mvaddstr(DEFAULT_DEBUG_START_LINE+1, 10, "                ");
          mvaddstr(DEFAULT_DEBUG_START_LINE+1, 10, botGroup);
        }
        
      }
      
      // is MESSAGE GLOBAL ??
      else if( std::strcmp( m.AddressPattern(), "/" ) == 0 ){
        // NOT GLOBAL MESSAGE
        //
        // NO OPT
        //
      }
      
      else {
        
        // DO NOTHING
        
        if(curses) {
          mvaddstr(DEFAULT_DEBUG_START_LINE+1, 10, "                ");
          mvaddstr(DEFAULT_DEBUG_START_LINE+1, 10, "nothing");
        }
        
      }
      
      
      
    } catch( osc::Exception& e ){
      // any parsing errors such as unexpected argument types, or
      // missing arguments get thrown as exceptions.
      std::stringstream msg;
      msg << "error while parsing message: "
      << m.AddressPattern() << ": " << e.what() << "\n";
      debug(msg.str());
    }
    
    return;
  }
  
  
  
  
};

Vector getRestingFootVector(int index) {
//  Quaternion hexapodOrientation;
//  hexapodOrientation.makeFromAngleAndAxis(hex.getWalkingDynamics()->getBodyHeading(), Vector::zAxis);
//  Vector hexapodLocation = hex.bodyLocationDesired;
//  hexapodLocation(2) = 0;
//  Vector footLocation = hexapodOrientation.makeRotationMatrix()*hex.footPivotForWalking[index] + hexapodLocation;
//  return footLocation;
	return hex.getCurrentPivotForFoot(index);
}

bool allFeetOnGround() {
  for (int i = 0; i < 6; i++) {
    if (fabs(hex.foot[i](2)) > 1e-6) {
      return false;
    }
  }
  return true;
}

void updateStateWithCommand(int key) {
  // check which key behavior was queued
  
  bool newBehavior = false;


//  if (current_magnitude == 1.0 &&
	if( previous_magnitude == 0 &&
			 key != previousKey && key > 0)	// Conditions for switching behaviors
  {
		if (previous_magnitude > 0) {	// Bunting: this is hacky but gets the job done for single key queue
			current_behavior = previous_behavior;
			current_magnitude = previous_magnitude;
			current_behavior_name = previous_behavior_name;
			current_blend_duration = previous_blend_duration;
		}

    switch (key) {
      case BEHAVIOR_KEY_STAND:
        previous_behavior = current_behavior;
        previous_magnitude = current_magnitude;
        previous_behavior_name = current_behavior_name;
        previous_blend_duration = current_blend_duration;
        current_behavior = BEHAVIOR_ID_STAND;
        current_magnitude = 0.0;
        current_behavior_name = BEHAVIOR_NAME_STAND;
        timer_behavior.update();
        behavior_duration = 0.0;	// There are problems here when switching to a new behavior (like from legwaves to standing) that causes jerking.
        current_blend_duration = BEHAVIOR_DURATION_STAND;
        newBehavior = true;
        break;
      case BEHAVIOR_KEY_FIST_BUMP_RIGHT:
        previous_behavior = current_behavior;
        previous_magnitude = current_magnitude;
        previous_behavior_name = current_behavior_name;
        previous_blend_duration = current_blend_duration;
        current_behavior = BEHAVIOR_ID_FIST_BUMP_RIGHT;
        current_magnitude = 0.0;
        current_behavior_name = BEHAVIOR_NAME_FIST_BUMP_RIGHT;
        timer_behavior.update();
        behavior_duration = 0.0;
        current_blend_duration = BEHAVIOR_DURATION_FIST_BUMP_RIGHT;
        newBehavior = true;
        break;
      case BEHAVIOR_KEY_FIST_BUMP_LEFT:
        previous_behavior = current_behavior;
        previous_magnitude = current_magnitude;
        previous_behavior_name = current_behavior_name;
        previous_blend_duration = current_blend_duration;
        current_behavior = BEHAVIOR_ID_FIST_BUMP_LEFT;
        current_magnitude = 0.0;
        current_behavior_name = BEHAVIOR_NAME_FIST_BUMP_LEFT;
        timer_behavior.update();
        behavior_duration = 0.0;
        current_blend_duration = BEHAVIOR_DURATION_FIST_BUMP_LEFT;
        newBehavior = true;
        break;
      case BEHAVIOR_KEY_FIST_BUMP_DOUBLE:
        previous_behavior = current_behavior;
        previous_magnitude = current_magnitude;
        previous_behavior_name = current_behavior_name;
        previous_blend_duration = current_blend_duration;
        current_behavior = BEHAVIOR_ID_FIST_BUMP_DOUBLE;
        current_magnitude = 0.0;
        current_behavior_name = BEHAVIOR_NAME_FIST_BUMP_DOUBLE;
        timer_behavior.update();
        behavior_duration = 0.0;
        current_blend_duration = BEHAVIOR_DURATION_FIST_BUMP_DOUBLE;
        newBehavior = true;
        break;
      case BEHAVIOR_KEY_LEG_WAVE_CLOCKWISE:
        previous_behavior = current_behavior;
        previous_magnitude = current_magnitude;
        previous_behavior_name = current_behavior_name;
        previous_blend_duration = current_blend_duration;
        current_behavior = BEHAVIOR_ID_LEG_WAVE_CLOCKWISE;
        current_magnitude = 0.0;
        current_behavior_name = BEHAVIOR_NAME_LEG_WAVE_CLOCKWISE;
        timer_behavior.update();
        behavior_duration = 0.0;
        current_blend_duration = BEHAVIOR_DURATION_LEG_WAVE_CLOCKWISE;
        newBehavior = true;
        break;
      case BEHAVIOR_KEY_LEG_WAVE_COUNTER_CLOCKWISE:
        previous_behavior = current_behavior;
        previous_magnitude = current_magnitude;
        previous_behavior_name = current_behavior_name;
        previous_blend_duration = current_blend_duration;
        current_behavior = BEHAVIOR_ID_LEG_WAVE_COUNTER_CLOCKWISE;
        current_magnitude = 0.0;
        current_behavior_name = BEHAVIOR_NAME_LEG_WAVE_COUNTER_CLOCKWISE;
        timer_behavior.update();
        behavior_duration = 0.0;
        current_blend_duration = BEHAVIOR_DURATION_LEG_WAVE_COUNTER_CLOCKWISE;
        newBehavior = true;
        break;
      case BEHAVIOR_KEY_BODY_WAVES:
        previous_behavior = current_behavior;
        previous_magnitude = current_magnitude;
        previous_behavior_name = current_behavior_name;
        previous_blend_duration = current_blend_duration;
        current_behavior = BEHAVIOR_ID_BODY_WAVES;
        current_magnitude = 0.0;
        current_behavior_name = BEHAVIOR_NAME_BODY_WAVES;
        timer_behavior.update();
        behavior_duration = 0.0;
        current_blend_duration = BEHAVIOR_DURATION_BODY_WAVES;
        newBehavior = true;
        break;
      case BEHAVIOR_KEY_SHUFFLE_RIGHT:
        previous_behavior = current_behavior;
        previous_magnitude = current_magnitude;
        previous_behavior_name = current_behavior_name;
        previous_blend_duration = current_blend_duration;
        current_behavior = BEHAVIOR_ID_SHUFFLE_RIGHT;
        current_magnitude = 0.0;
        current_behavior_name = BEHAVIOR_NAME_SHUFFLE_RIGHT;
        timer_behavior.update();
        behavior_duration = 0.0;
        current_blend_duration = BEHAVIOR_DURATION_SHUFFLE_RIGHT;
        newBehavior = true;
        break;
      case BEHAVIOR_KEY_SHUFFLE_LEFT:
        previous_behavior = current_behavior;
        previous_magnitude = current_magnitude;
        previous_behavior_name = current_behavior_name;
        previous_blend_duration = current_blend_duration;
        current_behavior = BEHAVIOR_ID_SHUFFLE_LEFT;
        current_magnitude = 0.0;
        current_behavior_name = BEHAVIOR_NAME_SHUFFLE_LEFT;
        timer_behavior.update();
        behavior_duration = 0.0;
        current_blend_duration = BEHAVIOR_DURATION_SHUFFLE_LEFT;
        newBehavior = true;
        break;
      case BEHAVIOR_KEY_SHUFFLE_FORWARD:
        previous_behavior = current_behavior;
        previous_magnitude = current_magnitude;
        previous_behavior_name = current_behavior_name;
        previous_blend_duration = current_blend_duration;
        current_behavior = BEHAVIOR_ID_SHUFFLE_FORWARD;
        current_magnitude = 0.0;
        current_behavior_name = BEHAVIOR_NAME_SHUFFLE_FORWARD;
        timer_behavior.update();
        behavior_duration = 0.0;
        current_blend_duration = BEHAVIOR_DURATION_SHUFFLE_FORWARD;
        newBehavior = true;
        break;
      case BEHAVIOR_KEY_SHUFFLE_BACK:
        previous_behavior = current_behavior;
        previous_magnitude = current_magnitude;
        previous_behavior_name = current_behavior_name;
        previous_blend_duration = current_blend_duration;
        current_behavior = BEHAVIOR_ID_SHUFFLE_BACK;
        current_magnitude = 0.0;
        current_behavior_name = BEHAVIOR_NAME_SHUFFLE_BACK;
        timer_behavior.update();
        behavior_duration = 0.0;
        current_blend_duration = BEHAVIOR_DURATION_SHUFFLE_BACK;
        newBehavior = true;
        break;
      case BEHAVIOR_KEY_CORPSE:
        previous_behavior = current_behavior;
        previous_magnitude = current_magnitude;
        previous_behavior_name = current_behavior_name;
        previous_blend_duration = current_blend_duration;
        current_behavior = BEHAVIOR_ID_CORPSE;
        current_magnitude = 0.0;
        current_behavior_name = BEHAVIOR_NAME_CORPSE;
        timer_behavior.update();
        behavior_duration = 0.0;
        current_blend_duration = BEHAVIOR_DURATION_CORPSE;
        newBehavior = true;
        break;
      case BEHAVIOR_KEY_RESURRECTION:
        previous_behavior = current_behavior;
        previous_magnitude = current_magnitude;
        previous_behavior_name = current_behavior_name;
        previous_blend_duration = current_blend_duration;
        current_behavior = BEHAVIOR_ID_RESURRECTION;
        current_magnitude = 0.0;
        current_behavior_name = BEHAVIOR_NAME_RESURRECTION;
        timer_behavior.update();
        behavior_duration = 0.0;
        current_blend_duration = BEHAVIOR_DURATION_RESURRECTION;
        newBehavior = true;
        break;
      case BEHAVIOR_KEY_CIRCLE_RIGHT:
        previous_behavior = current_behavior;
        previous_magnitude = current_magnitude;
        previous_behavior_name = current_behavior_name;
        previous_blend_duration = current_blend_duration;
        current_behavior = BEHAVIOR_ID_CIRCLE_RIGHT;
        current_magnitude = 0.0;
        current_behavior_name = BEHAVIOR_NAME_CIRCLE_RIGHT;
        timer_behavior.update();
        behavior_duration = 0.0;
        current_blend_duration = BEHAVIOR_DURATION_CIRCLE_RIGHT;
				newBehavior = true;
        break;
      case BEHAVIOR_KEY_CIRCLE_LEFT:
        previous_behavior = current_behavior;
        previous_magnitude = current_magnitude;
        previous_behavior_name = current_behavior_name;
        previous_blend_duration = current_blend_duration;
        current_behavior = BEHAVIOR_ID_CIRCLE_LEFT;
        current_magnitude = 0.0;
        current_behavior_name = BEHAVIOR_NAME_CIRCLE_LEFT;
        timer_behavior.update();
        behavior_duration = 0.0;
        current_blend_duration = BEHAVIOR_DURATION_CIRCLE_LEFT;
        newBehavior = true;
        break;

			case BEHAVIOR_KEY_FLY:
				previous_behavior = current_behavior;
				previous_magnitude = current_magnitude;
				previous_behavior_name = current_behavior_name;
				previous_blend_duration = current_blend_duration;
				current_behavior = BEHAVIOR_ID_FLY;
				current_magnitude = 0.0;
				current_behavior_name = BEHAVIOR_NAME_FLY;
				timer_behavior.update();
				behavior_duration = 0.0;
				current_blend_duration = BEHAVIOR_DURATION_FLY;
				newBehavior = true;
				break;

			case BEHAVIOR_KEY_KILL:
				previous_behavior = current_behavior;
				previous_magnitude = current_magnitude;
				previous_behavior_name = current_behavior_name;
				previous_blend_duration = current_blend_duration;
				current_behavior = BEHAVIOR_ID_KILL;
				current_magnitude = 0.0;
				current_behavior_name = BEHAVIOR_NAME_KILL;
				timer_behavior.update();
				behavior_duration = 0.0;
				current_blend_duration = BEHAVIOR_DURATION_KILL;
				newBehavior = true;
				break;

			case BEHAVIOR_KEY_HIP_SWING:
				previous_behavior = current_behavior;
				previous_magnitude = current_magnitude;
				previous_behavior_name = current_behavior_name;
				previous_blend_duration = current_blend_duration;
				current_behavior = BEHAVIOR_ID_HIP_SWING;
				current_magnitude = 0.0;
				current_behavior_name = BEHAVIOR_NAME_HIP_SWING;
				timer_behavior.update();
				behavior_duration = 0.0;
				current_blend_duration = BEHAVIOR_DURATION_HIP_SWING;
				newBehavior = true;
				break;

			case BEHAVIOR_KEY_BODY_WAVE_BUMP:
				previous_behavior = current_behavior;
				previous_magnitude = current_magnitude;
				previous_behavior_name = current_behavior_name;
				previous_blend_duration = current_blend_duration;
				current_behavior = BEHAVIOR_ID_BODY_WAVE_BUMP;
				current_magnitude = 0.0;
				current_behavior_name = BEHAVIOR_NAME_BODY_WAVE_BUMP;
				timer_behavior.update();
				behavior_duration = 0.0;
				current_blend_duration = BEHAVIOR_DURATION_BODY_WAVE_BUMP;
				newBehavior = true;
				break;

      default:
        //do nothing
        break;
        
    }
    
    if (newBehavior) {
      previousKey = key;
      newBehavior = true;
      exitingABehavior = true;
    }
  }
  
  switch (key) {
    case LIGHTING_KEY_BLUE_SOLID:
      current_lighting = LIGHTING_ID_BLUE_SOLID;
      current_lighting_name = LIGHTING_NAME_BLUE_SOLID;
      current_lighting_period = 0.1;
      break;
    case LIGHTING_KEY_BLUE_BLINK:
      current_lighting = LIGHTING_ID_BLUE_BLINK;
      current_lighting_name = LIGHTING_NAME_BLUE_BLINK;
      current_lighting_period = 0.1;
      break;
    case LIGHTING_KEY_BLUE_STROBE:
      current_lighting = LIGHTING_ID_BLUE_STROBE;
      current_lighting_name = LIGHTING_NAME_BLUE_STROBE;
      current_lighting_period = 0.2;
      break;
    case LIGHTING_KEY_WHITE_SOLID:
      current_lighting = LIGHTING_ID_WHITE_SOLID;
      current_lighting_name = LIGHTING_NAME_WHITE_SOLID;
      current_lighting_period = 0.1;
      break;
    case LIGHTING_KEY_WHITE_BLINK:
      current_lighting = LIGHTING_ID_WHITE_BLINK;
      current_lighting_name = LIGHTING_NAME_WHITE_BLINK;
      current_lighting_period = 0.1;
      break;
    case LIGHTING_KEY_WHITE_STROBE:
      current_lighting = LIGHTING_ID_WHITE_STROBE;
      current_lighting_name = LIGHTING_NAME_WHITE_STROBE;
      current_lighting_period = 0.2;
      break;
    case LIGHTING_KEY_PINK_SOLID:
      current_lighting = LIGHTING_ID_PINK_SOLID;
      current_lighting_name = LIGHTING_NAME_PINK_SOLID;
      current_lighting_period = 0.1;
      break;
    case LIGHTING_KEY_PINK_BLINK:
      current_lighting = LIGHTING_ID_PINK_BLINK;
      current_lighting_name = LIGHTING_NAME_PINK_BLINK;
      current_lighting_period = 0.1;
      break;
    case LIGHTING_KEY_PINK_STROBE:
      current_lighting = LIGHTING_ID_PINK_STROBE;
      current_lighting_name = LIGHTING_NAME_PINK_STROBE;
      current_lighting_period = 0.2;
      break;
    case LIGHTING_KEY_YELLOW_SOLID:
      current_lighting = LIGHTING_ID_YELLOW_SOLID;
      current_lighting_name = LIGHTING_NAME_YELLOW_SOLID;
      current_lighting_period = 0.1;
      break;
    case LIGHTING_KEY_YELLOW_BLINK:
      current_lighting = LIGHTING_ID_YELLOW_BLINK;
      current_lighting_name = LIGHTING_NAME_YELLOW_BLINK;
      current_lighting_period = 0.1;
      break;
    case LIGHTING_KEY_YELLOW_STROBE:
      current_lighting = LIGHTING_ID_YELLOW_STROBE;
      current_lighting_name = LIGHTING_NAME_YELLOW_STROBE;
      current_lighting_period = 0.2;
      break;
    case LIGHTING_KEY_RED_SOLID:
      current_lighting = LIGHTING_ID_RED_SOLID;
      current_lighting_name = LIGHTING_NAME_RED_SOLID;
      current_lighting_period = 0.1;
      break;
    case LIGHTING_KEY_RED_BLINK:
      current_lighting = LIGHTING_ID_RED_BLINK;
      current_lighting_name = LIGHTING_NAME_RED_BLINK;
      current_lighting_period = 0.1;
      break;
    case LIGHTING_KEY_RED_STROBE:
      current_lighting = LIGHTING_ID_RED_STROBE;
      current_lighting_name = LIGHTING_NAME_RED_STROBE;
      current_lighting_period = 0.2;
      break;
    case LIGHTING_KEY_GREEN_SOLID:
      current_lighting = LIGHTING_ID_GREEN_SOLID;
      current_lighting_name = LIGHTING_NAME_GREEN_SOLID;
      current_lighting_period = 0.1;
      break;
    case LIGHTING_KEY_GREEN_BLINK:
      current_lighting = LIGHTING_ID_GREEN_BLINK;
      current_lighting_name = LIGHTING_NAME_GREEN_BLINK;
      current_lighting_period = 0.1;
      break;
    case LIGHTING_KEY_GREEN_STROBE:
      current_lighting = LIGHTING_ID_GREEN_STROBE;
      current_lighting_name = LIGHTING_NAME_GREEN_STROBE;
      current_lighting_period = 0.2;
      break;
    case LIGHTING_KEY_CYAN_SOLID:
      current_lighting = LIGHTING_ID_CYAN_SOLID;
      current_lighting_name = LIGHTING_NAME_CYAN_SOLID;
      current_lighting_period = 0.1;
      break;
    case LIGHTING_KEY_CYAN_BLINK:
      current_lighting = LIGHTING_ID_CYAN_BLINK;
      current_lighting_name = LIGHTING_NAME_CYAN_BLINK;
      current_lighting_period = 0.1;
      break;
    case LIGHTING_KEY_CYAN_STROBE:
      current_lighting = LIGHTING_ID_CYAN_STROBE;
      current_lighting_name = LIGHTING_NAME_CYAN_STROBE;
      current_lighting_period = 0.2;
      break;

    case LIGHTING_KEY_MULTI_STROBE_1:
      current_lighting = LIGHTING_ID_MULTI_STROBE_1;
      current_lighting_name = LIGHTING_NAME_MULTI_STROBE_1;
      current_lighting_period = 0.1;
      break;
    case LIGHTING_KEY_MULTI_STROBE_2:
      current_lighting = LIGHTING_ID_MULTI_STROBE_2;
      current_lighting_name = LIGHTING_NAME_MULTI_STROBE_1;
      current_lighting_period = 0.1;
      break;
    case LIGHTING_KEY_OFF:
      current_lighting = LIGHTING_ID_OFF;
      current_lighting_name = LIGHTING_NAME_OFF;
      current_lighting_period = 0.1;
      break;
    default:
      // do nothing
      break;
  }
  
  return;
}


char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
  char ** itr = std::find(begin, end, option);
  if (itr != end && ++itr != end)
  {
    return *itr;
  }
  return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
  return std::find(begin, end, option) != end;
}


////////////////////////////////////////////////////
//                      Main                      //
////////////////////////////////////////////////////
int main(int argc, char *argv[]) {
  
  // initialize
  init();
  
  // ensure that we have an attempt at a file name
  cli(argc, argv);
  
  // init hexapod stuff
  initHex();

  //nuking for now
  // init the iOS interface for simulation
  // initAppInterface();
  
  debug("finished init hex");
  
  // init curses stuff
  initCurses();

	pthread_t behaviorThread;
	pthread_create(&behaviorThread, NULL, stateUpdate, NULL);
  
  ////////////////////////////////////////////////////
  //                    OSC Mode                    //
  ////////////////////////////////////////////////////
  if (use_osc) {
    
    
    mvaddstr(DEFAULT_HEADER_LINE, 0, "OSC Mode");
    
    
    // OSC SETUP
    OscBehaviorListener oscListener(botGroupParam);
    UdpListeningReceiveSocket s(IpEndpointName( IpEndpointName::ANY_ADDRESS, PORT ), &oscListener );
    //std::cout << "press ctrl-c to end\n";
    
    
    // not needed since we can loop crazy below
    pthread_t oscPThread;
    pthread_create(&oscPThread, NULL, &oscVoidRun, (void *)&s);
    
    
    
    // BLOCKS while listening
    //s.RunUntilSigInt();
    
    
    // loop over console inputs forever
    while (!terminate) {
      int key = getch();
      updateStateWithCommand(key);
    }
    
    ////////////////////////////////////////////////////
    //                  Manual Mode                   //
    ////////////////////////////////////////////////////
  } else if (manual) {
    // print out the screen header
    if (curses) {
      mvaddstr(DEFAULT_HEADER_LINE, 0, "Manual Mode");
      if (appInterface.isOpen()) {
        mvaddstr(DEFAULT_HEADER_LINE, 20, "iOS connected");
      }
    }
    
    
    // loop over console inputs forever
    while (!terminate) {
      int key = getch();
      updateStateWithCommand(key);
    }



    
    ////////////////////////////////////////////////////
    //                Automatic Loop                  //
    ////////////////////////////////////////////////////
  } else {
    // print out the screen header
    mvaddstr(DEFAULT_HEADER_LINE, 0, "Script Mode (Running)");
    
    // load the file
    load(argv[1]);
    
    while(behavior_index < behaviors.size()) {
      // check for behavior timeout
      bool next_behavior = first_behavior;
      first_behavior = false;
      
      remaining_time = durations.at(behavior_index) - behavior_duration;
      if (behavior_duration >= durations.at(behavior_index)) {
        behavior_index++;
        next_behavior = true;
      }
      
      // continue on if the behavior index is out of scope
      if (behavior_index >= behaviors.size()) {
        continue;
      }
      
      if (next_behavior) {
        previous_behavior = current_behavior;
        previous_magnitude = current_magnitude;
        previous_behavior_name = current_behavior_name;
        current_behavior = behaviors.at(behavior_index);
        current_magnitude = 0.0;
        timer_behavior.update();
        behavior_duration = 0.0;
        current_blend_duration = blend_durations.at(behavior_index);
        switch (behaviors.at(behavior_index)) {
          case BEHAVIOR_ID_STAND:
            current_behavior_name = BEHAVIOR_NAME_STAND;
            break;
          case BEHAVIOR_ID_FIST_BUMP_RIGHT:
            current_behavior_name = BEHAVIOR_NAME_FIST_BUMP_RIGHT;
            break;
          case BEHAVIOR_ID_FIST_BUMP_LEFT:
            current_behavior_name = BEHAVIOR_NAME_FIST_BUMP_LEFT;
            break;
          case BEHAVIOR_ID_FIST_BUMP_DOUBLE:
            current_behavior_name = BEHAVIOR_NAME_FIST_BUMP_DOUBLE;
            break;
          case BEHAVIOR_ID_LEG_WAVE_CLOCKWISE:
            current_behavior_name = BEHAVIOR_NAME_LEG_WAVE_CLOCKWISE;
            break;
          case BEHAVIOR_ID_LEG_WAVE_COUNTER_CLOCKWISE:
            current_behavior_name = BEHAVIOR_NAME_LEG_WAVE_COUNTER_CLOCKWISE;
            break;
          case BEHAVIOR_ID_BODY_WAVES:
            current_behavior_name = BEHAVIOR_NAME_BODY_WAVES;
            break;
          case BEHAVIOR_ID_SHUFFLE_RIGHT:
            current_behavior_name = BEHAVIOR_NAME_SHUFFLE_RIGHT;
            break;
          case BEHAVIOR_ID_SHUFFLE_LEFT:
            current_behavior_name = BEHAVIOR_NAME_SHUFFLE_LEFT;
            break;
          case BEHAVIOR_ID_SHUFFLE_FORWARD:
            current_behavior_name = BEHAVIOR_NAME_SHUFFLE_FORWARD;
            break;
          case BEHAVIOR_ID_SHUFFLE_BACK:
            current_behavior_name = BEHAVIOR_NAME_SHUFFLE_BACK;
            break;
          case BEHAVIOR_ID_CORPSE:
            current_behavior_name = BEHAVIOR_NAME_CORPSE;
            break;
          case BEHAVIOR_ID_RESURRECTION:
            current_behavior_name = BEHAVIOR_NAME_RESURRECTION;
            break;
          case BEHAVIOR_ID_CIRCLE_RIGHT:
            current_behavior_name = BEHAVIOR_NAME_CIRCLE_RIGHT;
            break;
          case BEHAVIOR_ID_CIRCLE_LEFT:
            current_behavior_name = BEHAVIOR_NAME_CIRCLE_LEFT;
            break;
					case BEHAVIOR_ID_FLY:
						current_behavior_name = BEHAVIOR_NAME_FLY;
						break;
					case BEHAVIOR_ID_KILL:
						current_behavior_name = BEHAVIOR_NAME_KILL;
						break;
					case BEHAVIOR_ID_HIP_SWING:
						current_behavior_name = BEHAVIOR_NAME_HIP_SWING;
						break;
					case BEHAVIOR_ID_BODY_WAVE_BUMP:
						current_behavior_name = BEHAVIOR_NAME_BODY_WAVE_BUMP;
						break;
          default:
            // do nothing
            break;
        }
      }
    }
    
    // keep the screen up after completion
    while(1){
      script_finished = true;
    }
  }
  
  
  // cleanup
//  delwin(mainwin);
//  endwin();
//  refresh();

  // Need to clear memory
  // LEDStringObserver

	std::cout << "Waiting for hex to power down..." << std::endl;
//	while (hex.walking()) {
		updateStateWithCommand(27);
		usleep(10000);
	//}
	std::cout << " - Done." << std::endl;

	std::cout << "Joining thread ..." << std::endl;
	pthread_join(behaviorThread, NULL);
	std::cout << " - Done." << std::endl;

	std::cout << "Thanks for letting me express myself.  -Nico" << std::endl;
	std::cout << "Done with main" << std::endl;
  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////
//                    Helpers                     //
////////////////////////////////////////////////////
void init() {
	keepThreadAlive = true;
	terminate = false;

  // ensure manual is off by default
  manual = false;
  use_osc = false;
  curses = false;
  
  // console index should start at 0
  behavior_index = 0;
  
  // set the default blend duration
  current_blend_duration = DEFAULT_BLEND_DURATION;
  
  // behavior states
  current_behavior = BEHAVIOR_ID_STAND;
  previous_behavior = -1;
  current_magnitude = 1.0;
  previous_magnitude = 0.0;
  current_behavior_name = std::string(BEHAVIOR_NAME_STAND);
  previous_behavior_name = std::string("None");
  first_behavior = true;
  remaining_time = 0.0;
  script_finished = false;
  
  // set up lighting
  current_lighting = LIGHTING_ID_OFF;
  current_lighting_period = 0.0;
  lighting_counter = 1000;
  
  // set up time
  behavior_duration = 0.0;
  timer_iteration.initialize();
  timer_behavior.initialize();
  
  // debugging
  time_marker = 0.0;
  debug_line = DEFAULT_DEBUG_START_LINE;
  
  hexapodType = HEXAPOD_TYPE_NICO;
}

void initHex() {
  
  //mDanceParameters.setFromType(hexapodType);
	std::string danceConfigurationFilename;
	switch (hexapodType) {
  case HEXAPOD_TYPE_ANOUK:
			danceConfigurationFilename = "danceAnouk.json";
			voltageShutdownThreshold = 6.0;
			break;

		case HEXAPOD_TYPE_DMITRI:
			danceConfigurationFilename = "danceDmitri.json";
			voltageShutdownThreshold = 10.0;
			break;

		case HEXAPOD_TYPE_ANOUK_CABLE:
			danceConfigurationFilename = "danceAnoukCable.json";
			voltageShutdownThreshold = 6.0;
			break;

		default:
			danceConfigurationFilename = "danceNico.json";
			voltageShutdownThreshold = 6.0;

	}

	std::string jsonDanceConfigurationPath = std::string(getResourceDirectory()) + "/configurations/" + danceConfigurationFilename;
	mDanceParameters.setFromFile(jsonDanceConfigurationPath);


  // This is the dynamixel handler that the hexapod will use.
  int baudrate = 1000000;
	if(dynamixelHandler.openInterface(Interface::FTDI, baudrate) == Interface::NOERROR)
  {
    std::cerr << "  - Opened an FTDI device!\n";
  } else {
    if(dynamixelHandler.openInterface(Interface::COM, baudrate, "/dev/ttyACM0"))
    {
      std::cerr << "  - Opened an serial port!\n";
    } else {
      std::cerr << "  - ERROR! Unable to open serial port or an FTDI device, disabling functionality.\n";
    }
  }
  if (jsonFileName.compare("dmitri.json") == 0 ||
	  jsonFileName.compare("dmitriFly.json") == 0) {	// Bunting: Bleh...
    dynamixelHandler.setPacketType(Packet::VER_1_0);
  } else {
    dynamixelHandler.setPacketType(Packet::VER_2_0);
  }

  hex.setDynamixelHandler( &dynamixelHandler ); // must happen here otherwise calibration will not be set
  
	std::string jsonConfigurationPath = std::string(getResourceDirectory()) + "/configurations/" + jsonFileName;
  std::cout << "Loading configuration file: " << jsonConfigurationPath << std::endl;
  std::ifstream jsonConfigurationFile( jsonConfigurationPath.c_str() );
  std::string jsonConfiguration((std::istreambuf_iterator<char>(jsonConfigurationFile)),
                                std::istreambuf_iterator<char>());
  std::cout << " - Parsing ... ";
  hex.setConfigurationFromJSONString(jsonConfiguration);
  std::cout << "Done." << std::endl;
  
//  debug("init kinematics");
//  hex.initializeKinematics();
//  debug("done init'ing kinematics");

  // For walking around:
  headingControl.enableLocationControl();
  headingControl.setAcceleration(mDanceParameters.headingAcceleration);
  headingControl.setMaxSpeed(mDanceParameters.headingMaxSpeed);
  
  locationControl.enableLocationControl();
  locationControl.setAcceleration(mDanceParameters.locationAcceleration);
  locationControl.setMaxSpeed(mDanceParameters.locationMaxSpeed);
  
  locationControl.setGoalPosition(zeroVector);
  headingControl.setGoalPosition(0);
  
  //hex.disableWalking(); // BUT FIRST!  disable all walking dynamics


//  bool needToSaveFoot = true;
  
  LowPassFilter<Vector> userControlLPF, userControlLPF2;
  userControlLPF.setTimeConstant(0.5);
  userControlLPF2.setTimeConstant(0.5);
  LowPassFilter<double> standingHeightLPF;
  standingHeightLPF.setTimeConstant(0.5);
  
  danceState.updateStates(&hex);
  
  danceState.controlMode = KEYBOARD;
  
  //hex.disableWalking(); // BUT FIRST!  disable all walking dynamics

	for (std::map<unsigned char, Motor*>::iterator it = hex.dyn.begin(); it != hex.dyn.end(); it++) {
		Motor* dynamixel = it->second;
		dynamixel->addObserver(&voltageObserver);
	}

#ifdef DANGER_ZONE
	for (std::vector<Motor*>::iterator it = hex.dyn.begin(); it != hex.dyn.end(); it++) {
		Motor* dynamixel = *it;
		dynamixel->registers[Alarm_Shutdown] = 0x00;
	}
	dynamixelHandler.pushInstruction(new Instruction(Instruction::SYNC_WRITE, Alarm_Shutdown, 1));
#endif
  debug("power up");
  hex.powerUp();
  debug("powered up");
}


App::AppOption ledOptions;
JsonStringObserver *LEDStringObserver;
void initAppInterface() {
  appInterface.setPort( 6789 );
  appInterface.start();
  
  //	AppOption ledOptions;
  //	ledOptions.optionType = APP_SEGMENTED_CONTROL;
  //	ledOptions.segments.push_back("Off");
  //	ledOptions.segments.push_back("R");
  //	ledOptions.segments.push_back("G");
  //	ledOptions.segments.push_back("B");
  //	ledOptions.segments.push_back("X");
  //	ledOptions.segments.push_back("A");
  //	ledOptions.title = "LED";
  //	appInterface.addOptionMenuOption(&ledOptions);
  //
  ////	JsonStringObserver LEDStringObserver( &danceState );
  //	LEDStringObserver = new JsonStringObserver( &danceState );
  //	appInterface.addJsonValueListener("LED", LEDStringObserver);
}

void initCurses() {
  debug("init curses");
  srand((unsigned)time(NULL));
 // setTimer();
  setSignals();
  if (curses) {
    if ((mainwin = initscr()) == NULL) {
      std::cerr << "Error initializing ncurses" << std::endl;
      exit(EXIT_NCURSES_FAILED);
    }
    noecho();
    keypad(mainwin, TRUE);
    oldcur = curs_set(0);
  }
}

void cli(int argc, char *argv[]) {
  
  if (argc < 2) {
    manual = false;
    use_osc = true;
  }
      
  if (cmdOptionExists(argv, argv+argc, "-o")) {
    manual = false;
    use_osc = true;
    curses = false;
  }

	if(cmdOptionExists(argv, argv+argc, "-s"))
	{
		initAppInterface();
	}

  if(cmdOptionExists(argv, argv+argc, "-t"))
  {
    botGroupParam = getCmdOption(argv, argv + argc, "-t");
  }
  
  std::cout << "botgroup: " << botGroupParam << std::endl;
  
  if(cmdOptionExists(argv, argv+argc, "-d")) {
    daemonMode = true;
    curses = false;
  } else {
    daemonMode = false;
    curses = true;
  }
    
  if(cmdOptionExists(argv, argv+argc, "-j"))
  {
    jsonFileName = getCmdOption(argv, argv + argc, "-j");
  }
  
  if (jsonFileName.compare("dmitri.json") == 0 ||
	  jsonFileName.compare("dmitriFly.json") == 0) {	// Bunting: I don't like this method...
    hexapodType = HEXAPOD_TYPE_DMITRI;
  } else if (jsonFileName.compare("anouk.json") == 0) {
    hexapodType = HEXAPOD_TYPE_ANOUK;
  }
  
  
  // if we have more then check them
  for (int ii = 1; ii < argc; ii++) {
    std::string arg = std::string(argv[ii]);
    
    // argument for manual control
    if (isArg(arg, 'm') || isArg(arg, 'M')) {
      manual = true;
      use_osc = false;
    }
  }
}

bool isArg(std::string arg, char id) {
  if (arg.length() >= 2 && arg.at(0) == '-' && arg.at(1) == id) {
    manual = true;
    return true;
  }
  return false;
}

void usage(char *program) {
  usage(std::string(program));
}

void usage(std::string program) {
  std::cout << "Usage: " << program << " [Filename | -manual] [-j nico.json]" << std::endl;
}

void load(char *filename) {
  load(std::string(filename));
}

void load(std::string filename) {
  // define the line and open the file
  char line[100];
  std::ifstream file(filename.c_str(), std::ifstream::in);
  
  // read in the file
  while (file.good()) {
    // get a line
    file.getline(line, 100);
    
    // continue if the line starts with a hash (comments)
    if (line[0] == '#') {
      continue;
    }
    
    // convert to a vector of strings for easy manipulation, split on spaces
    std::vector<std::string> strings = split(line);
    
    // make sure we at least have a behavior and a duration
    if (strings.size() < 2) {
      continue;
    }
    
    // check which item this is
    bool success = true;
    if (strings.at(0).compare(BEHAVIOR_NAME_STAND) == 0) {
      behaviors.push_back(BEHAVIOR_ID_STAND);
    } else if (strings.at(0).compare(BEHAVIOR_NAME_FIST_BUMP_RIGHT) == 0) {
      behaviors.push_back(BEHAVIOR_ID_FIST_BUMP_RIGHT);
    } else if (strings.at(0).compare(BEHAVIOR_NAME_FIST_BUMP_LEFT) == 0) {
      behaviors.push_back(BEHAVIOR_ID_FIST_BUMP_LEFT);
    } else if (strings.at(0).compare(BEHAVIOR_NAME_FIST_BUMP_DOUBLE) == 0) {
      behaviors.push_back(BEHAVIOR_ID_FIST_BUMP_DOUBLE);
    } else if (strings.at(0).compare(BEHAVIOR_NAME_LEG_WAVE_CLOCKWISE) == 0) {
      behaviors.push_back(BEHAVIOR_ID_LEG_WAVE_CLOCKWISE);
    } else if (strings.at(0).compare(BEHAVIOR_NAME_LEG_WAVE_COUNTER_CLOCKWISE) == 0) {
      behaviors.push_back(BEHAVIOR_ID_LEG_WAVE_COUNTER_CLOCKWISE);
    } else if (strings.at(0).compare(BEHAVIOR_NAME_BODY_WAVES) == 0) {
      behaviors.push_back(BEHAVIOR_ID_BODY_WAVES);
    } else if (strings.at(0).compare(BEHAVIOR_NAME_SHUFFLE_RIGHT) == 0) {
      behaviors.push_back(BEHAVIOR_ID_SHUFFLE_RIGHT);
    } else if (strings.at(0).compare(BEHAVIOR_NAME_SHUFFLE_LEFT) == 0) {
      behaviors.push_back(BEHAVIOR_ID_SHUFFLE_LEFT);
    } else if (strings.at(0).compare(BEHAVIOR_NAME_SHUFFLE_FORWARD) == 0) {
      behaviors.push_back(BEHAVIOR_ID_SHUFFLE_FORWARD);
    } else if (strings.at(0).compare(BEHAVIOR_NAME_SHUFFLE_BACK) == 0) {
      behaviors.push_back(BEHAVIOR_ID_SHUFFLE_BACK);
    } else if (strings.at(0).compare(BEHAVIOR_NAME_CORPSE) == 0) {
      behaviors.push_back(BEHAVIOR_ID_CORPSE);
    } else if (strings.at(0).compare(BEHAVIOR_NAME_RESURRECTION) == 0) {
      behaviors.push_back(BEHAVIOR_ID_RESURRECTION);
    } else if (strings.at(0).compare(BEHAVIOR_NAME_CIRCLE_RIGHT) == 0) {
      behaviors.push_back(BEHAVIOR_ID_CIRCLE_RIGHT);
    } else if (strings.at(0).compare(BEHAVIOR_NAME_CIRCLE_LEFT) == 0) {
      behaviors.push_back(BEHAVIOR_ID_CIRCLE_LEFT);
		} else if (strings.at(0).compare(BEHAVIOR_NAME_FLY) == 0) {
			behaviors.push_back(BEHAVIOR_ID_FLY);
		} else if (strings.at(0).compare(BEHAVIOR_NAME_KILL) == 0) {
			behaviors.push_back(BEHAVIOR_ID_KILL);
		} else if (strings.at(0).compare(BEHAVIOR_NAME_HIP_SWING) == 0) {
			behaviors.push_back(BEHAVIOR_ID_HIP_SWING);
		} else if (strings.at(0).compare(BEHAVIOR_NAME_BODY_WAVE_BUMP) == 0) {
			behaviors.push_back(BEHAVIOR_ID_BODY_WAVE_BUMP);
		} else {
      success = false;
    }
    
    // if we found a valid behavior then add the duration and blend duration
    if (success) {
      durations.push_back(atof(strings.at(1).c_str()));
      if (strings.size() > 2) {
        blend_durations.push_back(atof(strings.at(2).c_str()));
      } else {
        blend_durations.push_back(DEFAULT_BLEND_DURATION);
      }
    }
  }
}

Vector fromVectorToVector(Vector from, Vector to, double magnitude) {
  if (from.size() >= to.size()) {
    Vector value(to.size());
    for (int ii = 0; ii < value.size(); ii++) {
      value(ii) = fromValueToValue(from(ii), to(ii), magnitude);
    }
    std::stringstream msg;
    msg << "f: (" << from(0) << "," << from(1) << "," << from(2) << ") t: (" << to(0) << "," << to(1) << "," << to(2) << ") m: " << magnitude << " v: " << value(0) << "," << value(1) << "," << value(2) << ")";
    debug(msg.str());
    return value;
  } //else if (from.size() < to.size()) {
  Vector value(from.size());
  for (int ii = 0; ii < value.size(); ii++) {
    value(ii) = fromValueToValue(from(ii), to(ii), magnitude);
  }
  std::stringstream msg;
  msg << "f: (" << from(0) << "," << from(1) << "," << from(2) << ") t: (" << to(0) << "," << to(1) << "," << to(2) << ") m: " << magnitude << " v: " << value(0) << "," << value(1) << "," << value(2) << ")";
  debug(msg.str());
  return value;
  //}
}

double fromValueToValue(double from, double to, double magnitude) {
  if (magnitude < 0.0) {
    magnitude = 0.0;
  } else if (magnitude > 1.0) {
    magnitude = 1.0;
  }
  double value = from;
  if (from < to) {
    value = (to - from) * magnitude + from;
  } else if (from > to) {
    value = (from - to) * (1.0 - magnitude) + to;
  }
  return value;
}

std::vector<std::string> split(char *str) {
  return split(std::string(str), DEFAULT_STRING_SPLIT_DELIMITER);
}

std::vector<std::string> split(std::string str) {
  return split(str, DEFAULT_STRING_SPLIT_DELIMITER);
}

std::vector<std::string> split(char *str, char delim) {
  return split(std::string(str), delim);
}

std::vector<std::string> split(std::string str, char delim) {
  std::vector<std::string> strings;
  int index = 0;
  int found = 0;
  while ((found = str.find(delim, index)) != -1) {
    if (found - index > 1) {
      strings.push_back(str.substr(index, found - index));
      index = found + 1;
    } else {
      index++;
    }
  }
  if (index < str.length()) {
    strings.push_back(str.substr(index, str.length() - index));
  }
  return strings;
}

////////////////////////////////////////////////////
//                   Behaviors                    //
////////////////////////////////////////////////////
void callBehavior(int id, double magnitude) {
  switch (id) {
    case BEHAVIOR_ID_STAND:
      behavior_stand(id, magnitude);
      break;
    case BEHAVIOR_ID_FIST_BUMP_RIGHT:
      behavior_fist_bump_right(id, magnitude);
      break;
    case BEHAVIOR_ID_FIST_BUMP_LEFT:
      behavior_fist_bump_left(id, magnitude);
      break;
    case BEHAVIOR_ID_FIST_BUMP_DOUBLE:
      behavior_fist_bump_double(id, magnitude);
      break;
    case BEHAVIOR_ID_LEG_WAVE_CLOCKWISE:
      behavior_leg_wave_clockwise(id, magnitude);
      break;
    case BEHAVIOR_ID_LEG_WAVE_COUNTER_CLOCKWISE:
      behavior_leg_wave_counter_clockwise(id, magnitude);
      break;
    case BEHAVIOR_ID_BODY_WAVES:
      behavior_body_waves(id, magnitude);
      break;
    case BEHAVIOR_ID_SHUFFLE_RIGHT:
      behavior_shuffle_right(id, magnitude);
      break;
    case BEHAVIOR_ID_SHUFFLE_LEFT:
      behavior_shuffle_left(id, magnitude);
      break;
    case BEHAVIOR_ID_SHUFFLE_FORWARD:
      behavior_shuffle_forward(id, magnitude);
      break;
    case BEHAVIOR_ID_SHUFFLE_BACK:
      behavior_shuffle_back(id, magnitude);
      break;
    case BEHAVIOR_ID_CORPSE:
      behavior_corpse(id, magnitude);
      break;
    case BEHAVIOR_ID_RESURRECTION:
      behavior_resurrection(id, magnitude);
      break;
    case BEHAVIOR_ID_CIRCLE_RIGHT:
      behavior_circle_right(id, magnitude);
      break;
    case BEHAVIOR_ID_CIRCLE_LEFT:
      behavior_circle_left(id, magnitude);
      break;
		case BEHAVIOR_ID_FLY:
			behavior_fly(id, magnitude);
			break;
		case BEHAVIOR_ID_KILL:
			behavior_kill(id, magnitude);
			break;
		case BEHAVIOR_ID_HIP_SWING:
			behavior_hip_swing(id, magnitude);
			break;
		case BEHAVIOR_ID_BODY_WAVE_BUMP:
			behavior_body_wave_bump(id, magnitude);
			break;
    default:
      // do nothing
      break;
  }
}

void behavior_stand(int id, double magnitude) {
	hex.enableWalking();
	if (newBehavior == true) {
		newBehavior = false;
	}
//  for (int i = 0; i < 6; i++) {
//    hex.foot[i] = getRestingFootVector(i); // + magnitude * 0
//  }
}

void behavior_fist_bump_right(int id, double magnitude) {
  hex.disableWalking();
  debug_line = DEFAULT_DEBUG_START_LINE;
  std::stringstream msg;
  msg << "behavior fist bump right: " << hex.foot[1](0) << "\t" <<  hex.foot[1](1) << "\t" <<  hex.foot[1](2);
  debug(msg.str());
  

    //    if (magnitude <= (2 * current_blend_duration * (DEFAULT_UPDATE_PERIOD * 1e-6))) {
    if (newBehavior == true) {
      newBehavior = false;
      // save the right foot state
      saved_right_front = getRestingFootVector(1);
    } else {
      // frequency for movement
      double currentFrequency = 128. /(60. * 2. * 16.);
      //      Quaternion hexapodOrientation = hex.getBodyOrientation();
      Quaternion hexapodOrientation; // Bunting:
      hexapodOrientation.makeFromAngleAndAxis(hex.getWalkingDynamics()->getBodyHeading(), Vector::zAxis);
      Quaternion additionalRotation;
      additionalRotation.makeFromAngleAndAxis(MOGI_PI/4.0, Vector::zAxis);
      
      // coordinate for the foot's center position
      Vector footXYCenter = (mDanceParameters.fistBumpCenterRadiusFromBody) * sqrt(2)/2 *((const Quaternion&)(additionalRotation*hexapodOrientation)).makeRotationMatrix() * Vector::xAxis;
      
      std::stringstream msg2;
      msg2 << "foot xy center: " << footXYCenter(0) << "\t" <<  footXYCenter(1) << "\t" <<  footXYCenter(2);
      debug(msg2.str());
      //      std::stringstream msg3;
      //      msg3 << "saved position: " << saved_right_front(0) << "\t" <<  saved_right_front(1) << "\t" <<  saved_right_front(2);
      //      debug(msg3.str());
      
      
      Vector footLocation(3);
      
      footLocation(0) = footXYCenter(0) + mDanceParameters.fistBumpRadius*cos(behavior_duration * MOGI_PI * currentFrequency * 32.0);
      footLocation(1) = footXYCenter(1) + mDanceParameters.fistBumpRadius*sin(behavior_duration * MOGI_PI * currentFrequency * 32.0);
      footLocation(2) = mDanceParameters.fistBumpHeight;//200;

			  hex.beginModifying();
			{
      hex.foot[1] = getRestingFootVector(1) + footLocation*magnitude;
    }
			hex.endModifying();
  }
}

void behavior_fist_bump_left(int id, double magnitude) {
  hex.disableWalking();
  debug_line = DEFAULT_DEBUG_START_LINE;
  std::stringstream msg;
  msg << "behavior fist bump left: " << hex.foot[2](0) << "\t" <<  hex.foot[2](1) << "\t" <<  hex.foot[2](2);
  debug(msg.str());
  

    //    if (magnitude <= (2 * current_blend_duration * (DEFAULT_UPDATE_PERIOD * 1e-6))) {
    if (newBehavior == true) {
      newBehavior = false;
      
      // save the left foot state
      saved_left_front = getRestingFootVector(2);
    } else {
      // frequency for arm motions
      double currentFrequency = 128. /(60. * 2. * 16.);
      //			Quaternion hexapodOrientation = hex.getBodyOrientation();
      Quaternion hexapodOrientation;	// Bunting:
      hexapodOrientation.makeFromAngleAndAxis(hex.getWalkingDynamics()->getBodyHeading(), Vector::zAxis);
      Quaternion additionalRotation;
      additionalRotation.makeFromAngleAndAxis(3*MOGI_PI/4.0, Vector::zAxis);
      
      // coordinate for the foot's center position
      Vector footXYCenter = (mDanceParameters.fistBumpCenterRadiusFromBody) * sqrt(2)/2 *((const Quaternion&)(additionalRotation*hexapodOrientation)).makeRotationMatrix() * Vector::xAxis;

      // debugging
      std::stringstream msg2;
      msg2 << "foot xy center: " << footXYCenter(0) << "\t" <<  footXYCenter(1) << "\t" <<  footXYCenter(2);
      debug(msg2.str());
      std::stringstream msg3;
      msg3 << "saved position: " << saved_left_front(0) << "\t" <<  saved_left_front(1) << "\t" <<  saved_left_front(2);
      debug(msg3.str());
      
      Vector footLocation(3);
      
      // move the foot
      footLocation(0) = footXYCenter(0) + mDanceParameters.fistBumpRadius*cos(behavior_duration * MOGI_PI * currentFrequency * 32.0);
      footLocation(1) = footXYCenter(1) + mDanceParameters.fistBumpRadius*sin(behavior_duration * MOGI_PI * currentFrequency * 32.0);
      footLocation(2) = mDanceParameters.fistBumpHeight;//200;

			hex.beginModifying();
			{
      	hex.foot[2] = getRestingFootVector(2) + footLocation*magnitude;
    	}
			hex.endModifying();
  }

}

void behavior_fist_bump_double(int id, double magnitude) {
  hex.disableWalking();
  debug_line = DEFAULT_DEBUG_START_LINE;
  

    Vector hexapodVector = hex.getBodyVector();
    //    if (magnitude <= (2 * current_blend_duration * (DEFAULT_UPDATE_PERIOD * 1e-6))) {
    if(newBehavior) {
      newBehavior = false; //Bunting must be done after calling both behaviors.
		}
//      
//      // Bunting:
//      // Expanded this to work wiht non-Nico configurations
//      saved_corpse_foot_0 = getRestingFootVector(0);  // rotate and translate to global
//      saved_corpse_foot_1 = getRestingFootVector(1);  // rotate and translate to global
//      saved_corpse_foot_2 = getRestingFootVector(2);  // rotate and translate to global
//      saved_corpse_foot_3 = getRestingFootVector(3);  // rotate and translate to global
//      saved_corpse_foot_4 = getRestingFootVector(4);  // rotate and translate to global
//      saved_corpse_foot_5 = getRestingFootVector(5);  // rotate and translate to global
//      
//      //	saved_hexapod_orientation.makeFromAngleAndAxis(hex.getWalkingDynamics()->getBodyHeading(), zAxis);

//    } else {
      std::stringstream msg;
      msg << "behavior fist bump double: " << hexapodVector(0) << "\t" <<  hexapodVector(1) << "\t" <<  hexapodVector(2);
      debug(msg.str());
      
      // set leg magnitude goes from 0 to 1 for magnitude 0 to .3333
//      double set_leg_magnitude = magnitude * 3.0;
//      if (set_leg_magnitude > 1.0) {
//        set_leg_magnitude = 1.0;
//      }
      
      // body magnitude goes from 0 to 1 for magnitude .3333 to .6667
			double body_mag_min = 0.0;
			double body_mag_max = 0.333;
      double body_magnitude = (magnitude - body_mag_min) / (body_mag_max - body_mag_min) ;
      if (body_magnitude < 0.0) {
        body_magnitude = 0.0;
      }
      if (body_magnitude > 1.0) {
        body_magnitude = 1.0;
      }
      
      // leg magnitude goes from 0 to 1 for magnitude .6667 to 1.0
			double foot_mag_min = body_mag_max;
			double foot_mag_max = 1.0;
      double leg_magnitude = (magnitude - foot_mag_min) / (foot_mag_max - foot_mag_min);
      if (leg_magnitude < 0.0) {
        leg_magnitude = 0.0;
      }
      if (leg_magnitude > 1.0) {
        leg_magnitude = 1.0;
      }
      
      // frequency for arm motions
      double currentFrequency = 128. /(60. * 2. * 16.);
      //      Quaternion hexapodOrientation = hex.getBodyOrientation();
      Quaternion hexapodOrientation;	// Bunting:
      hexapodOrientation.makeFromAngleAndAxis(hex.getWalkingDynamics()->getBodyHeading(), Vector::zAxis);
      Quaternion additionalRotation;
      additionalRotation.makeFromAngleAndAxis(MOGI_PI/4.0, Vector::zAxis);
      
      // move feet into position, note that I don't care about going back to the previous position.
      //      hex.foot[0] = fromVectorToVector(hex.foot[0], saved_corpse_foot_0, set_leg_magnitude);	// Does this become like a y=sqrt(1-x)? Since foot is updated...
      //      hex.foot[1] = fromVectorToVector(hex.foot[1], saved_corpse_foot_1, set_leg_magnitude);
      //      hex.foot[2] = fromVectorToVector(hex.foot[2], saved_corpse_foot_2, set_leg_magnitude);
      //      hex.foot[3] = fromVectorToVector(hex.foot[3], saved_corpse_foot_3, set_leg_magnitude);
      //      hex.foot[4] = fromVectorToVector(hex.foot[4], saved_corpse_foot_4, set_leg_magnitude);
      //      hex.foot[5] = fromVectorToVector(hex.foot[5], saved_corpse_foot_5, set_leg_magnitude);
      
      // move the body back and up a bit
      bodyOffset = body_magnitude * mDanceParameters.doubleFistBumpBodyOffset;
      //Quaternion hexapodOrientation;
      //hexapodOrientation.makeFromAngleAndAxis(hex.getWalkingDynamics()->getBodyHeading(), zAxis);
      bodyOffset = hexapodOrientation.makeRotationMatrix() * bodyOffset;
      
      // angle the body so the head goes up a bit
      //Quaternion tiltAngle;
      //tiltAngle.makeFromAngleAndAxis(-15.0 * body_magnitude * MOGI_PI / 180, xAxis);
      //bodyAngleAdjust = ((const Quaternion&)(tiltAngle*saved_hexapod_orientation)).makeRotationMatrix() * xAxis;
      //      bodyAngleAdjust(0) = -15.0 * body_magnitude * MOGI_PI / 180;
      //      bodyAngleAdjust(1) = 0.0;
      //      bodyAngleAdjust(2) = 0.0;
      bodyAngleAdjust = body_magnitude * mDanceParameters.doubleFistBumpAngleOffset;
      //bodyAngleAdjust = hexapodOrientation.makeRotationMatrix() * bodyAngleAdjust;
      
      // lift the front feet
      Vector leg1(3), leg2(3);
      leg1(0) = mDanceParameters.doubleFistBumpRadius*sin(behavior_duration * MOGI_PI * currentFrequency * 32.0);
      leg1(1) = mDanceParameters.doubleFistBumpRadius*cos(behavior_duration * MOGI_PI * currentFrequency * 32.0);
      leg1(2) = mDanceParameters.doubleFistBumpHeight;
      leg2(0) = mDanceParameters.doubleFistBumpRadius*cos(behavior_duration * MOGI_PI * currentFrequency * 32.0);
      leg2(1) = mDanceParameters.doubleFistBumpRadius*sin(behavior_duration * MOGI_PI * currentFrequency * 32.0);
      leg2(2) = mDanceParameters.doubleFistBumpHeight;

			hex.beginModifying();
			{
      hex.foot[1] = getRestingFootVector(1) + leg_magnitude * leg1;
      hex.foot[2] = getRestingFootVector(2) + leg_magnitude * leg2;
      
    }
		hex.endModifying();
//  }

}

void behavior_leg_wave_clockwise(int id, double magnitude) {
  hex.disableWalking();
  // TODO: behavior code goes here
  double leg_rate = 0.5;
  int leg_start = 0;
  hex.beginModifying();
  {
    // raise the body a bit
    bodyOffset(2) = mDanceParameters.legWaveBodyHeightOffset * magnitude;
    
    // leg iteration should count from 0 to 5
    //    int leg_iteration = ((int)((behavior_duration / leg_rate) + leg_start)) % 6;
    //    // the magnitude should go from 0.0 to 1.0 then back to 0.0 then back to 1.0, etc.
    //    double leg_magnitude = 2.0 * ((behavior_duration / leg_rate) - ((int)(behavior_duration / leg_rate)));
    // Bunting: attempting to fix the jerking issue cause by resetting behavior_duration to 0 on behavior switch
    int leg_iteration = ((int)(( behaviorTime / leg_rate) + leg_start)) % 6;
    // the magnitude should go from 0.0 to 1.0 then back to 0.0 then back to 1.0, etc.
    double leg_magnitude = 2.0 * ((behaviorTime / leg_rate) - ((int)(behaviorTime / leg_rate)));
    if (leg_magnitude > 1.0) {
      leg_magnitude = 2.0 - leg_magnitude;  // this goes back down from 1.0 to 0.0
    }
    switch (leg_iteration) {
      case 0:
        hex.foot[5](2) = mDanceParameters.legWavefootLiftHeight * leg_magnitude * magnitude;
        break;
      case 1:
        hex.foot[4](2) = mDanceParameters.legWavefootLiftHeight * leg_magnitude * magnitude;
        break;
      case 2:
        hex.foot[3](2) = mDanceParameters.legWavefootLiftHeight * leg_magnitude * magnitude;
        break;
      case 3:
        hex.foot[2](2) = mDanceParameters.legWavefootLiftHeight * leg_magnitude * magnitude;
        break;
      case 4:
        hex.foot[1](2) = mDanceParameters.legWavefootLiftHeight * leg_magnitude * magnitude;
        break;
      case 5:
        hex.foot[0](2) = mDanceParameters.legWavefootLiftHeight * leg_magnitude * magnitude;
        break;
      default:
        break;
    }
  }
  hex.endModifying();
}

void behavior_leg_wave_counter_clockwise(int id, double magnitude) {
  hex.disableWalking();
  // TODO: behavior code goes here
  double leg_rate = 0.5;
  int leg_start = 0;
  hex.beginModifying();
  {
    // raise the body a bit
    bodyOffset(2) = mDanceParameters.legWaveBodyHeightOffset * magnitude;
    
    //    // leg iteration should count from 0 to 5
    //    int leg_iteration = ((int)((behavior_duration / leg_rate) + leg_start)) % 6;
    //    // the magnitude should go from 0.0 to 1.0 then back to 0.0 then back to 1.0, etc.
    //    double leg_magnitude = 2.0 * ((behavior_duration / leg_rate) - ((int)(behavior_duration / leg_rate)));
    // Bunting: attempting to fix the jerking issue cause by resetting behavior_duration to 0 on behavior switch
    int leg_iteration = ((int)(( behaviorTime / leg_rate) + leg_start)) % 6;
    // the magnitude should go from 0.0 to 1.0 then back to 0.0 then back to 1.0, etc.
    double leg_magnitude = 2.0 * ((behaviorTime / leg_rate) - ((int)(behaviorTime / leg_rate)));
    if (leg_magnitude > 1.0) {
      leg_magnitude = 2.0 - leg_magnitude;  // this goes back down from 1.0 to 0.0
    }
    switch (leg_iteration) {
      case 0:
        hex.foot[0](2) = mDanceParameters.legWavefootLiftHeight * leg_magnitude * magnitude;
        break;
      case 1:
        hex.foot[1](2) = mDanceParameters.legWavefootLiftHeight * leg_magnitude * magnitude;
        break;
      case 2:
        hex.foot[2](2) = mDanceParameters.legWavefootLiftHeight * leg_magnitude * magnitude;
        break;
      case 3:
        hex.foot[3](2) = mDanceParameters.legWavefootLiftHeight * leg_magnitude * magnitude;
        break;
      case 4:
        hex.foot[4](2) = mDanceParameters.legWavefootLiftHeight * leg_magnitude * magnitude;
        break;
      case 5:
        hex.foot[5](2) = mDanceParameters.legWavefootLiftHeight * leg_magnitude * magnitude;
        break;
      default:
        break;
    }
  }
  hex.endModifying();
}

void behavior_body_waves(int id, double magnitude) {
  hex.disableWalking();

	// Lakshman HAATES this:
	//			headAngles(0) =  (0*(26.506 - 3.7601)/2.0 + .5 *(26.506 - 3.7601)*sin(27.0/40.0 * MOGI_PI*time.runningTime())) * MOGI_PI/180.0;
	//			headAngles(1) = .5*0.9 * 20.0 * sin(30.0/40.0*MOGI_PI*time.runningTime()) * MOGI_PI/180.0;
	//			headAngles(2) = .5*.6 * 13.0 * sin(24.0/40.0*MOGI_PI*time.runningTime()) * MOGI_PI/180.0;
	// Lakshman LOVES this:
	//			double newRatePercentage = 1.5;
	//			headAngles(0) =  ((26.506 - 3.7601)/2.0 + .5 *(26.506 - 3.7601)*sin(27.0/40.0 * MOGI_PI*time.runningTime()*newRatePercentage)) * MOGI_PI/180.0;
	//			headAngles(1) = 0.9 * 20.0 * sin(30.0/40.0*MOGI_PI*time.runningTime()*newRatePercentage) * MOGI_PI/180.0;
	//			headAngles(2) = .6 * 13.0 * sin(24.0/40.0*MOGI_PI*time.runningTime()*newRatePercentage) * MOGI_PI/180.0;
	// New time code:
	// double newRatePercentage = 1.5;
	// What lakshman likes is the above for the z which results in a timing of 0.45*2pi*time
	// For 115 BPM, let's do 115/60/4 ~= .48
	double newFrequency = 115.0/60.0/4.0;
//	headAngles(0) =  ((26.506 - 3.7601)/2.0 + .5 *(26.506 - 3.7601)*sin(newFrequency * 2.0*MOGI_PI * behaviorTime*1.1)) * MOGI_PI/180.0;
//	headAngles(1) = 0.9 * 20.0 * sin(newFrequency * 2.0*MOGI_PI * behaviorTime +MOGI_PI/2) * MOGI_PI/180.0;
//	headAngles(2) = .6 * 13.0 * sin(newFrequency * 2.0*MOGI_PI * behaviorTime) * MOGI_PI/180.0;

	Vector headAngleMagnitude(3);
	for (int i = 0; i < 3; i++)	{
	headAngleMagnitude(i) = mDanceParameters.waveHeadAngleMagnitude(i)*
		sin(newFrequency * 2.0*MOGI_PI * behaviorTime * mDanceParameters.waveHeadAngleFrequencySkew(i) + mDanceParameters.waveHeadAnglePhase(i));
	}
	headAngles = mDanceParameters.waveHeadAngleOffset + headAngleMagnitude;

  // With the desired head angles, now apply motions to the body:
  Matrix xRot(3,3), yRot(3,3), zRot(3,3), totalRotation;
  xRot.makeXRotation( -headAngles(0) );
  yRot.makeYRotation( -headAngles(1) );
  zRot.makeZRotation( -headAngles(2) );
  totalRotation = xRot * zRot * yRot;
  
  Quaternion headQuaternion;
	headQuaternion = totalRotation.rotationToQuaternion();
  
	bodyAngleAdjust = headQuaternion.eulerAngles();
  
  // Now that the body is rotated, need to shift the body so that the head is stationary
  //  Matrix camOrigin(3,1);
  //
  //  camOrigin(0,0) = 0.0;
  //  camOrigin(1,0) = 40;
  //  //camOrigin(1,0) = 133.48;
  //  camOrigin(2,0) = 23.6;
  
  xRot.makeXRotation( -bodyAngleAdjust(0) );
  yRot.makeYRotation( -bodyAngleAdjust(1) );
  zRot.makeZRotation( -bodyAngleAdjust(2) );
  
  Matrix camRotate = xRot*yRot*zRot*mDanceParameters.cameraOrigin;
  
  Vector camOffset = mDanceParameters.cameraOrigin - camRotate; // CHECK THIS changed Matrix to Vector
  
  bodyOffset = camOffset + mDanceParameters.waveBodyOffset;
  
  headAngles *= magnitude;
  bodyOffset *= magnitude;
  if(bodyAngleAdjust(0) > MOGI_PI)
    bodyAngleAdjust(0) -= 2*MOGI_PI;
  if(bodyAngleAdjust(0) < -MOGI_PI)
    bodyAngleAdjust(0) += 2*MOGI_PI;
  bodyAngleAdjust *= magnitude;
}

bool pivotHigh = false;
double sideShufflePivotRatio = 1.1;
void behavior_shuffle_right(int id, double magnitude) {
  hex.enableWalking();
  //  if (magnitude <= (2 * current_blend_duration * (DEFAULT_UPDATE_PERIOD * 1e-6))) {
		if (newBehavior == true) {
      newBehavior = false; //Bunting must be done after calling both behaviors.
    }
	if (magnitude > 0.75) {
		if (pivotHigh == false) {
			for (int i = 0; i < 6; i++) {
//				hex.footPivotForWalking[i] *= sideShufflePivotRatio;
			}
			pivotHigh = true;
		}
	} else {
		if (pivotHigh == true) {
			for (int i = 0; i < 6; i++) {
//				hex.footPivotForWalking[i] /= sideShufflePivotRatio;
			}
			pivotHigh = false;
		}
	}
  if (previous_magnitude <= 1e-6) {	// Bunting: we cannot begin walking until feet are no longer controlled
		Quaternion hexapodOrientation;
		hexapodOrientation.makeFromAngleAndAxis(hex.getWalkingDynamics()->getBodyHeading(), Vector::zAxis);
    bodyVelocity(0) = magnitude * mDanceParameters.walkingSpeedHorizontal;
    bodyVelocity(1) = 0.0;
    bodyVelocity(2) = 0.0;
    bodyVelocity = hexapodOrientation.makeRotationMatrix() * bodyVelocity;

  }
	bodyOffset(2) = 20*magnitude;
  
}

void behavior_shuffle_left(int id, double magnitude) {
  hex.enableWalking();
  //  if (magnitude <= (2 * current_blend_duration * (DEFAULT_UPDATE_PERIOD * 1e-6))) {
		if (newBehavior == true) {
      newBehavior = false; //Bunting must be done after calling both behaviors.
    }
	if (magnitude > 0.75) {
		if (pivotHigh == false) {
			for (int i = 0; i < 6; i++) {
//				hex.footPivotForWalking[i] *= sideShufflePivotRatio;
			}
			pivotHigh = true;
		}
	} else {
		if (pivotHigh == true) {
			for (int i = 0; i < 6; i++) {
//				hex.footPivotForWalking[i] /= sideShufflePivotRatio;
			}
			pivotHigh = false;
		}
	}
  if (previous_magnitude <= 1e-6) {
		Quaternion hexapodOrientation;
		hexapodOrientation.makeFromAngleAndAxis(hex.getWalkingDynamics()->getBodyHeading(), Vector::zAxis);
    bodyVelocity(0) = -magnitude * mDanceParameters.walkingSpeedHorizontal;
    bodyVelocity(1) = 0.0;
    bodyVelocity(2) = 0.0;
    bodyVelocity = hexapodOrientation.makeRotationMatrix() * bodyVelocity;
  }
	bodyOffset(2) = 20*magnitude;
}

void behavior_shuffle_forward(int id, double magnitude) {
  hex.enableWalking();
  //  if (magnitude <= (2 * current_blend_duration * (DEFAULT_UPDATE_PERIOD * 1e-6))) {
		if (newBehavior == true) {
      newBehavior = false; //Bunting must be done after calling both behaviors.
    }
  if (previous_magnitude <= 1e-6) {
		Quaternion hexapodOrientation;
		hexapodOrientation.makeFromAngleAndAxis(hex.getWalkingDynamics()->getBodyHeading(), Vector::zAxis);
    bodyVelocity(0) = 0.0;
    bodyVelocity(1) = magnitude * mDanceParameters.walkingSpeedVertical;
    bodyVelocity(2) = 0.0;
    bodyVelocity = hexapodOrientation.makeRotationMatrix() * bodyVelocity;
  }
}


void behavior_shuffle_back(int id, double magnitude) {
  hex.enableWalking();
  //  if (magnitude <= (2 * current_blend_duration * (DEFAULT_UPDATE_PERIOD * 1e-6))) {
  if (newBehavior == true) {
    newBehavior = false; //Bunting must be done after calling both behaviors.
  }
  if (previous_magnitude <= 1e-6) {
		Quaternion hexapodOrientation;
		hexapodOrientation.makeFromAngleAndAxis(hex.getWalkingDynamics()->getBodyHeading(), Vector::zAxis);
    bodyVelocity(0) = 0.0;
    bodyVelocity(1) = -magnitude * mDanceParameters.walkingSpeedVertical;
    bodyVelocity(2) = 0.0;
    bodyVelocity = hexapodOrientation.makeRotationMatrix() * bodyVelocity;
  }
}

//double deltaBodyCorpse=0;
void behavior_corpse(int id, double magnitude) {
  debug_line = DEFAULT_DEBUG_START_LINE;
//  hex.beginModifying();
  {
    //    if (magnitude <= (2 * current_blend_duration * (DEFAULT_UPDATE_PERIOD * 1e-6))) {
    if (newBehavior == true) {
      newBehavior = false; //Bunting must be done after calling both behaviors.
      // save the state
//      Vector saved = hex.getBodyVector();
//      saved_body_position = saved;
//      deltaBodyCorpse = saved(2);
      //      saved_corpse_foot_0 = hex.foot[0]; // saved_corpse_foot_0(1) = hex.foot[0](1); saved_corpse_foot_0(2) = hex.foot[0](2);
      //      saved_corpse_foot_1 = hex.foot[1]; // Bunting: the "=" operator is overloaded to make this less laborious
      //      saved_corpse_foot_2 = hex.foot[2];
      //      saved_corpse_foot_3 = hex.foot[3];
      //      saved_corpse_foot_4 = hex.foot[4];
      //      saved_corpse_foot_5 = hex.foot[5];
      //saved_hexapod_orientation = bodyAngleAdjust;
    }
		{
      // from magnitude 0.0 to 0.5 bring the body down
      //      double body_magnitude = magnitude * 2.0;
      //      if (body_magnitude > 1.0) {
      //        body_magnitude = 1.0;
      //      }
      //
      //      // from magnitude 0.5 to 1.0 bring the legs in from 0.0 to 0.25
      //			double foot_magnitude = 0;//(magnitude * 2.0 - 1.0) / 4.0;
      //      if (foot_magnitude < 0.0) {
      //        foot_magnitude = 0.0;
      //      }
      
      if (magnitude == 0.0) {
//				hex.keepUpdatingMotors = true;	// Bunting: IDF total hack
        for (std::map<unsigned char, Motor*>::iterator it = hex.dyn.begin(); it != hex.dyn.end(); it++) {
          Motor* dynamixel = it->second;
          dynamixel->setByte(REG_Torque_EN, 1);
        }
        dynamixelHandler.pushInstruction(new Instruction(Instruction::SYNC_WRITE, DataRange( REG_Torque_EN, 1)));
      } else if (magnitude == 1.0) {
//				hex.keepUpdatingMotors = false;	// Bunting: IDF total hack
        for (std::map<unsigned char, Motor*>::iterator it = hex.dyn.begin(); it != hex.dyn.end(); it++) {
          Motor* dynamixel = it->second;
          dynamixel->setByte(REG_Torque_EN, 0);
        }
        dynamixelHandler.pushInstruction(new Instruction(Instruction::SYNC_WRITE, DataRange(REG_Torque_EN, 1)));
      }
      // drop the body
      bodyOffset(2) = -hex.standingHeight * magnitude;
      //	bodyOffset(0) = magnitude * 100;
      
      // untwist the body
      //bodyAngleAdjust *= (1.0 - body_magnitude);
      
      // debugging
      std::stringstream msg;
      msg << "sb: (" << saved_body_position(0) << "," << saved_body_position(1) << "," << saved_body_position(2) << ") bo: (" << bodyOffset(0) << "," << bodyOffset(1) << "," << bodyOffset(2) << ")";
      debug(msg.str());
      
      // bring the feet in
      //      hex.foot[0] = fromVectorToVector(getRestingFootVector(0), saved_body_position + bodyOffset, foot_magnitude);
      //      hex.foot[1] = fromVectorToVector(getRestingFootVector(1), saved_body_position + bodyOffset, foot_magnitude);
      //      hex.foot[2] = fromVectorToVector(getRestingFootVector(2), saved_body_position + bodyOffset, foot_magnitude);
      //      hex.foot[3] = fromVectorToVector(getRestingFootVector(3), saved_body_position + bodyOffset, foot_magnitude);
      //      hex.foot[4] = fromVectorToVector(getRestingFootVector(4), saved_body_position + bodyOffset, foot_magnitude);
      //      hex.foot[5] = fromVectorToVector(getRestingFootVector(5), saved_body_position + bodyOffset, foot_magnitude);
    }
  }
//  hex.endModifying();
}

void behavior_resurrection(int id, double magnitude) {
  // TODO: behavior code goes here
  if (newBehavior == true) {
    newBehavior = false;
  }
  //hex.beginModifying();
  {
    //bodyOffset(2) = 0;//-deltaBodyCorpse * (1.0-magnitude);
    //		if (newBehavior == true) {
    //			newBehavior = false;
    //			hex.powerUp();
    ////    hex.setWalking(false);
    ////    hex.setWalking(true);
    //		}
  }
//  hex.endModifying();
}

void behavior_circle_right(int id, double magnitude) {
	if (newBehavior) {
		newBehavior = false;
	}
  hex.enableWalking();
  if (previous_magnitude <= 1e-6) {
    bodyAngularVelocity = -magnitude * mDanceParameters.walkingSpeedRotation;
  }
}

void behavior_circle_left(int id, double magnitude) {
	if (newBehavior) {
		newBehavior = false;
	}
  hex.enableWalking();
  {
    bodyAngularVelocity = magnitude * mDanceParameters.walkingSpeedRotation;
  }
}

void behavior_fly(int id, double magnitude) {
	hex.disableWalking();
	if (newBehavior) {
		newBehavior = false;
	}
	debug_line = DEFAULT_DEBUG_START_LINE;

	double timeForThisBehavior = behaviorTime;
	double extension = 0;
	double bodyHeightOffset = 0;

	if ( jsonFileName.compare("dmitriFly.json") == 0 ) {
		timeForThisBehavior = 0;
		extension = 40;
		bodyHeightOffset = -30;
	}

	Quaternion hexapodOrientation;
	hexapodOrientation.makeFromAngleAndAxis(hex.getWalkingDynamics()->getBodyHeading(), Vector::zAxis);

#ifdef FLYING_ANOUK
	if (hexapodType != HEXAPOD_TYPE_ANOUK) {
#endif
		bodyOffset(0) = 0;
		bodyOffset(1) = 0;
		bodyOffset(2) = (50 + mDanceParameters.flyingBodyLiftMagnitude*sin(2*MOGI_PI*timeForThisBehavior + MOGI_PI/4) + bodyHeightOffset ) * magnitude;
		bodyOffset = hexapodOrientation.makeRotationMatrix() * bodyOffset;
		#ifdef FLYING_ANOUK
	} else {

		bodyOffset(2) = -mDanceParameters.flyingCircleRadius * cos(2*MOGI_PI*timeForThisBehavior) * magnitude;
		bodyOffset = hexapodOrientation.makeRotationMatrix() * bodyOffset;

	}
	#endif

		Vector leg0(3), leg3(3);

		leg0 = mDanceParameters.flyingCircleCenter;
		leg0(0) += mDanceParameters.flyingCircleRadius * sin(2*MOGI_PI*timeForThisBehavior) - bodyHeightOffset*3;
		leg0(1) += 2.0*mDanceParameters.flyingCircleRadius * sin(2*MOGI_PI*timeForThisBehavior);
		leg0(2) += 2.0*mDanceParameters.flyingCircleRadius * cos(2*MOGI_PI*timeForThisBehavior) + extension - 30;
		leg3(0) = -leg0(0);
		leg3(1) = leg0(1);
		leg3(2) = leg0(2);

	leg0 = hexapodOrientation.makeRotationMatrix() * leg0;
	leg3 = hexapodOrientation.makeRotationMatrix() * leg3;




	hex.beginModifying();
	{
		#ifdef FLYING_ANOUK
		if (hexapodType == HEXAPOD_TYPE_ANOUK) {
			leg0 = mDanceParameters.flyingCircleCenter;
			leg0(0) += mDanceParameters.flyingCircleRadius * sin(2*MOGI_PI*timeForThisBehavior + .1);
			leg0(1) += 0.0;
			leg0(2) += mDanceParameters.flyingCircleRadius * cos(2*MOGI_PI*timeForThisBehavior);
			leg0 = hexapodOrientation.makeRotationMatrix() * leg0;

			Matrix zRotation;
			for (int i = 0; i < 6; i++) {
				zRotation.makeZRotation(i * MOGI_PI/3.0);
				hex.foot[i] = getRestingFootVector(i) + ( magnitude * (zRotation * leg0));
			}

		} else {
			#endif
			hex.foot[0] = getRestingFootVector(0) + ( magnitude * leg0);
			hex.foot[3] = getRestingFootVector(3) + ( magnitude * leg3);
			#ifdef FLYING_ANOUK
		}
		#endif
	}
	hex.endModifying();
	// debugging
	std::stringstream msg;
	msg << "flying foot 0 : (" << hex.foot[0](0) << "," << hex.foot[0](1) << "," << hex.foot[0](2) << ")";
	debug(msg.str());

		bodyAngleAdjust(0) = (1 + 5*sin(2*MOGI_PI*timeForThisBehavior- MOGI_PI/4)) * MOGI_PI/180.0 * magnitude;	// may parameterize this out later
	//bodyAngleAdjust = hexapodOrientation.makeRotationMatrix() * bodyAngleAdjust;

}

void behavior_hip_swing(int id, double magnitude) {
	hex.disableWalking();
	if (newBehavior) {
		newBehavior = false;
	}
	debug_line = DEFAULT_DEBUG_START_LINE;

	Quaternion hexapodOrientation;
	hexapodOrientation.makeFromAngleAndAxis(hex.getWalkingDynamics()->getBodyHeading(), Vector::zAxis);

	//

	bodyAngleAdjust(2) = 10.0 *	pow((	// magnitude of hip swing in degrees
															sin(1.0 * 0.5*(MOGI_PI*behaviorTime))/1.0 +
															sin(3.0 * 0.5*(MOGI_PI*behaviorTime))/3.0 +
															sin(5.0 * 0.5*(MOGI_PI*behaviorTime))/5.0 +
															sin(7.0 * 0.5*(MOGI_PI*behaviorTime))/7.0 +	// ~smooth square wave

															sin(1.0 * 0.5*(MOGI_PI*behaviorTime+MOGI_PI/2.0))/2.0 -
															sin(3.0 * 0.5*(MOGI_PI*behaviorTime+MOGI_PI/2.0))/3.0 +
															sin(5.0 * 0.5*(MOGI_PI*behaviorTime+MOGI_PI/2.0))/5.0 -
															sin(7.0 * 0.5*(MOGI_PI*behaviorTime+MOGI_PI/2.0))/7.0		// add hip oomph
															) * 0.6275, 3)	// normalize
 															* MOGI_PI/180.0 * magnitude;	// may parameterize this out later
	//bodyAngleAdjust = hexapodOrientation.makeRotationMatrix() * bodyAngleAdjust;
	bodyOffset(0) = sin(bodyAngleAdjust(2)) * -mDanceParameters.cameraOrigin(1);
	bodyOffset(1) = sin(bodyAngleAdjust(2)) * -mDanceParameters.cameraOrigin(0);

	bodyOffset = hexapodOrientation.makeRotationMatrix() * bodyOffset;

}

void behavior_body_wave_bump(int id, double magnitude) {
	hex.disableWalking();
	if (newBehavior) {
		newBehavior = false;
	}
	debug_line = DEFAULT_DEBUG_START_LINE;

	Quaternion hexapodOrientation;
	hexapodOrientation.makeFromAngleAndAxis(hex.getWalkingDynamics()->getBodyHeading(), Vector::zAxis);

	double timeComponent = fmod(behaviorTime/4*2*MOGI_PI + MOGI_PI, 2*MOGI_PI) - MOGI_PI;

	if(timeComponent > MOGI_PI/2.0) {
		timeComponent = MOGI_PI - timeComponent;
	} else if (timeComponent < -MOGI_PI/2.0) {
		timeComponent = -MOGI_PI - timeComponent;
	}

	timeComponent *= 2.0;	// bring range to +/- pi

	timeComponent *= 1.5; // exaggerate the stop

	double maxTimeAngle = 135 * MOGI_PI/180.0;
	if (timeComponent > maxTimeAngle) {
		timeComponent = maxTimeAngle;
	} else if (timeComponent < -maxTimeAngle) {
		timeComponent = -maxTimeAngle;
	}

	bodyOffset = mDanceParameters.waveBumpCircleOffset;
	bodyOffset(0) += mDanceParameters.waveBumpCircleMagnitude(0) * sin(timeComponent);
	bodyOffset(1) += -mDanceParameters.waveBumpCircleMagnitude(1) * cos(timeComponent);
	bodyOffset(2) += -mDanceParameters.waveBumpCircleMagnitude(2) * cos(timeComponent);
	bodyOffset *= magnitude;
	bodyAngleAdjust(0) = (mDanceParameters.waveBumpAngleOffset(0)-mDanceParameters.waveBumpAngleMagnitude(0)*cos(timeComponent)) * MOGI_PI/180.0;
	bodyAngleAdjust(2) = -mDanceParameters.waveBumpAngleMagnitude(2)*(sin(1.0 * 0.5*(MOGI_PI*behaviorTime - MOGI_PI/2.0 + 0.7))/1.0 +
														sin(3.0 * 0.5*(MOGI_PI*behaviorTime - MOGI_PI/2.0 + 0.7))/3.0 +
														sin(5.0 * 0.5*(MOGI_PI*behaviorTime - MOGI_PI/2.0 + 0.7))/5.0 +
														sin(7.0 * 0.5*(MOGI_PI*behaviorTime - MOGI_PI/2.0 + 0.7))/7.0) * MOGI_PI/180.0;

	bodyAngleAdjust *= magnitude;
	bodyOffset = hexapodOrientation.makeRotationMatrix() * bodyOffset;
	//bodyAngleAdjust = hexapodOrientation.makeRotationMatrix() * bodyAngleAdjust;

	
}

void behavior_kill(int id, double magnitude) {

	 hex.enableWalking();

		if (magnitude == 1.0) {

			keepThreadAlive = false;	// no more behavior updates

			delwin(mainwin);
			curs_set(oldcur);
			clear();
			endwin();
			//refresh();

			std::cout << "Curses complete." << std::endl;
			//hex.setWalking(true);
			std::cout << "Powering down..." << std::endl;
			hex.powerDown();
			std::cout << " - powered down." << std::endl;
			if( appInterface.isOpen()) {
				appInterface.stop();	// This does nto quit the thread.  Need to fix AppInterface
			}
			terminate = true;
			std::cout << "Waiting for hex..." << std::endl;
//			while (hex.walking()) {
//
//			}
			std::cout << " - hex is complete" << std::endl;
			std::cout << "waiting for dynamixel handler..." << std::endl;

			std::cout << " - handler is complete" << std::endl;
			//exit(EXIT_SUCCESS);

			terminate = true;	// why not?

		}

}

void hexapod_update() {
  hex.beginModifying();
  {
   // if (hex.walking())
    {
      headingControl.enableVelocityControl();
      locationControl.enableVelocityControl();
      locationControl.setVelocity( bodyVelocity );
      headingControl.setVelocity( bodyAngularVelocity );
//      hex.getWalkingDynamics()->setBodyHeading(headingControl.processLocationControl(timer_behavior.dTime()));
//      hex.getWalkingDynamics()->setBodyLocation(locationControl.processLocationControl(timer_behavior.dTime()));

		hex.getWalkingDynamics()->setBody(locationControl.processLocationControl(timer_behavior.dTime()),
										  headingControl.processLocationControl(timer_behavior.dTime()),
										  timer_behavior.dTime());
      // Set the head angles:
      hex.headMechanism.setAngles( headAngles );
      
      // Set the body angles opposite to the head angles
      Quaternion xRotation, yRotation, zRotation, totalRotation;
      
      xRotation.makeFromAngleAndAxis( -1.0 * bodyAngleAdjust(0), Vector::xAxis);
      yRotation.makeFromAngleAndAxis( -1.0 * bodyAngleAdjust(1), Vector::yAxis);
      zRotation.makeFromAngleAndAxis( -1.0 * bodyAngleAdjust(2), Vector::zAxis);
			totalRotation = zRotation * yRotation * xRotation;

			Quaternion hexapodOrientation;
			hexapodOrientation.makeFromAngleAndAxis(hex.getWalkingDynamics()->getBodyHeading(), Vector::zAxis);

			Quaternion offsetRotation = hexapodOrientation * totalRotation * hexapodOrientation.inverse();	// Bunting: This shouldn't be necessary, I have an operation backwards in Hexapod
      hex.setBodyOrientation(offsetRotation);
      
      // Set the body offset:
      hex.setBodyLocation( bodyOffset );
    }

		if (appInterface.isOpen()) {
			appInterface.setBody( hex.getBodyVector());
			appInterface.setBodyOrientation(hex.getBodyOrientation());
			Vector legAngles[6];
			for (int i = 0; i < hex.legs.size(); i++) {
				legAngles[i] = hex.legs[i]->getKinematics();
			}
			appInterface.setLegAngles( legAngles );
			appInterface.setHeadAngles( hex.headMechanism.getAngles() );
		}

  }
  hex.endModifying();
}

////////////////////////////////////////////////////
//                   Debugging                    //
////////////////////////////////////////////////////
void debug(const char *str) {
  debug(std::string(str));
}

void debug(std::string str) {
  if (curses) {
    mvaddstr(debug_line, 0, std::string(str + "              ").c_str());
    debug_line++;
    if (debug_line > DEFAULT_MAX_DEBUG_LINES + DEFAULT_DEBUG_START_LINE) {
      debug_line = DEFAULT_DEBUG_START_LINE;
    }
  } else {
    std::cout << str << std::endl;
  }
}

void debug(const char *str, int line) {
  debug(std::string(str), line);
}

void debug(std::string str, int line) {
  if (curses) {
    mvaddstr(line, 0, std::string(str + "              ").c_str());
  } else {
    std::cout << str << std::endl;
  }
}

////////////////////////////////////////////////////
//                    Signals                     //
////////////////////////////////////////////////////
// Sets up the game timer
void setTimer(void) {
  struct itimerval it;
  
  // Clear itimerval struct members
  timerclear(&it.it_interval);
  timerclear(&it.it_value);
  
  // Set timer
  it.it_interval.tv_usec = DEFAULT_UPDATE_PERIOD;
  it.it_value.tv_usec    = DEFAULT_UPDATE_PERIOD;
  setitimer(ITIMER_REAL, &it, NULL);
}

void stopTimer(void) {
	struct itimerval it;

	// Clear itimerval struct members
	timerclear(&it.it_interval);
	timerclear(&it.it_value);

	// Set timer
	it.it_interval.tv_usec = 0;
	it.it_value.tv_usec    = 0;
	setitimer(ITIMER_REAL, &it, NULL);
}


// Sets up signal handlers we need
void setSignals(void) {
  struct sigaction sa;
  
  // Fill in sigaction struct
  sa.sa_handler = handler;
  sa.sa_flags   = 0;
  sigemptyset(&sa.sa_mask);
  
  // Set signal handlers
  sigaction(SIGTERM, &sa, NULL);
  sigaction(SIGINT,  &sa, NULL);
  sigaction(SIGALRM, &sa, NULL);
  
  // Ignore SIGTSTP
  sa.sa_handler = SIG_IGN;
  sigaction(SIGTSTP, &sa, NULL);
}

void *stateUpdate(void*) {
	Time time, timeTest;
	time.initialize();
	timeTest.initialize();
	double desiredLoopTime = 1.0/60.0;
	double sleepTime = 0;
	double loopTime = 0;
	double sleepTimeOffset = 0;

	while (keepThreadAlive) {
		timeTest.update();
		loopTime = timeTest.dTime();
//		if (loopTime > desiredLoopTime * 1.05) {
//			std::cerr << "WARING! Excessive loop time:" << loopTime << ", d:" << desiredLoopTime << ", c:" << loopCounter << std::endl;
//			mTimeDebug.print();
//		}
//		mTimeDebug.reset();
		sleepTime = desiredLoopTime - loopTime + sleepTimeOffset;
		if (sleepTime > 0) {
			usleep( sleepTime * 1000000.0 );
		}
		timeTest.reset();
		sleepTimeOffset += 0.001 * (desiredLoopTime - 1.0/time.frequency());
		if ((fabs(sleepTimeOffset) >= 10000000000) || (sleepTimeOffset != sleepTimeOffset)) {
			sleepTimeOffset = 0;
		}


	pthread_mutex_lock(&commandMutex);
	// Bunting:
	timer_iteration.update();
	behaviorTime = timer_iteration.runningTime() + timer_iteration_offset;	// No, this doesn't work yet but prevents hiccups
	// Also setting the velocities to 0, and only let behaviors that control walking make them non-zero
	bodyVelocity(0) = 0;
	bodyVelocity(1) = 0;
	bodyVelocity(2) = 0;
	bodyAngularVelocity = 0;

	// update the magnitudes based on the current time
	timer_behavior.update();
	behavior_duration += timer_behavior.dTime();

//	if (behavior_duration < previous_blend_duration) {
	if (previous_magnitude > 0) {


		current_magnitude = 0.0;
		//previous_magnitude = 1.0 - (behavior_duration / (previous_blend_duration));
		previous_magnitude -= timer_behavior.dTime()/previous_blend_duration;
		if (previous_magnitude < 0) {
			previous_magnitude = 0;
		}

//	} else if (behavior_duration < (current_blend_duration+previous_blend_duration)) {
	} else if (current_magnitude < 1.0) {
		if (exitingABehavior && allFeetOnGround()) {
			exitingABehavior = false;	// wait until done walking
		}

		if( !exitingABehavior ) {

			//current_magnitude = (behavior_duration - previous_blend_duration) / (current_blend_duration);
			current_magnitude += timer_behavior.dTime() / current_blend_duration;
			if (current_magnitude < 0.0) {
				current_magnitude = 0.0;
			}
			if (current_magnitude > 1.0) {
				current_magnitude = 1.0;
			}
			previous_magnitude = 0.0;
		} else {
			current_magnitude = 0;
			previous_magnitude = 0;
			behavior_duration = previous_blend_duration;
		}


	} else {
		// set magnitudes to 1.0 and 0.0
		current_magnitude = 1.0;
		previous_magnitude = 0.0;
	}

	// run behaviors if they are not at magnitude 0
	if (current_magnitude > 0) {	// Bunting: changed this since we explicitely set the value to 0
		callBehavior(current_behavior, current_magnitude);	// Bunting Order is important, must be called before previous behavior
	}
	if (previous_magnitude > 0 || exitingABehavior) {
		callBehavior(previous_behavior, previous_magnitude);
	}

	//behavior_stand(BEHAVIOR_ID_STAND, 1-current_magnitude-previous_magnitude);

	lighting_counter++;
	if (lighting_counter >= DEFAULT_LIGHTING_UPDATE) {
		callLighting(current_lighting, current_lighting_period);
		lighting_counter = 0;
	}

	// update the screen
		static int arbitraryCounter = 0;
//		static std::vector<Motor*>::iterator dynamixel = hex.dynamixelHandler->getDynamixels().begin();
		static int motorID = 1;
		arbitraryCounter++;
		if (arbitraryCounter > 10) {
			motorID++;
			if (motorID >= 22) {
				motorID = 1;
			}

			hex.dynamixelHandler->pushInstruction(
												  new Instruction(Instruction::READ,
												DataRange(REG_Present_Voltage, 1),
												hex.dynamixelHandler->getDynamixels().at(motorID)));	// This kills the instruction
			arbitraryCounter = 0;

		}

		static bool shuttingDown = false;
		if (voltageShutdownThreshold > voltageObserver.getVoltage() &&
			shuttingDown == false) {
			system("shutdown -h");
			shuttingDown = true;
		}

		std::stringstream voltageString;
		voltageString << "Voltage: " << voltageObserver.getVoltage() << "           ";
		mvaddstr(DEFAULT_HEADER_LINE, 0, voltageString.str().c_str());

	std::stringstream prev;
	prev << "Previous Behavior: " << previous_behavior_name << "           ";
	mvaddstr(DEFAULT_PREVIOUS_BEHAVIOR_LINE, 0, prev.str().c_str());
	std::stringstream prev_mag;
	prev_mag << "Magnitude: " << previous_magnitude << "         ";
	mvaddstr(DEFAULT_PREVIOUS_BEHAVIOR_LINE, 50, prev_mag.str().c_str());
	std::stringstream curr;
	curr << "Current Behavior:  " << current_behavior_name << "          ";
	mvaddstr(DEFAULT_CURRENT_BEHAVIOR_LINE, 0, curr.str().c_str());
	std::stringstream curr_mag;
	curr_mag << "Magnitude: " << current_magnitude << "         ";
	mvaddstr(DEFAULT_CURRENT_BEHAVIOR_LINE, 50, curr_mag.str().c_str());
	refresh();
	if (!manual) {
		std::stringstream rem;
		rem << "Remaining Time: " << remaining_time << "              ";
		mvaddstr(DEFAULT_REMAINING_TIME_LINE, 0, rem.str().c_str());
	} else {
		std::stringstream rem;
		rem << "Current Behavior Duration: " << behavior_duration << "              ";
		mvaddstr(DEFAULT_REMAINING_TIME_LINE, 0, rem.str().c_str());
	}

	if (script_finished) {
		std::stringstream fin;
		fin << "Script Mode (finished, Ctrl+C to quit)";
		mvaddstr(DEFAULT_HEADER_LINE, 0, fin.str().c_str());
	}

	hexapod_update();

	pthread_mutex_unlock(&commandMutex);
}
	return NULL;
}

void handler(int signum) {
	pthread_mutex_lock(&commandMutex);
	std::cout << "Received: " << strsignal(signum) << std::endl;
  // Switch on signal number
  switch (signum) {
//    case SIGALRM:
    case SIGTERM:
    case SIGINT:
			updateStateWithCommand(27);	// Send ESC, the behavior_kill() will handle cleaning the window
	  {
		  static int timesCaught = 1;
		  if (timesCaught++ > 2) {
			  exit(EXIT_SUCCESS);
		  }
	  }

  }
	pthread_mutex_unlock(&commandMutex);
}




