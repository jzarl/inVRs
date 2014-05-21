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

#ifndef _APPLYCURSORTRANSFORMATIONMODIFIER_H
#define _APPLYCURSORTRANSFORMATIONMODIFIER_H

#include "TransformationModifierFactory.h"

/******************************************************************************
 * @class ApplyCursorTransformationModifier
 * @brief Modifier for applying the CursorTransformation to the user
 *  
 * The transformation of the user's cursor is set. This user could either be the
 * local user or a remote user. The attribute cursorTransformation is changed in
 * the corresponding user object. Based on the registered
 * CursorTranformationModel additional modifications of the cursorTransformation
 * can be applied. When this modifier is executed the current
 * cursorTransformation based on the model is calculated.
 */
class INVRS_SYSTEMCORE_API ApplyCursorTransformationModifier :
		public TransformationModifier {
public:
	/**
	 *
	 */
	ApplyCursorTransformationModifier(bool useLocalUser = false);

	/**
	 *
	 */
	virtual TransformationData execute(TransformationData* resultLastStage,
			TransformationPipe* currentPipe);

protected:
	bool useLocalUser;
}; // ApplyCursorTransformationModifier


/******************************************************************************
 * @class ApplyCursorTransformationModifierFactory
 * @brief Factory for ApplyCursorTransformationModifier
 */
class INVRS_SYSTEMCORE_API ApplyCursorTransformationModifierFactory :
		public TransformationModifierFactory {
public:
	/**
	 *
	 */
	ApplyCursorTransformationModifierFactory();

protected:

	virtual TransformationModifier* createInternal(ArgumentVector* args = NULL);

	virtual bool needInstanceForEachPipe();
}; // ApplyCursorTransformationModifierFactory

#endif // _APPLYCURSORTRANSFORMATIONMODIFIER_H
