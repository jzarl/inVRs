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

#ifndef GLUTCHARKEYBOARDDEVICE_H
#define GLUTCHARKEYBOARDDEVICE_H

#include "ControllerManagerSharedLibraryExports.h"
#include "InputDeviceBase.h"

/******************************************************************************
 * InputDevice for mapping the keyboard input to button values. This class can
 * be used in GLUT applications to bind the keyboard to the application as
 * input device. The keys of the keyboard are therefore mapped to buttons. The
 * buttons are identified by the ASCII value of the pressed character (or at
 * least the character value provided by GLUT).
 * Examples:
 * 'a' .. 97
 * 'b' .. 98
 *  ...
 * 'z' .. 122
 */
class CONTROLLERMANAGER_API GlutCharKeyboardDevice : public InputDeviceBase {
public:

	/**
	 * Constructor
	 */
	GlutCharKeyboardDevice();

	/**
	 * Destructor
	 */
	virtual ~GlutCharKeyboardDevice();

	/**
	 * Static GLUT callback when key is pressed.
	 */
	static void cbGlutKeyboard(unsigned char k, int x, int y);

	/**
	 * Static GLUT callback when key is released.
	 */
	static void cbGlutKeyboardUp(unsigned char k, int x, int y);

private:
	/**
	 * Updates the internal button state
	 */
	void setButtonState(unsigned int buttonIndex, int value);

	/**
	 * List of all existing GlutCharKeyboardDevices. This list is needed when
	 * more than one GlutCharKeyboardDevice is used. The GLUT callback methods
	 * call the setButtonState method of each of the device in this list.
	 */
	static std::vector<GlutCharKeyboardDevice*> activeDevices;
}; // GlutCharKeyboardDevice

/******************************************************************************
 * Factory class for GlutCharKeyboardDevices
 */
class GlutCharKeyboardDeviceFactory : public InputDeviceFactory {
public:

	/**
	 * Destructor
	 */
	virtual ~GlutCharKeyboardDeviceFactory() {};

	/**
	 * Creates a new instance of the GlutCharKeyboardDevice if the passed
	 * className matches
	 */
	virtual InputDevice* create(std::string className, ArgumentVector* args = NULL);
}; // GlutCharKeyboardDeviceFactory

#endif
