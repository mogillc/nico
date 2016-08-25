/*
 * ApplicationCommunication.cpp
 *
 *  Created on: Apr 29, 2016
 *      Author: adrian
 */

#include <mogi/remoteapp/ApplicationCommunicator.h>
#include <cmath>


using namespace Mogi;
using namespace Math;
using namespace App;
using namespace RemoteApp;

class JsonOptionPruner: public JsonValueObserver {
private:
//	static int objectCount;
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

//int JsonOptionPruner::objectCount = 0;

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

ApplicationCommunicator::ApplicationCommunicator(ApplicationServer& manager): Network::ConnectionHandler(manager){
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


ApplicationCommunicator::~ApplicationCommunicator() {
	releaseMemory();
}

void ApplicationCommunicator::releaseMemory() {
	for (std::vector<JsonValueObserver*>::iterator it = valueObservers.begin();
			it != valueObservers.end(); it++) {
		delete *it;
	}
}

void ApplicationCommunicator::setBody(const Vector& bodyVector) {
	lock();
	if (bodyVector.size() == 3) {
		body = bodyVector;
	} else {
		std::cout
				<< "Error! bodyVector should be of size 6 in HexapodBLE::setBody "
						"Vector ) call!\n";
	}
	unlock();
}

void ApplicationCommunicator::setBodyOrientation(const Quaternion& value) {
	lock();
	bodyOrientation = value;
	unlock();
}

void ApplicationCommunicator::setBodyOrientationOffset(const Quaternion& value) {
	lock();
	bodyOrientationOffset = value;
	unlock();
}

void ApplicationCommunicator::setBodyOffset(const Vector& bodyOffsetVector) {
	lock();
	if (bodyOffsetVector.size() == 3) {
		bodyOffset = bodyOffsetVector;
	} else {
		std::cout << "Error! bodyOffsetVector should be of size 6 in "
				"HexapodBLE::setBodyOffset Vector ) call!\n";
	}
	unlock();
}

void ApplicationCommunicator::setHeadAngles(const Vector& angles) {
	lock();
	if (angles.size() == 3) {
		headAngles = angles;
	} else {
		std::cout << "Error! headAngles should be of size 3 in "
				"AppInterface::setHeadAngles( Vector ) call!\n";
	}
	unlock();
}

void ApplicationCommunicator::setLegAngles(Vector* legAngles) {
	lock();
	for (unsigned int i = 0; i < 6; i++) {
		coxaIn(i) = legAngles[i](0); //((uint16_t) round( coxa[i] * 4096.0/(360.0))) % 4096;
		femurIn(i) = legAngles[i](1); //((uint16_t) round( femur[i] * 4096.0/(360.0))) % 4096;
		tibiaIn(i) = legAngles[i](2); //((uint16_t) round( tibia[i] * 4096.0/(360.0))) % 4096;
	}
	unlock();
}
const double& ApplicationCommunicator::xLeft() {
	return leftJoystick(0);
}
const double& ApplicationCommunicator::yLeft() {
	return leftJoystick(1);
}
const double& ApplicationCommunicator::xRight() {
	return rightJoystick(0);
}
const double& ApplicationCommunicator::yRight() {
	return rightJoystick(1);
}
const Vector& ApplicationCommunicator::acceleration() {
	return accelerationValue;
}
const double& ApplicationCommunicator::slider1() {
	return slider1Value;
}
const double& ApplicationCommunicator::slider2() {
	return slider2Value;
}

void ApplicationCommunicator::addJsonValueListener(const std::string& key,
		JsonValueObserver* observer) {
	lock();
	jsonSubject.addValueObserver(key, observer);
	unlock();
}

void ApplicationCommunicator::addOptionMenuOption(AppOption* option) {
	lock();
	if (optionsFromUser[option->title] != NULL) {
		unlock();
		return;
	}

	optionsFromUser[option->title] = option;
	unlock();

	sendOption(option);

}

void ApplicationCommunicator::sendOption(AppOption* option) {
	lock();
	if (optionsToPush[option->title] != NULL) {
		unlock();
		return;  // already scheduled with an observer
	}

	optionsToPush[option->title] = option;
	jsonSubject.addValueObserver("optionAdded",
			new JsonOptionPruner(&optionsToPush, option->title, &jsonSubject));
	unlock();
}

void ApplicationCommunicator::didConnect() {
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

std::string ApplicationCommunicator::generateTransmitBuffer() {
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
	for (std::map<std::string, AppOption*>::iterator it = optionsToPush.begin(); it != optionsToPush.end(); it++) {
		std::string command = it->second->typeString();

		root["specialCommands"][commandIndex][command]["title"] = it->second->title;

		switch (it->second->optionType) {
		case APP_SEGMENTED_CONTROL: {
			int segmentIndex = 0;
			for (std::vector<std::string>::iterator it2 =
					it->second->segments.begin();
					it2 != it->second->segments.end(); it2++) {
				root["specialCommands"][commandIndex][command]["segments"][segmentIndex++] = *it2;
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
//	std::cout << "ApplicationCommunicator::generateTransmitBuffer() root: " << root.toStyledString() << std::endl; // ADRIAN: uncomment

	return root.toStyledString();
}

void ApplicationCommunicator::process(Network::TCPSocket& fd, const unsigned char* data, unsigned int size,
		const Network::SocketAddress& sourceAddress) {

	if (currentClient.isValid() && !currentClient.equalIP(sourceAddress) &&
			!currentClient.equalPort(sourceAddress)) {
		currentClient = sourceAddress;
		std::cout << "calling didConnect" << std::endl;
		didConnect();
	}

	int consumed = 0;
	int index = 0;
	//std::cout << "\t recvdSize = " << size << std::endl;

	int extraData = size;
	while(extraData > 0) {
		// Give raw received data to Packet class for later unpacking
		index += consumed;
		consumed = rxPacket.receiveData(data + index, extraData);
		extraData = extraData - consumed > 0 ? extraData - consumed : 0;

		//std::cout << "\tindex " << index << " consumed " << consumed << " extraData " << extraData << std::endl;

		lock();
		if (didReceiveData(fd, rxPacket) != 0) {
			std::cerr << "Failed to receive packet" << std::endl;
		}
		unlock();

	}

}

int ApplicationCommunicator::didReceiveData(Network::TCPSocket& fd, HexapodPacket& packet) {
	int result = 0;
	HexapodPacket::RXState rxState = packet.getReceiveStatus();

	// Done receiving all data for current rx packet
	if (rxState == HexapodPacket::RX_SUCCESSFUL) {

		// Unpack rx packet, parse received json string, and send a reply
		if (packet.unpackBuffer()) {
			std::string rxJsonString = packet.jsonString; // Get received payload

			// DEBUG
			std::cout << rxJsonString << std::endl;

			// Parse received json structure
			bool parsingSuccessful = jsonSubject.parseJson(rxJsonString.begin(), rxJsonString.end());

			if (parsingSuccessful) {
				// Send reply
				std::string reply = generateTransmitBuffer(); // generate reply as a json string


				txPacket.jsonString = reply; // Set tx packet's payload
				txPacket.packBuffer(); // Pack payload into raw bytes
				fd.send(txPacket.getTransmitBuffer(), txPacket.getTransmitBufferSize()); // Send packet to client
			}
			else {
				std::cerr << "Could not parse json string: " << rxJsonString << std::endl;
				result = -1;
			}
		}
		else {
			std::cerr << "Error unpacking rx packet" << std::endl;
			result = -1;
		}

	}
	// Don't yet have enough data to fill current rx packet
	else if (rxState == HexapodPacket::UNKNOWN_LENGTH || rxState == HexapodPacket::INSUFFICIENT_DATA) {
		// Do nothing ... Wait until we get more data
		std::cout << "Received partial packet ... " << std::endl;
	}
	// Checksum error when receivng packet
	else if (rxState == HexapodPacket::RX_ERROR) {
		// PANIC!!!
		std::cerr << "Received packet failed checksum!" << std::endl;
		result = -1;
	}

	return result;

}

void ApplicationCommunicator::exitAction() {
}


