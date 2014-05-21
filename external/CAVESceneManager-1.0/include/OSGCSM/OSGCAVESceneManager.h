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


#ifndef INVRS_OSGCAVESCENEMANAGER_H_
#define INVRS_OSGCAVESCENEMANAGER_H_
#pragma once

#include "compatibility.h"

#include "viewportmgr.h"
#include "OSGCAVEConfig.h"

#include <list>

#include <OpenSG/OSGBaseTypes.h>
#include <OpenSG/OSGVector.h>
#include <OpenSG/OSGQuaternion.h>
#include <OpenSG/OSGImageForeground.h>
#include <OpenSG/OSGTransform.h>
#include <OpenSG/OSGComponentTransform.h>
#include <OpenSG/OSGStatisticsForeground.h>
#include <OpenSG/OSGPerspectiveCamera.h>
#include <OpenSG/OSGStereoBufferViewport.h>
#include <OpenSG/OSGProjectionCameraDecorator.h>
#include <OpenSG/OSGSimpleMaterial.h>
#include <OpenSG/OSGRenderAction.h>
#include <OpenSG/OSGDirectionalLight.h>

//Shadow support
#if OSG_MAJOR_VERSION >= 2
//TODO: openSG2.0 Version		
#	include <OpenSG/OSGViewport.h>
#else //OpenSG1:
#	include <OpenSG/OSGShadowViewport.h>
#	include <OpenSG/OSGShadowMapViewport.h>
#endif
#if OSG_MAJOR_VERSION >= 2
#	include <OpenSG/OSGTextureBaseChunk.h>
#	include <OpenSG/OSGTypedGeoVectorProperty.h>
#	include <OpenSG/OSGTextureBackground.h>		
#	include <OpenSG/OSGTypedGeoIntegralProperty.h>
#	include <OpenSG/OSGLine.h>
#else //OpenSG1:
#	include <OpenSG/OSGDrawAction.h>
#	include <OpenSG/OSGGeoPropPtrs.h>
#	include <OpenSG/OSGImageBackground.h>
#endif


namespace OSGCSM
{
/*! \brief A helper class to simplify managing simple applications.
    \ingroup GrpSystemLib
*/

class CAVESCENEMANAGER_DLLMAPPING DefaultViewportMgr : public IViewportMgr
{
public:
	virtual OSGCompat::pointer_traits<OSG::Viewport>::TransitPtr createMonoViewport();
	virtual OSGCompat::pointer_traits<OSG::Viewport>::TransitPtr createStereoViewport(bool isLeftEye);
	virtual bool destroyViewport(OSGCompat::pointer_traits<OSG::Viewport>::RecPtr vp);
private:
	std::list<OSGCompat::pointer_traits<OSG::Viewport>::RecPtr> m_viewports;
	typedef std::list<OSGCompat::pointer_traits<OSG::Viewport>::RecPtr>::iterator VPIterator;
};

class CAVESCENEMANAGER_DLLMAPPING CAVESceneManager
{
	/*==========================  PUBLIC  =================================*/
	public:
		/*---------------------------------------------------------------------*/
		/*! \name                      Enums                                   */
		/*! \{                                                                 */

		enum {  MouseLeft   =  0,
			MouseMiddle =  1,
			MouseRight  =  2,
			MouseUp     =  3,
			MouseDown   =  4
		};

		// ADDED BY HBRESS / LANDI
		void setFarClippingPlane(float farDist);
		void setNearClippingPlane(float nearDist);
		void setScale(float scale);

		/*! \}                                                                 */
		/*---------------------------------------------------------------------*/
		/*! \name                   Constructors                               */
		/*! \{                                                                 */

		CAVESceneManager(void);
		CAVESceneManager(CAVEConfig *cfg);

		/*! \}                                                                 */
		/*---------------------------------------------------------------------*/
		/*! \name                   Destructor                                 */
		/*! \{                                                                 */

		virtual ~CAVESceneManager(void);

		/*! \}                                                                 */
		/*---------------------------------------------------------------------*/
		/*! \name                      Get                                     */
		/*! \{                                                                 */

		virtual OSGCompat::pointer_traits<OSG::Node>::RecPtr         getRoot             ();
		virtual OSGCompat::pointer_traits<OSG::Window>::RecPtr       getWindow          ();
		virtual OSG::Real32                 getEyeSeparation         ();
		virtual OSGCompat::pointer_traits<OSG::RenderAction>::OsnmPtr getAction          ();
		virtual OSG::Vec3f                  getTranslation           ();
		virtual OSG::Real32                 getYRotate               ();
		virtual OSGCompat::pointer_traits<OSG::Node>::RecPtr         getWand            ();
		virtual OSG::Quaternion                                      getWandRotate ();

		/*! \}                                                                 */
		/*---------------------------------------------------------------------*/
		/*! \name                      Set                                     */
		/*! \{                                                                 */
		void adoptViewportManager(IViewportMgr* vpMgr); //takes ownership, responsible for deletion

		virtual void  setRoot          ( OSGCompat::pointer_traits<OSG::Node>::RecPtr root );
		virtual void  setWindow        ( OSGCompat::pointer_traits<OSG::Window>::RecPtr win );
		virtual void  setEyeSeparation ( OSG::Real32 eyeSep );
		virtual void  setTranslation   ( OSG::Vec3f Trans );
		virtual void  setRelativeTranslation   ( OSG::Vec3f Trans );
		virtual void  setRotation      ( OSG::Quaternion Rotation );
		virtual void  setBackground    ( OSG::Int16 position, OSGCompat::pointer_traits<OSG::Background>::RecPtr bg );
		virtual void  clearBackgrounds ( void );
		virtual void  setYRotate       ( OSG::Real32 rotation );
		virtual void  addYRotate       ( OSG::Real32 rotation );
		
		virtual void  setWand          ( OSGCompat::pointer_traits<OSG::Node>::RecPtr wandNode);
		virtual void  setWandTranslation    ( OSG::Vec3f Trans );
		//virtual void  setWandMaterial    ( ChunkMaterialRecPtr mat );
		virtual void  setWandRotate    ( OSG::Quaternion rotation );
		virtual void  rotateWand (OSG::Quaternion rotation );
		virtual void  turnWandOn       ( void );
		virtual void  turnWandOff      ( void );

		virtual void  setUserTransform ( OSG::Vec3f trans, OSG::Quaternion rotate );
		virtual void  setSceneBoundaries   ( OSG::Vec3f Min, OSG::Vec3f Max );

		/* ADDED BY LANDI */
		virtual void  setCameraMatrix  ( OSG::Matrix m );


		/*! \}                                                                 */
		/*---------------------------------------------------------------------*/
		/*! \name       Manipulating objects attached to trolley                */
		/*! \{                                                                 */
		void setHeadlight(bool on);
		virtual bool  addToTrolley       ( OSGCompat::pointer_traits<OSG::Node>::RecPtr node, OSG::Int16 flags );
		virtual bool  removeFromTrolley  ( OSGCompat::pointer_traits<OSG::Node>::RecPtr node );

		virtual void  showAll          ( void );

		inline  bool  setClickCenter   ( bool mode );

		virtual void  setStatistics    ( bool on );
		virtual void  setForeground    ( OSG::Int16 position, OSGCompat::pointer_traits<OSG::ImageForeground>::RecPtr fg, bool clear);

		/*! \}                                                                 */
		/*---------------------------------------------------------------------*/
		/*! \name               Interaction handling                           */
		/*! \{                                                                 */

		virtual void resize(OSG::UInt16 width, OSG::UInt16 height);

		/*! \}                                                                 */
		/*---------------------------------------------------------------------*/
		/*! \name                     Actions                                  */
		/*! \{                                                                 */

		virtual void redraw( void );
		virtual void idle( void );


		/*! \}                                                                 */
		/*=========================  PROTECTED  ===============================*/
	protected:

		/*---------------------------------------------------------------------*/
		/*! \name                     Updates                                  */
		/*! \{                                                                 */

		void  initialize      (void);

		/*! \}                                                                 */
		/*---------------------------------------------------------------------*/
		/*! \name                      Member                                  */
		/*! \{                                                                 */

		OSGCompat::pointer_traits<OSG::Window>::RecPtr           _win;
		OSGCompat::pointer_traits<OSG::Node>::RecPtr                   _root;

		OSGCompat::pointer_traits<OSG::ImageForeground>::RecPtr        _foreground;
		OSGCompat::pointer_traits<OSG::StatisticsForeground>::RecPtr   _statforeground;
		bool                         _statstate;

	    //Shadow support
	    bool                         _useShadows;

		OSGCompat::pointer_traits<OSG::Node>::RecPtr                   _internalRoot;
		OSGCompat::pointer_traits<OSG::Node>::RecPtr                   _litScene;
		OSGCompat::pointer_traits<OSG::Node>::RecPtr                   _wand;
		OSGCompat::pointer_traits<OSG::RenderAction>::OsnmPtr             _action;

		OSGCompat::pointer_traits<OSG::Node>::RecPtr                   _cameraTrolleyNode;
		OSGCompat::pointer_traits<OSG::ComponentTransform>::RecPtr     _camTrolleyTrans;
		OSGCompat::pointer_traits<OSG::PerspectiveCamera>::RecPtr      _cameras[CSM_MAXCAVEWALLS];

		OSGCompat::pointer_traits<OSG::Node>::RecPtr                   _wandTransformNode;
		OSGCompat::pointer_traits<OSG::ComponentTransform>::RecPtr     _wandTransform;

		OSGCompat::pointer_traits<OSG::DirectionalLight>::RecPtr      _headlight;

		OSG::Vec3f                        _sceneBoundaryMin;
		OSG::Vec3f                        _sceneBoundaryMax;

		OSG::Int16                        _lastx;
		OSG::Int16                        _lasty;
		OSG::UInt16                       _mousebuttons;


		OSG::Real32                       _eyeseparation;
		OSGCompat::pointer_traits<OSG::Transform>::RecPtr                 _userTransform; // where the user is in relation to the screen
        OSG::Real32                       _yRotation;
		CAVEConfig *                 _cfg;
		IViewportMgr*                _viewportMgr;

		/* ADDED BY HBRESS,LANDI */
		OSG::Real32                       _scaleFactor;
		/*! \}                                                                 */
		/*==========================  PRIVATE  ================================*/
	private:

		/* prohibit default function (move to 'public' if needed) */

		CAVESceneManager(const CAVESceneManager &source);
		void operator =(const CAVESceneManager &source);


		OSGCompat::pointer_traits<OSG::Viewport>::TransitPtr createMonoProjCamViewport(
			OSGCompat::pointer_traits<OSG::PerspectiveCamera>::RecPtr camera, OSG::Pnt4f size, OSGCompat::pointer_traits<OSG::Background>::RecPtr bg,
			OSGCompat::pointer_traits<OSG::Node>::RecPtr user, const OSG::Pnt3f *vpproj);

		OSGCompat::pointer_traits<OSG::Viewport>::TransitPtr createProjStereoViewport(bool left,
			OSGCompat::pointer_traits<OSG::PerspectiveCamera>::RecPtr camera, OSG::Pnt4f size, OSGCompat::pointer_traits<OSG::Background>::RecPtr bg,
			OSGCompat::pointer_traits<OSG::Node>::RecPtr user, const OSG::Pnt3f *vpproj);

		void findMaxResolutionForPipe(int pipeIndex, float& maxX, float& maxY);

	//void CreateWand(OSGCompat::pointer_traits<OSG::Node>::RecPtr parentNode);

};

} //end namespace CSM

#endif /* _OSGCAVESCENEMANAGER_H_ */
