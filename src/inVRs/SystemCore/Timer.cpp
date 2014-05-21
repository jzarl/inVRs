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

#include "Timer.h"

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <sys/timeb.h>
#include <cmath>

#include "Platform.h"

INVRS_SYSTEMCORE_API inVRsUtilities::Timer timer = inVRsUtilities::Timer(true);

namespace inVRsUtilities {

//*****************//
// PUBLIC METHODS: //
//*****************//

Timer::Timer(bool start) {
	running = false;
	timerValue = 0;
	speedFactor = 1;
	startTime = 0;
	if (start)
		this->start();
} // Timer

Timer::~Timer() {

} // ~Timer

void Timer::start() {
	if (running)
		return;

	startTime = getSystemTime();
	previousTime = startTime;
	running = true;
} // start

void Timer::stop() {
	if (!running)
		return;

	getTime();
	running = false;
} // stop

void Timer::reset() {
	startTime = getSystemTime();
	previousTime = startTime;
	timerValue = 0;
	speedFactor = 1;
} // reset

void Timer::setSpeed(double speed) {
	speedFactor = speed;
} // setSpeed

double Timer::getSystemTime() {
#ifdef WIN32
	return ((double)GetTickCount()/1000.0);
#else
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	gettimeofday(&tv, 0);
	return ((double)tv.tv_sec + (double)tv.tv_usec / 1000000.0);
#endif
} // getSystemTime

double Timer::getTime() {
	if (!running)
		return timerValue;

	double currentTime = getSystemTime();
	double dt = currentTime - previousTime;
	timerValue += dt * speedFactor;

	previousTime = currentTime;
	return timerValue;
} // getTime

double Timer::getTimeInMilliseconds() {
	return getTime() * 1000.0;
} // getTimeInMilliseconds

double Timer::getSpeed() const
{
	return speedFactor;
} // getSpeed

bool Timer::isRunning() const
{
	return running;
} // isRunning

} // inVRsUtilities
