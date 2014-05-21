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

#include "InputDeviceBase.h"

#include <assert.h>

#include "../../SystemCore/DebugOutput.h"

InputDeviceBase::InputDeviceBase() :
	nButtons(0),
	nAxes(0),
	nSensors(0),
	buttonState(NULL),
	axisState(NULL),
	sensorState(NULL) {
} // InputDeviceBase

InputDeviceBase::~InputDeviceBase() {
	if(axisState) delete [] axisState;
	if(buttonState) delete [] buttonState;
	if(sensorState) delete [] sensorState;
} // ~InputDeviceBase

int InputDeviceBase::getNumberOfButtons() {
	return nButtons;
} // getNumberOfButtons

int InputDeviceBase::getNumberOfAxes() {
	return nAxes;
} // getNumberOfAxes

int InputDeviceBase::getNumberOfSensors() {
	return nSensors;
} // getNumberOfSensors

int InputDeviceBase::getButtonValue(int idx) {
	if (idx>=nButtons) {
		printd(WARNING,
				"InputDeviceBase::getButtonValue(): button index out of range : %i device has only %i buttons\n",
				idx, nButtons);
		return 0;
	} // if
	assert(buttonState != NULL);
	return buttonState[idx];
} // getButtonValue

float InputDeviceBase::getAxisValue(int idx) {
	if (idx>=nAxes) {
		printd(WARNING,
				"InputDeviceBase::getAxisValue(): axis index out of range : %i device has only %i axes\n",
				idx, nAxes);
		return 0.0f;
	} // if
	assert(axisState != NULL);
	return axisState[idx];
} // getAxisValue

SensorData InputDeviceBase::getSensorValue(int idx) {
	if (idx>=nSensors) {
		printd(WARNING,
				"InputDeviceBase::getSensorValue(): sensor index out of range : %i device has only %i sensors\n",
				idx, nSensors);
		return IdentitySensorData;
	} // if
	assert(sensorState != NULL);
	return sensorState[idx];
} // getSensorValue

void InputDeviceBase::update() {

} // update
