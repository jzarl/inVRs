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

#include "GlutMouseDevice.h"

#ifdef WIN32
  #include "GL/glut.h"
#else
  #if defined(MACOS_X) || defined(__APPLE_CC__)
    #include <glut.h>
  #else
    #include <GL/glut.h>
  #endif
#endif

#include <assert.h>
#include "../../SystemCore/Timer.h"
#include "../../SystemCore/DebugOutput.h"

float	GlutMouseDevice::winWidth = -1;
float	GlutMouseDevice::winHeight = -1;
float	GlutMouseDevice::screenWidth = 0;
float	GlutMouseDevice::screenHeight = 0;
bool	GlutMouseDevice::grabMouse = true;
const float	GlutMouseDevice::DefaultAxisReleaseSpeed = 20.f;

std::vector<GlutMouseDevice*> GlutMouseDevice::activeDevices;

GlutMouseDevice::GlutMouseDevice(float axisReleaseSpeed) :
	axisReleaseSpeed(axisReleaseSpeed) {

	nButtons = 3;
	nAxes = 2;
	buttonState = new int[3];
	axisState = new float[2];

	memset(buttonState, 0, sizeof(int) * nButtons);
	memset(axisState, 0, sizeof(float) * nAxes);
	memset(oldPos, 0, sizeof(float) * 2);
	memset(oldPosRead, 0, sizeof(bool) * 2);

	axisWriteTimeStamp[0] = timer.getTime();
	axisWriteTimeStamp[1] = axisWriteTimeStamp[0];

	activeDevices.push_back(this);
} // GlutMouseDevice

GlutMouseDevice::~GlutMouseDevice() {
	std::vector<GlutMouseDevice*>::iterator it;
	bool foundDevice = false;
	for (it = activeDevices.begin(); it != activeDevices.end(); ++it) {
		if ((*it) == this) {
			activeDevices.erase(it);
			foundDevice = true;
			break;
		} // if
	} // for
	assert(foundDevice);
} // ~GlutMouseDevice

void GlutMouseDevice::update() {
	float currentTime;
	float deltaTime;

	// Controller is automatically locked when this method is called, so there
	// is no need to call acquireControllerLock()

	currentTime = timer.getTime();
	// Assure that axis-value is read at least one frame (otherwise if the
	// framerate drops too low it could happen that deltaTime is so high that
	// axis-value is not obtained in a single frame)
	if (oldPosRead[0]) {
		deltaTime = axisReleaseSpeed * (currentTime - axisWriteTimeStamp[0]);
		if (deltaTime > 1)
			deltaTime = 1;
		axisState[0] = oldPos[0] * (1 - deltaTime);
	} else {
		oldPosRead[0] = true;
	} // else

	if (oldPosRead[1]) {
		deltaTime = axisReleaseSpeed * (currentTime - axisWriteTimeStamp[1]);
		if (deltaTime > 1)
			deltaTime = 1;
		axisState[1] = oldPos[1] * (1 - deltaTime);
	} else {
		oldPosRead[1] = true;
	} // else
} // update

void GlutMouseDevice::cbGlutMouse(int button, int state, int x, int y) {
	int value;
	int buttonIndex = -1;

        if (activeDevices.size() == 0)
                return;

	value = (state == GLUT_UP ? 0 : 1);
	if (button == GLUT_LEFT_BUTTON)
		buttonIndex = 0;
	else if (button == GLUT_MIDDLE_BUTTON)
		buttonIndex = 1;
	else if (button == GLUT_RIGHT_BUTTON)
		buttonIndex = 2;
	else
		printd(WARNING, "GlutMouseDevice::cbGlutMouse(): unknown button constant recognized: %i\n",
				button);

	if (buttonIndex >= 0) {
		std::vector<GlutMouseDevice*>::iterator it;
		for (it = activeDevices.begin(); it != activeDevices.end(); ++it) {
			(*it)->setButtonState(buttonIndex, value);
		} // for
	} // if
} // cbGlutMouse


void GlutMouseDevice::cbGlutMouseMove(int x, int y) {
	float deltaX, deltaY;

	if (!grabMouse)
		return;

	if (activeDevices.size() == 0)
		return;

	if ((winWidth <= 0) || (winHeight <= 0)) {
		printd(WARNING,
				"GlutMouseDevice::cbGlutMouseMove(): call setWindowSize() with the appropriate values of your glut window!\n");
		return;
	} // if

	if (x == (int)winWidth / 2 && y == (int)winHeight / 2)
		return;

	if (screenWidth)
		deltaX = 2 * (x - (int)winWidth / 2) / (float)screenWidth;
	else
		deltaX = 2 * (x - (int)winWidth / 2) / (float)winWidth;

	if (screenHeight)
		deltaY = 2 * ((int)winHeight / 2 - y) / (float)screenHeight;
	else
		deltaY = 2 * ((int)winHeight / 2 - y) / (float)winHeight;

	std::vector<GlutMouseDevice*>::iterator it;
	for (it = activeDevices.begin(); it != activeDevices.end(); ++it) {
		(*it)->handleAxesChange(deltaX, deltaY);
	} // for

	glutWarpPointer((int)winWidth / 2, (int)winHeight / 2);
} // cbGlutMouseMove

void GlutMouseDevice::setWindowSize(int width, int height) {
        if (activeDevices.size() == 0)
                return;

	if (width == 0 || height == 0) {
		printd(WARNING, "GlutMouseDevice::setWindowSize(): recognized zero values: width=%i / height=%i\n", width, height);
		return;
	} // if

	winWidth = width;
	winHeight = height;
	screenWidth = glutGet(GLUT_SCREEN_WIDTH);
	screenHeight = glutGet(GLUT_SCREEN_HEIGHT);

	std::vector<GlutMouseDevice*>::iterator it;
	for (it = activeDevices.begin(); it != activeDevices.end(); ++it) {
		(*it)->resetAxes();
	} // for

	if (grabMouse) {
		glutSetCursor(GLUT_CURSOR_NONE);
		glutWarpPointer((int)winWidth / 2, (int)winHeight / 2);
	} else {
		glutSetCursor(GLUT_CURSOR_INHERIT);
	} // else
} // setWindowSize

void GlutMouseDevice::setMouseGrabbing(bool on) {
        if (activeDevices.size() == 0)
                return;

	if (on) {
		glutSetCursor(GLUT_CURSOR_NONE);
		glutWarpPointer((int)winWidth / 2, (int)winHeight / 2);
	} else {
		glutSetCursor(GLUT_CURSOR_INHERIT);
	}
	grabMouse = on;
} // setMouseGrabbing

void GlutMouseDevice::setButtonState(unsigned int buttonIndex, int value) {
	bool change = false;

	// We have to lock the state here because the method is not called by the
	// update method of the InputDevice
	acquireControllerLock();
	if (buttonState[buttonIndex] != value) {
		buttonState[buttonIndex] = value;
		change = true;
	} // if
	releaseControllerLock();

	if (change)
		sendButtonChangeNotification(buttonIndex, value);
} // setButtonState

void GlutMouseDevice::handleAxesChange(float deltaX, float deltaY) {
	float currentTime = timer.getTime();

	// We have to lock here because the method is not called out of the update
	// method, so there could occur an axis-read at the same time
	acquireControllerLock();
	if (fabs(deltaX) > 0) {
		axisState[0] += deltaX;
		if (axisState[0] > 1)
			axisState[0] = 1;
		else if (axisState[0] < -1)
			axisState[0] = -1;

		oldPos[0] = axisState[0];
		oldPosRead[0] = false;
		axisWriteTimeStamp[0] = currentTime;
	} // if

	if (fabs(deltaY) > 0) {
		axisState[1] += deltaY;
		if (axisState[1] > 1)
			axisState[1] = 1;
		else if (axisState[1] < -1)
			axisState[1] = -1;

		oldPos[1] = axisState[1];
		oldPosRead[1] = false;
		axisWriteTimeStamp[1] = currentTime;
	} // if
	releaseControllerLock();
} // handleAxesChange

void GlutMouseDevice::resetAxes() {
	float currentTime = timer.getTime();

	// We have to lock here because the method is not called out of the update
	// method, so there could occur an axis-read at the same time
	acquireControllerLock();
	axisState[0] = 0;
	axisState[1] = 0;
	releaseControllerLock();

	oldPos[0] = 0;
	oldPos[1] = 0;
	oldPosRead[0] = false;
	oldPosRead[1] = false;
	axisWriteTimeStamp[0] = currentTime;
	axisWriteTimeStamp[1] = currentTime;
} // resetAxes


InputDevice* GlutMouseDeviceFactory::create(std::string className, ArgumentVector* args) {
	if (className != "GlutMouseDevice")
		return NULL;

	float axisReleaseSpeed = GlutMouseDevice::DefaultAxisReleaseSpeed;

	if (args) {
		if (args->keyExists("axisReleaseSpeed"))
			args->get("axisReleaseSpeed", axisReleaseSpeed);
	} // if

	return new GlutMouseDevice(axisReleaseSpeed);
} // create

