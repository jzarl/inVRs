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

#ifndef _AVATARANIMATIONWRITER_H
#define _AVATARANIMATIONWRITER_H

#include <inVRs/SystemCore/TransformationManager/TransformationModifierFactory.h>

#include "AvataraWrapperExports.h"

class INVRS_AVATARAWRAPPER_API AvatarAnimationWriter : public TransformationModifier {
public:
	AvatarAnimationWriter(float walkingSpeed = 1.f, bool useLocalUser = false);
	virtual TransformationData execute(TransformationData* resultLastStage,
			TransformationPipe* currentPipe);
protected:
	bool useLocalUser;
	TransformationData oldTrans[5];
	bool walking;
	float walkingSpeed;
	float lastTime;
};

class INVRS_AVATARAWRAPPER_API AvatarAnimationWriterFactory : public TransformationModifierFactory {
public:

	AvatarAnimationWriterFactory();

protected:

	virtual TransformationModifier* createInternal(ArgumentVector* args);
	virtual bool needInstanceForEachPipe();
}; // AvatarTransformationModifierFactory

#endif // _AVATARANIMATIONWRITER_H
