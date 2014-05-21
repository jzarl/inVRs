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

#ifndef _CONTROLLERMANAGER_H
#define _CONTROLLERMANAGER_H

#include <string>
#include <memory>

#include "ControllerManagerSharedLibraryExports.h"
#include "InputDevice.h"
#include "Controller.h"
#include "ButtonCorrection.h"
#include "AxisCorrection.h"
#include "SensorCorrection.h"

#include "../ControllerManagerInterface.h"
#include "../../SystemCore/XmlConfigurationLoader.h"


/******************************************************************************
 * Class for managing the abstract Controller which is used for application
 * input.
 */
class CONTROLLERMANAGER_API ControllerManager : public ControllerManagerInterface {
public:

	/**
	 * Constructor
	 */
	ControllerManager();

	/**
	 * Destructor
	 */
	virtual ~ControllerManager();

//****************************************
// Methods inherited from ModuleInterface:
//****************************************

	/**
	 * The method cleans up the ControllerManager. It therefore deletes all
	 * registered Controller-factories.
	 */
	void cleanup();

	/**
	 * Reads the configuration for the Controller from the xml file with the
	 * passed URL.
	 */
	bool loadConfig(std::string configFile);

	/**
	 * Returns the name of the Module (ControllerManager)
	 */
	std::string getName();

//***************************************************
// Methods inherited from ControllerManagerInterface:
//***************************************************

	/**
	 * Returns the pointer to the Controller managed by this class.
	 */
	ControllerInterface* getController();

//*************
// Own methods:
//*************

	/**
	 * Registers a new factory-class for a single or multiple InputDevices
	 */
	void registerInputDeviceFactory(InputDeviceFactory* factory);

private:

	/**
	 * Tries to obtain an InputDevice with the passed name by calling all
	 * factory classes.
	 */
	InputDevice* getInputDevice(std::string deviceName, ArgumentVector* args);

	/**
	 * Parses the element <device> from the XML file
	 */
	bool parseDeviceElement(const XmlElement* deviceElement, int deviceIndex,
			Controller* newController);

	/**
	 * Parses the element <button> from the XML file
	 */
	bool parseButtonElement(const XmlElement* buttonElement, int deviceIndex,
			std::map<int, DeviceIndexMapping>& buttonIndexMapping,
			std::map<int, ButtonCorrection>& buttonCorrections);
	/**
	 * Parses the element <axis> from the XML file
	 */
	bool parseAxisElement(const XmlElement* axisElement, int deviceIndex,
			std::map<int, DeviceIndexMapping>& axisIndexMapping,
			std::map<int, AxisCorrection>& axisCorrections);

	/**
	 * Parses the element <sensor> from the XML file
	 */
	bool parseSensorElement(const XmlElement* sensorElement, int deviceIndex,
			std::map<int, DeviceIndexMapping>& sensorIndexMapping,
			std::map<int, SensorCorrection>& sensorCorrections);

	/**
	 * Parses the device index mapping entries
	 */
	bool parseDeviceIndexMapping(const XmlElement* element, int deviceIndex,
			std::map<int, DeviceIndexMapping>& indexMapping, std::string type,
			int& indexInController);

	/// Stores if the cleanup-method was already called
	bool isCleanedUp;
	/// A pointer to the used controller
	std::auto_ptr<Controller> controller;
	/// The list of InputDevice-factories
	std::vector<InputDeviceFactory*> factoryList;

//*****************************************************************************
// Configuration loading
//*****************************************************************************
private:
	static XmlConfigurationLoader xmlConfigLoader;

	class ConverterToV1_0a4 : public XmlConfigurationConverter {
	public:
		ConverterToV1_0a4();
		virtual bool convert(XmlDocument* document, const Version& version, std::string configFile);
	}; // ConverterToV1_0a4

}; // ControllerManager

#endif
