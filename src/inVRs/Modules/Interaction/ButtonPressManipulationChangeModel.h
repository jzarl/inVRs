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

#ifndef _BUTTONPRESSSTATETRANSITIONMODEL_H
#define _BUTTONPRESSSTATETRANSITIONMODEL_H

#include "ManipulationChangeModel.h"
#include <inVRs/InputInterface/ControllerInterface.h>
#include <inVRs/InputInterface/ControllerManagerInterface.h>
#include <inVRs/InputInterface/ControllerButtonChangeCB.h>

/*****************************************************************************
 * @class ButtonStateTransitionModel
 */
class INVRS_INTERACTION_API ButtonPressManipulationChangeModel : public ManipulationChangeModel {
public:
	ButtonPressManipulationChangeModel(int buttonIndex = 0, int buttonValue = 1);

	virtual bool changeState();
	virtual void enterTransitionSource();

private:
	typedef ControllerButtonChangeCB<ButtonPressManipulationChangeModel> ControllerButtonCallback;

	void buttonPressed(int buttonIndex, int newButtonValue);

	int buttonIndex;
	int buttonValue;

	bool inTransitionSource;
	bool changeStateButtonPressed;
	ControllerInterface* control;
	ControllerButtonCallback callback;
};

#endif
