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

#ifndef MOGI_COMMUNICATION_H
#define MOGI_COMMUNICATION_H

#ifdef LIBJSONCPP_FOUND	// Currently heavily dependent on libjson

#include "mogi/app/json.h"
#include "mogi/math/mmath.h"
#include "mogi/thread.h"

namespace Mogi {

/*!
 * \namespace Mogi::App
 * \brief Tools for communicating with the downloadable app.
 * \image html app.png
 * https://itunes.apple.com/us/app/dmitri-hexapod-controller/id671078464?l=zh&mt=8
 */
namespace App {

/*!	\brief The set of supported UI option elements in the app.
 
 \image html appOptions.png
 */
typedef enum {
	APP_BUTTON, /*!< Designates a Button. */
	APP_SEGMENTED_CONTROL, /*!< Designates a Segmented Control. */
	APP_SWITCH, /*!< Designates a Switch */
	APP_TEXT_FIELD, /*!< Designates a Text Field */
	APP_SLIDER /*!< Designates a Slider */
} AppOptionType;

/*! \class AppOption
 \brief Used for creation of iOS app options.
 */
class AppOption {
public:

	/*! \brief The app option type.
	 */
	AppOptionType optionType;

	/*! \brief The app option label.
	 */
	std::string title;

	/*! \brief The segments, if optionType is APP_SEGMENTED_CONTROL.
	 */
	std::vector<std::string> segments;

	/*! \brief The app option button name, if optionType is APP_BUTTON.
	 */
	std::string buttonName;

	/*! \brief Converts the optionType to a readable string.
	 \return The app option type as a string.
	 */
	std::string typeString();
};

/*! \class Communicator
 \brief Handles JSON parsing thread for external high level communication.

 Support is implemented for simulations and primitive inputs, and also
 implements an observer pattern for value change listeners.
 */
class Communicator: protected Mogi::Thread {
public:
	Communicator();
	~Communicator();

	/*! \brief Sets the body vector, which will be sent on the next successful
	 update() call.
	 @param bodyVector A vector of size 3 corresponding to the body location.
	 */
	void setBody(const Math::Vector& bodyVector);

	/*! \brief Sets the body quaternion, which will be sent on the next successful
	 update() call.
	 @param value A quaternion describing the body orientation.
	 */
	void setBodyOrientation(const Math::Quaternion& value);

	/*! \brief Sets the head angles, which will be sent on the next successful
	 update() call.
	 @param angles A vector of size 3 corresponding to the head orientation.
	 */
	void setHeadAngles(const Math::Vector& angles);

	/*! \brief Sets the body vector offset, which will be sent on the next successful
	 update() call.
	 @param bodyOffsetVector A vector of size 6 corresponding to the body location
	 and orientation.
	 */
	void setBodyOffset(const Math::Vector& bodyOffsetVector);

	/*! \brief Sets the offset body quaternion, which will be sent on the next
	 successful update() call.
	 @param value A quaternion describing the body orientation offset.
	 */
	void setBodyOrientationOffset(const Math::Quaternion& value);

	/*! \brief Sets the leg angles, which will be sent on the next successful
	 update() call.
	 @param legAngles The array containing the 6 vectors, each with the coxa,
	 femur, and tibia angles in that order.
	 */
	void setLegAngles(Math::Vector* legAngles);

	/*! \brief Gets the last correctly read left joystick value in x.
	 @return The left joystick x value.
	 */
	const double& xLeft();

	/*! \brief Gets the last correctly read left joystick value in y.
	 @return The left joystick y value.
	 */
	const double& yLeft();

	/*! \brief Gets the last correctly read right joystick value in x.
	 @return The right joystick x value.
	 */
	const double& xRight();

	/*! \brief Gets the last correctly read right joystick value in y.
	 @return The right joystick y value.
	 */
	const double& yRight();

	/*! \brief Gets the last read acceleration vector of the iOS device.
	 @return A 3-vector of the sensed acceleration of the iOS device.
	 */
	const Math::Vector& acceleration();

	/*! \brief Gets the last correctly read first slider value.
	 @return The first slider value.
	 */
	const double& slider1();

	/*! \brief Gets the last correctly read second slider value.
	 @return The second slider value.
	 */
	const double& slider2();

	/*! \brief Adds an observer for a value change on a specific JSON key.
	 @param key The json key in the root of the incoming JSON string.
	 @param observer The value observer that is notified when the the key is sent.
	 */
	void addJsonValueListener(const std::string& key,
			JsonValueObserver* observer);

	/*! \brief Creates an option menu item.
	 */
	void addOptionMenuOption(AppOption* option);

	/*! \brief Checks for the open state of the communicator.
	 @return True if the interface is connected to a device.
	 */
	bool isOpen() {
		return currentlyOpen;
	}

	/*! \brief Sends a JSON value in the next packet to be sent.
	 @param value The JSON value to send.
	 */
	void sendJsonValue(Json::Value value);

private:
	std::map<std::string, AppOption*> optionsFromUser;
	std::map<std::string, AppOption*> optionsToPush;

	void sendOption(AppOption* option);

	// Variables to send:
	Math::Vector coxaIn, tibiaIn, femurIn;
	Math::Vector body, bodyOffset, headAngles;
	Math::Quaternion bodyOrientation, bodyOrientationOffset;

	// Variables received:
	Math::Vector leftJoystick, rightJoystick;
	Math::Vector accelerationValue;
	double slider1Value, slider2Value;

	std::string buffer;

	JsonSubject jsonSubject;  // the subject for both internal and externally
	// added listeners
	std::vector<JsonValueObserver*> valueObservers;  // this is just for memory
	// management, only for
	// internal observers

	int handlePossibleBuffer();
	std::string generateTransmitBuffer();

protected:
	/*! \brief Designates if the interface is currently open.
	 */
	bool currentlyOpen;

	/*! \brief This should send the given data to the concrete interface.
	 */
	virtual int sendState(std::string jsonState) = 0;

	// hmm this should handle the sending and receiving regardless of the
	// interface, but is diffcult to do at the moment...
	virtual void entryAction() = 0;

	// hmm this should handle the sending and receiving regardless of the
	// interface, but is diffcult to do at the moment...
	virtual void exitAction() = 0;

	/*!
	 This should be called whenever a new connection is established.
	 */
	void didConnect();

	/*!
	 Call this with a new set of incoming data
	 */
	int didReceiveData(std::string newData);
};

/*! \class AppInterface
 \brief Handles socket based communication with the Hexapod iOS app.

 This enables communication with the iOS app named Dmitri Hexapod Controller (or
 other socket-based client).
 
 https://itunes.apple.com/au/app/hexapod-controller/id671078464?mt=8
 (Android version in development)

 \image html app.png
 */
class AppInterface: public Communicator {
private:
	// Socket handling:
	//	struct sockaddr_in addr;
	int portNumber;
	int fd;
	int client;
	bool terminate;

	void entryAction();  // TODO: make this fit the entry/do/exit
	void exitAction();
	static void* multicastStart(void* This);
	void multicastThreadEntry();

	int sendState(std::string jsonData);

public:
	// Default constructors:
	AppInterface();
	~AppInterface();

	/*! \brief Sets the UDP port.
	 This only takes effect when called before the server is initialized.
	 Set the socket server port.
	 @param port The port number.
	 */
	void setPort(int port);

	/*! \brief Sets up the server to begin accepting clients.
	 Starts the server.
	 @return Returns 0 if successful.
	 */
	int start();

	/*! \brief Stops the server.
	 @return Returns 0 if successful.
	 */
	int stop();
};

}

}

#endif // LIBJSONCPP_FOUND

#endif
