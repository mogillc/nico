//
//  danceCurses.cpp
//  Project
//
//  Created by Matt Bunting on 12/23/14.
//
//

#include "CursesHandler.h"

#include <mogi/robot/hexapod.h>
#include <mogi.h>

#include <sys/select.h>
#include <cstdlib>
#include <unistd.h>
#include <cstdio>
#include <string.h>

using namespace Mogi;

//static int mainRunning = FALSE;
CursesHandler* CursesHandler::mCursesHandler = NULL;

CursesHandler::CursesHandler()
:daemonMode(false)
{
}

CursesHandler* CursesHandler::getInstance() {
	if (mCursesHandler == NULL) {
		mCursesHandler = new CursesHandler;
	}
	return mCursesHandler;
}

void CursesHandler::destroy() {
	if(mCursesHandler != NULL) {
		mCursesHandler->cleanupConsole();
		delete mCursesHandler;
		mCursesHandler = NULL;
	}
}


void CursesHandler::updateScreen( const DanceState* danceState )
{
	if (daemonMode == true) {
		return;
	}
	int row = 0;
	int col = 0;

	if (danceState->verbose) {

		clear();

		mvprintw(row,31,"Verbose mode(press v for quiet)");

		mvprintw(row++,0,"+---------------------------+");
		mvprintw(row++,0,"|        Nico Menu          |");
		mvprintw(row++,0,"|    MB version:  %s     |", getLibraryVersion().version_str);
		mvprintw(row++,0,"|                           |");
		mvprintw(row++,0,"| 1. Walking mode           |");
		mvprintw(row++,0,"| 2. IK demo mode           |");
		mvprintw(row++,0,"| 3. Fist Pump mode         |");
		mvprintw(row++,0,"| 4. Head Bob mode          |");
		mvprintw(row++,0,"| 5. Meander mode           |");
		mvprintw(row++,0,"|                           |");
		mvprintw(row++,0,"| o. Off,  c. Christmas     |");
		mvprintw(row++,0,"| b. Blue,  g. Green r. Red |");
		mvprintw(row++,0,"| m. Multicolor             |");
		mvprintw(row++,0,"| wasd. Translate (Walking) |");
		mvprintw(row++,0,"| jl. Rotate (Walking)      |");
		mvprintw(row++,0,"|                           |");
		mvprintw(row++,0,"| ESC to Exit               |");
		mvprintw(row++,0,"+---------------------------+");
		int rowSave = row+1;

		row = 2;
		col = 32;

		switch (danceState->controlMode) {

			case SOCKET:
				mvprintw(row++,col,"Control Mode: iPhone");
				break;

			case KEYBOARD:
				mvprintw(row++,col,"Control Mode: Keyboard");
				break;

			default:
    break;
		}

		mvprintw(row,col,"LED:");
		switch (danceState->getLightState()) {
			case BLUE:
				mvprintw(row++,col + 6,"Blue");
				break;
			case OFF:
				mvprintw(row++,col + 6,"Off");
				break;
			case RED:
				mvprintw(row++,col + 6,"Red");
				break;
			case GREEN:
				mvprintw(row++,col + 6,"Green");
				break;
			case XMAS:
				mvprintw(row++,col + 6,"Xmas");
				break;
			case ALL:
				mvprintw(row++,col + 6,"Multi");
				break;

			default:
				mvprintw(row++,col + 6,"WTF...");
				break;
		}

		switch(danceState->state)
		{
			case walking:
				mvprintw(row++,col,"Mode: Walking");
				break;

			case quaternion:
				mvprintw(row++,col,"Mode: IK demo");
				break;

			case fistpump:
				mvprintw(row++,col,"Mode: Fist Pump");
				break;

			case headbob:
				mvprintw(row++,col,"Mode: Head Bob");
				break;

			case SOFT_WAKE:
				mvprintw(row++,col,"Mode: Waking up");
				break;

			case SOFT_SLEEP:
				mvprintw(row++,col,"Mode: Sleeping");
				break;

			case MEANDER:
				mvprintw(row++,col,"Mode: Meander");
				switch (danceState->meanderState) {
					case SETTING_LOCATION:
						mvprintw(row++, col, "- State: Setting Location");
						break;

					case MOVING:
						mvprintw(row++, col, "- State: Moving to:");
						break;

					case WAITING:
						mvprintw(row++, col, "- State: Waiting");
						break;

					default:
						break;
				}
				//mvprintw(row++, col, " - at x:%.2f\ty:%.2f", danceState->currentLocation.value(0, 0), danceState->currentLocation.value(1, 0));
				mvprintw(row++, col, " - to x:%.2f\ty:%.2f", danceState->desiredLocation.valueAsConst(0, 0), danceState->desiredLocation.valueAsConst(1, 0));

				break;

				//		case 27:
				//			mvprintw(row++,col,"Quitting...");
				//			break;
			default:
				break;
		}





		mvprintw(row++,col,"Left Joystick x:%01.2f, y:%01.2f", danceState->xLeft, danceState->yLeft);
		mvprintw(row++,col,"Right Joystick x:%01.2f", danceState->xRight);


		mvprintw(rowSave++,0,"Console:");
		move(rowSave++,0);
		//move(24,0);
		refresh();
	} else {
		mvprintw(0,0,"Quiet mode(press v for verbose):");
		move(1,0);
	}

}


char CursesHandler::getUserInput( )
{

	if (daemonMode == false) {
		return getch();

	} else {
		struct timeval tv = { 0L, 0L };

		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(0, &fds);

		if(select(1, &fds, NULL, NULL, &tv))
		{

			return getchar();

		}
	}
	return -1;
}


void CursesHandler::setupTerminal( bool useDameon )
{
	daemonMode = useDameon;
	if (daemonMode == false) {
		// Start up Curses window
		initscr();
		cbreak();
		noecho();
		nodelay(stdscr, 1);	// Don't wait at the getch() function if the user hasn't hit a key
		keypad(stdscr, 1); // Allow Function key input and arrow key input
	} else {

		struct termios new_termios;

		tcgetattr(0, &orig_termios);
		memcpy(&new_termios, &orig_termios, sizeof(new_termios));


		cfmakeraw(&new_termios);
		tcsetattr(0, TCSANOW, &new_termios);

	}
	atexit(destroy);
}

void CursesHandler::cleanupConsole( )
{
	if (daemonMode == false) {
		clear();
		endwin();
	} else {
		tcsetattr(0, TCSANOW, &orig_termios);
	}
	std::cout << "Console has been cleaned!" << std::endl;
}
