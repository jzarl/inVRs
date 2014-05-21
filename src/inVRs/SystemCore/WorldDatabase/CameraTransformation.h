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

#ifndef _CAMERATRANSFORMATION_H
#define _CAMERATRANSFORMATION_H

#include <gmtl/Matrix.h>
#include <gmtl/Quat.h>

#include "../DataTypes.h"

/******************************************************************************
 * The class is used to store the transformation of the camera. It provides 
 * setter-methods to set the camera transformation by position and orientation 
 * and getter-methods to get either the camera transformation or to get the 
 * camera matrix (which is the matrix from world into camera space).
 * The camera matrix looks like:
 *
 * transformation matrix from world into camera space
 *
 *	r.x  r.y  r.z  -<r,p>
 *	u.x  u.y  u.z  -<u,p>
 *	-d.x -d.y -d.z  <d,p>
 *	 0    0    0     1
 *
 *	where:
 *	r = right, u = up, d = view direction, p = position
 */
class INVRS_SYSTEMCORE_API CameraTransformation {
public:
	/** 
	 * The constructor initializes the position and orientation of the camera
	 */
	CameraTransformation();

	/** 
	 * The method sets the camera transformation. The passed TransformationData 
	 * must not contain a scale value!
	 * @param transfData transformation of the camera.
	 */
	void setCameraTransformation(TransformationData& transfData); // from camera space into world space (= transformation of a camera object)

	/** 
	 * The method sets the camera matrix. The camera matrix is the transformation 
	 * matrix from world to camera space. If you want to set the camera's position 
	 * and orientation then use the setTransformation or setPosition and 
	 * setOrientation methods. The passed Matrix must not contain a scale factor.
	 * @param mat camera matrix (transformation matrix from world to camera space)
	 */
	void setCameraMatrix(gmtl::Matrix44f& mat); // from world into camera space

	/** 
	 * The method sets the position of the camera.
	 * @param position position of the camera
	 */
	void setPosition(gmtl::Vec3f position);

	/** 
	 * The method sets the orientation of the camera.
	 * @param camOrientation orientation of the camera
	 */
	void setOrientation(gmtl::Quatf camOrientation);

	/** 
	 * The method returns the camera transformation of the camera by storing the 
	 * value in the passed matrix.
	 * @param dst matrix to write the camera transformation into
	 */
	void getCameraTransformation(gmtl::Matrix44f& dst);

	/** 
	 * The method returns the camera matrix of the camera (which is the matrix 
	 * from world to camera space) by storing the value in the passed matrix.
	 * @param dst matrix to write the camera matrix into.
	 */
	void getCameraMatrix(gmtl::Matrix44f& dst);

	/** 
	 * The method returns the position of the camera.
	 * @return position of the camera
	 */
	gmtl::Vec3f getPosition();

	/** 
	 * The method returns the orientation of the camera.
	 * @return position of the camera
	 */
	gmtl::Quatf getOrientation();

	/** 
	 * The method returns the view direction of the camera. It assumes that the 
	 * view direction of an unrotated camera is (0, 0, -1)
	 * @return vector with view direction of camera
	 */
	gmtl::Vec3f getDirection();

protected:
	gmtl::Vec3f position;
	gmtl::Quatf orientation;
};

#endif
