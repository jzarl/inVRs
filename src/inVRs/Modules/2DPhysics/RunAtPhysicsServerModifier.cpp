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

#include "RunAtPhysicsServerModifier.h"

#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>
#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>
#include <inVRs/SystemCore/SystemCore.h>
#include <assert.h>

RunAtPhysicsServerModifier::RunAtPhysicsServerModifier(bool isServer)
{
	this->isServer = isServer;
	network = (NetworkInterface*)SystemCore::getModuleByName("Network");
	dstPipe = NULL;
} // ApplyForceModifier

TransformationData RunAtPhysicsServerModifier::execute(TransformationData* resultLastStage, TransformationPipe* currentPipe)
{
	NetMessage msg;
	unsigned srcId, dstId, pipeType, objectClass, objectType, objectId, priority;
	bool fromNetwork;

// 	printd("running for user %u\n", currentPipe->getOwner()->getId());

	assert(network);
	if (currentPipe->getOwner() == UserDatabase::getLocalUser() && !isServer)
	{
		msg.putUInt32(currentPipe->getOwner()->getId());
		msg.putUInt64(currentPipe->getPipeId());
		addTransformationToBinaryMsg(resultLastStage, &msg);
		network->sendMessageUDP(&msg, TRANSFORMATION_MANAGER_ID);
	} // if
	
	if (isServer && !dstPipe)
	{
		TransformationManager::unpackPipeId(currentPipe->getPipeId(), &srcId, &dstId, &pipeType, &objectClass, &objectType, &objectId, &fromNetwork);
		pipeType++;
		priority = currentPipe->getPriority();
/*		if (priority >= 16)
			priority -= 16;*/
		fromNetwork = false;
		dstPipe = TransformationManager::openPipe(srcId, dstId, pipeType, objectClass, objectType, objectId, priority, fromNetwork, currentPipe->getOwner());
	} // if
	else if (!isServer && !dstPipe)
	{
		TransformationManager::unpackPipeId(currentPipe->getPipeId(), &srcId, &dstId, &pipeType, &objectClass, &objectType, &objectId, &fromNetwork);
		pipeType++;
		priority = currentPipe->getPriority();
/*		if (priority >= 16)
			priority -= 16;*/
		fromNetwork = true;
		dstPipe = TransformationManager::openPipe(srcId, dstId, pipeType, objectClass, objectType, objectId, priority, fromNetwork, currentPipe->getOwner());
	} // else if

	if (isServer)
	{
		assert(dstPipe);
		dstPipe->push_back(*resultLastStage);
	} // if

	return *resultLastStage;
} // execute

bool RunAtPhysicsServerModifier::createNewInstanceForEachPipe()
{
	return true;
} // createNewInstanceForEachPipe

std::string RunAtPhysicsServerModifier::getClassName()
{
	return "RunAtPhysicsServerModifier";
} // getClassName

RunAtPhysicsServerModifierFactory::RunAtPhysicsServerModifierFactory(bool isServer)
{
	this->isServer = isServer;
} // RunAtPhysicsServerModifierFactory

TransformationModifier* RunAtPhysicsServerModifierFactory::create(std::string className, ArgumentVector* args)
{
	if (className != "RunAtPhysicsServerModifier")
		return NULL;
	
	return new RunAtPhysicsServerModifier(isServer);
} // create
