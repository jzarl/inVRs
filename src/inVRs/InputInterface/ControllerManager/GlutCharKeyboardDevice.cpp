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

#include "GlutCharKeyboardDevice.h"

#include <assert.h>

std::vector<GlutCharKeyboardDevice*> GlutCharKeyboardDevice::activeDevices;

GlutCharKeyboardDevice::GlutCharKeyboardDevice() {
	nButtons = 256;
	buttonState = new int[256];
	memset(buttonState, 0, sizeof(int) * nButtons);

	activeDevices.push_back(this);
} // GlutCharKeyboardDevice

GlutCharKeyboardDevice::~GlutCharKeyboardDevice() {
	std::vector<GlutCharKeyboardDevice*>::iterator it;
	bool foundDevice = false;
	for (it = activeDevices.begin(); it != activeDevices.end(); ++it) {
		if (*it == this) {
			activeDevices.erase(it);
			foundDevice = true;
			break;
		} // if
	} // for
	assert(foundDevice);
} // ~GlutCharKeyboardDevice

void GlutCharKeyboardDevice::cbGlutKeyboard(unsigned char k, int x, int y) {
	std::vector<GlutCharKeyboardDevice*>::iterator it;

	for (it = activeDevices.begin(); it != activeDevices.end(); ++it) {
		(*it)->setButtonState(k, 1);
	} // for
} // cbGlutKeyboard

void GlutCharKeyboardDevice::cbGlutKeyboardUp(unsigned char k, int x, int y) {
	std::vector<GlutCharKeyboardDevice*>::iterator it;

	for (it = activeDevices.begin(); it != activeDevices.end(); ++it) {
		(*it)->setButtonState(k, 0);
	} // for
} // cbGlutKeyboardUp

void GlutCharKeyboardDevice::setButtonState(unsigned int buttonIndex, int value) {
	bool change = false;

	// We have to lock the state here because the method is not called by the
	// update method of the InputDevice
	acquireControllerLock();
	if (buttonState[buttonIndex] != value) {
		buttonState[buttonIndex] = value;
		change = true;
	} // if
	releaseControllerLock();

	// If the button value has changed we have to notify the controller so that
	// callbacks can be initiated
	if (change)
		sendButtonChangeNotification(buttonIndex, value);
} // setButtonState


InputDevice* GlutCharKeyboardDeviceFactory::create(std::string className, ArgumentVector* args) {
	if (className != "GlutCharKeyboardDevice")
		return NULL;

	return new GlutCharKeyboardDevice;
} // create
