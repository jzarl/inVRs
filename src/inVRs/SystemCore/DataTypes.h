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

#ifndef _DATATYPES_H
#define _DATATYPES_H

#include <gmtl/AxisAngle.h>
#include <gmtl/Quat.h>
#include <gmtl/Matrix.h>
#include <gmtl/Generate.h>

#include "Platform.h"

#ifndef M_PI
#define M_PI 3.1415926535f
#endif

class Entity;

/**
 * Class for storing the inVRs version
 */
class INVRS_SYSTEMCORE_API Version {
public:
	enum STATUS {
		ALPHA=0, BETA=1, DEFAULT=2
	}; // VERSION_TYPE

	Version();
	Version(int majorVersion, int minorVersion, STATUS status = DEFAULT, int additionalVersion = 0);
	Version(std::string versionString);

	bool operator==(const Version& rhs) const;
	bool operator!=(const Version& rhs) const;
	bool operator<(const Version& rhs) const;
	bool operator<=(const Version& rhs) const;
	bool operator>(const Version& rhs) const;
	bool operator>=(const Version& rhs) const;

	bool parse(std::string versionString);

	void setMajorVersion(int majorVersion);
	void setMinorVersion(int minorVersion);
	void setStatus(STATUS status);
	void setAdditionalVersion(int additionalVersion);

	int getMajorVersion() const;
	int getMinorVersion() const;
	STATUS getStatus() const;
	int getAdditionalVersion() const;

	std::string toString() const;

private:
	int majorVersion;
	int minorVersion;
	STATUS status;
	int additionalVersion;
};

extern const Version UndefinedVersion;

struct AABB {
	gmtl::Vec3f p0, p1; // p0 should always be the minimum, p1 the maximum
};

struct SensorData {
	gmtl::Quatf orientation;
	gmtl::Vec3f position;
};

/**
 * Struct for storing general geometrical transformations.
 * It has been introduced because it can be used more intuitively than
 * matrices. An invertible transformation matrix (that is an invertible 4x4
 * matrix which has (0, 0, 0, 1) in the last row) can be converted into a
 * TransformationData and vice versa (though this conversion is not unique).
 * The matrix which corresponds to a TransformationData is obtained by applying
 * ('*') the components in the following order: (position * orientation *
 * inverse(scaleOrientation) * scale * scaleOrientation).
 *
 * A vector v transformed by a TransformationData is defined by multiplying the
 * equivalent matrix M with the vector v' = Mv (so first the vector is rotated
 * by the quaternion scaleOrientation, then it is scaled in the (x,y,z)
 * direction by the scale vector, afterwards its rotated by the inverse of the
 * scaleOrientation and so on).
 *
 * When two TransformationData are multiplied the resulting TransformationData
 * is the same as multiplying the both equivalent matrices.
 */
struct TransformationData {
	gmtl::Vec3f position;
	gmtl::Vec3f scale;
	gmtl::Quatf orientation;
	gmtl::Quatf scaleOrientation;
}; // TransformationData

/*
class TransformationData {
public:
	TransformationData() {};
	TransformationData(const gmtl::Vec3f& position, const gmtl::Vec3f& scale,
			const gmtl::Quatf& orientation, const gmtl::Quatf& scaleOrientation) :
				position(position),
				scale(scale),
				orientation(orientation),
				scaleOrientation(scaleOrientation) {}

	gmtl::Vec3f position;
	gmtl::Vec3f scale;
	gmtl::Quatf orientation;
	gmtl::Quatf scaleOrientation;
}; // TransformationData*/

/**
 * Converts SensorData into TransformationData (simply by setting scale to 1 and scaleOrientation to the identity quaternion)
 * @param src SensorData which should be converted
 * @param dst TransformationData where the result is written to
 * @return reference to dst
 */
INVRS_SYSTEMCORE_API TransformationData& convert(const SensorData& src, TransformationData& dst);

/**
 * Performs a matrix, matrix multiplication: C = A * B
 * @param first TransformationData describing matrix B
 * @param second TransformationData describing matrix A
 * @param dst TransformationData where the resulting TransformationData is written to
 * @return returns dst for conveniece
 */
INVRS_SYSTEMCORE_API TransformationData& multiply(TransformationData& dst,
		const TransformationData& second, const TransformationData& first);

/**
 * Converts a TransformationData into a matrix
 * @param src TransformationData which should be converted
 * @param dst matrix
 * @return returns reference to dst
 */
INVRS_SYSTEMCORE_API gmtl::Matrix44f& transformationDataToMatrix(TransformationData src,
		gmtl::Matrix44f& dst);

/**
 * Converts a matrix into a TransformationData.
 * In general it cannot be expected that <code>transformationDataToMatrix()</code> and <code>matrixToTransformationData()</code> are inverse to each other.
 * This method might be time consuming.
 * @param src matrix which should be converted
 * @param dst TransformationData where the result is written to
 * @return returns dst
 */
INVRS_SYSTEMCORE_API TransformationData& matrixToTransformationData(const gmtl::Matrix44f& src,
		TransformationData& dst);

/**
 * Inverts a TransformationData. Multiplying a TransformationData with its inverse results in an identity TransformationData.
 * @param dst TransformationData which will hold the inverse of src after call
 * @param src TransformationData which should be inverted
 * @return returns dst
 */
INVRS_SYSTEMCORE_API TransformationData& invert(TransformationData& dst,
		const TransformationData& src);

/**
 * Inverts a TransformationData. Multiplying a TransformationData with its inverse results in an identity TransformationData.
 * @param dst TransformationData which will be inverted (result will be written to dst)
 * @return returns dst
 */
inline TransformationData& invert(TransformationData& dst) {
	TransformationData tempSrc = dst;
	return invert(dst, tempSrc);
}

/**
 * Transforms a vector by a TransformationData transf. See TransformationData struct for a more detailed description of how a vector is transformed by a TransformationData.
 * @param dst transformed vector
 * @param transf TransformationData describing the transformation
 * @param src vector which should be transformed
 */
INVRS_SYSTEMCORE_API void transformVector(gmtl::Vec4f& dst, TransformationData& transf,
		gmtl::Vec4f& src);

INVRS_SYSTEMCORE_API void dumpTransformation(TransformationData& data);
INVRS_SYSTEMCORE_API std::string dumpTransformationToString(TransformationData& data,
		const char* prefix = "");
INVRS_SYSTEMCORE_API void dumpVec(gmtl::Vec3f vec, char* text);
INVRS_SYSTEMCORE_API void dumpMatrix(gmtl::Matrix44f& m);
INVRS_SYSTEMCORE_API void dumpMatrixDifference(gmtl::Matrix44f& m1, gmtl::Matrix44f& m2);

INVRS_SYSTEMCORE_API float gmtlDet3(gmtl::Matrix44f mat);
INVRS_SYSTEMCORE_API float distMatrix3x3(gmtl::Matrix44f mat1, gmtl::Matrix44f mat2);

INVRS_SYSTEMCORE_API gmtl::Quatf& setQuatfAsAxisAngleDeg(gmtl::Quatf& dst, float x, float y,
		float z, float deg);

template<typename DATA_TYPE>
gmtl::Quat<DATA_TYPE> operator*(const gmtl::Quat<DATA_TYPE>& q1, const gmtl::AxisAngle<DATA_TYPE>& q2) {
	gmtl::Quat<DATA_TYPE> result;
	gmtl::Quat<DATA_TYPE> quat;

	gmtl::set(quat, q2);
	result = q1 * quat;
	return result;
} // operator*

template<typename DATA_TYPE>
gmtl::Quat<DATA_TYPE> operator*(const gmtl::AxisAngle<DATA_TYPE>& q1, const gmtl::Quat<DATA_TYPE>& q2) {
	gmtl::Quat<DATA_TYPE> result;
	gmtl::Quat<DATA_TYPE> quat;

	gmtl::set(quat, q1);
	result = quat * q2;
	return result;
} // operator*


/**
 *	src = q * s * p
 *	where p, q orthogonal and s a diagonal matrix
 *	only the 3x3 part is considered
 */
INVRS_SYSTEMCORE_API void singularValueDecomposition(const gmtl::Matrix44f& src, gmtl::Matrix44f& q,
		gmtl::Matrix44f& s, gmtl::Matrix44f& p);

inline unsigned int join(unsigned short upper, unsigned short lower) {
	unsigned int result = upper;
	result = result << 16;
	result += lower;
	return result;
} // join

inline void split(unsigned int source, unsigned short& upper, unsigned short& lower) {
	upper = (unsigned int)(source >> 16);
	// 	lower = (unsigned int)(source - (((unsigned int)upper) << 16));
	lower = (unsigned short)(source & 0xFFFF);
} // split

inline uint64_t join(uint32_t upper, uint32_t lower) {
	uint64_t result = upper;
	result = result << 32;
	result += lower;
	return result;
} // join

inline void split(uint64_t source, uint32_t& upper, uint32_t& lower) {
	upper = (uint32_t)(source >> 32);
	lower = (uint32_t)(source & 0xFFFFFFFF);
}

INVRS_SYSTEMCORE_API TransformationData identityTransformation();
INVRS_SYSTEMCORE_API bool isIdentityTransformation(TransformationData& toCmp);

INVRS_SYSTEMCORE_API SensorData identitySensorData();

INVRS_SYSTEMCORE_API float getMinDistanceToAABBox(gmtl::Vec3f pos, Entity* entity);

INVRS_SYSTEMCORE_API bool isInside(gmtl::Vec3f pos, Entity* entityTrans);
INVRS_SYSTEMCORE_API Entity* smallerBBox(Entity* e1, Entity *e2);

INVRS_SYSTEMCORE_API unsigned int generateHashCode(std::string name);

static const TransformationData IdentityTransformation = identityTransformation();
static const SensorData IdentitySensorData = identitySensorData();

#endif

