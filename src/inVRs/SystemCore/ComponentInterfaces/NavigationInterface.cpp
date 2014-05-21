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
#include <gmtl/AxisAngle.h>
#include <gmtl/MatrixOps.h>
#include <gmtl/Generate.h>

#include "NavigationInterface.h"
#include "../TransformationManager/TransformationManager.h"


NavigationInterface::NavigationInterface() {
	navigationPipe = NULL;
	// 	localUser = UserDatabase::getLocalUser();
	// 	assert(localUser);
}

NavigationInterface::~NavigationInterface() {
	// TRANSFORMATIONMANAGER CLOSES PIPE BY HIMSELF!!!
	// 	if(navigationPipe) TransformationManager::closePipe(navigationPipe);
}

void NavigationInterface::update(float dt) {
	if (!navigationPipe)
		navigationPipe = TransformationManager::openPipe(
				NAVIGATION_MODULE_ID, TRANSFORMATION_MANAGER_ID, 0, 0, 0, 0, 0x0E000000, false);

	assert(navigationPipe);

	NavigationInfo result = navigate(dt);
	TransformationData data, final;
	int i;

	data = identityTransformation();

	for (i = 0; i < 3; i++)
		data.position[i] = result.translation[i];

	data.orientation = result.rotation;
	data.scale = gmtl::Vec3f(1, 1, 1);

	// 	dumpVec(data.position, "position from navigation");
	// 	gmtl::Vec3f direction;
	// 	gmtl::Quatf invOrientation;
	// 	gmtl::Matrix44f invOrientationMat;
	// 	gmtl::Matrix44f orientationMat;
	// 	orientationMat = gmtl::MAT_IDENTITY44F;
	// 	setRot(orientationMat, data.orientation);
	// 	invertFull(invOrientationMat, orientationMat);
	// 	invOrientation = invert(data.orientation);
	// 	invOrientationMat = gmtl::MAT_IDENTITY44F;
	// 	setRot(invOrientationMat, invOrientation);
	// 	direction = orientationMat*gmtl::Vec3f(0, 0, 1);
	// // 	dumpVec(direction, "direction from navigation");
	//
	// 	multiply(final, data, localUser->getNavigatedTransformation());
	// 	final = data;

	navigationPipe->push_back(data);
	// 	localUser->setNavigatedTransformation(final);
}
