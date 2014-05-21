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

#include "TransformationDistributionModifier.h"
#include "../SystemCore.h"
#include "TransformationManager.h"
#include "../NetMessage.h"
#include "../XMLTools.h"

TransformationDistributionModifier::TransformationDistributionModifier(bool bUseTCP) {
	this->bUseTCP = bUseTCP;
	network = (NetworkInterface*)SystemCore::getModuleByName("Network");
} // TransformationDistributionModifier

TransformationData TransformationDistributionModifier::execute(TransformationData* resultLastStage,
		TransformationPipe* currentPipe) {
	NetMessage msg;

	if (network) {
		// 		msg.putUInt32(UserDatabase::getLocalUserId());
		msg.putUInt32(currentPipe->getOwner()->getId());
		msg.putUInt64(currentPipe->getPipeId());
		addTransformationToBinaryMsg(resultLastStage, &msg);

		//TODO: add timestamp to distribution!!!

		if (bUseTCP)
			network->sendMessageTCP(&msg, TRANSFORMATION_MANAGER_ID);
		else
			network->sendMessageUDP(&msg, TRANSFORMATION_MANAGER_ID);
	} // if

	return *resultLastStage;
} // execute

void TransformationDistributionModifier::useTCP() {
	bUseTCP = true;
} // useTCP

void TransformationDistributionModifier::useUDP() {
	bUseTCP = false;
} // useUDP

TransformationDistributionModifierFactory::TransformationDistributionModifierFactory() {
	className = "TransformationDistributionModifier";
}

TransformationModifier* TransformationDistributionModifierFactory::createInternal(
		ArgumentVector* args) {
	bool useTCP = true;
	std::string protocol;

	if (args && args->get("protocol", protocol)) {
		if (protocol == "UDP" || protocol == "udp" || protocol == "Udp")
			useTCP = false;
		return new TransformationDistributionModifier(useTCP);
	} // if

	return new TransformationDistributionModifier();
} // create

bool TransformationDistributionModifierFactory::needInstanceForEachPipeConfiguration() {
	return true;
}

