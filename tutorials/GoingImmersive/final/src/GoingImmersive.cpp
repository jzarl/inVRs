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

#include <OpenSGApplicationBase/OpenSGApplicationBase.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/InputInterface/ControllerManager/ControllerManager.h>
#include "inVRs/SystemCore/DebugOutput.h"

//----------------------------------------------------------------------------//
// Snippet-2-1 - BEGIN                                                        //
//----------------------------------------------------------------------------//
#ifdef ENABLE_VRPN_SUPPORT
#include <inVRs/tools/libraries/VrpnDevice/VrpnDevice.h>
#endif
#ifdef ENABLE_TRACKD_SUPPORT
#include <inVRs/tools/libraries/TrackdDevice/TrackdDevice.h>
#endif
//----------------------------------------------------------------------------//
// Snippet-2-1 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-3-1 - BEGIN                                                        //
//----------------------------------------------------------------------------//
#include <inVRs/tools/libraries/AvataraWrapper/AvataraAvatar.h>
//----------------------------------------------------------------------------//
// Snippet-3-1 - END                                                          //
//----------------------------------------------------------------------------//

OSG_USING_NAMESPACE

class GoingImmersive: public OpenSGApplicationBase {

private:
	std::string defaultConfigFile;		// config file

//----------------------------------------------------------------------------//
// Snippet-3-3 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	AvatarInterface* avatar;
	gmtl::Vec3f COORDINATE_SYSTEM_CENTER;
//----------------------------------------------------------------------------//
// Snippet-3-3 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-4-1 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	int numberOfSensors;
//----------------------------------------------------------------------------//
// Snippet-4-1 - END                                                          //
//----------------------------------------------------------------------------//

public:
	GoingImmersive() {
		defaultConfigFile = "final/config/general.xml";

//----------------------------------------------------------------------------//
// Snippet-3-4 - BEGIN                                                        //
//----------------------------------------------------------------------------//
		avatar = NULL;
		COORDINATE_SYSTEM_CENTER = gmtl::Vec3f(5, 1, 5);
//----------------------------------------------------------------------------//
// Snippet-3-4 - END                                                          //
//----------------------------------------------------------------------------//

	} // constructor

	~GoingImmersive() {
		globalCleanup();
	} // destructor

	std::string getConfigFile(const CommandLineArgumentWrapper& args) {
		if (args.containOption("config"))
			return args.getOptionValue("config");
		else
			return defaultConfigFile;
	} // getConfigFile

	bool initialize(const CommandLineArgumentWrapper& args) {
		OpenSGSceneGraphInterface* sgIF =
			dynamic_cast<OpenSGSceneGraphInterface*>(sceneGraphInterface);
		// must exist because it is created by the OutputInterface
		if (!sgIF) {
			printd(ERROR, "GoingImmersive::initialize(): Unable to obtain SceneGraphInterface!\n");
			return false;
		} // if

		// obtain the scene node from the SceneGraphInterface
		NodeRecPtr scene = sgIF->getNodePtr();

		// set root node to the responsible SceneManager (managed by OpenSGApplicationBase)
		setRootNode(scene);

		// set our transformation to the start transformation
		TransformationData startTrans =
			WorldDatabase::getEnvironmentWithId(1)->getStartTransformation(0);
		localUser->setNavigatedTransformation(startTrans);

//----------------------------------------------------------------------------//
// Snippet-3-5 - BEGIN                                                        //
//----------------------------------------------------------------------------//
		avatar = localUser->getAvatar();
		if (!avatar) {
			printd(ERROR,
					"GoingImmersive::initialize(): unable to obtain avatar! Check UserDatabase configuration!\n");
			return false;
		} // if
//----------------------------------------------------------------------------//
// Snippet-3-5 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-4-2 - BEGIN                                                        //
//----------------------------------------------------------------------------//
		ControllerInterface* controller = controllerManager->getController();
		if (!controller) {
			printd(ERROR,
					"GoingImmersive::initialize(): unable to obtain controller! Check ControllerManager configuration!\n");
			return false;
		} // if
		numberOfSensors = controller->getNumberOfSensors();

		// create an instance of the coordinate system entity (ID=10) for each
		// sensor in the environment with ID 1
		for (int i=0; i < numberOfSensors; i++) {
			WorldDatabase::createEntity(10, 1);
		} // for
//----------------------------------------------------------------------------//
// Snippet-4-2 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-1-1 - BEGIN                                                        //
//----------------------------------------------------------------------------//
		setBackgroundImage("background_128.png");
//----------------------------------------------------------------------------//
// Snippet-1-1 - END                                                          //
//----------------------------------------------------------------------------//

		return true;
	} // initialize

	void display(float dt) {

//----------------------------------------------------------------------------//
// Snippet-3-7 - BEGIN                                                        //
//----------------------------------------------------------------------------//
		updateAvatar();
//----------------------------------------------------------------------------//
// Snippet-3-7 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-4-4 - BEGIN                                                        //
//----------------------------------------------------------------------------//
		updateCoordinateSystems();
//----------------------------------------------------------------------------//
// Snippet-4-4 - END                                                          //
//----------------------------------------------------------------------------//

	} // display

//----------------------------------------------------------------------------//
// Snippet-3-6 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	void updateAvatar() {
		TransformationData trackedUserTrans = localUser->getTrackedUserTransformation();
		trackedUserTrans.position += COORDINATE_SYSTEM_CENTER;
		avatar->setTransformation(trackedUserTrans);
	} // updateAvatar
//----------------------------------------------------------------------------//
// Snippet-3-6 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-4-3 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	void updateCoordinateSystems() {
		TransformationData trackedUserTrans, sensorTrans;
		// get the list of coordinate system entities (entity type ID = 10)
		EntityType* coordinateSystemType = WorldDatabase::getEntityTypeWithId(10);
		const std::vector<Entity*>& entities = coordinateSystemType->getInstanceList();

		// map the tracked user transformation to the first entity
		if (entities.size() > 0) {
			trackedUserTrans = localUser->getTrackedUserTransformation();
			trackedUserTrans.position += COORDINATE_SYSTEM_CENTER;
			entities[0]->setEnvironmentTransformation(trackedUserTrans);
		} // if
		// map the tracked sensor transformations to the remaining entities
		for (int i=0; i < numberOfSensors; i++) {
			sensorTrans = localUser->getTrackedSensorTransformation(i);
			sensorTrans.position += COORDINATE_SYSTEM_CENTER;
			if (i+1 < entities.size()) {
				entities[i+1]->setEnvironmentTransformation(sensorTrans);
			} // if
		} // for
	} // updateCoordinateSystems
//----------------------------------------------------------------------------//
// Snippet-4-3 - END                                                          //
//----------------------------------------------------------------------------//

	void cleanup() {

	} // cleanup

//----------------------------------------------------------------------------//
// Snippet-3-2 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	void initCoreComponentCallback(CoreComponents comp) {
		// register factory for avatara avatars
		if (comp == USERDATABASE) {
			WorldDatabase::registerAvatarFactory (new AvataraAvatarFactory());
		} // else if
	} // initCoreComponentCallback
//----------------------------------------------------------------------------//
// Snippet-3-2 - END                                                          //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Snippet-2-2 - BEGIN                                                        //
//----------------------------------------------------------------------------//
	void initInputInterfaceCallback(ModuleInterface* moduleInterface)
	{
		if (moduleInterface->getName() == "ControllerManager")
		{
			ControllerManager* contInt = dynamic_cast<ControllerManager*>(moduleInterface);
			assert(contInt);
#ifdef ENABLE_VRPN_SUPPORT
			contInt->registerInputDeviceFactory(new VrpnDeviceFactory);
#endif
#ifdef ENABLE_TRACKD_SUPPORT
			contInt->registerInputDeviceFactory(new TrackdDeviceFactory);
#endif
		}
	}
//----------------------------------------------------------------------------//
// Snippet-2-2 - END                                                          //
//----------------------------------------------------------------------------//

};


int main(int argc, char** argv)
{
#if WIN32
	OSG::preloadSharedObject("OSGFileIO");
	OSG::preloadSharedObject("OSGImageFileIO");
#endif

	GoingImmersive app;

	if (!app.start(argc, argv))
	{
		printd(ERROR, "Error occured during startup!\n");
		return EXIT_FAILURE;
	}
}
