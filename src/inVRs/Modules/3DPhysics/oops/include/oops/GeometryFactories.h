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
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#ifndef _GEOMETRYFACTORIES_H
#define _GEOMETRYFACTORIES_H

#include "Geometries.h"
#include <inVRs/SystemCore/ClassFactory.h>
#include <inVRs/SystemCore/XmlElement.h>
#ifndef INVRS_BUILD_TIME
#include "configOops.h"
#endif

// Object Oriented Physics Simulation
namespace oops
{

class XMLLoader;

class GeometryFactory : public ClassFactory<Geometry, const XmlElement*>
{
public:
	GeometryFactory(){};
	virtual ~GeometryFactory(){};
}; // GeometryFactory

//*****************************************************************************

class BoxFactory : public GeometryFactory
{
public:
	virtual Geometry* create(std::string className, const XmlElement* element);
}; // BoxFactory

//*****************************************************************************

class SphereFactory : public GeometryFactory
{
public:
	virtual Geometry* create(std::string className, const XmlElement* element);
}; // SphereFactory

//*****************************************************************************

class PlaneFactory : public GeometryFactory
{
public:
	virtual Geometry* create(std::string className, const XmlElement* element);
}; // PlaneFactory

//*****************************************************************************

class CylinderFactory : public GeometryFactory
{
public:
	virtual Geometry* create(std::string className, const XmlElement* element);
}; // CylinderFactory

//*****************************************************************************

class CapsuleFactory : public GeometryFactory
{
public:
	virtual Geometry* create(std::string className, const XmlElement* element);
}; // CapsuleFactory

//*****************************************************************************

class CompositeGeometryFactory : public GeometryFactory
{
public:
	CompositeGeometryFactory(XMLLoader* xmlLoader);
	virtual Geometry* create(std::string className, const XmlElement* element);
protected:
	CompositeGeometryFactory() {};
	XMLLoader* xmlLoader;
}; // CompositeGeometryFactory

//*****************************************************************************

class HeightFieldFactory : public GeometryFactory
{
public:
	HeightFieldFactory(XMLLoader* xmlLoader);
	
	virtual Geometry* create(std::string className, const XmlElement* element);
	
protected:
	HeightFieldFactory() {};
	
	XMLLoader* xmlLoader;
}; // HeightFieldFactory

//*****************************************************************************

class TriangleMeshFactory : public GeometryFactory
{
public:
	TriangleMeshFactory(XMLLoader* xmlLoader);
	virtual Geometry* create(std::string className, const XmlElement* element);
	
protected:
	TriangleMeshFactory() {};
	
	XMLLoader* xmlLoader;
}; // TriangleMeshFactory

//*****************************************************************************

} // oops

#endif // _GEOMETRYFACTORIES_H
