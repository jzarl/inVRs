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

#include "AvatarAnimationWriter.h"
#include "AvataraAvatarBase.h"

#include <inVRs/SystemCore/DataTypes.h>
#include <inVRs/SystemCore/Timer.h>
#include <inVRs/SystemCore/DebugOutput.h>

AvatarAnimationWriter::AvatarAnimationWriter(float walkingSpeed, bool useLocalUser) {
	int i;
	this->useLocalUser = useLocalUser;
	this->walkingSpeed = walkingSpeed;

	for (i = 0; i < 5; i++)
		oldTrans[i] = identityTransformation();

	lastTime = 0;
	walking = false;
} // AvatarAnimationWriter

TransformationData AvatarAnimationWriter::execute(TransformationData* resultLastStage,
		TransformationPipe* currentPipe) {
	int i;
	User* user;
	TransformationData currentTrans = *resultLastStage;
	gmtl::Vec3f posDiff;
	float currentTime = timer.getTime();
	float currentSpeed;

	if (lastTime == 0 || currentTime <= lastTime) {
		lastTime = currentTime;
		return *resultLastStage;
	} // if

	if (useLocalUser)
		user = UserDatabase::getLocalUser();
	else
		user = currentPipe->getOwner();

	AvataraAvatarBase* avatarInt = dynamic_cast<AvataraAvatarBase*> (user->getAvatar());
	if (avatarInt) {
		posDiff = currentTrans.position - oldTrans[0].position;
		currentSpeed = gmtl::length(posDiff) / (currentTime - lastTime);
		if (currentSpeed >= walkingSpeed && !walking) {
			printd("AvatarAnimationWriter::execute(): starting WALKING: speed = %f (>= %f)\n",
					currentSpeed, walkingSpeed);
			avatarInt->startAnimation("walking", 400);
			walking = true;
		} // if
		else if (currentSpeed < walkingSpeed && walking) {
			printd("AvatarAnimationWriter::execute(): stopping WALKING: speed = %f\n", currentSpeed);
			avatarInt->startAnimation("standing", 400);
			walking = false;
		} // else if
		 //else if (walking)
		 //printd("AvatarAnimationWriter::execute(): WALKING: walking speed = %f\n", gmtl::length(posDiff));
		 //else if (!walking)
		 //printd("AvatarAnimationWriter::execute(): NOT_WALKING: walking speed = %f\n", gmtl::length(posDiff));
	} // if

	for (i = 1; i < 5; i++) {
		oldTrans[i] = oldTrans[i - 1];
	} // for
	oldTrans[0] = currentTrans;

	lastTime = currentTime;

	return *resultLastStage;
} // execute

AvatarAnimationWriterFactory::AvatarAnimationWriterFactory() {
	className = "AvatarAnimationWriter";
}

TransformationModifier* AvatarAnimationWriterFactory::createInternal(ArgumentVector* args) {
	bool useLocalUser = false;
	float walkingSpeed = 1;

	if (!args)
		return new AvatarAnimationWriter();

	args->get("walkingSpeed", walkingSpeed);
	args->get("useLocalUser", useLocalUser);

	return new AvatarAnimationWriter(walkingSpeed, useLocalUser);
} // create

bool AvatarAnimationWriterFactory::needInstanceForEachPipe() {
	return true;
}
