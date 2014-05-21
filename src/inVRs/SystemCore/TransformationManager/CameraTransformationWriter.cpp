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

#include <assert.h>

#include <gmtl/Xforms.h>

#include "CameraTransformationWriter.h"
#include "../DataTypes.h"
#include "../WorldDatabase/CameraTransformation.h"
#include "../UserDatabase/UserDatabase.h"

CameraTransformationWriter::CameraTransformationWriter(float cameraHeight, bool useLocalUser,
		bool useGlobalYAxis) {
	this->cameraHeight = cameraHeight;
	this->useLocalUser = useLocalUser;
	this->useGlobalYAxis = useGlobalYAxis;
} // CameraTransformationWriter

TransformationData CameraTransformationWriter::execute(TransformationData* resultLastStage,
		TransformationPipe* currentPipe) {
	User* user;
	TransformationData result = *resultLastStage;
	gmtl::Vec3f upVec = gmtl::Vec3f(0, cameraHeight, 0);

	if (useLocalUser)
		user = UserDatabase::getLocalUser();
	else
		user = currentPipe->getOwner();

	assert(user);
	CameraTransformation* camTrans = user->getCamera();
	assert(camTrans);

	if (!useGlobalYAxis)
		upVec = result.orientation * upVec;
	result.position += upVec;

	camTrans->setCameraTransformation(result);

	return result;
} // execute

CameraTransformationWriterFactory::CameraTransformationWriterFactory() {
	className = "CameraTransformationWriter";
}

TransformationModifier* CameraTransformationWriterFactory::createInternal(ArgumentVector* args) {
	float cameraHeight = 0;
	bool useLocalUser = false;
	bool useGlobalYAxis = false;

	if (!args)
		return new CameraTransformationWriter(cameraHeight, useLocalUser, useGlobalYAxis);

	args->get("cameraHeight", cameraHeight);
	args->get("useLocalUser", useLocalUser);
	args->get("useGlobalYAxis", useGlobalYAxis);

	return new CameraTransformationWriter(cameraHeight, useLocalUser, useGlobalYAxis);
} // create

bool CameraTransformationWriterFactory::needInstanceForEachPipe() {
	return true;
}
