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

#ifndef _CURSORTRANSFORMATIONWRITER_H
#define _CURSORTRANSFORMATIONWRITER_H

#include "TransformationModifierFactory.h"

/**
 * The transformation of the user's cursor is set. This user could either be the
 * local user or a remote user. The attribute cursorTransformation is changed in 
 * the corresponding user object. Based on the registered
 * CursorTranformationModel additional modifications of the cursorTransformation
 * can be applied. When this modifier is executed the current
 * cursorTransformation based on the model is calculated.
 */
class INVRS_SYSTEMCORE_API CursorTransformationWriter : public TransformationModifier {
public:
	CursorTransformationWriter(bool useLocalUser = false);
	virtual TransformationData execute(TransformationData* resultLastStage,
			TransformationPipe* currentPipe);

protected:
	bool useLocalUser;
}; // CursorTransformationWriter

class INVRS_SYSTEMCORE_API CursorTransformationWriterFactory : public TransformationModifierFactory {
public:
	CursorTransformationWriterFactory();

protected:
	virtual TransformationModifier* createInternal(ArgumentVector* args = NULL);
	virtual bool needInstanceForEachPipe();
}; // CursorTransformationWriterFactory

#endif // _CURSORTRANSFORMATIONWRITER_H
