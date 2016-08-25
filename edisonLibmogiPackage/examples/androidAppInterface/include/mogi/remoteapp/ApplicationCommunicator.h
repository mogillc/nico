/*
 * ApplicationCommunication.h
 *
 *  Created on: Apr 29, 2016
 *      Author: adrian
 */

#ifndef INCLUDE_MOGI_REMOTEAPP_APPLICATIONCOMMUNICATOR_H_
#define INCLUDE_MOGI_REMOTEAPP_APPLICATIONCOMMUNICATOR_H_

#include <mogi/remoteapp/ApplicationServer.h>
#include <mogi/remoteapp/HexapodPayload.h>
#include "mogi/app/json.h"
#include "mogi/math/mmath.h"
#include "mogi/network/Packet.h"

namespace Mogi {
	namespace RemoteApp {

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


		/*! @class ApplicationCommunicator
		 * \brief Application thread that handles hexapod control commands from TCP clients. Replies with hexapod state information.
		 */
		class ApplicationCommunicator : public Network::ConnectionHandler {
		public:
			ApplicationCommunicator(ApplicationServer& manager);
			~ApplicationCommunicator();

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
					App::JsonValueObserver* observer);

			/*! \brief Creates an option menu item.
			 */
			void addOptionMenuOption(AppOption* option);
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

			//std::string buffer;
			HexapodPacket rxPacket;
			HexapodPacket txPacket;
			Network::SocketAddress currentClient;

			App::JsonSubject jsonSubject;  // the subject for both internal and externally
			// added listeners
			std::vector<App::JsonValueObserver*> valueObservers;  // this is just for memory
			// management, only for
			// internal observers

			std::string generateTransmitBuffer();

			/*!
			 This should be called whenever a new connection is established.
			 */
			void didConnect();

			void exitAction();
			void process(Network::TCPSocket& fd, const unsigned char* data, unsigned int size, const Network::SocketAddress& sourceAddress);
			int didReceiveData(Network::TCPSocket& fd, HexapodPacket& packet);

			void releaseMemory();

			ApplicationCommunicator(const ApplicationCommunicator& other);
			ApplicationCommunicator& operator=(const ApplicationCommunicator& other);

		};

	}
}


#endif /* INCLUDE_MOGI_REMOTEAPP_APPLICATIONCOMMUNICATOR_H_ */
