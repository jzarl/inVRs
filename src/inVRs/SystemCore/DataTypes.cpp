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

#include "DataTypes.h"

#include <sstream>
#include <math.h>
#include <limits>
#include <assert.h>
#include <stdlib.h>

#include <gmtl/AxisAngle.h>
#include <gmtl/MatrixOps.h>
#include <gmtl/Generate.h>
#include <gmtl/Numerics/Eigen.h>
#include <gmtl/VecOps.h>
#include <gmtl/Xforms.h>

#include "DebugOutput.h"
#include "WorldDatabase/Entity.h"

const Version UndefinedVersion = Version();

Version::Version() :
	majorVersion(0),
	minorVersion(0),
	status(DEFAULT),
	additionalVersion(0) {

}

Version::Version(int majorVersion, int minorVersion, STATUS status, int additionalVersion) :
	majorVersion(majorVersion),
	minorVersion(minorVersion),
	status(status),
	additionalVersion(additionalVersion) {
}

Version::Version(std::string versionString) :
	majorVersion(0),
	minorVersion(0),
	status(DEFAULT),
	additionalVersion(0) {
	parse(versionString);
} // Version

bool Version::parse(std::string versionString) {
	std::string majorVersionString;
	std::string minorVersionString;
	std::string additionalVersionString;

	// remove v at beginning if existing
	if (versionString[0] == 'v')
		versionString.erase(versionString.begin());

	size_t dotIndex = versionString.find('.');
	size_t alphaIndex = versionString.find('a');
	size_t betaIndex = versionString.find('b');
	size_t dotIndexTest = versionString.find('.', dotIndex+1);

	if (dotIndex == std::string::npos ||
			(dotIndexTest != std::string::npos) ||
			(alphaIndex != std::string::npos && betaIndex != std::string::npos) ||
			(alphaIndex < dotIndex) ||
			(betaIndex < dotIndex) ||
			(alphaIndex == versionString.size() - 1) ||
			(betaIndex == versionString.size() - 1)) {
		printd(WARNING, "Version::Version(): invalid version string %s passed!\n", versionString.c_str());
		return false;
	} // if
	else {
		majorVersionString = versionString.substr(0, dotIndex);
		if (alphaIndex != std::string::npos) {
			status = ALPHA;
			int length = alphaIndex - (dotIndex+1);
			minorVersionString = versionString.substr(dotIndex+1, length);
			additionalVersionString = versionString.substr(alphaIndex+1);
		} // if
		else if (betaIndex != std::string::npos) {
			status = BETA;
			int length = betaIndex - (dotIndex+1);
			minorVersionString = versionString.substr(dotIndex+1, length);
			additionalVersionString = versionString.substr(betaIndex+1);
		} // if
		else {
			status = DEFAULT;
			minorVersionString = versionString.substr(dotIndex+1);
		} // else
		majorVersion = atoi(majorVersionString.c_str());
		minorVersion = atoi(minorVersionString.c_str());
		if (status != DEFAULT)
			additionalVersion = atoi(additionalVersionString.c_str());
	} // else
	return true;
} // parse


bool Version::operator==(const Version& rhs) const {
	return (majorVersion == rhs.majorVersion &&
			minorVersion == rhs.minorVersion &&
			status == rhs.status &&
			additionalVersion == rhs.additionalVersion);
} // operator==

bool Version::operator!=(const Version& rhs) const {
	return !(operator==(rhs));
} // operator!=

bool Version::operator<(const Version& rhs) const {
	return (majorVersion < rhs.majorVersion ||
			(majorVersion == rhs.majorVersion && minorVersion < rhs.minorVersion) ||
			(majorVersion == rhs.majorVersion && minorVersion == rhs.minorVersion &&
					status < rhs.status) ||
			(majorVersion == rhs.majorVersion && minorVersion == rhs.minorVersion &&
					status == rhs.status && additionalVersion < rhs.additionalVersion));
} // operator<

bool Version::operator<=(const Version& rhs) const {
	return !(operator>(rhs));
} // operator<=

bool Version::operator>(const Version& rhs) const {
	return (majorVersion > rhs.majorVersion ||
			(majorVersion == rhs.majorVersion && minorVersion > rhs.minorVersion) ||
			(majorVersion == rhs.majorVersion && minorVersion == rhs.minorVersion &&
					status > rhs.status) ||
			(majorVersion == rhs.majorVersion && minorVersion == rhs.minorVersion &&
					status == rhs.status && additionalVersion > rhs.additionalVersion));
} // operator>

bool Version::operator>=(const Version& rhs) const {
	return !(operator<(rhs));
} // operator>=

void Version::setMajorVersion(int majorVersion) {
	this->majorVersion = majorVersion;
} // setMajorVersion

void Version::setMinorVersion(int minorVersion) {
	this->minorVersion = minorVersion;
} // setMinorVersion

void Version::setStatus(STATUS status) {
	this->status = status;
} // setStatus

void Version::setAdditionalVersion(int additionalVersion) {
	this->additionalVersion = additionalVersion;
} // setAdditionalVersion

int Version::getMajorVersion() const {
	return majorVersion;
} // getMajorVersion

int Version::getMinorVersion() const {
	return minorVersion;
} // getMinorVersion

Version::STATUS Version::getStatus() const {
	return status;
} //getStatus

int Version::getAdditionalVersion() const {
	return additionalVersion;
} // getAdditionalVersion

std::string Version::toString() const {
	std::stringstream result;
	result << majorVersion << "." << minorVersion;
	if (status == ALPHA) {
		result << "a" << additionalVersion;
	} // if
	else if (status == BETA) {
		result << "b" << additionalVersion;
	} // else if
	return result.str();
} // toString


//#define TEST_SVD

INVRS_SYSTEMCORE_API TransformationData& convert(const SensorData& src, TransformationData& dst) {
	dst.scale = gmtl::Vec3f(1, 1, 1);
	dst.position = src.position;
	dst.orientation = src.orientation;
	dst.scaleOrientation = gmtl::QUAT_IDENTITYF;
	return dst;
} // convert

INVRS_SYSTEMCORE_API TransformationData& multiply(TransformationData& dst,
		const TransformationData& second, const TransformationData& first) {
	if (second.scale == gmtl::Vec3f(1, 1, 1)) {
		dst.position = second.orientation * first.position;
		dst.position += second.position;
		dst.scaleOrientation = first.scaleOrientation;
		dst.scale = first.scale;
		dst.orientation = second.orientation * first.orientation;
		// 		printd(ERROR, "doing fast multiply\n");
	} else {
		gmtl::Matrix44f f, s, temp;
		transformationDataToMatrix(first, f);
		transformationDataToMatrix(second, s);
		temp = s * f;
		matrixToTransformationData(temp, dst);
		// 		printd(ERROR, "doing svd\n");
	}

#ifdef TEST_SVD
	gmtl::Matrix44f test;
	transformationDataToMatrix(dst, test);
	transformationDataToMatrix(first, f);
	transformationDataToMatrix(second, s);
	temp = s*f;

	float dist;
	dist = distMatrix3x3(test, temp);
	if(dist>0.01)
	printd(ERROR, "multiply(): made an error of %0.5f, used%sSVD\n", dist, usedSVD ? " " : " not ");
#endif

	return dst;
}


INVRS_SYSTEMCORE_API gmtl::Matrix44f& transformationDataToMatrix(TransformationData src,
		gmtl::Matrix44f& dst) {
	gmtl::Matrix44f scale;
	gmtl::Matrix44f trans;
	gmtl::Matrix44f scaleOri;
	gmtl::AxisAnglef ori;
	gmtl::Matrix44f invScaleOri;

	// 	setRot(ori, src.orientation);
	dst = gmtl::MAT_IDENTITY44F;
	scaleOri = gmtl::MAT_IDENTITY44F;
	gmtl::normalize(src.orientation);
	gmtl::normalize(src.scaleOrientation);
	setRot(scaleOri, src.scaleOrientation);
	setRot(dst, src.orientation);
	scale = gmtl::MAT_IDENTITY44F;
	setScale(scale, src.scale);
	trans = gmtl::MAT_IDENTITY44F;
	setTrans(trans, src.position);
	invScaleOri = scaleOri;
	gmtl::transpose(invScaleOri);

	dst = trans * dst;
	dst = dst * invScaleOri;
	dst = dst * scale;
	dst = dst * scaleOri;
	// 	setTrans(dst, src.position);

	return dst;
}

INVRS_SYSTEMCORE_API TransformationData& matrixToTransformationData(const gmtl::Matrix44f& src,
		TransformationData& dst) {
	int i;
	gmtl::Matrix44f q, s, p;
	for (i = 0; i < 3; i++) {
		dst.position[i] = src(i, 3);
	}
	singularValueDecomposition(src, q, s, p);
	// 	printf("det p = %0.03f\n", gmtlDet3(p));
	if (gmtlDet3(p) < 0) {
		s(0, 0) = -s(0, 0);
		for (i = 0; i < 3; i++)
			p[0][i] = -p[0][i]; // should be a row vector
			// 		printf("det p_after = %0.03f\n", gmtlDet3(p));
	}
	// 	printf("det q = %0.03f\n", gmtlDet3(q));
	if (gmtlDet3(q) < 0) {
		s(1, 1) = -s(1, 1);
		transpose(q);
		for (i = 0; i < 3; i++)
			q[1][i] = -q[1][i];
		transpose(q);
		// 		printf("det q_after = %0.03f\n", gmtlDet3(q));
	}

	//	setRot(dst.orientation, q);
	setRot(dst.orientation, q * p);
	gmtl::normalize(dst.orientation);
	setRot(dst.scaleOrientation, p);
	gmtl::normalize(dst.scaleOrientation);

	if ((s[0][0] < 0) && (s[1][1] < 0)) {
		// 		printd("matrixToTransformationData(): special case 0\n");
		gmtl::Matrix44f rotadj;
		// 		gmtl::Quatf rotadjquat;
		rotadj = gmtl::MAT_IDENTITY44F;
		setScale(rotadj, gmtl::Vec3f(-1.0, -1.0, 1.0));
		// 		setRot(rotadjquat, rotadj);
		setRot(dst.orientation, q * rotadj * p);
		dst.scale = gmtl::Vec3f(-s[0][0], -s[1][1], s[2][2]);
	} else if ((s[0][0] < 0) && (s[2][2] < 0)) {
		// 		printd("matrixToTransformationData(): special case 1\n");
		gmtl::Matrix44f rotadj;
		rotadj = gmtl::MAT_IDENTITY44F;
		setScale(rotadj, gmtl::Vec3f(-1.0, 1.0, -1.0));
		setRot(dst.orientation, q * rotadj * p);
		dst.scale = gmtl::Vec3f(-s[0][0], s[1][1], -s[2][2]);
	} else if ((s[1][1] < 0) && (s[2][2] < 0)) {
		// 		printd("matrixToTransformationData(): special case 2\n");
		gmtl::Matrix44f rotadj;
		rotadj = gmtl::MAT_IDENTITY44F;
		setScale(rotadj, gmtl::Vec3f(1.0, -1.0, -1.0));
		setRot(dst.orientation, q * rotadj * p);
		dst.scale = gmtl::Vec3f(s[0][0], -s[1][1], -s[2][2]);
	} else {
		// 		printd("matrixToTransformationData(): no special case\n");
		dst.scale = gmtl::Vec3f(s[0][0], s[1][1], s[2][2]);
	}

	for (i = 0; i < 3; i++)
		if (dst.scale[i] <= 0.0)
			printd("got a negative scale of %0.3f at %d\n", dst.scale[i], i);

	gmtl::Matrix44f test;
	float dist;
	transformationDataToMatrix(dst, test);
	dist = distMatrix3x3(test, src);
	if (dist > 0.01)
		printd(ERROR, "matrixToTransformationData(): made an error of %0.5f\n", dist);

	return dst;
}

INVRS_SYSTEMCORE_API TransformationData& invert(TransformationData& dst,
		const TransformationData& src) {
	// 	if (src.scale == gmtl::Vec3f(1, 1, 1))
	// 	{
	// // 		TransformationData source = src;
	// 		dst = src;
	// 		gmtl::invert(dst.orientation);
	// 		dst.position = (dst.orientation * src.position)*(-1.f);
	// // 		printf("Testing difference:\n");
	// // 		TransformationData testResult;
	// // 		multiply(testResult, source, dst);
	// // 		dumpTransformation(testResult);
	// 	} else
	// 	{
	// 		gmtl::Matrix44f s, d;
	//
	// 		transformationDataToMatrix(src, s);
	// 		invertFull(d, s);
	// 		matrixToTransformationData(d, dst);
	// 	} // else
	gmtl::Vec3f tempPos[2];
	TransformationData tempSrc = src;
	gmtl::Quatf invScaleOri;
	int i;

	// remove scaleOrientation component from orientation in src:
	invScaleOri = src.scaleOrientation;
	gmtl::invert(invScaleOri);
	tempSrc.orientation = src.orientation * invScaleOri;

	dst.scaleOrientation = tempSrc.orientation;
	gmtl::invert(dst.scaleOrientation);
	dst.orientation = tempSrc.scaleOrientation;
	gmtl::invert(dst.orientation);
	if (src.scale == gmtl::Vec3f(1, 1, 1)) {
		dst.scale = gmtl::Vec3f(1, 1, 1);
		dst.orientation = dst.orientation * dst.scaleOrientation;
		dst.scaleOrientation = gmtl::QUAT_IDENTITYF;
	} else {
		for (i = 0; i < 3; i++)
			dst.scale[i] = 1.0f / tempSrc.scale[i];
	}

	tempPos[0] = (dst.scaleOrientation * tempSrc.position) * (-1.0f);
	for (i = 0; i < 3; i++)
		tempPos[1][i] = dst.scale[i] * tempPos[0][i];
	dst.position = dst.orientation * tempPos[1];

	dst.orientation = dst.orientation * dst.scaleOrientation;

	TransformationData testResult;
	gmtl::Matrix44f shouldBeIdentity;
	multiply(testResult, src, dst);
	transformationDataToMatrix(testResult, shouldBeIdentity);
	float dist = distMatrix3x3(gmtl::MAT_IDENTITY44F, shouldBeIdentity);
	if (dist > 0.01f)
		printd(ERROR, "invert(): made an error of %0.5f\n", dist);
	// 	else
	// 		printd("invert(): made no errror :-)\n");

	return dst;
}

INVRS_SYSTEMCORE_API void transformVector(gmtl::Vec4f& dst, TransformationData& transf,
		gmtl::Vec4f& src) {
	gmtl::Matrix44f transfAsMatrix;
	transformationDataToMatrix(transf, transfAsMatrix);
	dst = transfAsMatrix * src;
}
INVRS_SYSTEMCORE_API void dumpTransformation(TransformationData& data) {
	char output[1024];
	char temp[1024];
	sprintf(output, "dumpTransformation():\n");
	sprintf(temp, "position:         (%0.6f, %0.6f, %0.6f)\n", data.position[0], data.position[1],
			data.position[2]);
	strcat(output, temp);
	sprintf(temp, "orientation:      (%0.6f, %0.6f, %0.6f, %0.6f)\n", data.orientation[0],
			data.orientation[1], data.orientation[2], data.orientation[3]);
	strcat(output, temp);
	sprintf(temp, "scale:            (%0.6f, %0.6f, %0.6f)\n", data.scale[0], data.scale[1],
			data.scale[2]);
	strcat(output, temp);
	sprintf(temp, "scaleOrientation: (%0.6f, %0.6f, %0.6f, %0.6f)\n", data.scaleOrientation[0],
			data.scaleOrientation[1], data.scaleOrientation[2], data.scaleOrientation[3]);
	strcat(output, temp);
	printd(INFO, output);
}

INVRS_SYSTEMCORE_API std::string dumpTransformationToString(TransformationData& data,
		const char* prefix) {
	std::string result;
	char output[1024];
	char temp[1024];
	sprintf(output, "%sposition:         (%0.6f, %0.6f, %0.6f)\n", prefix, data.position[0],
			data.position[1], data.position[2]);
	strcat(output, temp);
	sprintf(temp, "%sorientation:      (%0.6f, %0.6f, %0.6f, %0.6f)\n", prefix,
			data.orientation[0], data.orientation[1], data.orientation[2], data.orientation[3]);
	strcat(output, temp);
	sprintf(temp, "%sscale:            (%0.6f, %0.6f, %0.6f)\n", prefix, data.scale[0],
			data.scale[1], data.scale[2]);
	strcat(output, temp);
	sprintf(temp, "%sscaleOrientation: (%0.6f, %0.6f, %0.6f, %0.6f)", prefix,
			data.scaleOrientation[0], data.scaleOrientation[1], data.scaleOrientation[2],
			data.scaleOrientation[3]);
	strcat(output, temp);
	result = std::string(output);
	return result;
} // dumpTransformationToString

INVRS_SYSTEMCORE_API void dumpVec(gmtl::Vec3f vec, char* text) {
	printd(INFO, "dumpVec(): %s: %0.03f, %0.03f, %0.03f\n", text != NULL ? text : "unnamed",
			vec[0], vec[1], vec[2]);
}

INVRS_SYSTEMCORE_API void dumpMatrix(gmtl::Matrix44f& m) {
	int i, j;
	char output[512];
	output[0] = '\0';
	sprintf(output, "dumpMatrix():\n");
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++)
			sprintf(output, "%s%0.6f ", output, m[i][j]);
		strcat(output, "\n");
	} // for
	printd(INFO, output);
} // dumpMatrix

INVRS_SYSTEMCORE_API void dumpMatrixDifference(gmtl::Matrix44f& m1, gmtl::Matrix44f& m2) {
	int i, j;
	char output[512];
	output[0] = '\0';
	sprintf(output, "dumpMatrixDifference():\n");
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++)
			sprintf(output, "%s%0.6f ", output, m1[j][i] - m2[j][i]);
		strcat(output, "\n");
	} // for
	printd(INFO, output);
} // dumpMatrixDifference

INVRS_SYSTEMCORE_API float gmtlDet3(gmtl::Matrix44f mat) {
	float det;
	det = 0;
	det += mat(0, 0) * mat(1, 1) * mat(2, 2);
	det += mat(0, 1) * mat(1, 2) * mat(2, 0);
	det += mat(0, 2) * mat(1, 0) * mat(2, 1);
	det -= mat(2, 0) * mat(1, 1) * mat(0, 2);
	det -= mat(2, 1) * mat(1, 2) * mat(0, 0);
	det -= mat(2, 2) * mat(1, 0) * mat(0, 1);
	return det;
}


INVRS_SYSTEMCORE_API float distMatrix3x3(gmtl::Matrix44f mat1, gmtl::Matrix44f mat2) {
	float ret;
	int i, j;
	ret = 0;
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			ret += fabs(mat1(i, j) - mat2(i, j));

	return ret;
}

INVRS_SYSTEMCORE_API gmtl::Quatf& setQuatfAsAxisAngleDeg(gmtl::Quatf& dst, float x, float y,
		float z, float deg) {
	gmtl::Vec3f axis(x, y, z);
	gmtl::normalize(axis);
	gmtl::AxisAnglef src(deg * M_PI / 180.f, axis);
	gmtl::set(dst, src);
	return dst;
} // setQuatfAsAxisAngleDeg

INVRS_SYSTEMCORE_API void singularValueDecomposition(const gmtl::Matrix44f& src, gmtl::Matrix44f& q,
		gmtl::Matrix44f& s, gmtl::Matrix44f& p) {
	// 	printd(WARNING, "WARNING: singularValueDecomposition can lead to negative scale values!!!\n");
	int i, j;
	gmtl::Matrix44f a, aT, aaT, si, r, rT, finalRot;
	gmtl::Eigen esystem(3);
	float* rowPtrs[3];
	float scale[3];

	a = src;

	for (i = 0; i < 3; i++) {
		a(i, 3) = 0.0;
		a(3, i) = 0.0;
	}
	a(3, 3) = 1;

	si = gmtl::MAT_IDENTITY44F;

	transpose(aT, a);
	mult(aaT, aT, a);
	rowPtrs[0] = (float*)aaT.getData();
	rowPtrs[1] = (float*)aaT.getData() + 4;
	rowPtrs[2] = (float*)aaT.getData() + 8;
	// 	rowPtrs[3] = (float*)aaT.getData()+12;
	esystem.SetMatrix(rowPtrs);
	esystem.EigenStuff();

	for (i = 0; i < 3; i++) {
		// 		assert(esystem.GetEigenvalue(i)>=0.0);
		if (esystem.GetEigenvalue(i) < 0.0) {
			printd(INFO, "matrixToTransformationData(): encountered eigenvalue %0.3f in row %d\n",
					esystem.GetEigenvalue(i), i);
			printd(INFO, "matrixToTransformationData(): matrix:\n");
			for (j = 0; j < 3; j++)
				printd(INFO, "\t%0.3f %0.3f %0.3f\n", rowPtrs[j][0], rowPtrs[j][1], rowPtrs[j][2]);
			assert(false);
		}
		scale[i] = sqrtf(esystem.GetEigenvalue(i));
		si(i, i) = 1.0f/scale[i];
	}

	p = gmtl::MAT_IDENTITY44F;
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++) {
			p(i, j) = esystem.GetEigenvector(i, j);
		}
	// p: need eigenvectors in cols here

	q = a * p * si;
	setScale(s, gmtl::Vec3f(scale[0], scale[1], scale[2]));
	transpose(p);
}// split

INVRS_SYSTEMCORE_API TransformationData identityTransformation() {
	TransformationData ret;
	ret.position = gmtl::Vec3f(0, 0, 0);
	ret.scale = gmtl::Vec3f(1, 1, 1);
	ret.orientation = gmtl::Quatf(0, 0, 0, 1);
	ret.scaleOrientation = gmtl::Quatf(0, 0, 0, 1);
	return ret;
}

INVRS_SYSTEMCORE_API bool isIdentityTransformation(TransformationData& toCmp) {
	int i;
	for (i = 0; i < 3; i++)
		if (toCmp.position[i] != 0)
			return false;
	for (i = 0; i < 3; i++)
		if (toCmp.scale[i] != 1)
			return false;
	return (toCmp.orientation == gmtl::QUAT_IDENTITYF) && (toCmp.scaleOrientation
			== gmtl::QUAT_IDENTITYF);
}

INVRS_SYSTEMCORE_API SensorData identitySensorData() {
	SensorData ret;
	ret.position = gmtl::Vec3f(0, 0, 0);
	ret.orientation = gmtl::QUAT_IDENTITYF;
	return ret;
}

INVRS_SYSTEMCORE_API float getMinDistanceToAABBox(gmtl::Vec3f pos, Entity* entity) {
	unsigned id;
	unsigned short envId, entId;
	ModelInterface *mi;
	float xDistance = 0;
	float yDistance = 0;
	float zDistance = 0;
	gmtl::Vec3f distanceVec;

	mi = entity->getVisualRepresentation();
	if (!mi) {
		id = entity->getEnvironmentBasedId();
		split(id, envId, entId);
		printd(ERROR,
				"getMinDistanceToBBox(): entity (envID=%u entID=%u) does not have a model set\n",
				(unsigned)envId, (unsigned)entId);
#if defined max
	#undef max //some where defined can't find origin
#endif
		return std::numeric_limits<float>::max();

	} // if

	AABB bbox = mi->getAABB();

	if (pos[0] < bbox.p0[0])
		xDistance = bbox.p0[0] - pos[0];
	else if (pos[0] > bbox.p1[0])
		xDistance = pos[0] - bbox.p1[0];

	if (pos[1] < bbox.p0[1])
		yDistance = bbox.p0[1] - pos[1];
	else if (pos[1] > bbox.p1[1])
		yDistance = pos[1] - bbox.p1[1];

	if (pos[2] < bbox.p0[2])
		zDistance = bbox.p0[2] - pos[2];
	else if (pos[2] > bbox.p1[2])
		zDistance = pos[2] - bbox.p1[2];

	distanceVec = gmtl::Vec3f(xDistance, yDistance, zDistance);

	return gmtl::length(distanceVec);
} // getMinDistanceToBBox


// TODO: Check if isInside works for world coordinates or if it should
/**
 * This method returns if the passed Position is inside the
 * BoundingBox of the passed Entity.
 * @param pos Position in world coordinates
 * @param entity Entity to check
 * @return true, if the position is inside
 **/
INVRS_SYSTEMCORE_API bool isInside(gmtl::Vec3f pos, Entity* entity) {
	unsigned id;
	unsigned short envId, entId;
	ModelInterface *mi;
	mi = entity->getVisualRepresentation();
	if (!mi) {
		id = entity->getEnvironmentBasedId();
		split(id, envId, entId);
		printd(ERROR, "isInside(): entity (envID=%u entID=%u) does not have a model set\n",
				(unsigned)envId, (unsigned)entId);
		return false;
	}

	AABB bbox = mi->getAABB();

	if (pos[0] < bbox.p0[0] || pos[0] > bbox.p1[0])
		return false;
	if (pos[2] < bbox.p0[2] || pos[2] > bbox.p1[2])
		return false;
	if (pos[1] < bbox.p0[1] || pos[1] > bbox.p1[1])
		return false;
	return true;
} // isInside

/**
 * This method simply compares the size of the BoundingBoxes
 * of 2 Entities and returns the Entity with
 * the smaller box.
 * @param e1 Entity to compare
 * @param e2 Entity to compare with
 * @return Entity with the smaller BoundingBox
 **/
INVRS_SYSTEMCORE_API Entity* smallerBBox(Entity* e1, Entity *e2) {
	// 	AABB bbox1 = e1->getSceneGraphInterface()->getAABB();
	// 	AABB bbox2 = e2->getSceneGraphInterface()->getAABB();

	ModelInterface *m1, *m2;

	m1 = e1->getVisualRepresentation();
	m2 = e2->getVisualRepresentation();

	if (!m1 || !m2) {
		printd(ERROR, "smallerBBox(): at least one entity has no visual representation!\n");
		return e1;
	}

	AABB bbox1, bbox2;

	bbox1 = m1->getAABB();
	bbox2 = m2->getAABB();

	float size1 = gmtl::length(gmtl::Vec3f(bbox1.p0 - bbox1.p1));
	float size2 = gmtl::length(gmtl::Vec3f(bbox2.p0 - bbox2.p1));
	if (size1 < size2)
		return e1;
	return e2;
} // smallerBBox

INVRS_SYSTEMCORE_API unsigned int generateHashCode(std::string name) {
	int i;
	char c;
	unsigned int hashValue = 0;

	for (i = 0; i < (int)name.length(); i++) {
		c = name[i];
		hashValue ^= (((unsigned)c) << ((i % 4) * 8));
	} // for

	return hashValue;
} // generateHashCode


