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
#include "ButtonPressManipulationChangeModel.h"

#include <assert.h>

#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/InputInterface/InputInterface.h>
#include <inVRs/SystemCore/DebugOutput.h>

ButtonPressManipulationChangeModel::ButtonPressManipulationChangeModel(int buttonIndex, int buttonValue) :
	buttonIndex(buttonIndex),
	buttonValue(buttonValue),
	inTransitionSource(false),
	changeStateButtonPressed(false),
	control(NULL),
	callback() {
}

void ButtonPressManipulationChangeModel::enterTransitionSource() {
	ControllerManagerInterface* contIntf;

	inTransitionSource = true;
	changeStateButtonPressed = false;

	if (!control) {
		// thats the first time enterTransitionSource is invoked
		// register callback

		contIntf = (ControllerManagerInterface*)InputInterface::getModuleByName("ControllerManager");
		if (!contIntf) {
			printd(
					ERROR,
					"ButtonPressStateTransitionModel::enterTransitionSource(): cannot find a suitable ControllerManager\n");
			return;
		} // if
		control = (ControllerInterface*)contIntf->getController();
		assert(control);
		callback.setSource(control);
		callback.setTarget(this, &ButtonPressManipulationChangeModel::buttonPressed);
		callback.activate();
	} // if
} // enterTransitionSource

bool ButtonPressManipulationChangeModel::changeState() {
	assert(inTransitionSource);

	if (!changeStateButtonPressed)
		return false;

	inTransitionSource = false; // assuming state change will take place if true is returned
	changeStateButtonPressed = false;

	return true;
}

void ButtonPressManipulationChangeModel::buttonPressed(int buttonIndex, int newButtonValue) {
	if (!inTransitionSource)
		return;

	changeStateButtonPressed = ((buttonIndex == this->buttonIndex)
			&& (newButtonValue == this->buttonValue));
} // buttonPressed

