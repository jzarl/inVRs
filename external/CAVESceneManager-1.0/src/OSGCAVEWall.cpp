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

/* \file OSGCAVEWall.cpp
 *
 *  Adrian Haffegee, Sept 2004
 *	Implementation file for CAVE Display wall class
 */
#include <stdlib.h>
#include <stdio.h>
// std::strchr:
#include <cstring>

#include <OpenSG/OSGBaseFunctions.h>

#include "../include/OSGCSM/OSGCAVEWall.h"

using namespace OSGCSM;
OSG_USING_NAMESPACE



/**
 *  Constructor
 * @param name name of the new wall to be created
 */
CAVEWall::CAVEWall(const std::string& name)
 :	m_name(name),
	m_active(false),
	m_windowed(false),
	m_pipe(),
	m_wallPos(wallUserDefined),
	m_pipeIndex(-1),
	m_geometry()
{
	if (name == "front") {
		m_wallPos = wallFront;
	} else if (name == "left") {
		m_wallPos = wallLeft;
	} else if (name == "right") {
		m_wallPos = wallRight;
	} else if (name == "floor") {
		m_wallPos = wallFloor;
	} else if (name == "back") {
		m_wallPos = wallBack;
	} else if (name == "ceiling") {
		m_wallPos = wallCeiling;
	}

	for (int i=0; i<4; i++) 
	{
		m_projData[i] = Pnt3f(0,0,0);
	}
}

CAVEWall::~CAVEWall()
{
}


/**
 *  display wall configuration
 */
void CAVEWall::printConfig() const
{
	printf("CAVE Wall:%s ", getName().c_str());
	if (isActive()) {
		printf("- active, pipe '%s' (index:%d)%s", getPipe().c_str(), getPipeIndex(),
				isWindowed() ? ", windowed" : "");
		if (hasGeometry()) {
			printf(", geometry '%dx%d+%d+%d'", m_geometry.maxX-m_geometry.minX, m_geometry.maxY-m_geometry.minY, m_geometry.minX, m_geometry.minY);
		}
		printf("\n\t\tProj data:");
		for (int i=0; i<4; i++) {
			printf(" %.3f,%.3f,%.3f ", m_projData[i][0], m_projData[i][1], m_projData[i][2] );
		}
		printf("\n");
	} else {
		printf("- not active\n");
	}
	//    printf("\n\tDisplay Mode: %s", isStereo()?"stereo":"mono");
}


/** checks + sets projection data for walls
 * @param origin for the CAVE (in distance from the left wall (x), the floor (y) and the wall in front (z) of the user standing in the CAVE
 * @param width width and depth of CAVE
 * @param height height of CAVE
 * @return true if projection data is ok
 */
bool CAVEWall::checkProjectionData(const Vec3f& dist, float w, float h)
{
	// check we have cave width/height (both must be non 0 for a CAVE display)
	switch(m_wallPos)
	{
		case wallFront:
		case wallLeft:
		case wallRight:
		case wallFloor:
		case wallBack:
		case wallCeiling:
			if ((w == 0) || (h == 0)) 
			{
#ifdef CSM_DEBUG
				printf("Cfg: CAVE height/width not configured\n");
#endif
				return false;
			}
			break;
		default:
			break;
	}
	
	//Projection Points are specified as four points in counter-clockwise order that span a quadrilateral
	//Be consistent with the order, think of how points have to be specified in OpenGL for correct normal direction, normal points inwards here
	//since the user is located within the cube
	//With the viewer inside the cube, the coordinate origin is located in the lower, left, front corner
	//the points are transformed from this coordinate space to the a new one by the parameter vector
	switch(m_wallPos)
	{
		case wallFront:
			m_projData[0] = Pnt3f(0, 0, 0) - dist;
			m_projData[1] = Pnt3f(w, 0, 0) - dist;
			m_projData[2] = Pnt3f(w, h, 0) - dist;
			m_projData[3] = Pnt3f(0, h, 0) - dist;
			break;

		case wallLeft:
			m_projData[0] = Pnt3f(0, 0, w) - dist;
			m_projData[1] = Pnt3f(0, 0, 0) - dist;
			m_projData[2] = Pnt3f(0, h, 0) - dist;
			m_projData[3] = Pnt3f(0, h, w) - dist;
			break;

		case wallRight:
			m_projData[0] = Pnt3f(w, 0, 0) - dist;
			m_projData[1] = Pnt3f(w, 0, w) - dist;
			m_projData[2] = Pnt3f(w, h, w) - dist;
			m_projData[3] = Pnt3f(w, h, 0) - dist;
			break;

		case wallFloor:
			m_projData[0] = Pnt3f(0, 0, w) - dist;
			m_projData[1] = Pnt3f(w, 0, w) - dist;
			m_projData[2] = Pnt3f(w, 0, 0) - dist;
			m_projData[3] = Pnt3f(0, 0, 0) - dist;
			break;

		case wallBack:
			m_projData[0] = Pnt3f(w, 0, w) - dist;
			m_projData[1] = Pnt3f(0, 0, w) - dist;
			m_projData[2] = Pnt3f(0, h, w) - dist;
			m_projData[3] = Pnt3f(w, h, w) - dist;
			break;

		case wallCeiling:
			m_projData[0] = Pnt3f(0, h, 0) - dist;
			m_projData[1] = Pnt3f(w, h, 0) - dist;
			m_projData[2] = Pnt3f(w, h, w) - dist;
			m_projData[3] = Pnt3f(0, h, w) - dist;
			break;

		default:
			// otherwise just check there is some data
			if ((m_projData[0] == Pnt3f(0,0,0)) &&
					(m_projData[1] == Pnt3f(0,0,0)) &&
					(m_projData[2] == Pnt3f(0,0,0)) &&
					(m_projData[3] == Pnt3f(0,0,0)) ) {
				return false;
			}
			return true;
	}
	return true;
}

/*! set active
 * @param activate wheter to set active or not
 */
void CAVEWall::setActive(bool activate)
{
	m_active = activate;
}

/*! set gfx pipe
 * @param pipe string of pipe for wall
 * @return true if set ok
 */
bool CAVEWall::setPipe(const char *pipe)
{
	if (!pipe) {
		return false;
	}

	// if the pipe value passed in is just a single number it should
	// be prefixed with ":0."
	if ((pipe[0] == '0') || (atoi(pipe) > 0)) 
	{
		m_pipe = std::string(":0.").append(pipe);
	} 
	else 
	{
		m_pipe = std::string(pipe);
	}

	// could do some checking???
	return true;
}

/*! set if window or full screen
 * @param window if window
 */
void CAVEWall::setWindowed(bool windowed)
{
	m_windowed = windowed;
}

/*! parse and set window geometry
 * @param geometry string of containing geometry info
 * @return true if string is in correct format (WIDTHxHEIGHT+XOFFSET+YOFFSET)
 */
bool CAVEWall::parseGeometry(const std::string& geometryStr)
{
	if (const char* charXPos = std::strchr(geometryStr.c_str(), 'x'))
	{		
		int w = std::atoi(geometryStr.c_str());
		int h = std::atoi(charXPos+1);

		if ((w > 0) && (h > 0))
		{
			if (const char* char1stPlusPos = std::strchr(charXPos, '+')) 
			{
				int x = std::atoi(char1stPlusPos+1);
				if (const char* char2ndPlusPos = std::strchr(char1stPlusPos, '+')) 
				{
					int y = std::atoi(char2ndPlusPos+1);
					m_geometry.minX = x;
					m_geometry.minY = y;
					m_geometry.maxX = x + w;
					m_geometry.maxY = y + h;
					return true;
				}
			}
		}
	}
	printf("Invalid geometry config '%s'\n", geometryStr.c_str());
	return false;
}

/*! set display index
 * @param index index to use for this wall
 */
void CAVEWall::setPipeIndex(int index)
{
	m_pipeIndex = index;
}

/*! set projection data (the projection plane can be specified by three points, the required forth coordinate for the projection quadrilateral gets calculated)
 * @param lowerLeft coordinates of lower left corner of screen
 * @param upperLeft coordinates of upper left corner of screen
 * @param lowerRight coordinates of lower right corner of screen
 * @param convFact conversion factor to convert units (into cm)
 */
void CAVEWall::setProjectionData(const Pnt3f& lowerLeft, 
	const Pnt3f& upperLeft, 
	const Pnt3f& lowerRight, 
	float convFact)
{
	m_projData[0] = lowerLeft * convFact;
	m_projData[1] = lowerRight * convFact;
	m_projData[2] = (lowerRight + (upperLeft - lowerLeft)) * convFact;
	m_projData[3] = upperLeft * convFact;
}

/*! get name
 * @return name of wall
 */
std::string CAVEWall::getName() const
{
	return m_name;
}

/*! if wall active
 * @return if wall active
 */
bool CAVEWall::isActive() const
{
	return m_active;
}

/*! get pipe
 * @return string of wall pipe
 */
std::string CAVEWall::getPipe() const
{
	return m_pipe;
}

/*! get window
 * @return if a window
 */
bool CAVEWall::isWindowed() const
{
	return m_windowed;
}

bool CAVEWall::hasGeometry() const
{
	return m_geometry.maxX - m_geometry.minX > 0;
}

/*! get geometry minimum X geometry of wall
 * @return min X geometry
 */
int CAVEWall::getMinX() const
{
	return m_geometry.minX;
}

int CAVEWall::getMinY() const
{
	return m_geometry.minY;
}

int CAVEWall::getMaxX() const
{
	return m_geometry.maxX;
}

int CAVEWall::getMaxY() const
{
	return m_geometry.maxY;
}


/*! get wall projection data
 * @return array of points for projection data
 */
const Pnt3f* CAVEWall::getProjData() const
{
	return m_projData;
}

/*! get pipe index
 * @return pipe index
 */
int CAVEWall::getPipeIndex() const
{
	return m_pipeIndex;
}






