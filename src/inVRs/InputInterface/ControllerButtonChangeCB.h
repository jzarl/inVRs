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

#ifndef CONTROLLERBUTTONCHANGECB_H_
#define CONTROLLERBUTTONCHANGECB_H_

#include <assert.h>

/******************************************************************************
 * @class AbstractControllerButtonChangeCB
 * @brief Abstract callback class controller button changes
 */
class AbstractControllerButtonChangeCB {
public:

	/**
	 * Empty destructor
	 */
	virtual ~AbstractControllerButtonChangeCB() {
	}

	/**
	 * Activates the callback
	 */
	virtual bool activate() = 0;

	/**
	 * Deactivates the callback
	 */
	virtual bool deactivate() = 0;

	/**
	 * Returns if the callback is currently active
	 */
	virtual bool isActive() const = 0;

private:
	friend class ControllerInterface;

	/**
	 * Calls the stored callback function
	 */
	virtual void call(int buttonIndex, int newButtonValue) const = 0;

	/**
	 * Cleans the controller pointer and deactivates the callback (is called by
	 * the destructor of the controller)
	 */
	virtual void invalidateController() = 0;

}; // AbstractControllerButtonChangeCB

/******************************************************************************
 * @class ControllerButtonChangeCB
 * @brief Callback class for button changes
 */
template<class TClass> class ControllerButtonChangeCB
: public AbstractControllerButtonChangeCB {
public:

	/**
	 * Constructor
	 */
	ControllerButtonChangeCB();

	/**
	 * Constructor setting the source of the callback (controller)
	 */
	explicit ControllerButtonChangeCB(ControllerInterface* controller);

	/**
	 * Constructor setting the source of the callback (controller) as well as
	 * the destination of the callback (object and function which should be
	 * called)
	 */
	explicit ControllerButtonChangeCB(ControllerInterface* controller, TClass* object,
			void(TClass::*function)(int buttonIndex, int newButtonValue));

	/**
	 * Destructor
	 */
	virtual ~ControllerButtonChangeCB();

	/**
	 * Sets the source of the callback (controller)
	 */
	void setSource(ControllerInterface* controller);

	/**
	 * Sets the target of the callback (object and function which should be
	 * called)
	 */
	void setTarget(TClass* object,
			void(TClass::*function)(int buttonIndex, int newButtonValue));

//*********************************************************
// Methods inherited from AbstractControllerButtonChangeCB:
//*********************************************************

	/**
	 * Activates the callback
	 */
	bool activate();

	/**
	 * Deactivates the callback
	 */
	bool deactivate();

	/**
	 * Returns if the callback is active
	 */
	bool isActive() const;

private:

//*********************************************************
// Methods inherited from AbstractControllerButtonChangeCB:
//*********************************************************

	/**
	 * Calls the stored callback function.
	 */
	void call(int buttonIndex, int newButtonValue) const;

	/**
	 * Cleans the controller pointer and deactivates the callback (is called by
	 * the destructor of the controller)
	 */
	void invalidateController();

//*************
// Own methods:
//*************

	// Forbid copy constructor
	ControllerButtonChangeCB(const ControllerButtonChangeCB& src);
	// Forbid copy assignment
	ControllerButtonChangeCB<TClass>& operator=(const ControllerButtonChangeCB<TClass>& rhs);

	bool active;				// stores if the callback is active
	ControllerInterface* controller;		// pointer to the callback source
	TClass* object;				// pointer to the target object

	// pointer to the target function
	void (TClass::*function)(int buttonIndex, int newButtonValue);
}; // ControllerButtonChangeCB

// Now let's include the implementation - since we are writing a template class
// the implementation has to be contained in the header file!
#include "ControllerButtonChangeCB.inl"

#endif /* CONTROLLERBUTTONCHANGECB_H_ */
