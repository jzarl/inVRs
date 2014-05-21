/*
 * OpenSGApplicationBase.cpp
 *
 *  Created on: Apr 23, 2009
 *      Author: rlander
 */

#include "OpenSGApplicationBase.h"
#include "shadowviewportmgr.h"

// GMTL includes
#include <gmtl/External/OpenSGConvert.h>

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGSolidBackground.h>
#include <OpenSG/OSGImage.h>
#include <OpenSG/OSGSceneFileHandler.h>
#if OSG_MAJOR_VERSION >= 2
#	include <OpenSG/OSGShadowStage.h>
#	include <OpenSG/OSGViewport.h>
#	include <OpenSG/OSGTextureBackground.h>
#	include <OpenSG/OSGTextureObjChunk.h>
#	include <OpenSG/OSGPolygonChunk.h>
#else //OpenSG1:
#	include <OpenSG/OSGShadowViewport.h>
#	include <OpenSG/OSGImageBackground.h>
#endif

#include <inVRs/SystemCore/UtilityFunctions.h>
#include <inVRs/OutputInterface/OutputInterface.h>
#include <inVRs/InputInterface/ControllerManager/GlutMouseDevice.h>
#include <inVRs/InputInterface/ControllerManager/GlutCharKeyboardDevice.h>
#include <inVRs/InputInterface/ControllerManager/GlutSensorEmulatorDevice.h>
#include "inVRs/SystemCore/DebugOutput.h"


OSG_USING_NAMESPACE

#ifndef DISABLE_CAVESCENEMANAGER_SUPPORT
using namespace OSGCSM;
#endif

OpenSGApplicationBase* OpenSGApplicationBase::_instance = NULL;

OpenSGApplicationBase::OpenSGApplicationBase() :
_preInitCalled(false),
_initCalled(false),
_mouseInterrupt(false),
_showStatistics(false),
_useSensorMouse(false),
_useShadows(false),
_rootNodeSet(false),
#if OSG_MAJOR_VERSION >= 2
_appRootNode(0),
_appWireframeCore(0),
#else //OpenSG1:
_appRootNode(NullFC),
_appWireframeCore(NullFC),
#endif
_smgr(NULL),
#ifndef DISABLE_CAVESCENEMANAGER_SUPPORT
_caveCfg(),
#endif
_farClippingPlane(0),
_nearClippingPlane(0),
_osgSGIF(NULL)
{
#ifndef DISABLE_CAVESCENEMANAGER_SUPPORT
	_csmgr = NULL;
	_useCSM = false;
	_useControlWindowImage = false;
#endif
}

OpenSGApplicationBase::~OpenSGApplicationBase() {
}

/******************************************************************************
 * Methods to overload / implement in derived classes:
 *****************************************************************************/

bool OpenSGApplicationBase::preInitialize(const CommandLineArgumentWrapper& args) {
	return true;
} // preInitialize

void OpenSGApplicationBase::initCoreComponentCallback(CoreComponents comp) {

} // initCoreComponentCallback

void OpenSGApplicationBase::initInputInterfaceCallback(ModuleInterface* moduleInterface) {

} // initInputInterfaceCallback

void OpenSGApplicationBase::initOutputInterfaceCallback(ModuleInterface* moduleInterface) {

} // initOutputInterfaceCallback

void OpenSGApplicationBase::initModuleCallback(ModuleInterface* module) {

} // initModuleCallback

void OpenSGApplicationBase::cbGlutSetWindowSize(int w, int h) {

} // cbGlutSetWindowSize

void OpenSGApplicationBase::cbGlutMouse(int button, int state, int x, int y) {

} // cbGlutMouse

void OpenSGApplicationBase::cbGlutMouseMove(int x, int y) {

} // cbGlutMouseMove

void OpenSGApplicationBase::cbGlutKeyboard(unsigned char k, int x, int y) {

} // cbGlutKeyboard

void OpenSGApplicationBase::cbGlutKeyboardUp(unsigned char k, int x, int y) {

} // cbGlutKeyboardUp

/******************************************************************************
 * Methods to call from derived classes
 *****************************************************************************/

#if OSG_MAJOR_VERSION >= 2
void OpenSGApplicationBase::setRootNode(NodeRecPtr root)
#else //OpenSG1:
void OpenSGApplicationBase::setRootNode(NodePtr root)
#endif
{
	int shadowMapSize = 1024;
#if OSG_MAJOR_VERSION >= 2
	//Note: the "better" way to do shadows in OpenSG2 is using LightEngines...
	int shadowMode = ShadowStage::VARIANCE_SHADOW_MAP;
#else //OpenSG1:
	int shadowMode = ShadowViewport::VARIANCE_SHADOW_MAP;
#endif
	float shadowSmoothness = -1;

#if OSG_MAJOR_VERSION >= 2
	std::vector<OSG::NodeRecPtr>::iterator it;

	ShadowStageUnrecPtr svp;
	_appRootNode->addChild(root);
#else //OpenSG1:
	std::vector<OSG::NodePtr>::iterator it;

	ShadowViewportPtr svp;
	beginEditCP(_appRootNode, Node::ChildrenFieldMask);
		_appRootNode->addChild(root);
	endEditCP(_appRootNode, Node::ChildrenFieldMask);
#endif


	if (_useShadows) {
		if (Configuration::contains("OpenSGApplicationBase.ShadowMapSize")) {
			shadowMapSize = Configuration::getInt("OpenSGApplicationBase.ShadowMapSize");
		} // if
		if (Configuration::contains("OpenSGApplicationBase.ShadowMode")) {
			std::string shadowModeName = Configuration::getString("OpenSGApplicationBase.ShadowMode");
#if OSG_MAJOR_VERSION >= 2
			if (shadowModeName == "STD")
				shadowMode = ShadowStage::STD_SHADOW_MAP;
			else if (shadowModeName == "PERSPECTIVE")
				shadowMode = ShadowStage::PERSPECTIVE_SHADOW_MAP;
			else if (shadowModeName == "DITHER")
				shadowMode = ShadowStage::DITHER_SHADOW_MAP;
			else if (shadowModeName == "PCF")
				shadowMode = ShadowStage::PCF_SHADOW_MAP;
			else if (shadowModeName == "PCSS")
				shadowMode = ShadowStage::PCSS_SHADOW_MAP;
			else if (shadowModeName == "VARIANCE")
				shadowMode = ShadowStage::VARIANCE_SHADOW_MAP;
#else //OpenSG1:
			if (shadowModeName == "STD")
				shadowMode = ShadowViewport::STD_SHADOW_MAP;
			else if (shadowModeName == "PERSPECTIVE")
				shadowMode = ShadowViewport::PERSPECTIVE_SHADOW_MAP;
			else if (shadowModeName == "DITHER")
				shadowMode = ShadowViewport::DITHER_SHADOW_MAP;
			else if (shadowModeName == "PCF")
				shadowMode = ShadowViewport::PCF_SHADOW_MAP;
			else if (shadowModeName == "PCSS")
				shadowMode = ShadowViewport::PCSS_SHADOW_MAP;
			else if (shadowModeName == "VARIANCE")
				shadowMode = ShadowViewport::VARIANCE_SHADOW_MAP;
#endif
		} // if
		if (Configuration::contains("OpenSGApplicationBase.ShadowSmoothness")) {
			shadowSmoothness = Configuration::getFloat("OpenSGApplicationBase.ShadowSmoothness");
		} // if
	} // if

#ifndef DISABLE_CAVESCENEMANAGER_SUPPORT
	if(_useCSM) 
	{
		_csmgr->setRoot(_appRootNode);

#if OSG_MAJOR_VERSION < 2
		if (_useShadows) 
		{
			ShadowViewportMgr* svpMgr = new ShadowViewportMgr();
			svpMgr->enableShadows();
			for (it = _lightNodes.begin(); it != _lightNodes.end(); ++it)
				svpMgr->addShadowLightSource(*it);
			svpMgr->setShadowMapSize(shadowMapSize);
			svpMgr->setShadowMode(shadowMode);
			if (shadowSmoothness >= 0) {
				svpMgr->setShadowSmoothness(shadowSmoothness);
			} // if
			_csmgr->adoptViewportManager(svpMgr);
		} // if
#endif			
		_mwin->init();

#if OSG_MAJOR_VERSION >= 2
		OSG::commitChanges();
#endif
	} // if
	else
#endif
	{
#if OSG_MAJOR_VERSION >= 2
		if(_useShadows){
			svp = OSG::ShadowStage::create();
			// ZaJ: setBackground and setSize don't exist anymore:
			//SolidBackgroundRecPtr sbg = SolidBackground::create();
			//svp->setBackground(sbg);
			//svp->setSize(0, 0, 1, 1);

			//svp->setOffFactor(4.0);
			//svp->setOffBias(8.0);
			//used to set global shadow intensity, ignores shadow intensity from light sources if != 0.0
			//svp->setGlobalShadowIntensity(0.8);
			svp->setMapSize(shadowMapSize);
			svp->setShadowMode(shadowMode);
			//ShadowSmoothness used for PCF_SHADOW_MAP and VARIANCE_SHADOW_MAP, defines Filter Width. Range can be 0.0 ... 1.0.
			//ShadowSmoothness also used to define the light size for PCSS_SHADOW_MAP
			if (shadowSmoothness >= 0) {
				svp->setShadowSmoothness(shadowSmoothness);
			} // if
			// add light sources here
			//svp->getExcludeNodes().push_back(obj1_trans_node);
			// Activate for automatic light source searching
			//svp->setAutoSearchForLights(true);
			for (it = _lightNodes.begin(); it != _lightNodes.end(); ++it)
				svp->editMFLightNodes()->push_back(*it);
			// ZaJ: no available in OpenSG2
			//svp->setRoot(_appRootNode);

		}//if
#else //OpenSG1:
		if (_useShadows) {
			svp = ShadowViewport::create();
			SolidBackgroundPtr sbg = SolidBackground::create();
			// Shadow viewport
			beginEditCP(svp);
				svp->setBackground(sbg);
				svp->setSize(0, 0, 1, 1);
//				svp->setOffFactor(4.0);
//				svp->setOffBias(8.0);
				//used to set global shadow intensity, ignores shadow intensity from light sources if != 0.0
				//svp->setGlobalShadowIntensity(0.8);
				svp->setMapSize(shadowMapSize);
				svp->setShadowMode(shadowMode);
				//ShadowSmoothness used for PCF_SHADOW_MAP and VARIANCE_SHADOW_MAP, defines Filter Width. Range can be 0.0 ... 1.0.
				//ShadowSmoothness also used to define the light size for PCSS_SHADOW_MAP
				if (shadowSmoothness >= 0) {
					svp->setShadowSmoothness(shadowSmoothness);
				} // if
				// add light sources here
//				svp->getExcludeNodes().push_back(obj1_trans_node);
				// Activate for automatic light source searching
//				svp->setAutoSearchForLights(true);
				for (it = _lightNodes.begin(); it != _lightNodes.end(); ++it)
					svp->getLightNodes().push_back(*it);
				svp->setRoot(_appRootNode);

			endEditCP(svp);
		} // if
#endif

#if OSG_MAJOR_VERSION >= 2
		_gwin->setGlutId(_winid);
		_gwin->init();
#else //OpenSG1:
		beginEditCP(_gwin);//Window
			_gwin->setId(_winid);
			if (_useShadows)
				_gwin->addPort(svp);
			_gwin->init();
		endEditCP(_gwin);
#endif

		_smgr->setWindow(_gwin);
		_smgr->setRoot(_appRootNode);

		if (_useShadows) {
#if OSG_MAJOR_VERSION >= 2
#else //OpenSG1:
			beginEditCP(svp);//Viewport
				svp->setCamera(_smgr->getCamera());
			endEditCP(svp);
#endif
			_smgr->showAll();
		} // if
	} // else
	_rootNodeSet = true;
} // setRootNode


void OpenSGApplicationBase::setStatistics(bool onOff)
{
	if (_showStatistics != onOff) {
#ifndef DISABLE_CAVESCENEMANAGER_SUPPORT
		if(!_useCSM)
#endif
		{
			_smgr->setStatistics(onOff);
			_showStatistics = onOff;
		} // if
	} // if
} // setStatistics

void OpenSGApplicationBase::setWireframe(bool onOff)
{
	bool isWF = (_appRootNode->getCore() == _appWireframeCore);
	if (isWF == onOff)
		return;

#if OSG_MAJOR_VERSION >= 2
	if (onOff)
		_appRootNode->setCore(_appWireframeCore);
	else
		_appRootNode->setCore(Group::create());
#else //OpenSG1:
	if (onOff)
	{
		beginEditCP(_appRootNode, Node::CoreFieldMask);
			_appRootNode->setCore(_appWireframeCore);
		endEditCP(_appRootNode, Node::CoreFieldMask);
	} // if
	else
	{
		beginEditCP(_appRootNode, Node::CoreFieldMask);
			_appRootNode->setCore(Group::create());
		endEditCP(_appRootNode, Node::CoreFieldMask);
	} // else
#endif
} // setWireframe

void OpenSGApplicationBase::setHeadlight(bool onOff)
{
#ifndef DISABLE_CAVESCENEMANAGER_SUPPORT
	if(_useCSM)
		_csmgr->setHeadlight(onOff);
	else
#endif
		_smgr->setHeadlight(onOff);
} // setHeadlight

bool OpenSGApplicationBase::setBackgroundImage(std::string imageUrl, int windowIndex) {
	bool success = false;
	std::string imageConcatenatedPath = getConcatenatedPath(imageUrl, "Images");
#if OSG_MAJOR_VERSION >= 2
	ImageRecPtr image = Image::create();
	success = image->read(imageConcatenatedPath.c_str());
#else //OpenSG1:
	ImagePtr image = Image::create();
	beginEditCP(image);
		success = image->read(imageConcatenatedPath.c_str());
	endEditCP(image);
#endif
	if (!success) {
		printd(WARNING, "OpenSGApplicationBase::setBackgroundImage(): could not load image %s!\n",
				imageConcatenatedPath.c_str());
		return false;
	} // if

#if OSG_MAJOR_VERSION >= 2
	OSG::TextureObjChunkUnrecPtr bkgTex = OSG::TextureObjChunk::create();
	bkgTex->setImage( image );
	bkgTex->setScale(true);
#endif
	
#ifndef DISABLE_CAVESCENEMANAGER_SUPPORT
	if (_useCSM) 
	{
#if OSG_MAJOR_VERSION >= 2
		OSG::TextureBackgroundUnrecPtr bkg = OSG::TextureBackground::create();
		bkg->setTexture(bkgTex);
		bkg->setColor(OSG::Color4f(1.0f, 1.0f, 1.0f, 1.0f));
#else
		ImageBackgroundPtr bkg = ImageBackground::create();
		beginEditCP(bkg);
			bkg->setImage(image);
			bkg->setScale(true);
		endEditCP(bkg);
#endif
		if (windowIndex < 0) 
		{
			for (int i=0; i < _caveCfg.getNumActiveWalls(); i++) 
			{
				_csmgr->setBackground( i, bkg );
			}
		}
		else 
		{
			_csmgr->setBackground( windowIndex, bkg );
		}
	} else
#endif
	{
#if OSG_MAJOR_VERSION >= 2
		if (windowIndex <= 0) {
			ViewportRecPtr vp = _gwin->getPort(0);
			assert(vp != NULL);
			BackgroundRecPtr bg = vp->getBackground();
			TextureBackgroundRecPtr bkg = dynamic_pointer_cast<TextureBackground>( bg );
			if (bkg == NULL) {
				bkg = TextureBackground::create();
				vp->setBackground(bkg);
			} // if
			
			bkg->setTexture(bkgTex);
			bkg->setColor(OSG::Color4f(1.0, 1.0, 1.0, 1.0));
			//bkg->setScale(true);
			//TODO: test this
		} // if
		else {
			printd(WARNING, "OpenSGApplicationBase::setBackgroundImage(): window with index %i not available! Call will be ignored!\n", windowIndex);
		} // else
	} // else
#else //OpenSG1:
		if (windowIndex <= 0) {
			ViewportPtr vp = _gwin->getPort(0);
			assert(vp != NullFC);
			ImageBackgroundPtr bkg = ImageBackgroundPtr::dcast(vp->getBackground());
			if (bkg == NullFC) {
				bkg = ImageBackground::create();
				vp->setBackground(bkg);
			} // if
			beginEditCP(bkg);
				bkg->setImage(image);
				bkg->setScale(true);
			endEditCP(bkg);
		} // if
		else {
			printd(WARNING, "OpenSGApplicationBase::setBackgroundImage(): window with index %i not available! Call will be ignored!\n", windowIndex);
		} // else
	} // else
#endif
	return success;
} // setBackground

#if OSG_MAJOR_VERSION >= 2
void OpenSGApplicationBase::activateShadows(OSG::NodeRecPtr lightNode)
{
	std::vector<OSG::NodeRecPtr> lightNodes;
	lightNodes.push_back(lightNode);
	activateShadows(lightNodes);
} // activateShadows
#else //OpenSG1:
void OpenSGApplicationBase::activateShadows(OSG::NodePtr lightNode)
{
	std::vector<OSG::NodePtr> lightNodes;
	lightNodes.push_back(lightNode);
	activateShadows(lightNodes);
} // activateShadows
#endif

#if OSG_MAJOR_VERSION >= 2
void OpenSGApplicationBase::activateShadows(const std::vector<OSG::NodeRecPtr>& lightNodes)
#else //OpenSG1:
void OpenSGApplicationBase::activateShadows(const std::vector<OSG::NodePtr>& lightNodes)
#endif
{
	if (_rootNodeSet)
		printd(ERROR, "OpenSGApplicationBase::activateShadows(): Shadows have to be activated before setRootNode is called the first time!!!\n");
	else
		_lightNodes = lightNodes;

	_useShadows = true;
} // activateShadows

void OpenSGApplicationBase::setPhysicalToWorldScale(float scale) {
#ifndef DISABLE_CAVESCENEMANAGER_SUPPORT
	if (_useCSM)
		_csmgr->setScale(scale);
#endif

	if (!controllerManager || !controllerManager->getController())
		printd(ERROR, "OpenSGApplicationBase::setPhysicalToWorldScale(): ControllerManager is not initialized or no Controller is available! Can not scale Sensor-values!\n");
	else
		controllerManager->getController()->setPhysicalToWorldScale(scale);

/* THE OTHER SCALE-VALUES SHOULD BE MANAGED BY THE USER!!!
	localUser->getCursor()->setPhysicalToWorldScale(scale);
	localUser->setPhysicalToWorldScale(scale);
*/
} // setCSMScale

void OpenSGApplicationBase::setNearClippingPlane(float nearPlane) {

	this->_nearClippingPlane = nearPlane;

	#ifndef DISABLE_CAVESCENEMANAGER_SUPPORT
	if (_useCSM)
		_csmgr->setNearClippingPlane(nearPlane);
	else
#endif
	{
#if OSG_MAJOR_VERSION >= 2
		CameraRecPtr camera = _smgr->getCamera();
		assert(camera != NULL);
		camera->setNear(nearPlane);
#else //OpenSG1:
		PerspectiveCameraPtr camera = _smgr->getCamera();
		assert(camera != NullFC);
		beginEditCP(camera);
			camera->setNear(nearPlane);
		endEditCP(camera);
#endif
	} // else
} // setNearClippingPlane

void OpenSGApplicationBase::setFarClippingPlane(float farPlane) {

	this->_farClippingPlane = farPlane;

#ifndef DISABLE_CAVESCENEMANAGER_SUPPORT
	if (_useCSM)
		_csmgr->setFarClippingPlane(farPlane);
	else
#endif
	{
#if OSG_MAJOR_VERSION >= 2
		CameraRecPtr camera = _smgr->getCamera();
		assert(camera != NULL);
		camera->setFar(farPlane);
#else //OpenSG1:
		PerspectiveCameraPtr camera = _smgr->getCamera();
		assert(camera != NullFC);
		beginEditCP(camera);
			camera->setFar(farPlane);
		endEditCP(camera);
#endif
	} // else
} // setFarClippingPlane

void OpenSGApplicationBase::setEyeSeparation(float newEyeSep)
{
#ifndef DISABLE_CAVESCENEMANAGER_SUPPORT
	if(_useCSM) {
		_csmgr->setEyeSeparation(newEyeSep);
		printd(INFO, "OpenSGApplicationBase::setEyeSeparation(): set to %0.3f\n", newEyeSep);
	} // if
	else
#endif
	{
		printd(WARNING, "OpenSGApplicationBase::setEyeSeparation(): unable to set eye separation in a non CAVESceneManager application!\n");
	} // else
} // setEyeSeparation

float OpenSGApplicationBase::getEyeSeparation()
{
#ifndef DISABLE_CAVESCENEMANAGER_SUPPORT
	if(_useCSM)
		return _csmgr->getEyeSeparation();
#endif
	return 0;
} // getEyeSeparation

/******************************************************************************
 * Internal methods
 *****************************************************************************/

bool OpenSGApplicationBase::preInit(const CommandLineArgumentWrapper& args) {
	if (_instance != NULL) {
		printd(ERROR, "OpenSGApplicationBase::pre_init(): instance already registered!\n");
		return false;
	} // if
	_instance = this;
	ChangeList::setReadWriteDefault(); // ChangeList needs to be set for OpenSG 1.4

	CommandLineArguments arguments = args.getCommandLineArguments();
	osgInit(arguments.getArgc(), arguments.getArgv());
	_preInitCalled = true;
	return preInitialize(args);
} // preInit

/*
SceneGraphInterface* OpenSGApplicationBase::createSceneGraphInterface(const CommandLineArgumentWrapper& args) {
	if (!_osgSGIF) {
		_osgSGIF = new OpenSGSceneGraphInterface();
	} else {
		printd(WARNING, "OpenSGApplicationBase::createSceneGraphInterface(): method unexpectedly called twice!\n");
	} // else
	return _osgSGIF;
} // createSceneGraphInterface
*/

bool OpenSGApplicationBase::init(const CommandLineArgumentWrapper& args) {
	if (!_preInitCalled) {
		printd(ERROR, "OpenSGApplicationBase::init(): pre_init was not called!\n");
		return false;
	} // if

	_osgSGIF = dynamic_cast<OpenSGSceneGraphInterface*>(OutputInterface::getSceneGraphInterface());
	if (!_osgSGIF) {
		printd(ERROR,
				"OpenSGApplicationBase::init(): OpenSGSceneGraphInterface could not be found! Please check OutputInterface configuration!\n");
		return false;
	} // if

	_initializeOpenSGNodes();

	_winid = _setupGLUT(args);
	_gwin= GLUTWindow::create();

#ifndef DISABLE_CAVESCENEMANAGER_SUPPORT
	if (Configuration::contains("OpenSGApplicationBase.useCSM"))
		_useCSM = Configuration::getBool("OpenSGApplicationBase.useCSM");
	if(_useCSM)
	{
#if OSG_MAJOR_VERSION >= 2
		_gwin->setGlutId(_winid);
#else //OpenSG1:
		_gwin->setId(_winid);
#endif
		_gwin->init();

#if OSG_MAJOR_VERSION >= 2
		NodeRecPtr _controlRootNode = Node::create();
		_controlRootNode->setCore(Group::create());

		_smgr = SimpleSceneManager::create();
#else
		NodePtr _controlRootNode = Node::create();
		beginEditCP(_controlRootNode);
			_controlRootNode->setCore(Group::create());
		endEditCP(_controlRootNode);

		_smgr = new SimpleSceneManager();
#endif
		glutReshapeWindow(250,250);

		_smgr->setWindow(_gwin);
		_smgr->setRoot(_controlRootNode);

		if (Configuration::contains("OpenSGApplicationBase.controlWindowImage")) {
			//load the image file
			std::string image =
					Configuration::getString("OpenSGApplicationBase.controlWindowImage");
			std::string imageConcatenatedPath = getConcatenatedPath(image, "Images");
			if (fileExists(imageConcatenatedPath)) {
#if OSG_MAJOR_VERSION >= 2
				ViewportRecPtr controlViewport = _smgr->getWindow()->getPort(0);

				ImageRecPtr bkgImage = Image::create();

				bkgImage->read(imageConcatenatedPath.c_str());
				TextureBackgroundRecPtr imageBkg = TextureBackground::create();
				TextureObjChunkRecPtr bkgTex = TextureObjChunk::create();
				bkgTex->setImage(bkgImage);
				bkgTex->setScale(true);
				imageBkg->setTexture(bkgTex);
				imageBkg->setColor(OSG::Color4f(1.0, 1.0, 1.0, 1.0));
				//imageBkg->setScale(true);
				//TODO: test this

				controlViewport->setBackground(imageBkg);
#else //OpenSG1:
				ViewportPtr controlViewport = _smgr->getWindow()->getPort(0);

				ImagePtr bkgImage = Image::create();

				beginEditCP(bkgImage);
					bkgImage->read(imageConcatenatedPath.c_str());
				endEditCP(bkgImage);
				ImageBackgroundPtr imageBkg = ImageBackground::create();
				beginEditCP(imageBkg);
					imageBkg->setImage(bkgImage);
					imageBkg->setScale(true);
				endEditCP(imageBkg);

				beginEditCP(controlViewport);
					controlViewport->setBackground(imageBkg);
				endEditCP(controlViewport);
#endif

				_useControlWindowImage = true;
			} // if
			else {
				printd(WARNING,
						"OpenSGApplicationBase::init(): unable to find background image for control window in: %s\n!",
						imageConcatenatedPath.c_str());
			} // else
		} // if

		std::string broadcastAddress = "";
		bool startRenderServers = false;
		std::string csmConfigFile = Configuration::getString("OpenSGApplicationBase.csmConfigFile");
		std::string configFileFull = getConcatenatedPath(csmConfigFile, "CAVESceneManagerConfiguration");
		if (Configuration::contains("OpenSGApplicationBase.broadcastAddress")) {
			broadcastAddress = Configuration::getString("OpenSGApplicationBase.broadcastAddress");
		} // if
		if (Configuration::contains("OpenSGApplicationBase.startRenderServers")) {
			startRenderServers = Configuration::getBool("OpenSGApplicationBase.startRenderServers");
		} // if
		if (fileExists(configFileFull))
		{
			printd(INFO, "OpenSGApplicationBase::init(): loading csm config file %s\n",
					configFileFull.c_str());
			char *cfgFileName = new char[configFileFull.size()+1];
			strcpy(cfgFileName, configFileFull.c_str());
			_caveCfg.loadFile(cfgFileName);
			delete[] cfgFileName;
			_caveCfg.printConfig();
			if (startRenderServers) {
				printd(INFO, "OpenSGApplicationBase::init(): Trying to start render servers locally!\n");
				startServers(_caveCfg);
				printd(INFO, "OpenSGApplicationBase::init(): Startup of render servers finished!\n");
			} else {
				printd(INFO, "OpenSGApplicationBase::init(): Please start the render servers manually!\n");
				// HACK: don't start the server, but print the command for startup!
				startServers(_caveCfg, "target host");
			} // else
			_csmgr = new CAVESceneManager(&_caveCfg);
			if (broadcastAddress.size() > 0) {
				printd(INFO,
						"OpenSGApplicationBase::init(): Trying to connect to render server(s) via broadcast address %s!\n",
						broadcastAddress.c_str());
				_mwin = createAppWindow(_caveCfg, broadcastAddress);
			} // if
			else {
				printd(INFO,
						"OpenSGApplicationBase::init(): Trying to connect to render server(s) via broadcast address 255.255.255.255!\n");
				_mwin = createAppWindow(_caveCfg);
			}
			_csmgr->setWindow(_mwin);
		} else {
			printd(ERROR, "OpenSGApplicationBase::init(): csm config file %s does not exist!\n",
					configFileFull.c_str());
			return false;
		}
		if (Configuration::contains("OpenSGApplicationBase.physicalToWorldScale")) {
			float physicalToWorldScale = Configuration::getFloat("OpenSGApplicationBase.physicalToWorldScale");
			if (physicalToWorldScale > 0) {
				setPhysicalToWorldScale(physicalToWorldScale);
			} // if
			else {
				printd(WARNING,
						"OpenSGApplicationBase::init(): invalid physicalToWorldScale value %f found! Value will be ignored!\n",
						physicalToWorldScale);
			} // else
		} // if
	} else
#endif
	{
#if OSG_MAJOR_VERSION >= 2
		_smgr = SimpleSceneManager::create();
#else
		_smgr = new SimpleSceneManager();
#endif
		glutReshapeWindow(500,500);
	}
	_initCalled = true;

	return initialize(args);
} // init

void OpenSGApplicationBase::run() {
	char windowTitleTextBuffer[1024];

	if (Configuration::contains("OpenSGApplicationBase.NearClippingPlane")) {
		if (_nearClippingPlane > 0)
			printd(WARNING, "OpenSGApplicationBase::run(): NearClippingPlane already set, entry in Configuration file will be ignored!\n");
		else
			_nearClippingPlane = Configuration::getFloat("OpenSGApplicationBase.NearClippingPlane");
	} // if
	if (Configuration::contains("OpenSGApplicationBase.FarClippingPlane")) {
		if (_farClippingPlane > 0)
			printd(WARNING, "OpenSGApplicationBase::run(): FarClippingPlane already set, entry in Configuration file will be ignored!\n");
		else
			_farClippingPlane = Configuration::getFloat("OpenSGApplicationBase.FarClippingPlane");
	} // if

#ifndef DISABLE_CAVESCENEMANAGER_SUPPORT
	if(_useCSM) {
		if (_nearClippingPlane <= 0 && _farClippingPlane <= 0)
			_csmgr->showAll();
		_csmgr->setUserTransform(Vec3f(0,0,0), Quaternion(Vec3f(0,0,0),0) );     // set an initial user position (in case no tracker being used)
		_csmgr->setTranslation(Vec3f(0,0,0));
		_csmgr->setEyeSeparation(_caveCfg.getEyeSeparation());
		_csmgr->turnWandOff();     // wand off
		if (_farClippingPlane > 0)
			_csmgr->setFarClippingPlane(_farClippingPlane);
		if (_nearClippingPlane > 0)
			_csmgr->setNearClippingPlane(_nearClippingPlane);
	} // if
	else
#endif
	{
		if (_nearClippingPlane <= 0 && _farClippingPlane <= 0)
			_smgr->showAll();
		Navigator *nav = _smgr->getNavigator();
		nav->setMode(Navigator::NONE);
#if OSG_MAJOR_VERSION >= 2
		CameraRecPtr camera = _smgr->getCamera();
		assert(camera != NULL);
		if (_nearClippingPlane > 0)
			camera->setNear(_nearClippingPlane);
		if (_farClippingPlane > 0)
			camera->setFar(_farClippingPlane);
#else //OpenSG1:
		PerspectiveCameraPtr camera = _smgr->getCamera();
		assert(camera != NullFC);
		beginEditCP(camera);
			if (_nearClippingPlane > 0)
				camera->setNear(_nearClippingPlane);
			if (_farClippingPlane > 0)
				camera->setFar(_farClippingPlane);
		endEditCP(camera);
#endif
	}

	sprintf(windowTitleTextBuffer, "inVRs: user id %u", localUser->getId());
	glutSetWindowTitle(windowTitleTextBuffer);
	printd(INFO, "OpenSGApplicationBase::run(): entering main loop\n");
	glutMainLoop();
} // run

void OpenSGApplicationBase::_initializeOpenSGNodes()
{
	_appRootNode = Node::create();
#if OSG_MAJOR_VERSION >= 2
	_appRootNode->setCore(Group::create());

	PolygonChunkRecPtr p = PolygonChunk::create();
	p->setFrontMode(GL_LINE);
	p->setBackMode(GL_LINE);

	SimpleMaterialRecPtr mat = SimpleMaterial::create();
	mat->setDiffuse(Color3f(1,1,1));
	mat->setLit(false);
	mat->addChunk(p);

	_appWireframeCore = MaterialGroup::create();
	_appWireframeCore->setMaterial(mat);
#else //OpenSG1:
	beginEditCP(_appRootNode, Node::CoreFieldMask);
		_appRootNode->setCore(Group::create());
	endEditCP(_appRootNode, Node::CoreFieldMask);

	PolygonChunkPtr p = PolygonChunk::create();
	p->setFrontMode(GL_LINE);
	p->setBackMode(GL_LINE);

	SimpleMaterialPtr mat = SimpleMaterial::create();
	beginEditCP(mat);
		mat->setDiffuse(Color3f(1,1,1));
		mat->setLit(false);
		mat->addChunk(p);
	endEditCP(mat);

	_appWireframeCore = MaterialGroup::create();
	beginEditCP(_appWireframeCore);
		_appWireframeCore->setMaterial(mat);
	endEditCP(_appWireframeCore);
	addRefCP(_appWireframeCore);
#endif
} // initializeOpenSGNodes

void OpenSGApplicationBase::_setupCamera() {
	Matrix osgCamMatrix;
	gmtl::Matrix44f cameraMatrix;

	activeCamera->getCameraTransformation(cameraMatrix);
	gmtl::set(osgCamMatrix, cameraMatrix);
#ifndef DISABLE_CAVESCENEMANAGER_SUPPORT
	if(_useCSM) {
		_csmgr->setCameraMatrix(osgCamMatrix);
		TransformationData head = localUser->getTrackedHeadTransformation();
		Vec3f osgTrans;
		Quaternion osgRot;
		osgTrans = Vec3f(head.position[0], head.position[1], head.position[2]);
		osgRot.setValueAsQuat(head.orientation.getData());
		_csmgr->setUserTransform(osgTrans, osgRot);
	} // if
	else
#endif
	{
		Navigator* nav = _smgr->getNavigator();
		nav->set(osgCamMatrix);
	} // if

	if (!_osgSGIF) {
		printd(ERROR, "OpenSGApplicationBase::_setupCamera(): OpenSGSceneGraphInterface was not created yet!\n");
	} else {
		_osgSGIF->setCameraMatrix(osgCamMatrix);
	} // else
} // _setupCamera

void OpenSGApplicationBase::_cleanup() {
	// call globalCleanup method of parent class ApplicationBase
	globalCleanup();
	// delete SceneGraphInterface
//	delete _osgSGIF;
//	_osgSGIF = NULL;

#ifndef DISABLE_CAVESCENEMANAGER_SUPPORT
	if (_useCSM)
	{
		printf("OpenSGApplicationBase::_cleanup(): killing servers!\n");
#ifdef WIN32
		for (int i=0; i < _caveCfg.getNumActiveWalls(); i++) {
			if (csmServerHandles[i].hProcess != INVALID_HANDLE_VALUE) {
				printf("OpenSGApplicationBase::_cleanup(): trying to kill server with index %i\n", i);
				TerminateProcess(csmServerHandles[i].hProcess, 1);
				CloseHandle(csmServerHandles[i].hProcess);
				CloseHandle(csmServerHandles[i].hThread);
				printf("OpenSGApplicationBase::_cleanup(): successfully killed server with index %i\n", i);
			} // if
			else {
				printf("OpenSGApplicationBase::_cleanup(): unable to kill server with index %i\n", i);
			} // else
		} // for
#else
		if(_caveCfg.isStereo())
		{
			system("killall -9 server-stereo");
		} else
		{
			system("killall -9 server-mono");
		}
#endif
	}
#endif

	OSG::osgExit();
	printf("OpenSGApplicationBase::_cleanup(): Successfully finished application!\n");
} // _cleanup

int OpenSGApplicationBase::_setupGLUT(const CommandLineArgumentWrapper& args) {
	CommandLineArguments arguments = args.getCommandLineArguments();
	int argc = arguments.getArgc();
	glutInit(&argc, arguments.getArgv());
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	int winid = glutCreateWindow("OpenSG");

	glutReshapeFunc(_setWindowSize);
	glutDisplayFunc(_display);
	glutMouseFunc(_mouse);
	glutMotionFunc(_mouseMove);
	glutPassiveMotionFunc(_mouseMove);
	glutKeyboardFunc(_keyboard);
	#ifndef __IRIX__
		glutKeyboardUpFunc(_keyboardUp);
	#endif
	glutIdleFunc(_display);

	return winid;
} // setupGLUT

void OpenSGApplicationBase::_setWindowSize(int w, int h)
{
#ifndef DISABLE_CAVESCENEMANAGER_SUPPORT
	if (_instance->_useCSM) {
		_instance->_csmgr->resize(w, h);
		_instance->_smgr->resize(w, h);
		if (!_instance->_useControlWindowImage) {
			_instance->_smgr->redraw();
		}
	} // if
	else
#endif
		_instance->_smgr->resize(w, h);

	GlutMouseDevice::setWindowSize(w, h);
	GlutSensorEmulatorDevice::setWindowSize(w, h);

	_instance->cbGlutSetWindowSize(w, h);
	glutPostRedisplay();
}

void OpenSGApplicationBase::_display() {
	if (!_instance->_initCalled) {
		printd(ERROR,
				"OpenSGApplicationBase::_display(): init-method of OpenSGApplicationBase was never called!\n");
		_instance->_cleanup();
		exit(0);
	} // if

	_instance->globalUpdate();
	_instance->_setupCamera();

#ifndef DISABLE_CAVESCENEMANAGER_SUPPORT
	if (_instance->_useCSM) {
#if OSG_MAJOR_VERSION >= 2
		OSG::commitChanges();
#endif
		_instance->_csmgr->redraw();
		if (_instance->_useControlWindowImage) {
			_instance->_smgr->redraw();
		} // if
	} // if
	else
#endif
		_instance->_smgr->redraw();

#if OSG_MAJOR_VERSION >= 2
	OSG::Thread::getCurrentChangeList()->clear();     //the changelist should be cleared - else things could be copied multiple times
#else //OpenSG1:
	OSG::Thread::getCurrentChangeList()->clearAll();     //the changelist should be cleared - else things could be copied multiple times
#endif
} // display

void OpenSGApplicationBase::_mouse(int button, int state, int x, int y) {
//	if (!_instance->_mouseInterrupt) {
		if (_instance->_useSensorMouse)
			GlutSensorEmulatorDevice::cbGlutMouse(button, state, x, y);
		else
			GlutMouseDevice::cbGlutMouse(button, state, x, y);

		_instance->cbGlutMouse(button, state, x, y);
		glutPostRedisplay();
//	} // if
} // _mouse

void OpenSGApplicationBase::_mouseMove(int x, int y) {
//	if (!_instance->_mouseInterrupt) {
		if (_instance->_useSensorMouse)
			GlutSensorEmulatorDevice::cbGlutMouseMove(x, y);
		else
			GlutMouseDevice::cbGlutMouseMove(x, y);

		_instance->cbGlutMouseMove(x, y);
		glutPostRedisplay();
//	} // if
} // _mouseMove

void OpenSGApplicationBase::_keyboard(unsigned char k, int x, int y) {
	float oldEyeSep;

	GlutCharKeyboardDevice::cbGlutKeyboard(k, x, y);
	if (_instance->_useSensorMouse)
		GlutSensorEmulatorDevice::cbGlutKeyboard(k, x, y);

	switch(k)
	{
		case 27:
			_instance->_cleanup();
			exit(0);
			break;

#ifndef DISABLE_CAVESCENEMANAGER_SUPPORT
		case '+':
			oldEyeSep = _instance->getEyeSeparation();
			_instance->setEyeSeparation(oldEyeSep*1.1f);
			break;

		case '-':
			oldEyeSep = _instance->getEyeSeparation();
			_instance->setEyeSeparation(oldEyeSep/1.1f);
			break;

		case ':':
			oldEyeSep = _instance->getEyeSeparation();
			_instance->setEyeSeparation(-oldEyeSep);
			break;
#endif
		case ',':
			_instance->setStatistics(!_instance->_showStatistics);
			break;

		case '*':
#ifndef DISABLE_CAVESCENEMANAGER_SUPPORT
			if (!_instance->_useCSM)
#endif
#if OSG_MAJOR_VERSION >= 2
				_instance->_smgr->getRenderAction()->setVolumeDrawing(!_instance->_smgr->getRenderAction()->getVolumeDrawing());
#else //OpenSG1:
				_instance->_smgr->getAction()->setVolumeDrawing(!_instance->_smgr->getAction()->getVolumeDrawing());
#endif
			break;
	} // switch

	_instance->cbGlutKeyboard(k, x, y);
} // _keyboard

void OpenSGApplicationBase::_keyboardUp(unsigned char k, int x, int y)
{
	GlutCharKeyboardDevice::cbGlutKeyboardUp(k, x, y);

	if (k == 'M' || k == 'm') {
		_instance->_mouseInterrupt = !_instance->_mouseInterrupt;
		GlutMouseDevice::setMouseGrabbing(!_instance->_mouseInterrupt);
		GlutSensorEmulatorDevice::setMouseGrabbing(!_instance->_mouseInterrupt);
	} else if (k == '1')
	{
		_instance->_useSensorMouse = !_instance->_useSensorMouse;
	}
	_instance->cbGlutKeyboardUp(k, x, y);
} // _keyboardUp

