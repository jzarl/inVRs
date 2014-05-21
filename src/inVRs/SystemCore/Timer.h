/*---------------------------------------------------------------------------*\
 *           interactive networked Virtual Reality system (inVRs)            *
 *                                                                           *
 *    Copyright (C) 2005-2009 by the Johannes Kepler University, Linz        *
 *                                                                           *
 *                            www.inVRs.org                                  *
 *                                                                           *
 *              contact: canthes@inVRs.org, rlander@inVRs.org                *
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This library is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU Library General Public License as published    *
 * by the Free Software Foundation, version 2.                               *
 *                                                                           *
 * This library is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public         *
 * License along with this library; if not, write to the Free Software       *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#ifndef _TIMER_H_
#define _TIMER_H_

#include "Platform.h"

namespace inVRsUtilities {

class INVRS_SYSTEMCORE_API Timer {
public:

	//*****************//
	// PUBLIC METHODS: //
	//*****************//

	/** Create a timer.
	 * @param start Set this parameter to true, if you want the timer to start running immediately.
	 */
	Timer(bool start = false);

	/** Desctructor
	 */
	~Timer();

	/**
	 * Start the timer.
	 * Calling start on a running timer does not change the timer.
	 * Calling start on a stopped timer also resets the time.
	 */
	void start();

	/**
	 * Stop the timer.
	 * Stopping the timer also triggers an update, so that the timer
	 * always has the exact time when stop was called.
	 *
	 * @see start()
	 */
	void stop();

	/**
	 * Reset a timer.
	 * Resetting the timer resets the time and speed, but does not 
	 * affect whether the timer is running.
	 */
	void reset();

	/**
	 * Sets the timer speed relative to wallclock time.
	 */
	void setSpeed(double speed);

	/**
	 * Get wallclock  time since some unspecified point in the past.
	 * @return seconds
	 *
	 * @see getTimeInMilliseconds()
	 */
	static double getSystemTime();

	/**
	 * Update the timer and get the current time since the timer was started.
	 */
	double getTime();

	/**
	 * Update the timer and get the current time since the timer was started in milliseconds.
	 * @return milliseconds
	 *
	 * @see getTime()
	 */
	double getTimeInMilliseconds();

	/**
	 * Return the speed of the timer relative to wallclock time.
	 */
	double getSpeed() const;

	/**
	 * Returns whether the timer is running.
	 */
	bool isRunning() const;

protected:

	//********************//
	// PROTECTED METHODS: //
	//********************//

	//	void initialize

	//********************//
	// PROTECTED MEMBERS: //
	//********************//

	/// Time the timer was started
	double startTime;

	/// Value of the timer, always updated when getTime is called
	double timerValue;

	/// Value of the system time at last request
	double previousTime;

	/// factor of timer speed
	double speedFactor;

	/// defines if the timer is running
	bool running;

}; // Timer

} // inVRsUtilities

extern INVRS_SYSTEMCORE_API inVRsUtilities::Timer timer;

#endif /*_TIMER_H_*/
