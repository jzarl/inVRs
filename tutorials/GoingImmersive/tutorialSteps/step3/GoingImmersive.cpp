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

//----------------------------------------------------------------------------//
// Snippet-2-1 - BEGIN                                                        //
//----------------------------------------------------------------------------//
#ifdef WITH_VRPN_SUPPORT
#include <inVRs/tools/libraries/VrpnDevice/VrpnDevice.h>
#endif
#ifdef WITH_TRACKD_SUPPORT
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
// Snippet-4-1                                                                //
//----------------------------------------------------------------------------//

public:
	GoingImmersive() {
		defaultConfigFile = "config/general.xml";

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
		NodePtr scene = sgIF->getNodePtr();

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
// Snippet-4-2                                                                //
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
// Snippet-4-4                                                                //
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
// Snippet-4-3                                                                //
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
	void initInputInterfaceCallback(ModuleInterface* moduleInterface) {
#ifdef WITH_VRPN_SUPPORT
		if (moduleInterface->getName() == "ControllerManager") {
			ControllerManager* contInt = dynamic_cast<ControllerManager*>(moduleInterface);
			assert(contInt);
			contInt->registerInputDeviceFactory(new VrpnDeviceFactory);
		} // if
#endif
#ifdef WITH_TRACKD_SUPPORT
		if (moduleInterface->getName() == "ControllerManager") {
			ControllerManager* contInt = dynamic_cast<ControllerManager*>(moduleInterface);
			assert(contInt);
			contInt->registerInputDeviceFactory(new TrackdDeviceFactory);
		} // if
#endif
	} // initInterfaceCallback
//----------------------------------------------------------------------------//
// Snippet-2-2 - END                                                          //
//----------------------------------------------------------------------------//

}; // GoingImmersive

int main(int argc, char** argv) {
	GoingImmersive* app = new GoingImmersive();

	if (!app->start(argc, argv)) {
		printd(ERROR, "Error occured during startup!\n");
		delete app;
		return -1;
	} // if

	delete app;
	return 0;
} // main
