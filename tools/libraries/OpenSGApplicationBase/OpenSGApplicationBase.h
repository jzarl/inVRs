/*
 * OpenSGApplicationBase.h
 *
 *  Created on: Apr 23, 2009
 *      Author: rlander
 */

#ifndef OPENSGAPPLICATIONBASE_H_
#define OPENSGAPPLICATIONBASE_H_

#include <inVRs/SystemCore/ApplicationBase.h>

#include <OpenSG/OSGSimpleSceneManager.h>
#include <OpenSG/OSGMaterialGroup.h>
#include <OpenSG/OSGGLUTWindow.h>

#ifndef DISABLE_CAVESCENEMANAGER_SUPPORT
	#include <OSGCSM/OSGCAVESceneManager.h>
	#include <OSGCSM/OSGCAVEConfig.h>
	#include <OSGCSM/appctrl.h>
#endif

#include <inVRs/OutputInterface/OpenSGSceneGraphInterface/OpenSGSceneGraphInterface.h>

#ifdef WIN32
	#ifdef INVRSOPENSGAPPLICATIONBASE_EXPORTS
	#define INVRS_OPENSGAPPLICATIONBASE_API __declspec(dllexport)
	#else
	#define INVRS_OPENSGAPPLICATIONBASE_API __declspec(dllimport)
	#endif
#else
#define INVRS_OPENSGAPPLICATIONBASE_API
#endif

/**
 *
 */
class INVRS_OPENSGAPPLICATIONBASE_API OpenSGApplicationBase: public ApplicationBase {

/******************************************************************************
 * Methods to overload / implement in derived classes:
 *****************************************************************************/
protected:
	/**
	 *
	 */
	virtual std::string getConfigFile(const CommandLineArgumentWrapper& args) = 0;

	/**
	 *
	 */
	virtual bool preInitialize(const CommandLineArgumentWrapper& args);

	/**
	 *
	 */
	virtual bool initialize(const CommandLineArgumentWrapper& args) = 0;

	/**
	 *
	 */
	virtual void initCoreComponentCallback(CoreComponents comp);

	/**
	 *
	 */
	virtual void initInputInterfaceCallback(ModuleInterface* moduleInterface);

	/**
	 *
	 */
	virtual void initOutputInterfaceCallback(ModuleInterface* moduleInterface);

	/**
	 *
	 */
	virtual void initModuleCallback(ModuleInterface* module);

	/**
	 *
	 */
	virtual void display(float dt) = 0;

	/**
	 *
	 */
	virtual void cleanup() = 0;

	/**
	 *
	 */
	virtual void cbGlutSetWindowSize(int w, int h);

	/**
	 *
	 */
	virtual void cbGlutMouse(int button, int state, int x, int y);

	/**
	 *
	 */
	virtual void cbGlutMouseMove(int x, int y);

	/**
	 *
	 */
	virtual void cbGlutKeyboard(unsigned char k, int x, int y);

	/**
	 *
	 */
	virtual void cbGlutKeyboardUp(unsigned char k, int x, int y);

/******************************************************************************
 * Methods to call from derived classes
 *****************************************************************************/
public:

	/**
	 *
	 */
	OpenSGApplicationBase();

	/**
	 *
	 */
	virtual ~OpenSGApplicationBase();

	/**
	 *
	 */
#if OSG_MAJOR_VERSION >= 2
	void setRootNode(OSG::NodeRecPtr root);
#else //OpenSG1:
	void setRootNode(OSG::NodePtr root);
#endif

	/**
	 *
	 */
	void setStatistics(bool onOff);

	/**
	 *
	 */
	void setWireframe(bool onOff);

	/**
	 *
	 */
	void setHeadlight(bool onOff);

	/**
	 *
	 */
	bool setBackgroundImage(std::string imageUrl, int windowIndex = -1);

	/**
	 *
	 */
#if OSG_MAJOR_VERSION >= 2
	void activateShadows(OSG::NodeRecPtr lightNode);
#else //OpenSG1:
	void activateShadows(OSG::NodePtr lightNode);
#endif

	/**
	 *
	 */
#if OSG_MAJOR_VERSION >= 2
	void activateShadows(const std::vector<OSG::NodeRecPtr>& lightNodes);
#else //OpenSG1:
	void activateShadows(const std::vector<OSG::NodePtr>& lightNodes);
#endif

	/**
	 *
	 */
	void setPhysicalToWorldScale(float scale);

	/**
	 *
	 */
	void setNearClippingPlane(float nearPlane);

	/**
	 *
	 */
	void setFarClippingPlane(float farPlane);

	/**
	 *
	 */
	void setEyeSeparation(float eyeSeparation);

	/**
	 *
	 */
	float getEyeSeparation();

/******************************************************************************
 * Internal methods
 *****************************************************************************/
private:

	/**
	 *
	 */
	bool preInit(const CommandLineArgumentWrapper& args);

	/**
	 *
	 */
//	SceneGraphInterface* createSceneGraphInterface(const CommandLineArgumentWrapper& args);

	/**
	 *
	 */
	bool init(const CommandLineArgumentWrapper& args);

	/**
	 *
	 */
	void run();

	/**
	 *
	 */
	void _initializeOpenSGNodes();

	/**
	 *
	 */
	void _setupCamera();

	/**
	 *
	 */
	void _cleanup();

	/**
	 *
	 */
	int _setupGLUT(const CommandLineArgumentWrapper& args);

	/**
	 *
	 */
	static void _setWindowSize(int w, int h);

	/**
	 *
	 */
	static void _display();

	/**
	 *
	 */
	static void _mouse(int button, int state, int x, int y);

	/**
	 *
	 */
	static void _mouseMove(int x, int y);

	/**
	 *
	 */
	static void _keyboard(unsigned char k, int x, int y);

	/**
	 *
	 */
	static void _keyboardUp(unsigned char k, int x, int y);

	/// Single instance of OpenSGApplicationBase
	static OpenSGApplicationBase* _instance;
	/// Variable for checking if preInit was called correctly
	bool _preInitCalled;
	/// Variable for checking if init was called correctly
	bool _initCalled;

	/// Defines if mouse is warped into window or not
	bool _mouseInterrupt;
	/// Defines if statistics of SceneManager are displayed
	bool _showStatistics;
	/// Defines if sensor-emulation is used or not
	bool _useSensorMouse;
	/// Defines if Shadows are activated or not
	bool _useShadows;
	/// Defines if the root-node was already set
	bool _rootNodeSet;

#if OSG_MAJOR_VERSION >= 2
	/// Root node of application
	OSG::NodeRecPtr _appRootNode;
	/// Alternative core for wireframe rendering
	OSG::MaterialGroupRecPtr _appWireframeCore;
	/// Stores light nodes for shadow calculation
	std::vector<OSG::NodeRecPtr> _lightNodes;

	/// SceneManager for desktop / single display systems
	OSG::SimpleSceneManager::ObjRefPtr _smgr;
#else
	/// Root node of application
	OSG::NodePtr _appRootNode;
	/// Alternative core for wireframe rendering
	OSG::MaterialGroupPtr _appWireframeCore;
	/// Stores light nodes for shadow calculation
	std::vector<OSG::NodePtr> _lightNodes;

	/// SceneManager for desktop / single display systems
	OSG::SimpleSceneManager* _smgr;
#endif
	/// ID of GLUT window
	int _winid;
#if OSG_MAJOR_VERSION >= 2
	/// Pointer to GLUT window
	OSG::GLUTWindowRecPtr _gwin;
#else //OpenSG1:
	/// Pointer to GLUT window
	OSG::GLUTWindowPtr _gwin;
#endif

#ifndef DISABLE_CAVESCENEMANAGER_SUPPORT
#if OSG_MAJOR_VERSION >= 2
	/// Pointer to Multi-display window
	OSG::MultiDisplayWindowRecPtr _mwin;
#else //OpenSG1:
	/// Pointer to Multi-display window
	OSG::MultiDisplayWindowPtr _mwin;
#endif
	/// CAVESceneManager instance
	OSGCSM::CAVESceneManager* _csmgr;
	/// Configuration for CAVESceneManager
	OSGCSM::CAVEConfig _caveCfg;
	/// Define if CAVESceneManager is used or not
	bool _useCSM;
	/// Defines if a image for the control window is used
	bool _useControlWindowImage;
#endif

	/// Defines farClippingPlane
	float _farClippingPlane;
	/// Defines farClippingPlane
	float _nearClippingPlane;

	/// Pointer to OpenSGSceneGraphInterface
	OpenSGSceneGraphInterface* _osgSGIF;

//	rootNodeSet = false;

};

#endif /* OPENSGAPPLICATIONBASE_H_ */
