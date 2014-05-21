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

#ifndef GLUTSENSORKEYBOARDDEVICE_H
#define GLUTSENSORKEYBOARDDEVICE_H

#include "InputDeviceBase.h"

/******************************************************************************
 * InputDevice for emulating the input of multiple sensors. This class can be
 * used to emulate one ore more sensors via keyboard and mouse input. The
 * mouse motion is therefore mapped to the sensors' position and orientation
 * values. The sensors and the transformation part which should be changed
 * (position/orientation) can be switched by keyboard keys or mouse button
 * clicks (depending on the configuration).
 * The button indices in this device are defined as:
 * 0-255   .. Keyboard keys (ASCII values)
 * 256-258 .. Mouse buttons (LMB, MMB, RMB)
 */
class CONTROLLERMANAGER_API GlutSensorEmulatorDevice : public InputDeviceBase {
public:

	/**
	 * Constructor
	 */
	GlutSensorEmulatorDevice(unsigned int numSensors, unsigned int switchSensorButton,
			unsigned int switchTransformationTargetButton, unsigned int switchAxesButton);

	/**
	 * Destructor
	 */
	virtual ~GlutSensorEmulatorDevice();

	/**
	 * Static GLUT callback when key is pressed
	 */
	static void cbGlutKeyboard(unsigned char k, int x, int y);

	/**
	 * Static GLUT callback for mouse button click (or release)
	 */
	static void cbGlutMouse(int button, int state, int x, int y);

	/**
	 * Static GLUT callback for mouse motion
	 */
	static void cbGlutMouseMove(int x, int y);

	/**
	 * Static GLUT callback for resizing the window size
	 */
	static void setWindowSize(int width, int height);

	/**
	 * Defines wether the mouse is grabbed in the window or not. If the mouse
	 * is grabbed it won't be visible and will be fixed inside the window. By
	 * deactivating mouse grabbing the mouse gets visible again and can be
	 * moved freely again.
	 */
	static void setMouseGrabbing(bool on);

	/// Default button for switching between sensors (256 = LMB)
	static const unsigned int DefaultSwitchSensorButton = 256;
	/// Default button for switching between transformation targets (257 = MMB)
	static const unsigned int DefaultSwitchTransformationTargetButton = 257;
	/// Default button for switching between axes (258 = MMB)
	static const unsigned int DefaultSwitchAxesButton = 258;
	/// Default number of sensors
	static const unsigned int DefaultNumberOfSensors = 2;

private:
	/**
	 * Handles a pressed button (either keyboard or mouse)
	 */
	void handleButtonPress(unsigned int buttonIndex);

	/**
	 * Handles the changes of the mouse axes
	 */
	void handleAxesChange(float deltaX, float deltaY);

	/**
	 * Changes the translation of the current sensor
	 */
	void changeTranslation(float x, float y);

	/**
	 * Changes the orientation of the current sensor
	 */
	void changeOrientation(float x, float y);

	static float winWidth;			// width of the window
	static float winHeight;			// height of the window
	static float screenWidth;		// width of the screen
	static float screenHeight;		// height of the screen
	static bool grabMouse;			// defines whether the mouse is grabbed or not
	// List of all GlutSensorEmulatorDevices (needed for static callbacks)
	static std::vector<GlutSensorEmulatorDevice*> activeDevices;

	unsigned int switchSensorButton;				// button-index for switching between sensors
	unsigned int switchTransformationTargetButton;	// button-index for switching between transformation types
	unsigned int switchAxesButton;					// button-index for switching between axes

	unsigned int sensorIdx;			// current sensor index
	bool useYAxis;					// switch for linear axes (X/Z, X/Y)
	bool useZRotation;				// switch for rotation axes (X/Y, X/Z)
	bool useOrientation;			// switch between translation and orientation
}; // GlutSensorEmulatorDevice

/******************************************************************************
 * Factory for GlutSensorEmulatorDevices. The class creates and a new instance
 * of the GlutSensorEmulatorDevice with the passed xml-arguments. Following
 * arguments are valid:
 * numberOfSensors    .. defines the number of emulated sensors
 * switchSensorButton .. index of the button for switching between sensors
 * switchTransformationTargetButton .. index of the button for switching
 * 		between translation and orientation
 * switchAxesButton   .. index of the button for switching between axes
 *
 * For default values of the above arguments ses the static const members of
 * the GlutSensorEmulatorDevice.
 */
class GlutSensorEmulatorDeviceFactory : public InputDeviceFactory {
public:
	/**
	 * Empty destructor
	 */
	virtual ~GlutSensorEmulatorDeviceFactory() {};

	/**
	 * Creates a new instance of the GlutSensorEmulatorDevice if the passed
	 * className matches
	 */
	virtual InputDevice* create(std::string className, ArgumentVector* args = NULL);
}; // GlutSensorEmulatorDeviceFactory

#endif
