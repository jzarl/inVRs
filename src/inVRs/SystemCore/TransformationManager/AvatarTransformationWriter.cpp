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

#include "AvatarTransformationWriter.h"
#include "../DataTypes.h"

AvatarTransformationWriter::AvatarTransformationWriter(ArgumentVector* args) {
	useLocalUser = false;
	clipRotationToYAxis = false;
	if (args) {
		if (args->keyExists("useLocalUser"))
			args->get("useLocalUser", useLocalUser);
		if (args->keyExists("clipRotationToYAxis"))
			args->get("clipRotationToYAxis", clipRotationToYAxis);
	} // if
} // AvatarTransformationWriter

TransformationData AvatarTransformationWriter::execute(TransformationData* resultLastStage,
		TransformationPipe* currentPipe) {
	TransformationData handTrans, headTrans;
	TransformationData avatarTrans;
	gmtl::AxisAnglef axisAngle;
	float yAngle;
	gmtl::Vec3f yAxis;
	User* user;

	if (useLocalUser)
		user = UserDatabase::getLocalUser();
	else
		user = currentPipe->getOwner();

	AvatarInterface* avatarInt = user->getAvatar();

	avatarTrans = *resultLastStage;
	if (clipRotationToYAxis) {
		// calculate current view direction
		gmtl::Vec3f unitViewDir(0,0,-1);
		gmtl::Vec3f currentViewDir = avatarTrans.orientation * unitViewDir;
		// project view vector into x-z layer
		currentViewDir[1] = 0;
		// ignore update if view vector lies within Y-axis
		if (gmtl::length(currentViewDir) == 0) {
			return *resultLastStage;
		} // if
		// calculate angle between unitViewDir and currentViewDir
		gmtl::normalize(currentViewDir);
		yAngle = -acos(gmtl::dot(currentViewDir, unitViewDir));
		// calculate rotation axis for correct rotation direction (+/-)
		gmtl::cross(yAxis, currentViewDir, unitViewDir);
		gmtl::normalize(yAxis);
		// set new orientation
		gmtl::AxisAnglef finalAxAng(yAngle, yAxis);
		gmtl::set(avatarTrans.orientation, finalAxAng);
	} // if
	if (avatarInt) {
		avatarInt->setTransformation(avatarTrans);
//		if (writeAvatarHead) {
//			headTrans = user->getTrackedHeadTransformation();
//			avatarInt->setHeadTransformation(headTrans);
//		} // if
//		if (writeAvatarHand) {
//			handTrans = user->getTrackedHandTransformation();
//			handTrans.position[0] -= headTrans.position[0];
//			handTrans.position[2] -= headTrans.position[2];
//			avatarInt->setHandTransformation(handTrans);
//		} // if
	} // if

	return *resultLastStage;
} // execute

AvatarTransformationWriterFactory::AvatarTransformationWriterFactory() {
	className = "AvatarTransformationWriter";
}

TransformationModifier* AvatarTransformationWriterFactory::createInternal(ArgumentVector* args) {
	return new AvatarTransformationWriter(args);
} // create

bool AvatarTransformationWriterFactory::needInstanceForEachPipe() {
	return true;
}
