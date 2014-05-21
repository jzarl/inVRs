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

template<class TClass>
ControllerButtonChangeCB<TClass>::ControllerButtonChangeCB() :
		active(false),
		controller(NULL),
		object(NULL),
		function(NULL) {
} // ControllerButtonChangeCB

template<class TClass>
ControllerButtonChangeCB<TClass>::ControllerButtonChangeCB(ControllerInterface* controller) :
		active(false),
		controller(controller),
		object(NULL),
		function(NULL) {
} // ControllerButtonChangeCB

template<class TClass>
ControllerButtonChangeCB<TClass>::ControllerButtonChangeCB(ControllerInterface* controller,
		TClass* object,	void(TClass::*function)(int buttonIndex, int newButtonValue)) :
		active(false),
		controller(controller),
		object(object),
		function(function) {
} // ControllerButtonChangeCB

template<class TClass>
ControllerButtonChangeCB<TClass>::~ControllerButtonChangeCB() {
	if (active) {
		deactivate();
	} // if
} // ~ControllerButtonChangeCB

template<class TClass>
void ControllerButtonChangeCB<TClass>::setSource(ControllerInterface* controller) {
	if (this->controller == controller)
		return;

	bool wasActive = active;
	if (this->active) {
		assert(this->controller);
		bool success = deactivate();
		assert(success);
	} // if
	this->controller = controller;

	if (wasActive) {
		bool success = activate();
		assert(success);
	} // if
} // setSource

template<class TClass>
void ControllerButtonChangeCB<TClass>::setTarget(TClass* object,
		void(TClass::*function)(int buttonIndex, int newButtonValue)) {
	if (object == this->object && function == this->function)
		return;

	bool wasActive = active;
	if (active) {
		assert(this->controller);
		bool success = deactivate();
		assert(success);
	} // if
	this->object = object;
	this->function = function;

	if (wasActive) {
		bool success = activate();
		assert(success);
	} // if
} // setTarget

template<class TClass>
bool ControllerButtonChangeCB<TClass>::activate() {
	if (!controller || !object || !function)
		return false;

	if (!active) {
		active = true;
		controller->registerButtonChangeCallback(this);
	} // if
	return true;
} // activate

template<class TClass>
bool ControllerButtonChangeCB<TClass>::deactivate() {
	if (!controller || !object || !function)
		return false;

	if (active) {
		active = false;
		controller->unregisterButtonChangeCallback(this);
	} // if
	return true;
} // deactivate

template<class TClass>
bool ControllerButtonChangeCB<TClass>::isActive() const {
	return active;
} // isActive

template<class TClass>
void ControllerButtonChangeCB<TClass>::call(int buttonIndex, int newButtonValue) const {
	if (object != NULL && function != NULL)
		(*object.*function)(buttonIndex, newButtonValue);
} // call

template<class TClass>
void ControllerButtonChangeCB<TClass>::invalidateController() {
	this->controller = NULL;
	active = false;
} // cleanupController
