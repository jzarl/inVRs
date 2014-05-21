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

#ifndef _OPENSGTRANSFORMATIONWRITER_H
#define _OPENSGTRANSFORMATIONWRITER_H

#include "Interfaces/TransformationWriterInterface.h"
#include <OpenSG/OSGTransform.h>

// Object Oriented Physics Simulation
namespace oops
{

class OpenSGTransformationWriter : public TransformationWriterInterface
{
protected:
	OSG::TransformPtr transformCore;
	gmtl::Matrix44f nodeOffset;
	gmtl::Matrix44f globalOffset;
	gmtl::Matrix44f localOffset;
public:
	OpenSGTransformationWriter(OSG::TransformPtr transformCore);
	virtual ~OpenSGTransformationWriter();

	void updateTransformation(TransformationData trans);
	void setTransformationOffset(TransformationData trans, TransformationData offset);
}; // OpenSGTransformationWriter

} // oops

#endif // _OPENSGTRANSFORMATIONWRITER_H
