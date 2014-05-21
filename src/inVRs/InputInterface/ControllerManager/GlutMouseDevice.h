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

#ifndef GLUTMOUSEDEVICE_H
#define GLUTMOUSEDEVICE_H

#include "ControllerManagerSharedLibraryExports.h"
#include "InputDeviceBase.h"

/**
 * @class GlutMouseDevice
 * @brief Simple glut based mouse device
 *
 * usage:
 * call setWindowSize() to tell the mouse controller the initial window size
 * create one instance of the glut mouse controller
 * call static method cbGlutMouse() in glutMouseFunc() callback
 * call static method cbGlutMouseMove() in glut glutMotionFunc() AND
 * glutPassiveMotionFunc() callback
 * call setWindowSize() whenever the window size changes
 * (i.e. in glutReshapeFunc() callback)
 * @author hbress
 * @author rlander
 */
class CONTROLLERMANAGER_API GlutMouseDevice : public InputDeviceBase {
public:

	/**
	 * Constructor. The passed variable axisReleaseSpeed defines how fast the
	 * axis value is released to zero. If you set this speed to high you may
	 * get bumpy navigation results, if it is too low the axis will drift
	 */
	GlutMouseDevice(float axisReleaseSpeed);

	/**
	 * Destructor
	 */
	virtual ~GlutMouseDevice();

	/**
	 * The method updates the axis value according to the set release speed.
	 * It therefore measures the time between the last mouse position
	 * callback and the current time and linearly interpolates between the
	 * last pointer position and zero taking into accound the speed value.
	 */
	virtual void update();

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

	// Default speed value for axis release
	static const float DefaultAxisReleaseSpeed;
//	static const float DefaultAxisReleaseSpeed = 20.f;

private:

	/**
	 * Sets the button state for the passed index to the value
	 */
	void setButtonState(unsigned int buttonIndex, int value);

	/**
	 * Copes with mouse axis changes. This method is called by the
	 * cbGlutMouseMove method and gets the (normalized) offsets in X and Y
	 * direction of the mouse pointer. Out of this it calculates the new
	 * mouse axis values.
	 */
	void handleAxesChange(float deltaX, float deltaY);

	/**
	 * Resets the axes values to zero. This method is called whenever the
	 * window size is changed
	 */
	void resetAxes();

	float axisWriteTimeStamp[2];	// Time when the axis values were written
	float oldPos[2];				// axis values which were lastly written
	bool oldPosRead[2];				// used to ensure that written axis values are read
	float axisReleaseSpeed;			// speed of axis release (to zero)

	static float winWidth;			// width of the window
	static float winHeight;			// height of the window
	static float screenWidth;		// width of the screen
	static float screenHeight;		// height of the screen
	static bool grabMouse;			// defines whether the mouse is grabbed or not

	// List of all GlutMouseDevices (needed for static callbacks)
	static std::vector<GlutMouseDevice*> activeDevices;
}; // GlutMouseDevice

/******************************************************************************
 * Factory class for GlutMouseDevices
 */
class GlutMouseDeviceFactory : public InputDeviceFactory {
public:

	/**
	 * Empty Destructor
	 */
	virtual ~GlutMouseDeviceFactory() {};

	/**
	 * Creates a new instance of a GlutMouseDevice if the matching className
	 * is passed
	 */
	virtual InputDevice* create(std::string className, ArgumentVector* args = NULL);
}; // GlutMouseDeviceFactory

#endif
