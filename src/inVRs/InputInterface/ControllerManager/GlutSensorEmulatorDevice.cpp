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

#include "GlutSensorEmulatorDevice.h"

#include <assert.h>

#include <gmtl/AxisAngle.h>
#include <gmtl/Generate.h>

#ifdef WIN32
	#include "GL/glut.h"
#else
  #if defined(MACOS_X) || defined(__APPLE_CC__)
    #include <glut.h>
  #else
    #include <GL/glut.h>
  #endif
#endif

#include "../../SystemCore/DebugOutput.h"

float	GlutSensorEmulatorDevice::winWidth = -1;
float	GlutSensorEmulatorDevice::winHeight = -1;
float	GlutSensorEmulatorDevice::screenWidth = 0;
float	GlutSensorEmulatorDevice::screenHeight = 0;
bool 	GlutSensorEmulatorDevice::grabMouse = true;

// const values are defined in header!!!
//const int GlutSensorEmulatorDevice::DefaultSwitchSensorButton = 256;
//const int GlutSensorEmulatorDevice::DefaultSwitchTransformationTargetButton = 257;
//const int GlutSensorEmulatorDevice::DefaultSwitchAxesButton = 258;
//const int GlutSensorEmulatorDevice::DefaultNumberOfSensors = 2;

std::vector<GlutSensorEmulatorDevice*> GlutSensorEmulatorDevice::activeDevices;

GlutSensorEmulatorDevice::GlutSensorEmulatorDevice(unsigned int numSensors,
		unsigned int switchSensorButton, unsigned int switchTransformationTargetButton,
		unsigned int switchAxesButton) :
	switchSensorButton(switchSensorButton),
	switchTransformationTargetButton(switchTransformationTargetButton),
	switchAxesButton(switchAxesButton),
	sensorIdx(0),
	useYAxis(false),
	useZRotation(false),
	useOrientation(false) {

	nSensors = numSensors;
	sensorState = new SensorData[nSensors];

	for (int i = 0; i < nSensors; i++) {
		sensorState[i].orientation = gmtl::QUAT_IDENTITYF;
		sensorState[i].position = gmtl::Vec3f(0, 0, 0);
	} // for

	activeDevices.push_back(this);
} // GlutSensorEmulatorDevice

GlutSensorEmulatorDevice::~GlutSensorEmulatorDevice() {
	std::vector<GlutSensorEmulatorDevice*>::iterator it;

	bool foundDevice = false;
	for (it=activeDevices.begin(); it != activeDevices.end(); ++it) {
		if ((*it) == this) {
			activeDevices.erase(it);
			foundDevice = true;
			break;
		} // if
	} // for
	assert(foundDevice);
} // ~GlutSensorEmulatorDevice

void GlutSensorEmulatorDevice::cbGlutKeyboard(unsigned char k, int x, int y) {
	std::vector<GlutSensorEmulatorDevice*>::iterator it;

        if (activeDevices.size() == 0)
                return;

	for (it = activeDevices.begin(); it != activeDevices.end(); ++it) {
		(*it)->handleButtonPress(k);
	} // for
} // cbGlutKeyboard

void GlutSensorEmulatorDevice::cbGlutMouse(int button, int state, int x, int y) {
	int buttonIndex = -1;

        if (activeDevices.size() == 0)
                return;

	if (state == GLUT_UP)
		return;

	if (button == GLUT_LEFT_BUTTON)
		buttonIndex = 256;
	else if (button == GLUT_MIDDLE_BUTTON)
		buttonIndex = 257;
	else if (button == GLUT_RIGHT_BUTTON)
		buttonIndex = 258;
	else
		printd(WARNING, "GlutSensorEmulatorDevice::cbGlutMouse(): unknown button constant: %i\n",
				button);

	if (buttonIndex >= 0) {
		std::vector<GlutSensorEmulatorDevice*>::iterator it;
		for (it = activeDevices.begin(); it != activeDevices.end(); ++it) {
			(*it)->handleButtonPress(buttonIndex);
		} // for
	} // if
} // cbGlutMouse

void GlutSensorEmulatorDevice::cbGlutMouseMove(int x, int y) {
	float deltaX, deltaY;

        if (activeDevices.size() == 0)
                return;

	if (!grabMouse)
		return;

	if ((winWidth <= 0) || (winHeight <= 0)) {
		printd(WARNING,
				"GlutSensorEmulatorDevice::cbGlutMouseMove(): call setWindowSize() with the appropriate values of your glut window!\n");
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

	std::vector<GlutSensorEmulatorDevice*>::iterator it;
	for (it = activeDevices.begin(); it != activeDevices.end(); ++it) {
		(*it)->handleAxesChange(deltaX, deltaY);
	} // for

	glutWarpPointer((int)winWidth / 2, (int)winHeight / 2);
} // cbGlutMouseMove

void GlutSensorEmulatorDevice::setWindowSize(int width, int height) {
        if (activeDevices.size() == 0)
                return;

	if (width == 0 || height == 0) {
		printd(WARNING, "GlutSensorEmulatorDevice::setWindowSize(): recognized zero values: width=%i / height=%i\n",
				width, height);
		return;
	} // if

	winWidth = width;
	winHeight = height;
	screenWidth = glutGet(GLUT_SCREEN_WIDTH);
	screenHeight = glutGet(GLUT_SCREEN_HEIGHT);

	if (grabMouse) {
		glutSetCursor(GLUT_CURSOR_NONE);
		glutWarpPointer((int)winWidth / 2, (int)winHeight / 2);
	} else {
		glutSetCursor(GLUT_CURSOR_INHERIT);
	} // else
} // setWindowSize

void GlutSensorEmulatorDevice::setMouseGrabbing(bool on) {
        if (activeDevices.size() == 0)
                return;

	if (on) {
		glutSetCursor(GLUT_CURSOR_NONE);
		glutWarpPointer((int)winWidth / 2, (int)winHeight / 2);
	} else {
		glutSetCursor(GLUT_CURSOR_INHERIT);
	} // else
	grabMouse = on;
} // setMouseGrabbing

void GlutSensorEmulatorDevice::handleButtonPress(unsigned int buttonIndex) {
	if (buttonIndex == switchSensorButton) {
		sensorIdx = (sensorIdx + 1) % nSensors;
		useOrientation = false;
		useZRotation = false;
		useYAxis = false;
		printd(INFO, "GlutSensorEmulatorDevice::handleButtonChange(): switching to Sensor %i! Resetting emulation to translation along axes X/Z!\n",
				sensorIdx);
	} // if
	else if (buttonIndex ==  switchTransformationTargetButton) {
		useOrientation = !useOrientation;
		useZRotation = false;
		useYAxis = false;
		printd(INFO, "GlutSensorEmulatorDevice::handleButtonChange(): switching to %s!\n",
				(useOrientation ? "rotation emulation around axes X/Y" : "translation emulation along axes X/Z"));
	} // else if
	else if (buttonIndex == switchAxesButton) {
		if (useOrientation) {
			useZRotation = !useZRotation;
			printd(INFO, "GlutSensorEmulatorDevice::handleButtonChange(): switching to rotation emulation around axes %s!\n",
					(useZRotation ? "X/Z" : "X/Y"));
		} // if
		else {
			useYAxis = !useYAxis;
			printd(INFO, "GlutSensorEmulatorDevice::handleButtonChange(): switching to translation emulation along axes %s!\n",
					(useYAxis ? "X/Y" : "X/Z"));
		} // else
	} // else if
} // handleButtonPress

void GlutSensorEmulatorDevice::handleAxesChange(float deltaX, float deltaY) {
	if (useOrientation)
		changeOrientation(deltaX, deltaY);
	else
		changeTranslation(deltaX, deltaY);
} // handleAxesChange

void GlutSensorEmulatorDevice::changeTranslation(float x, float y) {
	// We have to lock the controller here to avoid that a sensor value is
	// read while this method is running
	acquireControllerLock();

	sensorState[sensorIdx].position[0] += x;

	if (useYAxis)
		sensorState[sensorIdx].position[1] += y;
	else
		sensorState[sensorIdx].position[2] -= y;

	releaseControllerLock();
} // changeTranslation

void GlutSensorEmulatorDevice::changeOrientation(float x, float y) {
	gmtl::Quatf quatX, quatY;
	gmtl::AxisAnglef rotationX, rotationY;

	if (useZRotation)
		rotationX = gmtl::AxisAnglef(x, 0, 0, -1);
	else
		rotationX = gmtl::AxisAnglef(-x, 0, 1, 0);

	rotationY = gmtl::AxisAnglef(y, 1, 0, 0);

	gmtl::set(quatX, rotationX);
	gmtl::set(quatY, rotationY);

	// We have to lock the controller here to avoid that a sensor value is
	// read while this method is running
	acquireControllerLock();
	sensorState[sensorIdx].orientation *= quatX;
	sensorState[sensorIdx].orientation *= quatY;
	releaseControllerLock();
} // changeOrientation


InputDevice* GlutSensorEmulatorDeviceFactory::create(std::string className, ArgumentVector* args) {
	if (className != "GlutSensorEmulatorDevice")
		return NULL;

	unsigned int numSensors = GlutSensorEmulatorDevice::DefaultNumberOfSensors;
	unsigned int switchSensorButton = GlutSensorEmulatorDevice::DefaultSwitchSensorButton;
	unsigned int switchTransformationTargetButton =
		GlutSensorEmulatorDevice::DefaultSwitchTransformationTargetButton;
	unsigned int switchAxesButton = GlutSensorEmulatorDevice::DefaultSwitchAxesButton;

	if (args) {
		if (args->keyExists("numberOfSensors"))
			args->get("numberOfSensors", numSensors);
		if (args->keyExists("switchSensorButton"))
			args->get("switchSensorButton", switchSensorButton);
		if (args->keyExists("switchTransformationTargetButton"))
			args->get("switchTransformationTargetButton", switchTransformationTargetButton);
		if (args->keyExists("switchAxesButton"))
			args->get("switchAxesButton", switchAxesButton);
	} // if
	return new GlutSensorEmulatorDevice(numSensors, switchSensorButton,
			switchTransformationTargetButton, switchAxesButton);
} // create
