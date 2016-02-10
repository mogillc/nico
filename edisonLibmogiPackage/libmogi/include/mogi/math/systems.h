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
	double maxDTime;
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

	/*! \brief Sets the maximum time difference, to avoid computation errors.
	 \param time The maximum dtime allowed.
	 */
	void setMaxDeltaTime(double time);

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
template<class T>
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
		ACCELERATION, /*!< Currently accelerating. */
		MOVING, /*!< Currently moving at constant velocity. */
		DECELERATING, /*!< Currently Decelerating. */
		STOPPED /*!< Currently stopped, i.e. at the goal. */
	};

	MotionControl<T>();
	virtual ~MotionControl<T>() {
	}

	/**
	 Called to update the motion control state and position based on a time
	 difference and the previously set parameters of acceleration, max speed, and
	 goal position.  The internal state after this call will reflect the state
	 after performing an update to the motion model.
	 @param dtime The change in time since the previous call.
	 @return The updated position.
	 */
	const T& processLocationControl(long double dtime);

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
	virtual void setGoalPosition(const T& location);

	/**
	 Sets the velocity for Velocity control mode.
	 @param velocity The velocity to move.
	 */
	void setVelocity(const T& velocity);

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

	long double tolerance;

	long double maxSpeed;         // use with setter

	void updateState();
	void calculateAcceleration();

	/**
	 \brief Computes the Euclidean magnitude of a value.
	 This is used during computation of velocities.
	 \param value The value to compute the magnitude.
	 @return The Euclidean magnitude to the goal.
	 */
	long double getMagnitude(const T& value) const;

	long double getNextStateChangeTime(double dtime);

protected:

	/*! The current control state.
	 */
	LocationState locationState;

	/*! The current velocity.
	 */
	T velocity;    // dynamically determined

	/*! The magnitude of the current velocity.
	 */
	long double speed;

	/*! The current acceleration/deceleration with direction.
	 */
	T acceleration;

	/*! The acceleration magnitude of the motion control.
	 */
	long double maxAcceleration;  // use with setter

	/*! The goal location, if in LOCATION control mode.
	 */
	T goalPosition;

	/*! The current location.
	 */
	T position;

	/**
	 \brief Returns the distance left to the goal.
	 This is used during computation of the internal state based on acceleration settings.
	 \return The Euclidean magnitude to the goal.
	 */
	virtual long double getDistanceLeft();

	/**
	 \brief Performs an update based on the time step.
	 This is used for computing the new position from the parameters.
	 \param dtime The time step.
	 */
	virtual void updateModel(long double dtime);

	/**
	 \brief Returns a normalized value to the goal.
	 This is what computes the new position from the parameters.
	 \return A normalized value designating goal direction.
	 */
	virtual T getDirectionToGoal() const;
};

/*!
 @class MotionControlAngular
 \brief This implements both velocity and acceleration based location control for angular motion.

 This finds the shortest path to the heading.  For example, if current heading is at -3pi/4, and the goal
 is pi/2, this will result in a motion through pi instead of 0.  In other words, it will be clockwise since
 that results in the shortest path.
 @since 2015-01-07
 */
class MotionControlAngular: public MotionControl<double> {
protected:
	long double getDistanceLeft();
	void updateModel(long double dtime);
	double getDirectionToGoal() const;

public:

	/**
	 Sets the goal position for Location control mode.
	 This sets the goal heading to the modulus of 2*pi.
	 @param location The goal location to move towards.
	 */
	void setGoalPosition(const double& location);
};

}

}

#endif
