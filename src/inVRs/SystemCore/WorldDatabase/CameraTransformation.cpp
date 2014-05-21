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

#include "CameraTransformation.h"

#include <gmtl/MatrixOps.h>
#include <gmtl/QuatOps.h>
#include <gmtl/AxisAngle.h>
#include <gmtl/Generate.h>

CameraTransformation::CameraTransformation() {
	position = gmtl::Vec3f(0, 0, 0);
	orientation = gmtl::QUAT_IDENTITYF;
} // CameraTransformation

void CameraTransformation::setCameraTransformation(TransformationData& transfData) {
	gmtl::Matrix44f asMatrix, asInvMatrix;
	transformationDataToMatrix(transfData, asMatrix);
	invertFull(asInvMatrix, asMatrix);
	setCameraMatrix(asInvMatrix);
} // setCameraTransformation

void CameraTransformation::setCameraMatrix(gmtl::Matrix44f& mat) {
	gmtl::Matrix44f invMat;
	int i;
	// TODO: remove the comments?!
	// 	dumpVec(gmtl::Vec3f(mat[1][0], mat[1][1], mat[1][2]), "camera up");
	// 	dumpVec(gmtl::Vec3f(mat[2][0], mat[2][1], mat[2][2]), "camera forward");
	// 	dumpVec(gmtl::Vec3f(mat[3][0], mat[3][1], mat[3][2]), "zero row");

	invertFull(invMat, mat);
	for (i = 0; i < 3; i++) {
		position[i] = invMat[i][3];
		invMat[i][3] = 0.0f;
	}
	// 	transpose(invMat);
	set(orientation, invMat);
	normalize(orientation);

	gmtl::Vec3f direction;
	gmtl::Vec3f right;
	gmtl::Vec3f up;
	gmtl::Quatf invOrientation;

	// 	invOrientation = orientation;
	// 	invert(invOrientation);
	direction = orientation * gmtl::Vec3f(0, 0, -1);
	right = orientation * gmtl::Vec3f(1, 0, 0);
	up = orientation * gmtl::Vec3f(0, 1, 0);
	// 	dumpVec(position, "camera position");
	// 	dumpVec(direction, "camera direction");
	// 	dumpVec(up, "camera up");
	// 	dumpVec(right, "camera right");

	// 	int j;
	// 	float err = 0;
	// 	gmtl::Matrix44f test;
	// 	getCameraMatrix(test);
	// 	for(i=0;i<4;i++)
	// 		for(j=0;j<4;j++)
	// 			err += fabs(test(i,j)-mat(i,j));

	// 	printf("setCameraMatrix(): conversion error: %0.3f\n", err);
} // setCameraMatrix

void CameraTransformation::setPosition(gmtl::Vec3f position) {
	this->position = position;
} // setPosition

void CameraTransformation::setOrientation(gmtl::Quatf camOrientation) {
	this->orientation = camOrientation;
} // setOrientation

void CameraTransformation::getCameraTransformation(gmtl::Matrix44f& dst) {
	gmtl::Matrix44f rotMat;
	gmtl::Matrix44f translMat;

	rotMat = gmtl::MAT_IDENTITY44F;
	translMat = gmtl::MAT_IDENTITY44F;

	setTrans(translMat, position);
	setRot(rotMat, orientation);
	dst = translMat * rotMat;
} // getCameraTransformation

void CameraTransformation::getCameraMatrix(gmtl::Matrix44f& dst) {
	gmtl::Matrix44f rotMat;
	gmtl::Matrix44f translMat;

	rotMat = gmtl::MAT_IDENTITY44F;
	translMat = gmtl::MAT_IDENTITY44F;

	setTrans(translMat, -position);
	setRot(rotMat, orientation);
	transpose(rotMat);
	dst = rotMat * translMat;
} // getCameraMatrix

gmtl::Vec3f CameraTransformation::getPosition() {
	return position;
} // getPosition

gmtl::Quatf CameraTransformation::getOrientation() {
	return orientation;
} // getOrientation

gmtl::Vec3f CameraTransformation::getDirection() {
	return orientation * gmtl::Vec3f(0, 0, -1);
} // getDirection

