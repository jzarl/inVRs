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

#include "SharedManipulationMerger.h"

#include <assert.h>
#include <gmtl/Generate.h>

#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/SystemCore/Platform.h>

SharedManipulationMerger::SharedManipulationMerger() {
	singleUser = true;
}

TransformationData SharedManipulationMerger::calculateResult() {
	TransformationData result;
	unsigned temp;
	unsigned short objectType;
	unsigned short objectId;
	unsigned entityId;
	unsigned tempObjectType, tempObjectId;
	bool tempBool;
	uint64_t pipeId = inputPipeList[0].pipe->getPipeId();
	TransformationManager::unpackPipeId(pipeId, &temp, &temp, &temp, &temp, &tempObjectType,
			&tempObjectId, &tempBool);
	objectType = (unsigned short)tempObjectType;
	objectId = (unsigned short)tempObjectId;
	entityId = join(objectType, objectId);
	Entity* ent;

	if (inputPipeList.size() == 1) {
		// do what offset modifier does

		TransformationData resultLastStage;
		User* user = inputPipeList[0].pipe->getOwner();
		TransformationData offset = user->getAssociatedEntityOffset(entityId);
		resultLastStage = inputPipeList[0].transf;
		multiply(result, resultLastStage, offset);

		// 		dumpVec(resultLastStage.position, "SharedManipulationMerger: cursor transf");
		// 		dumpVec(offset.position, "SharedManipulationMerger: picking offset");
		// 		assert(false);

		singleUser = true;
	} else if (singleUser) {
		// got two users the first time
		assert(inputPipeList.size() == 2);

		gmtl::Vec3f center;
		gmtl::Vec3f cursor[2];
		TransformationData entTransf;
		int i;

		for (i = 0; i < 2; i++)
			cursor[i] = inputPipeList[i].transf.position;

		center = (cursor[0] + cursor[1]) * 0.5f;
		ent = WorldDatabase::getEntityWithTypeInstanceId(objectType, objectId);
		assert(ent);

		entTransf = ent->getWorldTransformation();
		scale = entTransf.scale;
		dumpTransformation(entTransf);
		dEntityOrigin = entTransf.position - center;
		axis = cursor[1] - cursor[0];
		gmtl::normalize(axis);
		orientation = entTransf.orientation;

		if (gmtl::length(cursor[0]) < 0.001f || gmtl::length(cursor[1]) < 0.001f) {
			assert(false);
		}
		singleUser = false;
		result = entTransf;

	} else {
		// multiuser
		gmtl::Vec3f center;
		gmtl::Vec3f cursor[2];
		gmtl::Vec3f axisNew;
		gmtl::Vec3f normal;
		gmtl::AxisAnglef rotAngle;
		gmtl::Quatf rotq;
		TransformationData entTransf;
		float dotProduct;
		float angle;
		int i;

		for (i = 0; i < 2; i++)
			cursor[i] = inputPipeList[i].transf.position;

		center = (cursor[0] + cursor[1]) * 0.5f;

		axisNew = cursor[1] - cursor[0];
		gmtl::normalize(axisNew);

		// 		printd("Normal = %f %f %f\n", normal[0], normal[1], normal[2]);
		// 		printd("angle = %f\n", angle);

		dotProduct = gmtl::dot(axis, axisNew);
		// 		if(fabs(dotProduct) > 1)
		// 		{
		// 			printd("axis are very similar\n");
		// 			// gmtl::Vec3f li = axis + gmtl::Vec3f(1, 0, 0);
		// 			gmtl::Vec3f li;
		// 			gmtl::Quatf invOri;
		// 			gmtl::Matrix44f rotm;
		//
		// 			invOri = orientation;
		// 			invert(invOri);
		// 			li = invOri * gmtl::Vec3f(1, 0, 0);
		// 			normalize(li);
		// 			if(fabs(gmtl::dot(li, axis)) >= 0.99999)
		// 			{
		// 				printd("ohoh right and axis are not linear independent\n");
		// 				li = invOri * gmtl::Vec3f(0, 0, 1);
		// 				normalize(li);
		// 				assert(fabs(gmtl::dot(li, axis)) < 1);
		// 			}
		//
		// // 			normalize(li);
		// 			gmtl::cross(normal, axis, li);
		// 			normalize(normal);
		// 			gmtl::cross(li, normal, axis);
		// 			normalize(li);
		//
		// 			rotm.set(	li[0], li[1], li[2], 0,
		// 						normal[0], normal[1], normal[2], 0,
		// 						axis[0], axis[1], axis[2], 0,
		// 						0, 0, 0, 1);
		//
		// 			assert(gmtl::length(li) < 1.01);
		// 			assert(gmtl::length(normal) < 1.01);
		// 			assert(gmtl::length(axis) < 1.01);
		//
		// 			printd("det of rotm: %0.03f\n", gmtlDet3(rotm));
		// 			gmtl::set(rotq, rotm);
		//
		// 		} else
		// 		{
		// 			gmtl::cross(normal, axis, axisNew);
		// 			gmtl::normalize(normal);
		// 			angle = acos(dotProduct);
		// 			rotAngle = gmtl::AxisAnglef(angle, normal);
		// 			gmtl::set(rotq, rotAngle);
		// 		}

		gmtl::cross(normal, axis, axisNew);
		gmtl::normalize(normal);
		if (gmtl::length(normal) < 0.99999) {
			printd("bad normal: %0.04f\n", gmtl::length(normal));
			normal = gmtl::Vec3f(1, 0, 0);
			angle = 0;
			rotq = gmtl::QUAT_IDENTITYF;
		} // if
		else {
			dotProduct = gmtl::dot(axis, axisNew);
			if (dotProduct > 1)
				dotProduct = 1;
			if (dotProduct < -1)
				dotProduct = -1;
			angle = acos(dotProduct);
			// 			printd("Normal length = %f\n", gmtl::length(normal));
			// 			printd("Angle = %f\n", angle);
			rotAngle = gmtl::AxisAnglef(angle, normal);
			gmtl::set(rotq, rotAngle);
			// 			printd("Quaternion = %f %f %f %f\n", rotq[0], rotq[1], rotq[2], rotq[3]);
		} // else

		dEntityOrigin = rotq * dEntityOrigin;
		orientation = rotq * orientation;
		axis = axisNew;

		result = identityTransformation();
		result.position = center + dEntityOrigin;
		result.orientation = orientation;
		result.scale = scale;

		// 		dumpTransformation(result);

#if not defined(MACOS_X) && not defined(__APPLE_CC__)
		if (isnan(result.position[0])) {
			dumpTransformation(result);
			assert(false);
		}
#endif
	} // else

	return result;
}

std::string SharedManipulationMerger::getName() {
	return "SharedManipulationMerger";
}

int SharedManipulationMerger::getMaximumNumberOfInputPipes() {
	return 2;
}

TransformationMerger* SharedManipulationMergerFactory::create(std::string className, void* args) {
	if (className == "SharedManipulationMerger")
		return new SharedManipulationMerger();
	return NULL;
}

