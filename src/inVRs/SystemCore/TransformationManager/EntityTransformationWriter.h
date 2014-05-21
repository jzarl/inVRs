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

#ifndef _ENTITYTRANSFORMATIONWRITER_H
#define _ENTITYTRANSFORMATIONWRITER_H

#include "TransformationModifierFactory.h"

/**
 * The transformation of an entity is set. The entity is identified by the ID
 * and typeID stored in the pipe. The transformations to be applied are in world
 * coordinates. The input transformation is passed on.
 */

class INVRS_SYSTEMCORE_API EntityTransformationWriter : public TransformationModifier {
public:
	virtual TransformationData execute(TransformationData* resultLastStage,
			TransformationPipe* currentPipe);
}; // EntityTransformationWriter

class INVRS_SYSTEMCORE_API EntityTransformationWriterFactory : public TransformationModifierFactory {
public:
	EntityTransformationWriterFactory();

protected:
	virtual TransformationModifier* createInternal(ArgumentVector* args = NULL);
	virtual bool needSingleton();
}; // EntityTransformationWriterFactory

#endif // _ENTITYTRANSFORMATIONWRITER_H
