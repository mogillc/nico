/*
 *  simple_walk.cpp
 *  
 *
 *  Created by Matt Bunting on 10/18/12.
 *  Copyright 2012 12 Cent Dwarf. All rights reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream> // for std::cout (added by Adrian)

#include <mogi/app/appcomm.h>

using namespace Mogi::App;

// For Segmented Controllers, Buttons, and Text Fields:
class JsonStringObserver : public JsonValueObserver {
private:
	void update( JsonValueInterface& newValue ) {
		if (newValue.isString()) {
			std::string value = newValue.asString();
			std::cout << "Cool! Got the string: " << value << std::endl;
		} else {
			std::cout << "coding error" << std::endl;
		}
	}
public:
};

// For Switches:
class JsonBoolObserver : public JsonValueObserver {
private:
	void update( JsonValueInterface& newValue ) {
		if (newValue.isBool()) {
			std::cout << "Cool! Got the Bool: " << newValue.asBool() << std::endl;
		} else {
			std::cout << "coding error" << std::endl;
		}
	}
};

// For sliders:
class JsonDoubleObserver : public JsonValueObserver {
private:
	void update( JsonValueInterface& newValue ) {
		if (newValue.isDouble()) {
			std::cout << "Cool! Got the Double: " << newValue.asDouble() << std::endl;
		} else {
			std::cout << "coding error" << std::endl;
		}
	}
};

// For... nothing in this version...
class JsonIntObserver : public JsonValueObserver {
private:
	void update( JsonValueInterface& newValue ) {
		if (newValue.isInt()) {
			std::cout << "Cool! Got the Int: " << newValue.asInt() << std::endl;
		} else {
			std::cout << "coding error" << std::endl;
		}
	}
};

int main(int argc, char* argv[])
{
	// Start the app interface with a certain port:
	AppInterface mAppInterface;
	mAppInterface.setPort(10098);
	mAppInterface.start();

	/*// Customize the options menu:
	// 1) Add a segmented control:
	AppOption ledOptions;
	std::string optionTitle = "LED";
	ledOptions.optionType = APP_SEGMENTED_CONTROL;
	ledOptions.segments.push_back("Off");
	ledOptions.segments.push_back("A");
	ledOptions.segments.push_back("Option3");
	ledOptions.title = optionTitle;
	mAppInterface.addOptionMenuOption(&ledOptions);
	// 1.1) Create an observer to receive a change in value:
	JsonStringObserver LEDStringObserver;
	mAppInterface.addJsonValueListener(optionTitle, &LEDStringObserver);

	// 2) Add a slider:
	AppOption sliderOption;
	optionTitle = "Slider";
	sliderOption.optionType = APP_SLIDER;
	sliderOption.title = optionTitle;
	mAppInterface.addOptionMenuOption(&sliderOption);
	// 2.1) Create an observer to receive a change in value:
	JsonDoubleObserver SlideObserver;
	mAppInterface.addJsonValueListener(optionTitle, &SlideObserver); 

	// 3) Add an on/off switch:
	AppOption switchOption;
	optionTitle = "Switch";
	switchOption.optionType = APP_SWITCH;
	switchOption.title = optionTitle;
	mAppInterface.addOptionMenuOption(&switchOption);
	// 3.1) Create an observer to receive a change in value:
	JsonBoolObserver SwitchObserver;
	mAppInterface.addJsonValueListener(optionTitle, &SwitchObserver);

	// 4) Add a text input:
	AppOption textOption;
	optionTitle = "Text";
	textOption.optionType = APP_TEXT_FIELD;
	textOption.title = optionTitle;
	mAppInterface.addOptionMenuOption(&textOption);
	// 4.1) Create an observer to receive a change in value:
	JsonStringObserver StringObserver;
	mAppInterface.addJsonValueListener(optionTitle, &StringObserver);

	// 5) Add a button:
	AppOption buttonOption;
	optionTitle = "Button";
	buttonOption.optionType = APP_BUTTON;
	buttonOption.title = optionTitle;
	buttonOption.buttonName = "My Button";
	mAppInterface.addOptionMenuOption(&buttonOption);
	// 5.1) Create an observer to receive a change in value:
	JsonStringObserver buttonObserver;
	mAppInterface.addJsonValueListener(optionTitle, &buttonObserver);*/

	std::cout << "Ctrl-c to quit." << std::endl;
	double time = 0;
	double loopTimeInSeconds = 1.0/60.0;	// 60Hz

	while (1) {
		usleep(1000000.0 * loopTimeInSeconds);
		time += loopTimeInSeconds;

		// Make a bunch of crazy angles to be applied to the legs, just as an example:
		Mogi::Math::Vector legAngles[6];
		Mogi::Math::Vector bodyOffset(3), body(3);
		Mogi::Math::Quaternion bodyOrientation, bodyOrientationOffset;
		for (int i = 0; i < 6; i++) {
			legAngles[i].setLength(3);
			legAngles[i](0) = time + (double)i * 2.0*MOGI_PI/6.0;
			legAngles[i](1) = time + (double)i * 2.0*MOGI_PI/6.0 + 2.0*MOGI_PI/18.0;
			legAngles[i](2) = time + (double)i * 2.0*MOGI_PI/6.0 + 2.0*MOGI_PI/18.0;
		}

		bodyOrientation.makeFromAngleAndAxis(time, Mogi::Math::Vector::zAxis);
		bodyOrientationOffset.makeFromAngleAndAxis(time*MOGI_PI*2.0, Mogi::Math::Vector::yAxis);

		// Send the angles to the app:
		mAppInterface.setBodyOrientation(bodyOrientation);
		mAppInterface.setBodyOrientationOffset(bodyOrientationOffset);
		mAppInterface.setLegAngles( legAngles );
		mAppInterface.setBody(body);
		mAppInterface.setBodyOffset(bodyOffset);
	}

	return(0);
}
