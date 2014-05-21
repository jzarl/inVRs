#include <cstdlib>
#include <inVRs/tools/libraries/OpenSGApplicationBase/OpenSGApplicationBase.h>
#include <inVRs/tools/libraries/Skybox/Skybox.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>
#include <inVRs/InputInterface/ControllerManager/ControllerManager.h>
#include <inVRs/tools/libraries/HeightMap/HeightMapModifier.h>
#include <inVRs/tools/libraries/HeightMap/HeightMapManager.h>
#include <inVRs/tools/libraries/CollisionMap/CheckCollisionModifier.h>
#ifdef ENABLE_TRACKD_SUPPORT
#include <inVRs/tools/libraries/TrackdDevice/TrackdDevice.h>
#endif
#ifdef ENABLE_VRPN_SUPPORT
#include <inVRs/tools/libraries/VrpnDevice/VrpnDevice.h>
#endif

#include <inVRs/InputInterface/ControllerManagerInterface.h>
#include <inVRs/InputInterface/ControllerButtonChangeCB.h>
#include <inVRs/tools/libraries/AvataraWrapper/AvataraAvatar.h>
#include <inVRs/Modules/Interaction/Interaction.h>

// BEGIN Physics
#include <inVRs/Modules/3DPhysics/Physics.h>
#include <inVRs/tools/libraries/oops/OpenSGTriangleMeshLoader.h>
#include <inVRs/tools/libraries/oops/OpenSGRendererFactory.h>
#include <inVRs/tools/libraries/oops/HtmpHeightFieldLoader.h>
#include <inVRs/Modules/3DPhysics/PhysicsHomerManipulationActionModel.h>
// END Physics

OSG_USING_NAMESPACE

class MedievalTownPhysics : public OpenSGApplicationBase
{
private:
	Skybox skybox;						// scene surroundings
	float windMillSpeed;				// the rotational speed of a windmill
	OpenSGSceneGraphInterface* sgIF;	// pointer to the OpenSG scene graph interface
	OSG::NodePtr root;					// root pointer of the scene graph
	OSG::NodePtr scene;					// pointer to the scene loaded in WorldDatabase

	ControllerButtonChangeCB<MedievalTownPhysics> buttonCallback;

	// BEGIN Physics
	Physics* physics;
	bool showPhysicsObjects;
    oops::OpenSGTriangleMeshLoader triMeshLoader;
    oops::OpenSGRendererFactory rendererFactory;
    oops::HtmpHeightFieldLoader heightFieldLoader;
	// END Physics

public:
	MedievalTownPhysics() :
	windMillSpeed(0),
	sgIF(NULL),
	root(NullFC),
	scene(NullFC),
	physics(NULL),
	showPhysicsObjects(false)
	{}

	~MedievalTownPhysics() {
		globalCleanup();
	}

	bool preInitialize(const CommandLineArgumentWrapper& args) {
		// create root node first because it is needed when Physics module is loaded (which happens
		// before the initialize method is called)
		root = Node::create();
		beginEditCP(root);
			root->setCore(Group::create());
		endEditCP(root);

		return true;
	}


	std::string getConfigFile(const CommandLineArgumentWrapper& args) {
		if (args.containOption("config"))
			return args.getOptionValue("config");
		else
			return "final/config/general.xml";
	}

	bool initialize(const CommandLineArgumentWrapper& args) {
		// scale world from physical units (centimeter) to world units (meters)
		setPhysicalToWorldScale(0.01f);

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

		// cast sceneGraphInterface from ApplicationBase to OpenSGSceneGraphInterface
		sgIF = dynamic_cast<OpenSGSceneGraphInterface*>(sceneGraphInterface);
		// must exist because it is created in OpenSGApplicationBase
		assert (sgIF);

		// add scene to root node (which was created in preInitialize method)
		assert(root != OSG::NullFC);
		scene = sgIF->getNodePtr();

		beginEditCP(root);
			root->addChild(scene);
			root->addChild(skybox.getNodePtr());
		endEditCP(root);

#if OSG_MAJOR_VERSION >= 2
		OSG::commitChanges();
#endif

		// set to root node to the responsible SceneManager (managed by OpenSGApplicationBase)
		setRootNode(root);

		AvatarInterface* avatar = localUser->getAvatar();
		assert (avatar);
		avatar->showAvatar(false);

		// set our transformation to the start transformation
		TransformationData startTrans =
			WorldDatabase::getEnvironmentWithId(1)->getStartTransformation(0);
		localUser->setNavigatedTransformation(startTrans);

		setNearClippingPlane(0.1f);

		// try to connect to network first command line argument is {hostname|IP}:port
		if (args.getNumberOfArguments() > 1)
		{
			printf("Trying to connect to %s\n", args.getArgument(1).c_str());
			networkModule->connect(args.getArgument(1));
		}

		buttonCallback.setSource(controllerManager->getController());
		buttonCallback.setTarget(this, &MedievalTownPhysics::buttonChangeCallback);
		buttonCallback.activate();

		// BEGIN Physics
		physics->start();
		// END Physics

//		commitChanges();

		return true;
	}

	void initCoreComponentCallback(CoreComponents comp)
	{
		// register factory for HeightMapModifier as soon as the
		// TransformationManager is initialized
		if (comp == TRANSFORMATIONMANAGER)
		{
			TransformationManager::registerModifierFactory(new HeightMapModifierFactory());
			TransformationManager::registerModifierFactory(new CheckCollisionModifierFactory);
		}
		else if (comp == USERDATABASE) {
			WorldDatabase::registerAvatarFactory(new AvataraAvatarFactory());
		}
	}

	void initModuleCallback(ModuleInterface* moduleInterface)
	{
		if (moduleInterface->getName() == "Physics")
		{
			// BEGIN Physics
			// set root node for physics renderer factory (renderer allows to display the physics
			// objects instead of the entity models, which can be useful for debugging)
	        rendererFactory.setRoot(root);

	        // get the pointer to the Physics module
	        physics = dynamic_cast<Physics*>(moduleInterface);
	        assert(physics);

	        // get the XMLLoader and set the factories
	        oops::XMLLoader* physicsLoader = physics->getXMLLoader();
	        physicsLoader->setTriangleMeshLoader(&triMeshLoader);
	        physicsLoader->setRendererFactory(&rendererFactory);
	        physicsLoader->setHeightFieldLoader(&heightFieldLoader);
			// END Physics
		}
		else if (moduleInterface->getName() == "Interaction")
		{
			((Interaction*)moduleInterface)->registerStateActionModelFactory(
					new PhysicsHomerManipulationActionModelFactory);
		}
	}

	void initInputInterfaceCallback(ModuleInterface* moduleInterface)
	{
		if (moduleInterface->getName() == "ControllerManager")
		{
			ControllerManager* contInt = dynamic_cast<ControllerManager*>(moduleInterface);
			assert(contInt);
#ifdef ENABLE_TRACKD_SUPPORT
			contInt->registerInputDeviceFactory(new TrackdDeviceFactory);
#endif
#ifdef ENABLE_VRPN_SUPPORT
			contInt->registerInputDeviceFactory(new VrpnDeviceFactory);
#endif
		}
	}

	void display(float dt)
	{
		skybox.setupRender(activeCamera->getPosition()); // attach the SkyBox to the camera

		// BEGIN Physics
		// call update method for synchronization of physics thread with main application
		physics->update(dt);
		// END Physics

		if (controllerManager->getController()->getButtonValue(2)) { // the right mouse button is pressed
			windMillSpeed += dt*.5f;	         // increase speed of the windmill
			if (windMillSpeed > 2.f*M_PI) {
				windMillSpeed = 2.f*M_PI;
			}
		} else if (windMillSpeed > 0) {	     // pressing mouse button stopped
			windMillSpeed -= dt*0.5f;	         // decrease speed of windmill
		} else if (windMillSpeed < 0) {
			windMillSpeed = 0.f;
		} // else if
		if (windMillSpeed > 0.f) {	          // rotate sails
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
			gmtl::AxisAnglef axisAngle(windMillSpeed*dt, 0.f, 0.f, 1.f);
			gmtl::Quatf rotation;
			gmtl::set(rotation, axisAngle);
			trans.orientation *= rotation;
			transNode->setTransformation(trans);
		} // if
	} // display

	void buttonChangeCallback(int buttonIndex, int newButtonValue) {
		if (buttonIndex == 7 && newButtonValue != 0) {
			showPhysicsObjects = !showPhysicsObjects;
			beginEditCP(scene);
				scene->setActive(!showPhysicsObjects);
			endEditCP(scene);
			physics->getSimulation()->setObjectsVisible(showPhysicsObjects);
		} // if
	} // buttonChangeCallback

	void cleanup() {
		physics->kill();
	} // cleanup

}; // MedievalTownPhysics

int main(int argc, char** argv)
{
#if WIN32
	OSG::preloadSharedObject("OSGFileIO");
    OSG::preloadSharedObject("OSGImageFileIO");
#endif

	MedievalTownPhysics app;

	if (!app.start(argc, argv))
	{
		printd(ERROR, "Error occured during startup!\n");
		return EXIT_FAILURE;
	}
}
