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

#ifndef _SIMULATIONOBJECTINTERFACE_H
#define _SIMULATIONOBJECTINTERFACE_H

#include <inVRs/SystemCore/Platform.h>
#include <string>
#include <ode/ode.h>
#include <inVRs/SystemCore/ClassFactory.h>

namespace oops
{

class Simulation;

class SimulationObjectInterface
{
protected:
	friend class Simulation;

	uint64_t id;
	std::string name;
	bool addedToSimulation;

// 	virtual void build(dWorldID world, dSpaceID space = 0) = 0;
// 	virtual void destroy() = 0;

public:
	SimulationObjectInterface() : id(0), name("-none-"), addedToSimulation(false) {};
	virtual ~SimulationObjectInterface() {};

	virtual void setID(uint64_t _id) { this->id = _id; };
	virtual void setName(std::string& name) { this->name = name; };

	virtual uint64_t getID() { return id; };
	virtual std::string getName() { return name; };

	virtual bool isAddedToSimulation() { return addedToSimulation; };
}; // SimulationObject

} // oops

#endif // _SIMULATIONOBJECTINTERFACE_H
