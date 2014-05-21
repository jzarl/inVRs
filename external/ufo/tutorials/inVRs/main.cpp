/**
 * This tutorial is largely based on the MedievalTown tutorial of inVRs.
 *
 */
#include <inVRs/tools/libraries/OpenSGApplicationBase/OpenSGApplicationBase.h>

#include <inVRs/tools/libraries/Skybox/Skybox.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>

#include <inVRs/InputInterface/ControllerManager/ControllerManager.h>
#include <inVRs/tools/libraries/HeightMap/HeightMapModifier.h>
#include <inVRs/tools/libraries/HeightMap/HeightMapManager.h>
#include <inVRs/tools/libraries/CollisionMap/CheckCollisionModifier.h>
#include <inVRs/InputInterface/ControllerManagerInterface.h>
#include <inVRs/InputInterface/ControllerButtonChangeCB.h>
#include <inVRs/tools/libraries/AvataraWrapper/AvataraAvatar.h>
#include <inVRs/Modules/Interaction/Interaction.h>

using namespace std;
using namespace gmtl;
OSG_USING_NAMESPACE

class Ufo_inVRs : public OpenSGApplicationBase
{
private:
	std::string defaultConfigFile;		// config file
	Skybox skybox;						// scene surroundings
	float windMillSpeed;				// the rotational speed of a windmill
	OpenSGSceneGraphInterface* sgIF;	// pointer to the OpenSG scene graph interface
	OSG::NodePtr root;					// root pointer of the scene graph
	OSG::NodePtr scene;					// pointer to the scene loaded in WorldDatabase
	ControllerButtonChangeCB<Ufo_inVRs> buttonCallback; // for key-presses

public:
	Ufo_inVRs() :
	windMillSpeed(0.2*M_PI),
	sgIF(NULL),
	root(OSG::NullFC),
	scene(OSG::NullFC)
	{
		defaultConfigFile = "config/general_desktop_server.xml";
	} // Ufo_inVRs

	~Ufo_inVRs() {
		globalCleanup();
	} // ~Ufo_inVRs

	bool preInitialize(const CommandLineArgumentWrapper& args) {
		// create root node first because it is needed when Physics module is loaded (which happens
		// before the initialize method is called)
		root = Node::create();
		beginEditCP(root);
			root->setCore(Group::create());
		endEditCP(root);

		return true;
	} // preInitialize

	std::string getConfigFile(const CommandLineArgumentWrapper& args) {
		if (args.containOption("config"))
			return args.getOptionValue("config");
		else
			return defaultConfigFile;
	} // getConfigFile

	bool initialize(const CommandLineArgumentWrapper& args) {
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
			// add the SkyBox to the scene
			root->addChild(skybox.getNodePtr());
		endEditCP(root);

		// set to root node to the responsible SceneManager (managed by OpenSGApplicationBase)
		setRootNode(root);

		// hide local avatar since camera will be positioned approximately in avatar's head
		AvatarInterface* avatar = localUser->getAvatar();
		assert (avatar);
		avatar->showAvatar(false);

		// set our transformation to the start transformation
		TransformationData startTrans =
			WorldDatabase::getEnvironmentWithId(1)->getStartTransformation(0);
		localUser->setNavigatedTransformation(startTrans);

		// set the near clipping plane to a proper value
		setNearClippingPlane(0.1);

		// try to connect to network if first command line argument is {hostname|IP}:port
		if (args.getNumberOfArguments() > 1) {
			printf("Trying to connect to %s\n", args.getArgument(1).c_str());
			networkModule->connect(args.getArgument(1));
		} // if

		buttonCallback.setSource(controllerManager->getController());
		buttonCallback.setTarget(this, &Ufo_inVRs::buttonChangeCallback);
		buttonCallback.activate();

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

	void display(float dt) {
		skybox.setupRender(activeCamera->getPosition()); // attach the SkyBox to the camera

		// print info about grabbed objects:
		static unsigned int oldn = 0; //
		if (interactionModule->getState() == InteractionInterface::STATE_MANIPULATION) {
			unsigned int n = localUser->getNumberOfAssociatedEntities();
			// only print anything when the number changed:
			if ( n != oldn )
			{
				printf("INFO: localUser has %d associated Entities.\n",n);
				// print info about latest grabbed Entity:
				Entity *e = localUser->getAssociatedEntityByIndex(n-1);
				if ( e )
				{
					printf("INFO: grabbed Entity: TypeBasedId: %d (instanceId %d), EnvironmentBasedId: %d\n",
							e->getTypeBasedId(), e->getInstanceId(), e->getEnvironmentBasedId() );
				}
			}
			oldn=n;
		} else {
			oldn=localUser->getNumberOfAssociatedEntities();
		}

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

		// update ufo, i.e. the Steering Module:
		SystemCore::getModuleByName("Steering")->update(dt);
	} // display

	void buttonChangeCallback(int buttonIndex, int newButtonValue) {
		// see config/inputinterface/controllermanager/MouseKeybSensorController.xml
		// for the mapping keys -> buttonIndex
		if ( buttonIndex == 11 && newButtonValue != 0 )
		{
			TransformationData td = localUser->getWorldUserTransformation();
			// print current user position
			printf("INFO: localUser is at position (%3.2f,%3.2f,%3.2f).\n"
					,td.position[0] ,td.position[1] ,td.position[2]);
		}
	} // buttonChangeCallback

	void cleanup() {
	} // cleanup

}; // Ufo_inVRs

int main(int argc, char** argv) {
	Ufo_inVRs* app = new Ufo_inVRs();

	if (!app->start(argc, argv)) {
		printd(ERROR, "Error occured during startup!\n");
		delete app;
		return -1;
	} // if

	delete app;
	return 0;
} // main
