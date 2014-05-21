#ifndef INVRS_EXTERNAL_CSM_OSGCAVEWALL_H
#define INVRS_EXTERNAL_CSM_OSGCAVEWALL_H

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


#include "./csm_config.h"
#include <OpenSG/OSGBaseTypes.h>
#include <OpenSG/OSGVector.h>

namespace OSGCSM
{

/*! \brief CAVE wall display class
 */
class CAVESCENEMANAGER_DLLMAPPING CAVEWall {
	public:
		struct Geometry
		{
			int minX, minY, maxX, maxY;
			Geometry() : minX(-1), minY(-1), maxX(-1), maxY(-1) {}
		};

		enum wallPos{ wallFront   =  0, /**< front wall */
			wallLeft,					/**< left wall */
			wallRight,					/**< right wall */
			wallFloor,					/**< floor wall */
			wallBack,					/**< back wall */
			wallCeiling,				/**< ceiling wall */
			wallUserDefined				/**< user defined wall */
		};


		explicit CAVEWall(const std::string& name);
		virtual ~CAVEWall();

		void printConfig() const;
		bool checkProjectionData(const OSG::Vec3f& origin, float width, float height);

		void setActive(bool activate);
		bool setPipe(const char *pipe);
		void setWindowed(bool window);
		bool parseGeometry(const std::string& geometry);
		void setPipeIndex(int index);
		void setProjectionData(const OSG::Pnt3f& lowerLeft, 
			const OSG::Pnt3f& upperLeft, 
			const OSG::Pnt3f& lowerRight, 
			float convFact);

		std::string getName() const;
		bool isActive() const;
		std::string getPipe() const;
		bool hasGeometry() const;
		bool isWindowed() const;
		const OSG::Pnt3f *getProjData() const;
		int getPipeIndex() const;
		int getMinX() const;
		int getMinY() const;
		int getMaxX() const;
		int getMaxY() const;
	private:

		std::string m_name;
		bool m_active;
		bool m_windowed;
		std::string m_pipe;
		wallPos m_wallPos;
		OSG::Pnt3f m_projData[4];
		int m_pipeIndex;
		Geometry m_geometry;
};

}

#endif // _CVR_CAVEWALL_H_
