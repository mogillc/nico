/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                            Author: Matt Bunting                            *
 *                                                                            *
 *   Proprietary and confidential.                                            *
 *                                                                            *
 *   Unauthorized copying of this file via any medium is strictly prohibited  *
 *   without the explicit permission of Mogi, LLC.                            *
 *                                                                            *
 *   See license in root directory for terms.                                 *
 *   http://www.binpress.com/license/view/l/0088eb4b29b2fcff36e42134b0949f93  *
 *                                                                            *
 *****************************************************************************/

#ifdef IDENT_C
static const char* const Communicator_C_Id = "$Id$";
#endif

#include <iostream>

#include <math.h>
#include "appcomm.h"

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Math;
using namespace App;

class JsonOptionPruner: public JsonValueObserver {
private:
	static int objectCount;
	std::map<std::string, AppOption*>* optionSet;
	std::string optionTitle;
	JsonSubject* subject;
	void update( JsonValueInterface& newValue) {
		if (newValue.isString()) {
			if (newValue.asString() == optionTitle) {
				//					std::cerr << "Option success for " <<
				//optionTitle << std::endl;
				optionSet->erase(optionTitle); // erase the vector of to-be-added options
											   //					std::cout << "Objects to be added is
											   //now: " << optionSet->size() << std::endl;
				subject->eraseValueObserver("optionAdded", this); // remove self from subject
				delete this;  // Yikes!
			}
		}
	}

public:
	JsonOptionPruner(std::map<std::string, AppOption*>* optionSet,
			std::string optionTitle, JsonSubject* subject) :
			optionSet(optionSet), optionTitle(optionTitle), subject(subject) {
		//			std::cout << " - Addind observer for Pruner: " << optionTitle << "
		//count: " << ++objectCount << std::endl;
	}
	~JsonOptionPruner() {
		//			std::cout << " - Killing observer for Pruner: " << optionTitle << "
		//new count: " << --objectCount << std::endl;
	}
};

int JsonOptionPruner::objectCount = 0;

class JsonDoubleObserver: public JsonValueObserver {
private:
	double* pointerToValue;
	void update( JsonValueInterface& newValue) {
		if (newValue.isDouble()) {
			*pointerToValue = newValue.asDouble();
		}
	}

public:
	JsonDoubleObserver(double* valueToChange) {
		pointerToValue = valueToChange;
	}
};

Communicator::Communicator() {
	currentlyOpen = false;
	// buffer = new unsigned char[256];
	coxaIn.setLength(6);
	femurIn.setLength(6);
	tibiaIn.setLength(6);
	body.setLength(3);
	bodyOffset.setLength(3);
	headAngles.setLength(6);
	leftJoystick.setLength(2);
	rightJoystick.setLength(2);

	accelerationValue.setLength(3);

	JsonDoubleObserver* observer = new JsonDoubleObserver(&leftJoystick(0));
	jsonSubject.addValueObserver("xLeft", observer);
	valueObservers.push_back(observer);

	observer = new JsonDoubleObserver(&rightJoystick(0));
	jsonSubject.addValueObserver("xRight", observer);
	valueObservers.push_back(observer);

	observer = new JsonDoubleObserver(&leftJoystick(1));
	jsonSubject.addValueObserver("yLeft", observer);
	valueObservers.push_back(observer);

	observer = new JsonDoubleObserver(&rightJoystick(1));
	jsonSubject.addValueObserver("yRight", observer);
	valueObservers.push_back(observer);

	observer = new JsonDoubleObserver(&slider1Value);
	jsonSubject.addValueObserver("slider1Value", observer);
	valueObservers.push_back(observer);

	observer = new JsonDoubleObserver(&slider2Value);
	jsonSubject.addValueObserver("slider2Value", observer);
	valueObservers.push_back(observer);

	JsonArrayObserver* accObserver = new JsonArrayObserver;
	jsonSubject.addValueObserver("acceleration", accObserver);
	valueObservers.push_back(accObserver);

	observer = new JsonDoubleObserver(&accelerationValue(0));
	accObserver->addElementObserver(0, observer);
	valueObservers.push_back(observer);

	observer = new JsonDoubleObserver(&accelerationValue(1));
	accObserver->addElementObserver(1, observer);
	valueObservers.push_back(observer);

	observer = new JsonDoubleObserver(&accelerationValue(2));
	accObserver->addElementObserver(2, observer);
	valueObservers.push_back(observer);
}

Communicator::~Communicator() {
	for (std::vector<JsonValueObserver*>::iterator it = valueObservers.begin();
			it != valueObservers.end(); it++) {
		delete *it;
	}
}

void Communicator::setBody(const Vector& bodyVector) {
	if (bodyVector.size() == 3) {
		body = bodyVector;
	} else {
		std::cout
				<< "Error! bodyVector should be of size 6 in HexapodBLE::setBody "
						"Vector ) call!\n";
	}
}

void Communicator::setBodyOrientation(const Quaternion& value) {
	bodyOrientation = value;
}

void Communicator::setBodyOrientationOffset(const Quaternion& value) {
	bodyOrientationOffset = value;
}

void Communicator::setBodyOffset(const Vector& bodyOffsetVector) {
	if (bodyOffsetVector.size() == 3) {
		bodyOffset = bodyOffsetVector;
	} else {
		std::cout << "Error! bodyOffsetVector should be of size 6 in "
				"HexapodBLE::setBodyOffset Vector ) call!\n";
	}
}

void Communicator::setHeadAngles(const Vector& angles) {
	if (angles.size() == 3) {
		headAngles = angles;
	} else {
		std::cout << "Error! headAngles should be of size 3 in "
				"AppInterface::setHeadAngles( Vector ) call!\n";
	}
}

void Communicator::setLegAngles(Vector* legAngles) {
	for (unsigned int i = 0; i < 6; i++) {
		coxaIn(i) = legAngles[i](0); //((uint16_t) round( coxa[i] * 4096.0/(360.0))) % 4096;
		femurIn(i) = legAngles[i](1); //((uint16_t) round( femur[i] * 4096.0/(360.0))) % 4096;
		tibiaIn(i) = legAngles[i](2); //((uint16_t) round( tibia[i] * 4096.0/(360.0))) % 4096;
	}
}
const double& Communicator::xLeft() {
	return leftJoystick(0);
}
const double& Communicator::yLeft() {
	return leftJoystick(1);
}
const double& Communicator::xRight() {
	return rightJoystick(0);
}
const double& Communicator::yRight() {
	return rightJoystick(1);
}
const Vector& Communicator::acceleration() {
	return accelerationValue;
}
const double& Communicator::slider1() {
	return slider1Value;
}
const double& Communicator::slider2() {
	return slider2Value;
}

void Communicator::addJsonValueListener(const std::string& key,
		JsonValueObserver* observer) {
	jsonSubject.addValueObserver(key, observer);
}

void Communicator::addOptionMenuOption(AppOption* option) {
	if (optionsFromUser[option->title] != NULL) {
		return;
	}
	optionsFromUser[option->title] = option;

	sendOption(option);
}

void Communicator::sendOption(AppOption* option) {
	if (optionsToPush[option->title] != NULL) {
		return;  // already scheduled with an observer
	}
	optionsToPush[option->title] = option;
	jsonSubject.addValueObserver("optionAdded",
			new JsonOptionPruner(&optionsToPush, option->title, &jsonSubject));
}

void Communicator::didConnect() {
	for (std::map<std::string, AppOption*>::iterator it =
			optionsFromUser.begin(); it != optionsFromUser.end(); it++) {
		sendOption(it->second);
	}
}

std::string AppOption::typeString() {
	std::string result;
	switch (optionType) {
	case APP_BUTTON:
		result = "addButton";
		break;
	case APP_SEGMENTED_CONTROL:
		result = "addSegmentedControl";
		break;
	case APP_SWITCH:
		result = "addSwitch";
		break;
	case APP_SLIDER:
		result = "addSlider";
		break;
	case APP_TEXT_FIELD:
		result = "addTextField";
		break;
	}
	return result;
}

std::string Communicator::generateTransmitBuffer() {
	static int protocolCount = 0;
	protocolCount++;
	JsonValueInterface root;

	root["protocolCount"] = protocolCount;

	for (unsigned int i = 0; i < 6; i++) {
		root["coxa"][i] = coxaIn(i);
		root["femur"][i] = femurIn(i);
		root["tibia"][i] = tibiaIn(i);
	}
	for (unsigned int i = 0; i < 3; i++) {
		root["headAngles"][i] = headAngles(i);
		root["body"][i] = body(i);
		root["bodyOffset"][i] = bodyOffset(i);
	}
	for (unsigned int i = 0; i < 4; i++) {
		root["bodyOrientation"][i] = bodyOrientation(i);
		root["bodyOrientationOffset"][i] = bodyOrientationOffset(i);
	}

	int commandIndex = 0;
	for (std::map<std::string, AppOption*>::iterator it = optionsToPush.begin();
			it != optionsToPush.end(); it++) {
		std::string command = it->second->typeString();

		root["specialCommands"][commandIndex][command]["title"] =
				it->second->title;

		switch (it->second->optionType) {
		case APP_SEGMENTED_CONTROL: {
			int segmentIndex = 0;
			for (std::vector<std::string>::iterator it2 =
					it->second->segments.begin();
					it2 != it->second->segments.end(); it2++) {
				root["specialCommands"][commandIndex][command]["segments"][segmentIndex++] =
						*it2;
			}
		}
			break;

		case APP_BUTTON:
			root["specialCommands"][commandIndex][command]["buttonName"] =
					it->second->buttonName;
			break;

		default:
			break;
		}

		commandIndex++;
	}
//	std::cout << "result : " << root.toStyledString() << std::endl; // ADRIAN: uncomment

	return root.toStyledString();
}

//	void Communicator::printStats()
//	{
//		std::cout << "MBCommunicator stats:\n";
//		std::cout << " - xLeft:   " << leftJoystick(0) << "\tyLeft:   " <<
//leftJoystick(1) << std::endl;
//		std::cout << " - xRight:  " << rightJoystick(0) << "\tyRight:  "
//<< rightJoystick(1) << std::endl;
//		std::cout << " - accX:    " << accelerationValue(0) << "\taccY:
//" << accelerationValue(1) << "\taccZ:    " << accelerationValue(2) <<
//std::endl;
//		std::cout << " - slider1: " << slider1Value << "\tslider2: " <<
//slider2Value << std::endl;
//	}

enum {
	FOUNDSTART = -2, FOUNDNOTHING = -1
};

int getTokenLength(char* begin, char* end);

int getJsonObjectLength(char* begin, char* end) {
	int result = 1;
	if (*begin++ != '{') {
		return -1;
	}

	int lengthToAdd;
	while (begin != end && *begin != '}') {
		lengthToAdd = getTokenLength(begin, end);
		if (lengthToAdd < 0) {
			return -1;
		}
		//			std::cout << "checking value: " << begin[result] <<
		//std::endl;
		result += lengthToAdd;
		begin += lengthToAdd;
	}

	if (begin == end) {
		return -1;
	}
	//		std::cout << "object length: " << result << std::endl;
	return result + 1;
}

int getJsonStringLength(char* begin, char* end) {
	int result = 1;
	if (*begin++ != '\"') {
		return -1;
	}

	int lengthToAdd;
	while (begin != end && *begin != '\"' && *(begin - 1) != '\\') {
		if (*begin == '{' || *begin == '}') {
			lengthToAdd = 1;
		} else {
			lengthToAdd = getTokenLength(begin, end);
			if (lengthToAdd < 0) {
				return -1;
			}
		}
		result += lengthToAdd;
		begin += lengthToAdd;
	}
	if (begin == end) {
		return -1;
	}
	//		std::cout << "string length: " << result << std::endl;
	return result + 1;
}

int getTokenLength(char* begin, char* end) {
	switch (*begin) {
	case '{':
		return getJsonObjectLength(begin, end);
		break;

	case '\"':
		return getJsonStringLength(begin, end);
		break;

	default:
		return 1;
		break;
	}
}

int getGoodJsonString(std::string& string, std::string::iterator* begin,
		std::string::iterator* end) {
	int index = 0;
	int ret = FOUNDNOTHING;
	for (std::string::iterator it = string.begin(); it != string.end(); it++) {
		if (*it == '{') {
			//				std::cout << "found the beginning at
			//index: " << index << std::endl;
			*begin = it;
			int length = getJsonObjectLength(&*it, &*string.end());
			// std::cout << "getJsonObjectLength() returned " << length << std::endl;
			*end = *begin + length;
			if (length < 0 || *end > string.end()) {
				return FOUNDSTART;
			}

			return 0;
		}
		index++;
	}
	return ret;
}

int Communicator::didReceiveData(std::string newData) {
	buffer += newData;
	//std::cout << "didReceiveData( \"" << newData << "\" )" << std::endl; // ADRIAN: uncomment 
	// std::cout << "buffer =\"" << buffer << "\" )" << std::endl;
	unsigned int bufferSize = -1;
	// int ret = handlePossibleBuffer();
	int ret = 0;
	while (buffer.size() != bufferSize && buffer.size() != 0 && ret == 0) {
		bufferSize = buffer.size();
		ret = handlePossibleBuffer();
	}

	sendState(generateTransmitBuffer());
	return ret;
}

int Communicator::handlePossibleBuffer() {
	static int handleCount = 0;
	handleCount++;
	std::string::iterator begin, end;

	int findJsonResult = getGoodJsonString(buffer, &begin, &end);

	//  Goddammit I really want to use regex here but recursion and lookbehind is
	//  not supported in STL
	//  I may need to bite the bullet and compile with boost, but that means I'll
	//  need to install boost on edison.
	//  Leaving this here because it was hard work and may implement this
	//  eventually.  It would be WAY better.
	//  (\{((?R)|[^\{\}\"]|("([^"\\]|\\.)*"))*\})
	//  ( \{?R|[^ \{ \} \ "]| "[^ " \ \]| \ \.* "*\ }
	//  ( \{((?R)|[^ \{ \} \ "]|( "([^ " \ \]| \ \.)* "))*\ })
	//		std::string regexExpression =
	//"(\\{((?R)|[^\\{\\}\\\"]|(\"([^\"\\\\]|\\\\.)*\"))*\\})";
	//		std::cout << "regexExpression = " << regexExpression <<
	//std::endl;
	//		std::regex jsonRegex(regexExpression);
	//		//std::regex jsonRegex("(slider[^\s])");
	//
	//		auto words_begin =
	//		std::sregex_iterator(buffer.begin(), buffer.end(), jsonRegex);
	//		auto words_end = std::sregex_iterator();
	//
	//		std::cout << "Found " << std::distance(words_begin, words_end) << "
	//words\n";
	//		for (std::sregex_iterator i = words_begin; i != words_end; ++i)
	//{
	//			std::smatch match = *i;
	//			std::string match_str = match.str();
	//			std::cout << "  " << match_str << '\n';
	//		}

	if (findJsonResult != 0) {
		switch (findJsonResult) {
		case FOUNDNOTHING:
			std::cerr << "Error searching for a JSON string, handle count: "
					<< handleCount << std::endl;
			std::cerr << " - found nothing: " << buffer << std::endl;
			buffer = "";
			break;

		case FOUNDSTART:  // hopefully we wont die..
			// std::cerr << " - found the start, but no finish: " << buffer <<
			// std::endl;
			break;

		default:
			std::cerr << " - unknown error: " << buffer << std::endl;
			break;
		}

		return -1;
	}

	bool parsingSuccessful = jsonSubject.parseJson(begin, end);
	//std::cout << "received json : " << std::string(begin, end) << std::endl;

	buffer.erase(buffer.begin(), end); // erase the buffer from the begining up to the end of the JSON.

	if (!parsingSuccessful) {
		return -1;
	}

	return 0;
}

//	bool Communicator::isOpen() {
//		return currentlyOpen;
//	}

#ifdef _cplusplus
}
#endif

