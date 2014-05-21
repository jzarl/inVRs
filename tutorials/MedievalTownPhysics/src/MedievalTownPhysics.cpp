/*
 * MedievalTownNew.cpp
 *
 *  Created on: Apr 24, 2009
 *      Author: rlander
 */

#include <inVRs/tools/libraries/OpenSGApplicationBase/OpenSGApplicationBase.h>

#include <inVRs/tools/libraries/Skybox/Skybox.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>

#include <inVRs/InputInterface/ControllerManager/ControllerManager.h>
#include <inVRs/tools/libraries/HeightMap/HeightMapModifier.h>
#include <inVRs/tools/libraries/HeightMap/HeightMapManager.h>
#include <inVRs/tools/libraries/CollisionMap/CheckCollisionModifier.h>

#include <inVRs/Modules/Interaction/Interaction.h>

OSG_USING_NAMESPACE

class MedievalTownPhysics : public OpenSGApplicationBase
{
private:
	std::string defaultConfigFile;		// config file
	Skybox skybox;						// scene surroundings
	float windMillSpeed;				// the rotational speed of a windmill
	OpenSGSceneGraphInterface* sgIF;	// pointer to the OpenSG scene graph interface
	OSG::NodePtr root;					// root pointer of the scene graph
	OSG::NodePtr scene;					// pointer to the scene loaded in WorldDatabase

public:
	MedievalTownPhysics() :
	windMillSpeed(0),
	sgIF(NULL),
	root(OSG::NullFC),
	scene(OSG::NullFC)
	{
		defaultConfigFile = "config/general.xml";
	} // MedievalTownPhysics

	~MedievalTownPhysics() {
		globalCleanup();
	} // ~MedievalTownPhysics

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
	} // initCoreComponentCallback

	void display(float dt) {
		skybox.setupRender(activeCamera->getPosition()); // attach the SkyBox to the camera

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
	} // display

	void cleanup() {

	} // cleanup

}; // MedievalTownPhysics

int main(int argc, char** argv)
{
#if WIN32
	OSG::preloadSharedObject("OSGFileIO");
    OSG::preloadSharedObject("OSGImageFileIO");
#endif

	MedievalTownPhysics app;

	if (!app.start(argc, argv)) {
		printd(ERROR, "Error occured during startup!\n");
		return -1;
	} // if
} // main
