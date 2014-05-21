#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>
#include <inVRs/InputInterface/ControllerManager/ControllerManager.h>
#include <inVRs/InputInterface/ControllerManagerInterface.h>
#include <inVRs/tools/libraries/OpenSGApplicationBase/OpenSGApplicationBase.h>
#include <inVRs/tools/libraries/Skybox/Skybox.h>
#include <inVRs/tools/libraries/HeightMap/HeightMapModifier.h>
#include <inVRs/tools/libraries/HeightMap/HeightMapManager.h>
#include <inVRs/tools/libraries/CollisionMap/CheckCollisionModifier.h>
#include <inVRs/tools/libraries/AvataraWrapper/AvataraAvatar.h>
#ifdef ENABLE_TRACKD_SUPPORT
#include <inVRs/tools/libraries/TrackdDevice/TrackdDevice.h>
#endif
#ifdef ENABLE_VRPN_SUPPORT
#include <inVRs/tools/libraries/VrpnDevice/VrpnDevice.h>
#endif

OSG_USING_NAMESPACE

class MedievalTownAppBase : public OpenSGApplicationBase
{
private:
	struct CoordinateSystems {
		ModelInterface* hand;
		ModelInterface* head;
		ModelInterface* navigated;
		ModelInterface* user;
		ModelInterface* cursor;
	};

	std::string defaultConfigFile;		// config file
	Skybox skybox;				// scene surroundings
	float windMillSpeed;		// the rotational speed of a windmill
	std::vector<CoordinateSystems> userCoordinateSystems;
	OpenSGSceneGraphInterface* sgIF;
	bool showUserCoordinateSystems;

public:
	MedievalTownAppBase() :
	windMillSpeed(0),
	sgIF(NULL),
	showUserCoordinateSystems(false)
	{
		defaultConfigFile = "config/general.xml";
	} // MedievalTownAppBase

	~MedievalTownAppBase() {
		globalCleanup();
	} // ~MedievalTownAppBase

	std::string getConfigFile(const CommandLineArgumentWrapper& args) {
		if (args.containOption("config"))
			return args.getOptionValue("config");
		else
			return defaultConfigFile;
	} // getConfigFile

	bool initialize(const CommandLineArgumentWrapper& args) {
		// scale world from physical units (centimeter) to world units (meters)
		setPhysicalToWorldScale(0.011);

		// generate or load and configure height maps of the used tiles
		HeightMapManager::generateTileHeightMaps();

		// generate and configure the SkyBox
		std::string skyPath = Configuration::getPath("Skybox");
		skybox.init(5,5,5, 1000, (skyPath+"lostatseaday/lostatseaday_dn.jpg").c_str(),
			(skyPath+"lostatseaday/lostatseaday_up.jpg").c_str(),
			(skyPath+"lostatseaday/lostatseaday_ft.jpg").c_str(),
			(skyPath+"lostatseaday/lostatseaday_bk.jpg").c_str(),
			(skyPath+"lostatseaday/lostatseaday_rt.jpg").c_str(),
			(skyPath+"lostatseaday/lostatseaday_lf.jpg").c_str());

		sgIF = dynamic_cast<OpenSGSceneGraphInterface*>(sceneGraphInterface);
		// must exist because it is created in OpenSGApplicationBase
		assert (sgIF);

		NodeRefPtr scene = sgIF->getNodePtr();

		NodeRefPtr root = Node::create();
		root->setCore(Group::create());
		root->addChild(scene);
		// add the SkyBox to the scene
		root->addChild(skybox.getNodePtr());

		// set to root node to the responsible SceneManager (managed by OpenSGApplicationBase)
		setRootNode(root);

		AvatarInterface* avatar = localUser->getAvatar();
		assert (avatar);
		avatar->showAvatar(false);

		// set our transformation to the start transformation
		TransformationData startTrans =
			WorldDatabase::getEnvironmentWithId(1)->getStartTransformation(0);
		localUser->setNavigatedTransformation(startTrans);

		setNearClippingPlane(0.1);

		// try to connect to network first command line argument is {hostname|IP}:port
		if (args.getNumberOfArguments() > 1) {
			printf("Trying to connect to %s\n", args.getArgument(1).c_str());
			networkModule->connect(args.getArgument(1));
		} // if
		return true;
	} // init

	void initCoreComponentCallback(CoreComponents comp) {
		// register factory for HeightMapModifier as soon as the
		// TransformationManager is initialized
		if (comp == TRANSFORMATIONMANAGER) {
			TransformationManager::registerModifierFactory
				(new HeightMapModifierFactory());
		// register factory for CheckCollisionModifier
			TransformationManager::registerModifierFactory
				(new CheckCollisionModifierFactory);
		} // if
		else if (comp == USERDATABASE) {
			WorldDatabase::registerAvatarFactory (new AvataraAvatarFactory());
		} // else if
	} // initCoreComponentCallback

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

	void display(float dt) {
		skybox.setupRender(activeCamera->getPosition()); // attach the SkyBox to the camera

		UserDatabase::updateCursors(dt);

		if (controllerManager->getController()->getButtonValue(2)) { // the right mouse button is pressed
			windMillSpeed += dt*0.5;	         // increase speed of the windmill
			if (windMillSpeed > 2*M_PI) {
				windMillSpeed = 2*M_PI;
			}
		} else if (windMillSpeed > 0) {	     // pressing mouse button stopped
			windMillSpeed -= dt*0.5;	         // decrease speed of windmill
		} else if (windMillSpeed < 0) {
			windMillSpeed = 0;
		} // else if
		if (windMillSpeed > 0) {	          // rotate sails
			// retrieve the windmill entity
			Entity* windMill = WorldDatabase::getEntityWithEnvironmentId(1, 27);
			ModelInterface* windMillModel = windMill->getVisualRepresentation();
				// retrieve the windmill's sails
			SceneGraphNodeInterface* sceneGraphNode =
				windMillModel->getSubNodeByName("Sails");
				// make sure this node is a transformation node
			assert(sceneGraphNode->getNodeType() ==
				SceneGraphNodeInterface::TRANSFORMATION_NODE);
			TransformationSceneGraphNodeInterface* transNode =
				dynamic_cast<TransformationSceneGraphNodeInterface*>(sceneGraphNode);
			assert(transNode);
				// rotate the sails
			TransformationData trans = transNode->getTransformation();
			gmtl::AxisAnglef axisAngle(windMillSpeed*dt, 0, 0, 1);
			gmtl::Quatf rotation;
			gmtl::set(rotation, axisAngle);
			trans.orientation *= rotation;
			transNode->setTransformation(trans);
		} // if

		updateUserCoordinateSystems();
	} // display

	void cleanup() {}

	void updateUserCoordinateSystems() {
		TransformationData identityTrans = identityTransformation();
		int numRemoteUsers = UserDatabase::getNumberOfRemoteUsers();
		if (numRemoteUsers > userCoordinateSystems.size()) {
			for (int i = userCoordinateSystems.size(); i < numRemoteUsers; i++) {
				CoordinateSystems coords;
				coords.hand = sgIF->loadModel("VRML97", Configuration::getPath("Entities")+"coordinateSystem.wrl");
				assert(coords.hand);
//				coords.hand->setDeepCloneMode(true);
//				coords.head = coords.hand->clone();
//				coords.navigated = coords.hand->clone();
//				coords.user = coords.hand->clone();
				coords.head = sgIF->loadModel("VRML97", Configuration::getPath("Entities")+"coordinateSystem.wrl");
				coords.navigated = sgIF->loadModel("VRML97", Configuration::getPath("Entities")+"coordinateSystem.wrl");
				coords.user = sgIF->loadModel("VRML97", Configuration::getPath("Entities")+"coordinateSystem.wrl");
				coords.cursor = sgIF->loadModel("VRML97", Configuration::getPath("Entities")+"coordinateSystem.wrl");
				sgIF->attachModel(coords.hand, identityTrans);
				sgIF->attachModel(coords.head, identityTrans);
				sgIF->attachModel(coords.navigated, identityTrans);
				sgIF->attachModel(coords.user, identityTrans);
				sgIF->attachModel(coords.cursor, identityTrans);
				userCoordinateSystems.push_back(coords);
			} // for
		} // if
		else if (numRemoteUsers < userCoordinateSystems.size()) {
			for (int i = userCoordinateSystems.size()-1; i >= numRemoteUsers; i--) {
				CoordinateSystems &coords = userCoordinateSystems[i];
				sgIF->detachModel(coords.hand);
				sgIF->detachModel(coords.head);
				sgIF->detachModel(coords.navigated);
				sgIF->detachModel(coords.user);
				sgIF->detachModel(coords.cursor);
				delete coords.hand;
				delete coords.head;
				delete coords.navigated;
				delete coords.user;
				delete coords.cursor;
				userCoordinateSystems.pop_back();
			} // for
		} // else if

		for (int i=0; i < userCoordinateSystems.size(); i++) {
			User* remoteUser = UserDatabase::getRemoteUserByIndex(i);
			TransformationData handTrans = remoteUser->getWorldHandTransformation();
			handTrans.scale = gmtl::Vec3f(0.5, 0.5, 0.5);
			userCoordinateSystems[i].hand->setModelTransformation(handTrans);
			TransformationData headTrans = remoteUser->getWorldHeadTransformation();
			headTrans.scale = gmtl::Vec3f(0.5, 0.5, 0.5);
			userCoordinateSystems[i].head->setModelTransformation(headTrans);
			TransformationData navigatedTrans = remoteUser->getNavigatedTransformation();
			navigatedTrans.scale = gmtl::Vec3f(0.5, 0.5, 0.5);
			userCoordinateSystems[i].navigated->setModelTransformation(navigatedTrans);
			TransformationData userTrans = remoteUser->getWorldUserTransformation();
			userTrans.scale = gmtl::Vec3f(0.5, 0.5, 0.5);
			userCoordinateSystems[i].user->setModelTransformation(userTrans);
			TransformationData cursorTrans = remoteUser->getCursorTransformation();
			cursorTrans.scale = gmtl::Vec3f(0.5, 0.5, 0.5);
			userCoordinateSystems[i].cursor->setModelTransformation(cursorTrans);
		} // for
	} // updateRemoteCursors

}; // MedievalTownAppBase

int main(int argc, char** argv)
{
#if WIN32
	OSG::preloadSharedObject("OSGFileIO");
	OSG::preloadSharedObject("OSGImageFileIO");
#endif

	MedievalTownAppBase* app = new MedievalTownAppBase();

	if (!app->start(argc, argv)) {
		printd(ERROR, "Error occured during startup!\n");
		delete app;
		return -1;
	} // if

	delete app;
	return 0;
} // main
