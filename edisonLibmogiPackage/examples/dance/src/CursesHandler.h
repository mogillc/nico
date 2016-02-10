//
//  danceCurses.h
//  Project
//
//  Created by Matt Bunting on 12/23/14.
//
//

#ifndef __Project__danceCurses__
#define __Project__danceCurses__

//#include <stdio.h>

#include <ncurses.h>
#include <termios.h>

#include "danceState.h"

class CursesHandler {
private:
	// Singleton:
	static CursesHandler* mCursesHandler;
	CursesHandler();
	~CursesHandler(){};

	bool daemonMode;
	struct termios orig_termios;
	static int instanceCount;

	void cleanupConsole();

public:
	// Singleton:
	static CursesHandler* getInstance();

	static void destroy();

	void updateScreen( const DanceState* danceState );

	char getUserInput();

	void setupTerminal( bool daemonMode );

};
#endif /* defined(__Project__danceCurses__) */
