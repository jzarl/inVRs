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
 *                        Project: CAVESceneManager                          *
 *                                                                           *
 * The CAVE Scene Manager (CSM) is a wrapper for the OpenSG multi-display    *
 * functionality, which allows the easy configuration of a multi-display     *
 * setup via text files. It was developed by Adrian Haffegee of the Centre   *
 * for Advanced Computing and Emerging Technologies (ACET) at the            *
 * University of Reading, UK as part of his MSc Thesis.                      *
 *                                                                           *
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

/*! \file OSGCAVESceneManager.cpp
 *
 *  Adrian Haffegee, Sept 2004
 *	Implementation file for CAVE Scene Manager (CSM) - based on OpenSG SSM
 */

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>

#include <OpenSG/OSGConfig.h>

#include "../include/OSGCSM/OSGCAVESceneManager.h"
#include "../include/OSGCSM/compatibility.h"

#include <algorithm>

#include <OpenSG/OSGBaseFunctions.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGImageFileType.h>
#include <OpenSG/OSGSolidBackground.h>
#include <OpenSG/OSGViewport.h>
#if OSG_MAJOR_VERSION >= 2
#include <OpenSG/OSGTextureBackground.h>
#else //OpenSG1:
#include <OpenSG/OSGImageBackground.h>
#endif
#include <OpenSG/OSGLogoData.h>
#include <OpenSG/OSGSimpleStatisticsForeground.h>
#include <OpenSG/OSGStatElemTypes.h>
#include <OpenSG/OSGStatCollector.h>
#include <OpenSG/OSGDrawable.h>
#include <OpenSG/OSGPointLight.h>
#include <OpenSG/OSGSpotLight.h>
#include <OpenSG/OSGDirectionalLight.h>
#include <OpenSG/OSGRenderAction.h>
#include <OpenSG/OSGNode.h>
#include <OpenSG/OSGWindow.h>
#include <OpenSG/OSGRenderAction.h>
#include <OpenSG/OSGSimpleMaterial.h>



#if defined(OSG_WIN32_ICL) && !defined(OSG_CHECK_FIELDSETARG)
#pragma warning (disable : 383)
#endif

using namespace OSGCSM;

OSG_USING_NAMESPACE

OSGCompat::pointer_traits<OSG::Viewport>::TransitPtr
DefaultViewportMgr::createMonoViewport()
{
	OSGCompat::pointer_traits<OSG::Viewport>::RecPtr vp = Viewport::create();

	m_viewports.push_back(vp);

	return OSGCompat::pointer_traits<OSG::Viewport>::TransitPtr(vp);
}

OSGCompat::pointer_traits<OSG::Viewport>::TransitPtr
DefaultViewportMgr::createStereoViewport(bool isLeftEye)
{
	OSGCompat::pointer_traits<OSG::StereoBufferViewport>::RecPtr vp = StereoBufferViewport::create();
	vp->setLeftBuffer(isLeftEye);
	vp->setRightBuffer(!isLeftEye);
	m_viewports.push_back(vp);

	return OSGCompat::pointer_traits<OSG::Viewport>::TransitPtr(vp);
}

bool DefaultViewportMgr::destroyViewport(OSGCompat::pointer_traits<OSG::Viewport>::RecPtr vp)
{
	VPIterator it = std::find(m_viewports.begin(), m_viewports.end(), vp);
	if (it != m_viewports.end())
	{
		m_viewports.erase(it);
		return true;
	}
	return false;
}


/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class CAVESceneManager

    CAVESceneManager manages the OpenSG components that are needed to do
    multiscreen mono/stereo applications that want to display some geometry.
	It takes the multiwindow to render into and the root node of the graph
	to display and takes care of creating the necessary additional OpenSG
	objects, like viewports camers etc. Need a CSM config file!
 */


/*! \var CAVESceneManager::_win
    The managed window. Has to be set by the user.
 */

/*! \var CAVESceneManager::_root
    The root node to be rendered. Has to be set by the user.
 */

/*! \var CAVESceneManager::_highlight
    The Node to be highlighted (highlit? ;).  Has to be set by the user.
    May be OSGCompat::NullFC (which is the default), in which case nothing is highlighted.
 */

/*! \var CAVESceneManager::_highlightNode
    The node used to hold the actual highlight geometry.
 */

/*! \var CAVESceneManager::_highlightPoints
    The highlight points geo property. These are adjusted to include the
    _highlight object for every rendered frame.
 */

/*! \var CAVESceneManager::_internalRoot
    The real root used internally. It's the root for the light and has the
    camera beacon and the given root as children.
 */

/*! \var CAVESceneManager::_foreground
    ImageForeground used by the logo.
 */

/*! \var CAVESceneManager::_statstate
    Bool to indicate if statistics should be displayed or not.
 */

/*! \var CAVESceneManager::_action
    The action used to render the scene.
 */

/*! \var CAVESceneManager::_camTrolleyTrans
    The core of the camera beacons. Based on user's head
 */

/*! \var CAVESceneManager::_cameras
    The cameras used to view the scene.
 */

/*! \var CAVESceneManager::_mousebuttons
    Storage for the actively pressed mouse buttons. Needed to do the right
    thing for mouse motions, as mouseMove() doesn't get them.
 */

StatElemDesc<StatRealElem  > amhStatDesc ("amhStat","amhStat desc");

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

/*! Constructor
 */

CAVESceneManager::CAVESceneManager(void) :
	_win            (OSGCompat::NullFC),
	_root           (OSGCompat::NullFC),
	_wand           (OSGCompat::NullFC),
	_foreground     (OSGCompat::NullFC),
	_statforeground (OSGCompat::NullFC),
	_statstate      (false),

	_internalRoot   (OSGCompat::NullFC),
	_litScene       (OSGCompat::NullFC),
	_action         (NULL  ),
	_camTrolleyTrans (OSGCompat::NullFC),
	_headlight      (OSGCompat::NullFC),

	_lastx          (TypeTraits<Int16>::getMax()),
	_lasty          (TypeTraits<Int16>::getMax()),
	_mousebuttons   (0                             ),
	_userTransform  (OSGCompat::NullFC),
	_yRotation (0),
	_useShadows( false ),
	_viewportMgr(new DefaultViewportMgr())
{
	_cfg=NULL;
	for (int wallIdx=0; wallIdx<_cfg->getNumActiveWalls(); wallIdx++) {
		_cameras[wallIdx] = OSGCompat::NullFC;
	}

	/* ADDED BY LANDI */
	_scaleFactor = 1;
}


CAVESceneManager::CAVESceneManager(CAVEConfig *cfg) :
	_win            (OSGCompat::NullFC),
	_root           (OSGCompat::NullFC),
	_wand           (OSGCompat::NullFC),
	_foreground     (OSGCompat::NullFC),
	_statforeground (OSGCompat::NullFC),
	_statstate      (false),

	_internalRoot   (OSGCompat::NullFC),
	_litScene       (OSGCompat::NullFC),
	_action         (NULL  ),
	_camTrolleyTrans (OSGCompat::NullFC),
	_headlight      (OSGCompat::NullFC),

	_lastx          (TypeTraits<Int16>::getMax()),
	_lasty          (TypeTraits<Int16>::getMax()),
	_mousebuttons   (0                             ),
	_eyeseparation  (6),
	_userTransform  (OSGCompat::NullFC),
	_sceneBoundaryMin(-2000, -2000, -2000),
	_sceneBoundaryMax(2000, 2000, 2000),
	_yRotation (0),
	_useShadows ( false ),
	_viewportMgr(new DefaultViewportMgr())
{
	_cfg=cfg;

	for (int wallIdx=0; wallIdx<_cfg->getNumActiveWalls(); wallIdx++) 
	{
		_cameras[wallIdx] = OSGCompat::NullFC;
	}

	_eyeseparation = _cfg->getEyeSeparation();

	/* ADDED BY LANDI */
	_scaleFactor = 1;
}

CAVESceneManager::~CAVESceneManager(void)
{
#if OSG_MAJOR_VERSION >= 2
	// pointer is reference-counted
#else
	if(_action)
		delete _action;
#endif
	delete _viewportMgr;
}

void CAVESceneManager::setHeadlight(bool on)
{
	if(_headlight != OSGCompat::NullFC)
	{
#if OSG_MAJOR_VERSION < 2
		beginEditCP(_headlight, LightBase::OnFieldMask);
#endif
		_headlight->setOn(on);
#if OSG_MAJOR_VERSION < 2
		endEditCP(_headlight, LightBase::OnFieldMask);
#endif
	}
}

void CAVESceneManager::adoptViewportManager(IViewportMgr* vpMgr)
{
	if (_viewportMgr != vpMgr)
	{
		delete _viewportMgr;
		_viewportMgr = vpMgr;
	}
}


/*! get the window to be used for display
 */
OSGCompat::pointer_traits<OSG::Window>::RecPtr CAVESceneManager::getWindow(void)
{
	return _win;
}

/*! get the root of the displayed tree
 */
OSGCompat::pointer_traits<OSG::Node>::RecPtr CAVESceneManager::getRoot(void)
{
	return _root;
}

/*! get the trolley translation
 */
Vec3f CAVESceneManager::getTranslation(void)
{
	return _camTrolleyTrans->getTranslation();
}

/*! get the trolley Y rotation translation
 */
Real32 CAVESceneManager::getYRotate(void)
{
	return _yRotation;
}

/*! get the wand node
 */
OSGCompat::pointer_traits<OSG::Node>::RecPtr CAVESceneManager::getWand(void)
{
	return _wand;
}

/*! get the wand rotation
 *
 * @return wand rotation - amh should return OSGCompat::NullFC idf no wand...
 */
Quaternion CAVESceneManager::getWandRotate(void)
{
	return _wandTransform->getRotation();;
}


/*! set the window to be used for display
 */
void CAVESceneManager::setWindow(OSGCompat::pointer_traits<OSG::Window>::RecPtr win)
{
	_win = win;
//    if(_win->getMFPort()->size() > 0 && _win->getPort(0) != OSGCompat::NullFC)
//        _navigator.setViewport(_win->getPort(0));
}

/*! get the eye separation value
 */
Real32 CAVESceneManager::getEyeSeparation(void)
{
	return _eyeseparation;
}

/*! get the action used to render the scene
 */
OSGCompat::pointer_traits<RenderAction>::OsnmPtr 
CAVESceneManager::getAction(void)
{
	return _action;
}

/*! set the root of the displayed tree
 */
void CAVESceneManager::setRoot(OSGCompat::pointer_traits<OSG::Node>::RecPtr root)
{
	if(_internalRoot == OSGCompat::NullFC)
	{
		initialize();
	}

	if(_root != OSGCompat::NullFC)
	{
		_internalRoot->subChild(_root);
		OSGCompat::subRefCP(_root);
	}

	_root = root;

	OSGCompat::addRefCP(_root);
	_internalRoot->addChild(_root);
}

/*! set the trolley translation
 */
void CAVESceneManager::setTranslation(Vec3f Trans)
{
	OSGCompat::beginEditCP(_camTrolleyTrans, ComponentTransform::TranslationFieldMask);
	_camTrolleyTrans->setTranslation(Trans);
	OSGCompat::endEditCP(_camTrolleyTrans, ComponentTransform::TranslationFieldMask);
}

/*! set the trolley translation, based relative to the current position and
 *  direction being faced
 */
void CAVESceneManager::setRelativeTranslation(Vec3f Trans)
{
	// get current position
	Vec3f oldTrans = getTranslation();
	// rotate new translation based on current direction being faced
	Matrix m;
	Vec3f rotatedTrans;

	m.setIdentity();
	m.setTransform(Quaternion(Vec3f(0,1,0), getYRotate()));
	m.mult(Trans, rotatedTrans);

	// calculate new transform and apply it
	rotatedTrans += oldTrans;

	setTranslation(rotatedTrans);

}

/*! set the trolley rotation
 */
void CAVESceneManager::setRotation(Quaternion Rotation)
{
	OSGCompat::beginEditCP(_camTrolleyTrans, ComponentTransform::RotationFieldMask);
	_camTrolleyTrans->setRotation(Rotation);
	OSGCompat::endEditCP(_camTrolleyTrans, ComponentTransform::RotationFieldMask);
}

/* ADDED BY LANDI */
void CAVESceneManager::setCameraMatrix(Matrix m)
{
	OSGCompat::beginEditCP(_camTrolleyTrans, ComponentTransform::MatrixFieldMask);
	(static_cast<OSGCompat::pointer_traits<OSG::Transform>::RecPtr>(_camTrolleyTrans))->setMatrix(m);
	OSGCompat::endEditCP(_camTrolleyTrans, ComponentTransform::MatrixFieldMask);
}


void CAVESceneManager::setBackground(Int16 wallIdx, OSGCompat::pointer_traits<Background>::RecPtr background)
{
	if ((wallIdx < 0) || (wallIdx >= _cfg->getNumActiveWalls())) 
	{
		return;
	}

	if (!_cfg->isStereo()) 
	{
		OSGCompat::pointer_traits<Viewport>::RecPtr vp = _win->getPort(wallIdx);
		OSGCompat::beginEditCP(vp);
		vp->setBackground(background);
		OSGCompat::endEditCP(vp);
	} else 
	{
		for (int LRcnt=0; LRcnt<2; LRcnt++) 
		{
			
			OSGCompat::pointer_traits<Viewport>::RecPtr vp = _win->getPort(wallIdx*2 + LRcnt);
            OSGCompat::beginEditCP(vp);
			vp->setBackground(background);
			OSGCompat::endEditCP(vp);
		}
	}
}

/*! clear all backgrounds
 */
void CAVESceneManager::clearBackgrounds()
{
	// create an null image
	OSGCompat::pointer_traits<SolidBackground>::RecPtr background = SolidBackground::create();
	background->setColor(Color3f());

	for (int wallIdx=0; wallIdx<_cfg->getNumActiveWalls(); ++wallIdx) 
	{
		setBackground(wallIdx, background);
	}
}

/*! set the trolley Y rotation
 */
void CAVESceneManager::setYRotate(Real32 rotation)
{
	_yRotation = rotation;
	OSGCompat::beginEditCP(_camTrolleyTrans, ComponentTransform::RotationFieldMask);
	_camTrolleyTrans->setRotation(Quaternion (Vec3f(0,1,0), _yRotation));
	OSGCompat::endEditCP(_camTrolleyTrans, ComponentTransform::RotationFieldMask);
}


/*! add to the trolley Y rotation
 */
void CAVESceneManager::addYRotate(Real32 rotation)
{
	setYRotate(getYRotate()+rotation);
}


/*! set the wand geometry
 *
 * @param wandNode the nodetree of the wand to add (it's geometry, + other parameters)
 * @param SetPattern a preset pattern to use (e.g. cylinder, beam?) - default (0) = still to be decided)
 * @return -1 if already have a node present, else 0 (currently -2 for non supported preset shapes))
 */
void CAVESceneManager::setWand( OSGCompat::pointer_traits<OSG::Node>::RecPtr wandN )
{
	if (wandN == OSGCompat::NullFC)
		return;

	Vec3f offset(0,0,0);
	Quaternion rotation(Vec3f(0,1,0),Pi/2);

#if 0
	// if no node passed in, assume a set pattern is required
	if (wandNode == OSGCompat::NullFC) 
	{
		switch (setPattern) 
		{
			case 0: 
					{
						wandNode = makeCylinder(20, 2, 8, true, true, true);
						printf("\n preset shape = like old one (cylinder)");

						// and colour it
						typedef OSGCompat::pointer_traits<Geometry>::OmnsPtr GeometryCastPtr;

						GeometryRecPtr geo = OSGCompat::osg_cast<GeometryCastPtr>(wandNode->getCore());
						SimpleMaterialRecPtr sm = SimpleMaterial::create();

						OSGCompat::beginEditCP(sm, SimpleMaterial::DiffuseFieldMask);
						sm->setDiffuse(Color3f( 1, 1, 0));
						OSGCompat::endEditCP  (sm, SimpleMaterial::DiffuseFieldMask);

						OSGCompat::beginEditCP(geo, Geometry::MaterialFieldMask);
						geo->setMaterial(sm);
						OSGCompat::endEditCP  (geo, Geometry::MaterialFieldMask);
					}
					break;

			case 1:
					wandNode = makeSphere(3,6);
					printf("\n preset shape = sphere");
					break;

			case 2:
					{
						NodeRecPtr wand = makeBox(200, 5,5,1,1,1);

						ComponentTransformRecPtr wandRotate;
						wandNode = makeCoredNode<ComponentTransform>(&wandRotate);


						// add the wand geometry to its transform node, + set it to the origin
						OSGCompat::beginEditCP(wandRotate, ComponentTransform::RotationFieldMask);
						wandRotate->setRotation(Quaternion(Vec3f(0,1,0), Pi/2));
						OSGCompat::endEditCP(wandRotate, ComponentTransform::RotationFieldMask);

						OSGCompat::beginEditCP(wandNode, Node::ChildrenFieldMask);
						wandNode->addChild(wand);
						OSGCompat::endEditCP(wandNode, Node::ChildrenFieldMask);

					}

					printf("\n preset shape = ray");
					break;

			default:
					printf("\n preset shape %d not yet supported", setPattern);
					return -2;
		}
	}
#endif

	_wand = wandN;

	// create a transformation for the wand
	_wandTransform = ComponentTransform::create();
	// temporily set the transform

	OSGCompat::beginEditCP(_wandTransform, ComponentTransform::TranslationFieldMask);
	_wandTransform->setTranslation(Vec3f(0,0,0));
	OSGCompat::endEditCP(_wandTransform, ComponentTransform::TranslationFieldMask);

	_wandTransformNode = Node::create();

	OSGCompat::beginEditCP(_wandTransformNode, Node::ChildrenFieldMask | Node::CoreFieldMask);
	_wandTransformNode->addChild(_wand);
	_wandTransformNode->setCore(_wandTransform);
	OSGCompat::endEditCP(_wandTransformNode, Node::ChildrenFieldMask | Node::CoreFieldMask);

	// and add it to the parentNode
	OSGCompat::beginEditCP(_cameraTrolleyNode, Node::ChildrenFieldMask );
	_cameraTrolleyNode->addChild(_wandTransformNode);
	OSGCompat::endEditCP(_cameraTrolleyNode, Node::ChildrenFieldMask );
}


/*! set the wand translation
 */
void CAVESceneManager::setWandTranslation(Vec3f Trans)
{
	OSGCompat::beginEditCP(_wandTransform, ComponentTransform::TranslationFieldMask);
	_wandTransform->setTranslation(Trans);
	OSGCompat::endEditCP(_wandTransform, ComponentTransform::TranslationFieldMask);
}

#if 0
/*! set the wand color
 */
void CAVESceneManager::setWandColour(Color3f colour)
{
	if (_wand == OSGCompat::NullFC)
	{
		return;
	}

	typedef OSGCompat::pointer_traits<Geometry>::OmnsPtr GeometryCastPtr;
	typedef OSGCompat::pointer_traits<SimpleMaterial>::OmnsPtr SimpleMaterialCastPtr;

	GeometryRecPtr       geo = OSGCompat::osg_cast<GeometryCastPtr>(_wand->getCore());
	SimpleMaterialRecPtr  sm = OSGCompat::osg_cast<SimpleMaterialCastPtr>(geo->getMaterial());

	OSGCompat::beginEditCP(sm, SimpleMaterial::DiffuseFieldMask);
	sm->setDiffuse(colour);
	OSGCompat::endEditCP  (sm, SimpleMaterial::DiffuseFieldMask);
}
#endif

/*! set the wand rotation
 *
 * @param rotate angle to set wand rotation to
 * @return void
 */
void CAVESceneManager::setWandRotate (Quaternion rotation )
{
	OSGCompat::beginEditCP(_wandTransform, ComponentTransform::RotationFieldMask);
	_wandTransform->setRotation(rotation);
	OSGCompat::endEditCP(_wandTransform, ComponentTransform::RotationFieldMask);
}

/*! rotate the wand
 *
 * @param rotate angle to rotate by
 * @return void
 */
void CAVESceneManager::rotateWand (Quaternion rotation )
{
	Quaternion q = _wandTransform->getRotation();
	q.mult(rotation);
	setWandRotate(q);
}


/*! turn wand on.
 */
void CAVESceneManager::turnWandOn(void)
{
	//TODO: only attach node
	//CreateWand(_cameraTrolleyNode);
}

/*! turn wand off.
 */
void CAVESceneManager::turnWandOff(void)
{
	//TODO: only detach node
	//if(_wand != OSGCompat::NullFC)
	//{
	//	// is this next sufficient for clearing up? smart pointers should take care of mem???
	//	OSGCompat::beginEditCP(_cameraTrolleyNode, Node::ChildrenFieldMask );
	//	_cameraTrolleyNode->subChild(_wandTransformNode);
	//	OSGCompat::endEditCP(_cameraTrolleyNode, Node::ChildrenFieldMask );

	//	_wandTransformNode = OSGCompat::NullFC;
	//	_wand = OSGCompat::NullFC;
	//}
}

/*! set the user transform
 */
void  CAVESceneManager::setUserTransform ( Vec3f trans, Quaternion rotate )
{
	Matrix m;

	/* ADDED BY LANDI */
	//	 trans *= _scaleFactor;


	OSGCompat::beginEditCP(_userTransform, Transform::MatrixFieldMask);

	m.setIdentity();
	m.setTransform(trans, rotate);
	_userTransform->setMatrix(m);

	OSGCompat::endEditCP (_userTransform, Transform::MatrixFieldMask);


	// check to see if the user transform takes the user out of bounds (in which
	// case pull them back through the translation)
	Vec3f currentTrans = _camTrolleyTrans->getTranslation();
	/*
	   Vec3f postValidatedTrans  = validateNewTranslation(currentTrans);



	   if (postValidatedTrans != currentTrans) {
	//for now ignore setting trans - it seems to be causing the jumping
	printf("\n\nwould have teleported to %f, %f, %f - instead of %f, %f, %f\n",
	postValidatedTrans[0], postValidatedTrans[1],postValidatedTrans[2],
	currentTrans[0], currentTrans[1], currentTrans[2]);
	//		setTranslation(postValidatedTrans);
	}*/

}

void  CAVESceneManager::setSceneBoundaries ( Vec3f Min, Vec3f Max )
{
	_sceneBoundaryMin = Min;
	_sceneBoundaryMax = Max;
}

/*! set the eye separation value
 */
void CAVESceneManager::setEyeSeparation(Real32 eyeSep)
{
	if ((!_cfg->isStereo()) || (_win == OSGCompat::NullFC)) 
	{
		return;
	}
	
	_eyeseparation = eyeSep;

	for (int vpIdx=0; vpIdx < OSGCompat::getViewportCount(_win); ++vpIdx) 
	{
		typedef OSGCompat::pointer_traits<ProjectionCameraDecorator>::OmnsPtr ProjCamDecPtr;

		ProjCamDecPtr camDec = OSGCompat::osg_cast<ProjCamDecPtr>(_win->getPort(vpIdx)->getCamera());
		OSGCompat::beginEditCP(camDec, ProjectionCameraDecorator::EyeSeparationFieldMask );
		camDec->setEyeSeparation(_eyeseparation * _scaleFactor);
		OSGCompat::endEditCP  (camDec, ProjectionCameraDecorator::EyeSeparationFieldMask );
	}
}


/*! set the statistics setting. Only works after the window has been created.
 */
void CAVESceneManager::setStatistics(bool on)
{
	if(_statforeground != OSGCompat::NullFC && on != _statstate)
	{
		OSGCompat::beginEditCP(_win->getPort(0));
		if(on)
		{
			if (!_cfg->isStereo()) 
			{
				OSGCompat::beginEditCP(_win->getPort(0));
				OSGCompat::addForeground(_win->getPort(0), _statforeground);
				OSGCompat::endEditCP(_win->getPort(0));
			} 
			else 
			{
				OSGCompat::beginEditCP(_win->getPort(0));
				OSGCompat::addForeground(_win->getPort(0), _statforeground);
				OSGCompat::endEditCP(_win->getPort(0));
				OSGCompat::beginEditCP(_win->getPort(1));
				OSGCompat::addForeground(_win->getPort(1), _statforeground);
				OSGCompat::endEditCP(_win->getPort(1));
			}

#if OSG_MAJOR_VERSION >= 2
			//TODO, no statistics yet
#else //OpenSG1:
			_action->setStatistics(&_statforeground->getCollector());
#endif
		}
		else
		{
			if (!_cfg->isStereo()) 
			{
				OSGCompat::beginEditCP(_win->getPort(0));
				OSGCompat::removeForeground(_win->getPort(0), _statforeground);
				OSGCompat::endEditCP(_win->getPort(0));
			} 
			else 
			{
				OSGCompat::beginEditCP(_win->getPort(0));
				OSGCompat::removeForeground(_win->getPort(0), _statforeground);
				OSGCompat::endEditCP(_win->getPort(0));
				OSGCompat::beginEditCP(_win->getPort(1));
				OSGCompat::removeForeground(_win->getPort(1), _statforeground);
				OSGCompat::endEditCP(_win->getPort(1));
			}

#if OSG_MAJOR_VERSION >= 2
			//TODO, no statistics yet
#else //OpenSG1:
			_action->setStatistics(NULL);
#endif
		}

		_statstate = on;
	}
}

void CAVESceneManager::setForeground(Int16 wallIdx, OSGCompat::pointer_traits<ImageForeground>::RecPtr fg, bool clear)
{
	if ((wallIdx < 0) || (wallIdx >= _cfg->getNumActiveWalls())) 
	{
		return;
	}

	if(fg != OSGCompat::NullFC)
	{
		if (!_cfg->isStereo()) 
		{
			OSGCompat::pointer_traits<Viewport>::RecPtr vp = _win->getPort(wallIdx);

			OSGCompat::beginEditCP(vp);
			if(clear)
				OSGCompat::clearForegrounds(vp);
			OSGCompat::addForeground(vp, fg);
			OSGCompat::endEditCP(vp);
		} 
		else 
		{
			for (int LRcnt=0; LRcnt<2; LRcnt++) 
			{
				OSGCompat::pointer_traits<Viewport>::RecPtr vp = _win->getPort(wallIdx*2 + LRcnt);

				OSGCompat::beginEditCP(vp);
				if(clear)
					OSGCompat::clearForegrounds(vp);
				OSGCompat::addForeground(vp, fg);
				OSGCompat::endEditCP(vp);
			}
		}
	}
}

//Given a certain pipe identified by pipeIndex, iterates over every wall with the same pipeIndex to get the maximum x or y extents of the viewport (right/top coordinate)
//and stores the maximum x and y in maxX and maxY
void CAVESceneManager::findMaxResolutionForPipe(int pipeIndex, float& maxX, float& maxY)
{
	maxX = 0, maxY = 0;
    for (int j=0; j<_cfg->getNumActiveWalls(); ++j)
    {
        if (pipeIndex == _cfg->getActiveWall(j)->getPipeIndex())
        {
            if (!_cfg->getActiveWall(j)->hasGeometry()) 
			{
                continue;
            }
			if (maxX < _cfg->getActiveWall(j)->getMaxX()) 
			{
                maxX = static_cast<float>(_cfg->getActiveWall(j)->getMaxX());
            }
            if (maxY < _cfg->getActiveWall(j)->getMaxY()) 
			{
                maxY = static_cast<float>(_cfg->getActiveWall(j)->getMaxY());
            }
        }
    }		
}


/*-------------------------------------------------------------------------*/
/*							   Updates								   */

/*! create the parts that the app needs: viewport, camera, base graph etc.
 */
void CAVESceneManager::initialize()
{
	_action = RenderAction::create();

	_litScene = Node::create();
	OSGCompat::pointer_traits<Group>::RecPtr litGroup = Group::create();

	OSGCompat::beginEditCP(_litScene, Node::CoreFieldMask);
	_litScene->setCore(litGroup);
	OSGCompat::endEditCP (_litScene, Node::CoreFieldMask);

	// the camera trolley
	_cameraTrolleyNode = Node::create();
	_camTrolleyTrans = ComponentTransform::create();

	OSGCompat::beginEditCP(_camTrolleyTrans, ComponentTransform::TranslationFieldMask |
			ComponentTransform::RotationFieldMask);
	_camTrolleyTrans->setTranslation(Vec3f(0,0,0));
	_camTrolleyTrans->setRotation(Quaternion(Vec3f(0,0,0), 0));
	OSGCompat::endEditCP(_camTrolleyTrans, ComponentTransform::TranslationFieldMask |
			ComponentTransform::RotationFieldMask);


	// create the user translations (where as user's position is in the cave)
	_userTransform = Transform::create();

	OSGCompat::beginEditCP(_userTransform, Transform::MatrixFieldMask);
	_userTransform->setMatrix(Matrix::identity());
	OSGCompat::endEditCP (_userTransform, Transform::MatrixFieldMask);

	OSGCompat::pointer_traits<Node>::RecPtr user = Node::create();

	OSGCompat::beginEditCP(user, Node::CoreFieldMask);
	user->setCore(_userTransform);
	OSGCompat::endEditCP (user, Node::CoreFieldMask);

	_headlight	= DirectionalLight::create();
	OSGCompat::beginEditCP(_headlight);
	//		_headlight->setAmbient  (.7, .7, .7, 1);
	//		_headlight->setDiffuse  (.7, .7, .7, 1);
	//		_headlight->setSpecular (.9, .9, .9, 1);
	//		_headlight->setAmbient  (0.2,0.2,0.2, 1);
	//		_headlight->setDiffuse  (1,1,1, 1);
	//		_headlight->setSpecular (1,1,1, 1);
	_headlight->setAmbient  (0.9f,0.8f,0.8f, 1.f);
	_headlight->setDiffuse  (0.6f,0.6f,0.6f, 1.f);
	_headlight->setSpecular (1.f,1.f,1.f, 1.f);
	_headlight->setDirection( 0.f,  0.f,  -1.f);
	//		_headlight->setDirection( 0,  0,  1);
	//		_headlight->setBeacon   (_litScene);
	_headlight->setBeacon   (user);

	OSGCompat::endEditCP(_headlight);

	// put the camera becons on the trolley

	OSGCompat::beginEditCP(_cameraTrolleyNode, Node::ChildrenFieldMask | Node::CoreFieldMask );
	_cameraTrolleyNode->setCore(_camTrolleyTrans);
	_cameraTrolleyNode->addChild(user);
	OSGCompat::endEditCP(_cameraTrolleyNode, Node::ChildrenFieldMask | Node::CoreFieldMask );


	_internalRoot = Node::create();
	
	OSGCompat::addRefCP(_internalRoot);
	OSGCompat::beginEditCP(_internalRoot, Node::ChildrenFieldMask | Node::CoreFieldMask );
	_internalRoot->setCore(_headlight);
	_internalRoot->addChild(_litScene);
	OSGCompat::endEditCP(_internalRoot, Node::ChildrenFieldMask | Node::CoreFieldMask );

	OSGCompat::beginEditCP(_litScene, Node::ChildrenFieldMask);
	_litScene->addChild(_cameraTrolleyNode);
	OSGCompat::endEditCP (_litScene, Node::ChildrenFieldMask);

	// create the cameras
	for (int wallIdx=0; wallIdx<_cfg->getNumActiveWalls(); wallIdx++) 
	{
		_cameras[wallIdx] = PerspectiveCamera::create();

		OSGCompat::beginEditCP(_cameras[wallIdx]);
		_cameras[wallIdx]->setBeacon(_cameraTrolleyNode);
		_cameras[wallIdx]->setFov   (OSGCompat::degreeToRad(90.f));
		_cameras[wallIdx]->setNear  (0.003f);
		_cameras[wallIdx]->setFar   (1000.f);
		OSGCompat::endEditCP(_cameras[wallIdx]);
	}


	// need a viewport?
	if(_win != OSGCompat::NullFC && OSGCompat::getViewportCount(_win) == 0)
	{
		_foreground = ImageForeground::create();

		OSGCompat::pointer_traits<SimpleStatisticsForeground>::RecPtr sf = SimpleStatisticsForeground::create();

		OSGCompat::beginEditCP(sf);
		sf->setSize(25);
		sf->setColor(Color4f(0.f,1.f,0.f,0.7f));

#if OSG_MAJOR_VERSION >= 2
		//TODO, look for DrawActionBase replacement
#else //OpenSG1:
		sf->addElement(DrawActionBase::statCullTestedNodes,
				"%d Nodes culltested");
#endif


		sf->addElement(RenderAction::statTravTime,	  "Draw FPS: %r.3f");


#if OSG_MAJOR_VERSION >= 2
		//TODO look for DrawActionBase replacement
		//TODO look if the material changes are still needed
#else //OpenSG1:
		sf->addElement(DrawActionBase::statCulledNodes,
				"%d Nodes culled");
		sf->addElement(RenderAction::statNMaterials,
				"%d material changes");
#endif
		sf->addElement(RenderAction::statNMatrices,
				"%d matrix changes");
		// 2012-05-16 ZaJ: quick-n-dirty hack to make this compile on modern OpenSG2.
		// statNGeometries was removed in rev 9ea5a61ebb291e4a73a6404cf8ed2459b9d2c8d2
		// FIXME: try to get a reasonable replacement for this statistic
//		sf->addElement(RenderAction::statNGeometries,
//				"%d Nodes drawn");
#if OSG_MAJOR_VERSION >= 2
		//TODO look if they are still needed
#else //OpenSG1:
		sf->addElement(RenderAction::statNTransGeometries,
				"%d transparent Nodes drawn");
#endif
#if 0 // not ready for primetime yet
		sf->addElement(PointLight::statNPointLights,
				"%d active point lights");
		sf->addElement(DirectionalLight::statNDirectionalLights,
				"%d active directional lights");
		sf->addElement(SpotLight::statNSpotLights,
				"%d active spot lights");
#endif
		sf->addElement(Drawable::statNTriangles,	"%d triangles drawn");
		sf->addElement(Drawable::statNLines,		"%d lines drawn");
		sf->addElement(Drawable::statNPoints,	   "%d points drawn");
		sf->addElement(Drawable::statNVertices,	 "%d vertices transformed");

		OSGCompat::endEditCP(sf);

#if OSG_MAJOR_VERSION >= 2
		StatCollectorP collector = sf->editCollector();
#else //OpenSG1:
		StatCollector *collector = &sf->getCollector();
#endif

		// add optional elements
		collector->getElem(Drawable::statNTriangles);
		//		collector->getElem(amhStatDesc);

		_statforeground = sf;


		// construct array of where each walls image will be rendered
		if (CSM_MAXCAVEWALLS<_cfg->getNumActiveWalls())
		{
			std::cerr << "Too many walls: " << _cfg->getNumActiveWalls() << " max is: " << CSM_MAXCAVEWALLS << '\n';
			throw std::out_of_range("Too many walls");
		}
		Pnt4f vppos[CSM_MAXCAVEWALLS];
		// work out number of pipes being used (highest pipe index+1)
		float PipeDevisionSize = 1.0f/_cfg->getNumPipes();
		for (int wallIdx=0; wallIdx<_cfg->getNumActiveWalls(); wallIdx++) 
		{
			const CAVEWall* wall = _cfg->getActiveWall(wallIdx);
			// if no geometry for this wall, treat it as full size, and continue to next wall
			if (!wall->hasGeometry()) 
			{
				float startPos = wall->getPipeIndex() * PipeDevisionSize;
				vppos[wallIdx] = Pnt4f( startPos, 0, startPos+PipeDevisionSize, 1);
				continue;
			}


			float MaxX=0, MaxY=0;
			findMaxResolutionForPipe(wall->getPipeIndex(), MaxX, MaxY);

			// work out positions for this wall within the pipe
			vppos[wallIdx] = Pnt4f( 
				(wall->getPipeIndex() + (wall->getMinX()/MaxX)) * PipeDevisionSize,
				 wall->getMinY()/MaxY,
				(wall->getPipeIndex() + (wall->getMaxX()/MaxX)) * PipeDevisionSize,
				 wall->getMaxY()/MaxY);
		}

		printf("\nWallParams");
		for (int wallIdx=0; wallIdx<_cfg->getNumActiveWalls(); wallIdx++) {
			printf("\nWall %d, pipeIdx %d  %.4f, %.4f, %.4f, %.4f", wallIdx,
					_cfg->getActiveWall(wallIdx)->getPipeIndex(),
					vppos[wallIdx][0], vppos[wallIdx][1], vppos[wallIdx][2], vppos[wallIdx][3]);
		}

		// create 4/8 viewports - number + type depending on if we're using stereo or not...


		for (int wallIdx=0; wallIdx<_cfg->getNumActiveWalls(); ++wallIdx) 
		{
			const CAVEWall* wall = _cfg->getActiveWall(wallIdx);
			if (!wall) 
			{
				printf("\n\nSkipping Wall %d\n\n", wallIdx);
				continue;
			}

			OSGCompat::pointer_traits<SolidBackground>::RecPtr bkg = SolidBackground::create();
			bkg->setColor(Color3f(0.f, 0.f, 0.f));

			if (!_cfg->isStereo()) 
			{
				OSGCompat::pointer_traits<Viewport>::RecPtr vp = createMonoProjCamViewport(
						_cameras[wallIdx],
						vppos[wallIdx], 
						bkg,
						user,
						wall->getProjData());

				OSGCompat::beginEditCP(_win);
				_win->addPort(vp);
				OSGCompat::endEditCP(_win);
			} else 
			{
				for (int LRcnt=0; LRcnt<2; LRcnt++)
				{
					OSGCompat::pointer_traits<Viewport>::RecPtr vp = createProjStereoViewport(
						LRcnt?false:true, 
						_cameras[wallIdx],
						vppos[wallIdx], 
						bkg, 
						user, 
						wall->getProjData());
					OSGCompat::beginEditCP(_win);
					_win->addPort(vp);
					OSGCompat::endEditCP(_win);
				}
			}
		}
	}
}

/*-------------------------------------------------------------------------*/
/*				Manipulating other objects in the scene				  */

/*! attaching/manipulating external scene objects wrt the camera trolley
 */

/*! attach an object to the trolley
 * @param node node to be attached to trolley
 * @param flags flags relating to how to add
 * @return true is sucessfully added
 */
bool  CAVESceneManager::addToTrolley(OSGCompat::pointer_traits<OSG::Node>::RecPtr node, Int16 flags )
{
	// could check that passed in node is valid - i.e. doesn't already have parent etc.
	// don't use flags yet, but just refernce them to help the build!
	if (flags) {
		printf("\nCSM flags %d",flags);
	}

	// create a transform node for the object to add
	// amh - do this with cored nodes
    OSGCompat::pointer_traits<OSG::Transform>::RecPtr nodeTransform = Transform::create();

	OSGCompat::beginEditCP(nodeTransform, Transform::MatrixFieldMask);
	Matrix m;
	m.setIdentity();
	nodeTransform->setMatrix(m);
	OSGCompat::endEditCP (nodeTransform, Transform::MatrixFieldMask);

	// create a node for the transform. Set its core and child nodes
    OSGCompat::pointer_traits<OSG::Node>::RecPtr addedTransNode = Node::create();

	OSGCompat::beginEditCP(addedTransNode, Node::ChildrenFieldMask | Node::CoreFieldMask );
	addedTransNode->setCore(nodeTransform);
	addedTransNode->addChild(node);
	OSGCompat::endEditCP(addedTransNode, Node::ChildrenFieldMask | Node::CoreFieldMask );

	// and add the tranceform node to the camera trolley node
	OSGCompat::beginEditCP(_cameraTrolleyNode, Node::ChildrenFieldMask);
	_cameraTrolleyNode->addChild(addedTransNode);
	//		_litScene->addChild(node);
	OSGCompat::endEditCP (_cameraTrolleyNode, Node::ChildrenFieldMask);

	return true;
}


/*! remove an attached object from the trolley
 * @param node node to be removed
 * @return true is sucessfully removed, false if couldn't find node
 */
bool CAVESceneManager::removeFromTrolley(OSGCompat::pointer_traits<OSG::Node>::RecPtr node )
{
	// could check that passed in node is valid - i.e. that it already belongs
	// to the litscene etc

	// find the parent of passed in node
    OSGCompat::pointer_traits<OSG::Node>::RecPtr parent = node->getParent();
	if ((parent==OSGCompat::NullFC) ||
			(parent->getCore()==OSGCompat::NullFC) ||
			(parent->getCore()->getTypeId() != Transform::getClassTypeId())) {
		// invalid node passed in
		return false;
	}
	// delete the transform
	// is this next sufficient for clearing up? smart pointers should take care of
	// also removing the node itself, thereby freeing the extra mem???

	OSGCompat::beginEditCP(_cameraTrolleyNode, Node::ChildrenFieldMask );
	_cameraTrolleyNode->subChild(parent);
	OSGCompat::endEditCP(_cameraTrolleyNode, Node::ChildrenFieldMask );

	return true;
}


/*! show the whole scene: move out far enough  to see everything
 */
void CAVESceneManager::showAll()
{
	if(_root == OSGCompat::NullFC)
		return;

	_root->updateVolume();

	Vec3f min,max;
	_root->getVolume().getBounds( min, max );
	Vec3f d = max - min;

#if OSG_MAJOR_VERSION >= 2
	Real32 dist = osgMax(d[0],d[1]) / (2.f * osgTan(_cameras[0]->getFov() / 2.f));
#else //OpenSG1:
	Real32 dist = osgMax(d[0],d[1]) / (2.f * osgtan(_cameras[0]->getFov() / 2.f));
#endif

	Vec3f up(0,1,0);
	Pnt3f at((min[0] + max[0]) * .5f,(min[1] + max[1]) * .5f,(min[2] + max[2]) * .5f);
	Pnt3f from=at;
	from[2]-=dist;

	// set the camera to go from 1% of the object to twice(??) its size

	Real32 diag = osgMax(osgMax(d[0], d[1]), d[2]);
	for (int wallIdx=0; wallIdx<_cfg->getNumActiveWalls(); wallIdx++) {

		OSGCompat::beginEditCP(_cameras[wallIdx]);
		_cameras[wallIdx]->setNear (diag / 3000.f);
		_cameras[wallIdx]->setFar  (30.f * diag);
		OSGCompat::endEditCP(_cameras[wallIdx]);
	}

}

// ADDED BY HBRESS
void CAVESceneManager::setFarClippingPlane(float farDist)
{
	if(_root == OSGCompat::NullFC)
		return;

	for (int wallIdx=0; wallIdx<_cfg->getNumActiveWalls(); wallIdx++) 
	{
		OSGCompat::beginEditCP(_cameras[wallIdx]);
		_cameras[wallIdx]->setFar(farDist);
		OSGCompat::endEditCP(_cameras[wallIdx]);
	}
}

// ADDED BY LANDI
void CAVESceneManager::setNearClippingPlane(float nearDist)
{
	if(_root == OSGCompat::NullFC)
		return;

	for (int wallIdx=0; wallIdx<_cfg->getNumActiveWalls(); wallIdx++) 
	{
		OSGCompat::beginEditCP(_cameras[wallIdx]);
		_cameras[wallIdx]->setNear(nearDist);
		OSGCompat::endEditCP(_cameras[wallIdx]);
	}
}

// ADDED BY HBRESS,LANDI
void CAVESceneManager::setScale(float scale)
{
	for (int vpIdx=0; vpIdx < OSGCompat::getViewportCount(_win); ++vpIdx)
	{
		typedef OSGCompat::pointer_traits<ProjectionCameraDecorator>::OmnsPtr ProjectionCameraCastPtr;
		OSGCompat::pointer_traits<ProjectionCameraDecorator>::RecPtr camDec = OSGCompat::osg_cast<ProjectionCameraCastPtr>(_win->getPort(vpIdx)->getCamera());
		OSGCompat::beginEditCP(camDec, ProjectionCameraDecorator::SurfaceFieldMask | ProjectionCameraDecorator::EyeSeparationFieldMask);
		for (int i=0; i < 4; ++i)
		{
			OSGCompat::editSurfacePoint(camDec, i) *= scale/_scaleFactor;
		}
		camDec->setEyeSeparation(_eyeseparation * scale);
		OSGCompat::endEditCP(camDec, ProjectionCameraDecorator::SurfaceFieldMask | ProjectionCameraDecorator::EyeSeparationFieldMask);
	}

	_scaleFactor = scale;

}


/*! Draw the next frame, update if needed.
 */
void CAVESceneManager::idle(void)
{
}

/*! Draw the next frame, update if needed.
 */
void CAVESceneManager::redraw(void)
{
	if (_internalRoot == OSGCompat::NullFC)
	{
		initialize();
		showAll();
	}

	_win->render(_action);
}

/*-------------------------------------------------------------------------*/
/*						Interaction handling							 */

/*! resize
 */
void CAVESceneManager::resize(UInt16 width, UInt16 height)
{
	_win->resize(width, height);
}

/*------------------------------ access -----------------------------------*/

/*---------------------------- properties ---------------------------------*/

/*-------------------------- your_category---------------------------------*/

/*-------------------------- assignment -----------------------------------*/

/*------------------------ internal funcs ---------------------------------*/

OSGCompat::pointer_traits<Viewport>::TransitPtr 
CAVESceneManager::createMonoProjCamViewport(
	OSGCompat::pointer_traits<PerspectiveCamera>::RecPtr camera, 
	Pnt4f size, 
	OSGCompat::pointer_traits<Background>::RecPtr bg,
	OSGCompat::pointer_traits<Node>::RecPtr user, 
	const Pnt3f *vpproj)
{
	// first create the decorator
	OSGCompat::pointer_traits<ProjectionCameraDecorator>::RecPtr camDec = ProjectionCameraDecorator::create();
	OSGCompat::beginEditCP(camDec);
	camDec->setLeftEye(true);
	camDec->setEyeSeparation(0);
	camDec->setDecoratee(camera);
	camDec->setUser(user);
	for (int i=0; i<4; ++i)
	{
		OSGCompat::addSurfacePoint(camDec, vpproj[i] * _scaleFactor);
	}
	OSGCompat::endEditCP(camDec);

	// then the viewport
	OSGCompat::pointer_traits<Viewport>::RecPtr vp = _viewportMgr->createMonoViewport();
	OSGCompat::beginEditCP(vp);
	vp->setSize(size[0],size[1],size[2],size[3]);
	vp->setCamera(camDec);
	vp->setBackground(bg);
	vp->setRoot(_internalRoot);
	OSGCompat::addForeground(vp, _foreground);
	OSGCompat::endEditCP  (vp);
    return OSGCompat::pointer_traits<OSG::Viewport>::TransitPtr(vp);
}

OSGCompat::pointer_traits<Viewport>::TransitPtr 
CAVESceneManager::createProjStereoViewport(bool left,
	OSGCompat::pointer_traits<PerspectiveCamera>::RecPtr camera, 
	Pnt4f size, 
	OSGCompat::pointer_traits<Background>::RecPtr bg,
	OSGCompat::pointer_traits<Node>::RecPtr user, 
	const Pnt3f *vpproj)
{
	OSGCompat::pointer_traits<ProjectionCameraDecorator>::RecPtr camDec;
	camDec = ProjectionCameraDecorator::create();
	OSGCompat::beginEditCP(camDec);
	camDec->setLeftEye(left);
	camDec->setEyeSeparation(_eyeseparation * _scaleFactor);
	camDec->setDecoratee(camera);
	camDec->setUser(user);
	for (int i=0; i<4; ++i)
	{
		OSGCompat::addSurfacePoint(camDec, vpproj[i] * _scaleFactor);
	}
	OSGCompat::endEditCP(camDec);

	OSGCompat::pointer_traits<Viewport>::RecPtr vp = _viewportMgr->createStereoViewport(left);
	OSGCompat::beginEditCP(vp);
	vp->setSize(size[0],size[1],size[2],size[3]);
	vp->setCamera(camDec);
	vp->setBackground(bg);
	vp->setRoot(_internalRoot);
	OSGCompat::addForeground(vp, _foreground);
	OSGCompat::endEditCP  (vp);
	return OSGCompat::pointer_traits<Viewport>::TransitPtr(vp);
}
