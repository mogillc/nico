/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                            Author: Matt Bunting                            *
 *                                                                            *
 *            This program is distributed under the LGPL, version 2           *
 *                                                                            *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License              *
 *   version 2.1 as published by the Free Software Foundation;                *
 *                                                                            *
 *   See license in root directory for terms.                                 *
 *   https://github.com/mogillc/nico/tree/master/edisonLibmogiPackage/libmogi *
 *                                                                            *
 *****************************************************************************/

#ifndef MOGI_FILTER_H
#define MOGI_FILTER_H

#include <string.h>
#include <sys/time.h>
#include <iostream>
#include "mogi/math/mmath.h"

namespace Mogi {

	/**
	 * @namespace Mogi::Math
	 * \brief Math tools, mainly focused on matrices, vectors, and quaternions.
	 */
	namespace Math {

		/*!
		 @class Time
		 \brief Keeps track of time; useful for none RTOS setups

		 This keeps track of time such as running time and change in loop time.
		 @since 2013-02-12
		 */
		class Time {
		private:
			struct timeval tv;
			double fnum;
			double timecycle, oldtimecycle;
			double dTimeBuffer[10];
			double fpsLPF;
			double fps;
			double favg, dtime, runningtime;

		public:
			Time();

			/*! \brief Resets the references for computing running time.
			 */
			void reset();

			/*!
			 This function typically should be called once every loop cycle.  This updates
			 the measured components based on the last call.
			 Updates internal time values.
			 */
			void update();  // updates to get new time values

			/*!
			 This function starts the internal time measurement.  The next time update() is
			 called, values are updated based in reference to this call.
			 Initializes the internal values needed for time measurement.
			 */
			void initialize();  // self explanatory

			/*!
			 This returns the last computed running time since initialization.
			 Gets the last running time computation.
			 @return The time since initialize() was called.
			 */
			double runningTime();

			/*! Returns the last computed difference in time between the last update() and
			 update() prior to that.
			 Gets the last computed delta time
			 @return The delta time between the previous two update() calls.
			 */
			double dTime();

			/*!
			 This returns the last computed total update frequency average of update() calls.
			 This is averaged over the entire life span.
			 Gets the last computed average frequency of calls of update()
			 @return The average frequency of update() calls.
			 */
			double frequencyAverage();

			/*!
			 This returns the last computed running update frequency average of update()
			 calls.  This is a moving average from the last ten samples.
			 Gets the last computed moving average of frequency from calls of update()
			 @return The moving average of frequency from update() calls.
			 */
			double frequency();
		};

		/*!
		 @class LowPassFilter
		 \brief Implementation of a Low Pass Filter; useful for none RTOS setups

		 This class low pass filters the input signal based on a filter time constant.
		 There is an internal timer that computes the alpha value dynamically for use
		 with an alpha filter.
		 @since 2013-04-19
		 */
		template<class T>
		class LowPassFilter {
		private:
			//	Time filterTimer;
			T outputValue;
			double timeConstant;

			// Quaternion outputQuaternion;
		public:
			LowPassFilter();

			/*!
			 filter
			 This function typically should be called once every loop cycle.  This updates
			 the filtered value based on the time since the last call.
			 Updates the filter state.
			 @param input The input value to be filtered
			 @param dtime The difference in time since the last call to this method.
			 @return The output of the filter
			 */
			const T& filter(const T& input, double dtime);
			// const Quaternion& filter( const Quaternion& input, double dtime );

			/*!
			 setTimeConstant
			 This function typically should be called after initialization, though could be
			 called dynamically.  This sets the time constant of the filter.
			 Updates the filter time constant.
			 @param timeConstantValue The desired LPF time constant.
			 */
			void setTimeConstant(double timeConstantValue) {
				timeConstant = timeConstantValue;
			}

			/*! \brief Returns the last computed output of the filter

			 Gets the last filter output.
			 @return The previous output of the filter
			 */
			const T& output() {
				return outputValue;
			}

			/*! \brief Sets the current output of the filter.

			 This is useful for when needing to set the initial state of the filter.
			 \param currentOutput The current output
			 */
			void setCurrentOutput(const T& currentOutput) {
				outputValue = currentOutput;
			}

			//	/*!
			//	  initialize
			//	  This function starts the internal time measurement.  The next time
			//filter() is called, values are updated based in reference to this call.
			//	  Initializes the internal values needed for time measurement.
			//	 */
			//    void initialize();              // self explanatory
		};

		/*!
		 @class Random
		 \brief Computation of random variables.
		 @since 2015-01-07
		 */
		class Random {
		public:
			Random();

			/*!
			 uniform
			 Calculates a random variable under a uniform distribution, under a specified
			 range.
			 @param min The minimum value in the range.
			 @param max The maximum value in the range.
			 @return A random, uniform variable based on the specified range.
			 */
			double uniform(double min = 0, double max = 1);

			/*!
			 uniform
			 Calculates a random variable under a normal distribution, under a specified
			 mean and vairance.
			 @param mean The mean of the Gaussian.
			 @param variance The Variance of the Gaussian.
			 @return A random, normal variable based on the specified mean and variance.
			 */
			double normal(double mean = 0, double variance = 1);

		private:
		};

		/*!
		 @class MotionControl
		 \brief This implements both velocity and acceleration based location control.
		 @since 2015-01-07
		 */
//		template<class T>
		class MotionControl {
		public:

			/*!	\brief The control mode types of the motion controller.
			 */
			enum ControlMode {
				VELOCITY, /*!< Designates velocity based control. */
				LOCATION /*!< Designates acceleration based velocity-limited, goal location control. */
			};

			/*!	\brief The motion controller states.
			 */
			enum LocationState {
				ACCELERATION = 0, /*!< Currently accelerating. */
				MOVING = 1, /*!< Currently moving at constant velocity. */
				DECELERATING = 2, /*!< Currently Decelerating. */
				STOPPED = 3 /*!< Currently stopped, i.e. at the goal. */
			};

			MotionControl();
			virtual ~MotionControl() {
			}

			/**
			 Called to update the motion control state and position based on a time
			 difference and the previously set parameters of acceleration, max speed, and
			 goal position.  The internal state after this call will reflect the state
			 after performing an update to the motion model.
			 @param dtime The change in time since the previous call.
			 @return The updated position.
			 */
			const double& processLocationControl(double dtime);

			/**
			 Sets the motion controller into acceleration based location control with
			 speed limits.
			 */
			void enableLocationControl();

			/**
			 Sets the motion controller into velocity control mode with no restrictions.
			 */
			void enableVelocityControl();

			/**
			 Sets the desired max speed for location control mode.
			 @param speed The maximum desired speed.
			 */
			void setMaxSpeed(double speed);

			/**
			 Sets the acceleration and deceleration magnitude for Location control mode.
			 @param acceleration The desired acceleration and deceleration magnitude.
			 */
			void setAcceleration(double acceleration);

			/**
			 Sets the goal position for Location control mode.
			 @param location The goal location to move towards.
			 */
			virtual void setGoalPosition(const double& location);

			/**
			 Gets the goal position for Location control mode.
			 @return The goal location to move towards from setGoalPosition().
			 */
			double getGoalPosition() const;

			/**
			 Gets the position that was last computed.
			 @return The current location.
			 */
			double getCurrentPosition() const;

			/**
			 Gets the velocity that was last computed.
			 @return The current velocity.
			 */
			double getCurrentVelocity() const;

			double getMaxAcceleration() const;

			/**
			 Gets the amount of time to stop, based on the goal and current position.
			 @return The Time until the stopped state is reached.
			 */
			double getTimeToStop();

			/**
			 Sets the velocity for Velocity control mode.
			 @param velocity The velocity to move.
			 */
			void setVelocity(const double& velocity);

			double getSoonestStoppingLocation();

			/**
			 Returns the current state.
			 @return The current state.
			 */
			LocationState getState() {
				return locationState;
			}

			/**
			 Converts the state to a string.
			 \param state The state to be converted.
			 @return The current state in a readable from.
			 */
			static std::string stateToString(LocationState state) {
				switch (state) {
					case MOVING:
						return "MOVING";
					case ACCELERATION:
						return "ACCELERATION";
					case DECELERATING:
						return "DECELERATING";
					case STOPPED:
						return "STOPPED";
				}
				return "";
			}

		private:
			ControlMode controlMode;

//			double tolerance;

			double maxSpeed;         // use with setter

			double dt[3];	// region state times
			double dx[3];	// region distances to travel
			double acc[4];	// Acceleration regions

//			void updateState();
//			void calculateAcceleration();

			void determineStateTimes();

			/**
			 \brief Computes the Euclidean magnitude of a value.
			 This is used during computation of velocities.
			 \param value The value to compute the magnitude.
			 @return The Euclidean magnitude to the goal.
			 */
			double getMagnitude(const double& value) const;

			double getDirection(const double& value) const;

			virtual double getDifference(const double& to, const double& from) const;

		protected:

			/*! The current control state.
			 */
			LocationState locationState;

			/*! The current velocity.
			 */
			double velocity;    // dynamically determined

			/*! The magnitude of the current velocity.
			 */
//			double speed;

			/*! The current acceleration/deceleration with direction.
			 */
			double acceleration;

			/*! The acceleration magnitude of the motion control.
			 */
			double maxAcceleration;  // use with setter

			/*! The goal location, if in LOCATION control mode.
			 */
			double goalPosition;

			/*! The current location.
			 */
			double position;

			/**
			 \brief Returns the distance left to the goal.
			 This is used during computation of the internal state based on acceleration settings.
			 \return The Euclidean magnitude to the goal.
			 */
			virtual double getDistanceLeft();

			/**
			 \brief Performs an update based on the time step.
			 This is used for computing the new position from the parameters.
			 \param dtime The time step.
			 */
			virtual void updateModel(double dtime);

		};

		class MotionControlMultiple {
		private:
			double maxA;
			double maxS;
			unsigned int count;

			MotionControl* motionControllers;

		public:
			MotionControlMultiple( unsigned int count );
			~MotionControlMultiple();

			Vector process(double dtime);
			void setGoal(const Vector& goal);
			void setMaxSpeed(const double& max);
			void setMaxAcceleration(const double& max);

			Vector getCurrentPosition();
			Vector getCurrentVelocity();
			Vector getGoalPosition();
			Vector getSoonestStoppingLocation();

			bool isStopped();

		};

		/*!
		 @class MotionControlAngular
		 \brief This implements both velocity and acceleration based location control for angular motion.

		 This finds the shortest path to the heading.  For example, if current heading is at -3pi/4, and the goal
		 is pi/2, this will result in a motion through pi instead of 0.  In other words, it will be clockwise since
		 that results in the shortest path.
		 @since 2015-01-07
		 */
		class MotionControlAngular: public MotionControl {
		protected:
			double getDistanceLeft();
			void updateModel(double dtime);
			double getDifference(const double& to, const double& from) const;

		public:

			/**
			 Sets the goal position for Location control mode.
			 This sets the goal heading to the modulus of 2*pi.
			 @param location The goal location to move towards.
			 */
			void setGoalPosition(const double& location);
		};

		/*!
		 @class Gain
		 \brief This implements a system gain block with a set multiplier.
		 @since 2016-08-03
		 */
		class Gain {
		private:
			double gain;

		protected:
			double saturationMax;
			double saturationMin;

			/**
			 Sets the gain multiplier.
			 \param gain The multiplier.
			 \return The inherited computation.
			 */
			virtual double computeBase(const double& input, const double& dtime) = 0;

		public:
			Gain();
			virtual ~Gain();

			/**
			 Sets the gain (K) multiplier.
			 \param gain The multiplier.
			 */
			void setGain(const double& gain);

			/**
			 Gets the previously set gain multiplier.
			 \return gain The multiplier (K).
			 */
			const double& getGain() const;

			/**
			 Sets the saturation limits of the output
			 \param gain The multiplier (K).
			 */
			void setSaturation(const double& min, const double& max);

			/**
			 Performs the gain process given an input and returns the final value.
			 \param input The input to the system block.
			 \param dTime the difference in time.
			 \return The inherited computation * K, i.e. the system block output.
			 */
			double perform(const double& input, const double& dtime);
		};

		/*!
		 @class GainP
		 \brief This implements a proportional gain block.
		 @since 2016-08-03
		 */
		class GainP : public Gain {
		private:
			double computeBase(const double& input, const double& dtime);
		};

		/*!
		 @class GainI
		 \brief This implements an integrating gain block.
		 @since 2016-08-03
		 */
		class GainI : public Gain {
		private:
			double runningValue;
			double computeBase(const double& input, const double& dtime);
		public:
			GainI();

			/**
			 Resets the accumulated integral to the provided value (defualt 0;
			 \param initialValue The starting value of the integrator.
			 */
			void reset(const double& initialValue = 0);
		};
		
		/*!
		 @class GainD
		 \brief This implements a derivative gain block.
		 @since 2016-08-03
		 */
		class GainD : public Gain {
		private:
			int averagerLength;
			double* priorValues;
			double* priorTimes;
			double computeBase(const double& input, const double& dtime);
		public:
			GainD(int averagerKernelSize = 4);
			~GainD();
		};
		
		/*!
		 @class GainPID
		 \brief This implements a PID block.
		 
		 By defualt this block sets the I and D gains to 0.
		 @since 2016-08-03
		 */
		class GainPID : public Gain {
		private:
			GainP blockP;
			GainI blockI;
			GainD blockD;
			
			double computeBase(const double& input, const double& dtime);
		public:
			GainPID(int averagerKernelSize = 4);

			/**
			 Gets the internal proportional gain block (P).
			 \return The P gain block.
			 */
			GainP* gainP();
			
			/**
			 Gets the internal integral gain block (I).
			 \return The I gain block.
			 */
			GainI* gainI();
			
			/**
			 Gets the internal derivative gain block (D).
			 \return The D gain block.
			 */
			GainD* gainD();
		};
		
	}
	
}

#endif
