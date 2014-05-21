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

#ifndef _AVATARTRANSFORMATIONWRITER_H
#define _AVATARTRANSFORMATIONWRITER_H

#include "TransformationModifierFactory.h"

class INVRS_SYSTEMCORE_API AvatarTransformationWriter : public TransformationModifier {
public:
	AvatarTransformationWriter(ArgumentVector* args);
	virtual TransformationData execute(TransformationData* resultLastStage,
			TransformationPipe* currentPipe);
protected:
	bool clipRotationToYAxis;
	bool useLocalUser;
//	bool writeAvatarHead;
//	bool writeAvatarHand;
};

class INVRS_SYSTEMCORE_API AvatarTransformationWriterFactory : public TransformationModifierFactory {
public:
	AvatarTransformationWriterFactory();

protected:
	virtual TransformationModifier* createInternal(ArgumentVector* args);
	virtual bool needInstanceForEachPipe();
}; // AvatarTransformationModifierFactory

#endif // _AVATARTRANSFORMATIONWRITER_H
