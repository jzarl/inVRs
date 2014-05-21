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

#ifndef _MANIPULATIONOFFSETMODIFIER_H
#define _MANIPULATIONOFFSETMODIFIER_H

#include <inVRs/SystemCore/TransformationManager/TransformationModifierFactory.h>
#include "InteractionSharedLibraryExports.h"

class INVRS_INTERACTION_API ManipulationOffsetModifier : public TransformationModifier {
public:
	ManipulationOffsetModifier();

	virtual TransformationData execute(TransformationData* resultLastStage,
			TransformationPipe* currentPipe);
};

class INVRS_INTERACTION_API ManipulationOffsetModifierFactory :
		public TransformationModifierFactory {
public:

	ManipulationOffsetModifierFactory();

protected:

	virtual TransformationModifier* createInternal(ArgumentVector* args = NULL);
	virtual bool needSingleton();
}; // ManipulationOffsetModifierFactory

#endif // _MANIPULATIONOFFSETMODIFIER_H
