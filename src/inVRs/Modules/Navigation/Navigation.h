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

#ifndef _NAVIGATION_H
#define _NAVIGATION_H

#include <gmtl/Quat.h>
#include <gmtl/Matrix.h>

#include "NavigationSharedLibraryExports.h"
#include "OrientationModels.h"
#include "SpeedModels.h"
#include "TranslationModels.h"
#include "NavigationEvents.h"
#include <inVRs/SystemCore/XmlConfigurationLoader.h>
#include <inVRs/SystemCore/EventManager/EventManager.h>
#include <inVRs/SystemCore/ComponentInterfaces/NavigationInterface.h>

#ifdef WIN32
#pragma warning( disable : 4507 )
#endif

/*******************************************************************************
 * @class Navigation
 * @brief This is the main class of the Navigation module.
 *
 * This class makes the whole navigate calculation based on different models
 * set by calling the registered Orientation-, Translation-,and Speedmodels)
 */
class INVRS_NAVIGATION_API Navigation : public NavigationInterface {
public:
	Navigation();
	virtual ~Navigation();

	/**
	 * The loadConfig method parses a passed String, which should contain XML
	 * data. The configuration contains the names for speed-, orientation-, and
	 * translationmodel, which are subesequently parsed and evaluated.
	 * @param configFile the XML configuration for the navigation models
	 * @return in case the parsing does not detect errors in the configuration
	 * true is returned, if errors are detected false is returned
	 */
	virtual bool loadConfig(std::string configFile);
	virtual void cleanup();
	virtual std::string getName();
	// inherited from ModuleInterface
	virtual NavigationInfo navigate(float dt);

	void registerSpeedModelFactory(SpeedModelFactory* factory);
	void registerOrientationModelFactory(OrientationModelFactory* factory);
	void registerTranslationModelFactory(TranslationModelFactory* factory);

protected:
	std::vector<SpeedModelFactory*> speedModelFactories;
	std::vector<OrientationModelFactory*> orientationModelFactories;
	std::vector<TranslationModelFactory*> translationModelFactories;

	std::vector<SpeedModelFactory*> navProvidedSpeedModelFactories;
	std::vector<OrientationModelFactory*> navProvidedOrientationModelFactories;
	std::vector<TranslationModelFactory*> navProvidedTranslationModelFactories;

	SpeedModel* getSpeedModel(std::string name, ArgumentVector* argVec);
	OrientationModel* getOrientationModel(std::string name, ArgumentVector* argVec);
	TranslationModel* getTranslationModel(std::string name, ArgumentVector* argVec);

	ControllerInterface* controller;
	EventPipe* incomingEvents;

	TranslationModel* translationModel;
	OrientationModel* orientationModel;
	SpeedModel* speedModel;
	bool suspendNavigation;

	// 	User* localUser;

	friend class NavigationPauseEvent;
	friend class NavigationResumeEvent;

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

};

#endif // _NAVIGATION_H
