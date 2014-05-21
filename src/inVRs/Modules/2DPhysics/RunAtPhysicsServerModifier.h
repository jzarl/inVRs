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

#ifndef _RUNATPHYSICSSERVERMODIFIER_H
#define _RUNATPHYSICSSERVERMODIFIER_H

#include <inVRs/SystemCore/TransformationManager/TransformationModifierFactory.h>

class RunAtPhysicsServerModifier : public TransformationModifier
{
protected:
	bool isServer;
	NetworkInterface* network;
	TransformationPipe* dstPipe;

public:
	RunAtPhysicsServerModifier(bool isServer);
	virtual bool createNewInstanceForEachPipe();
	virtual std::string getClassName();
	virtual TransformationData execute(TransformationData* resultLastStage, TransformationPipe* currentPipe);
}; // RunAtPhysicsServerModifier

class RunAtPhysicsServerModifierFactory : public TransformationModifierFactory
{
protected:
	bool isServer;
public:
	RunAtPhysicsServerModifierFactory(bool isServer);
	virtual TransformationModifier* create(std::string className, ArgumentVector* args = NULL);
}; // RunAtPhysicsServerModifierFactory

#endif // _RUNATPHYSICSSERVERMODIFIER_H
