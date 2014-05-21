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

#include "oops/OpenSGTransformationWriter.h"
#include <OpenSG/OSGMatrix.h>
#include <OpenSG/OSGQuaternion.h>
#include <gmtl/MatrixOps.h>

OSG_USING_NAMESPACE

// Object Oriented Physics Simulation
namespace oops
{

OpenSGTransformationWriter::OpenSGTransformationWriter(TransformPtr transformCore)
{
	this->transformCore = transformCore;
} // OpenSGTransformationWriter

OpenSGTransformationWriter::~OpenSGTransformationWriter()
{
	// nothing to do here
} // ~OpenSGTransformationWriter

/**
 * globalOffset:
 * Toffglob = Tworld * Tode^-1
 * Tworld =  Toffglob * Tode
 *
 * localOffset:
 * Tofflocal = Tode^-1 * Tworld
 * Tworld = Tode * Toffloc
 *
 * nodeOffset:
 * Tcore = Tnodeoff * Tworld
 * Tnodeoff = Tcore * Tworld^-1
 */

void OpenSGTransformationWriter::setTransformationOffset(TransformationData trans, TransformationData offset)
{
	printf("calculateOffset\n");
//  	OSG::Matrix tOde, tOffGlob;
	OSG::Matrix tWorld, tCore, tNodeOff;
// 	OSG::Matrix tOffLoc;
// 	gmtl::Matrix44f gmtlMatrix;

/*	transformationDataToMatrix(trans, gmtlMatrix);
	tOde.setValue(gmtlMatrix.getData());*/
#if OSG_MAJOR_VERSION < 2 //OpenSG1:
	tWorld = transformCore->getParents()[0]->getToWorld();
#else
	OSG::Node* tmp = dynamic_cast<OSG::Node*>(transformCore->getParents()[0]);
	if (tmp == NULL)
	{
		SLOG << "dereferencing nullptr in OpenSGTransformationWriter::setTransformationOffset";
		throw OSG::Exception();
	};
	tWorld = tmp->getToWorld();
#endif
	tCore = transformCore->getMatrix();
// 	tOffGlob = tOde;
// 	tOffGlob.invert();
// 	tOffGlob.multLeft(tWorld);

	transformationDataToMatrix(offset, localOffset);
//	tOffLoc = tOde;
//	tOffLoc.invert();
//	tOffLoc.mult(tWorld);
//	std::cout << "OpenSG-World-Matrix:\n" << tWorld << "\n";
//	std::cout << "ODE-World Matrix:\n" << tOde << "\n";
//	std::cout << "Global Offset:\n" << tOffGlob << "\n";
//	std::cout << "Local Offset:\n" << tOffLoc << "\n";

//	globalOffset.set(tOffGlob.getValues());
//	localOffset.set(tOffLoc.getValues());

	tNodeOff = tWorld;
	tNodeOff.invert();
	tNodeOff.multLeft(tCore);
	nodeOffset.set(tNodeOff.getValues());

	printf("calculateOffset  DONE\n");
} // calculateOffset

void OpenSGTransformationWriter::updateTransformation(TransformationData trans)
{
	OSG::Matrix m, m1, mOffset;
	OSG::Quaternion quat;
	gmtl::Matrix44f transformMatrix;
	
	transformationDataToMatrix(trans, transformMatrix);
	// Calculate World Matrix
// 	gmtl::preMult(transformMatrix, globalOffset);
	gmtl::postMult(transformMatrix, localOffset);
	// Calculate Core Matrix
	gmtl::preMult(transformMatrix, nodeOffset);
	m.setValue(transformMatrix.getData());

// 	mOffset.setValue(offset.getData());
// 	m.setIdentity()	;
// 	m.setTranslate(trans.position[0], trans.position[1], trans.position[2]);
// 	quat.setValueAsQuat(trans.orientation.getData());
// 	m.setRotate(quat);
// 	m.mult(mOffset);
	beginEditCP(transformCore, OSG::Transform::MatrixFieldMask);
		transformCore->setMatrix(m);
	endEditCP(transformCore, OSG::Transform::MatrixFieldMask);
} // updateTransformation

} // oops
