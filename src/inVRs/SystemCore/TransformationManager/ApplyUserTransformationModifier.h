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

#ifndef _APPLYUSERTRANSFORMATIONMODIFIER_H
#define _APPLYUSERTRANSFORMATIONMODIFIER_H

#include "TransformationModifierFactory.h"

/******************************************************************************
 * @class ApplyUserTransformationModifier
 * @brief Modifier for applying the UserTransformation to the user
 *
 * The transformation of the user is calculated and set. This user could either 
 * be the local user or a remote user. The attribute userTransformation is 
 * changed in the corresponding user object. Based on the registered
 * IserTranformationModel additional modifications of the userTransformation
 * can be applied. When this modifier is executed the current
 * userTransformation based on the model is calculated.
 */
class INVRS_SYSTEMCORE_API ApplyUserTransformationModifier :
                public TransformationModifier {
public:
        /**
         *
         */
        ApplyUserTransformationModifier(bool useLocalUser = false);

        /**
         *
         */
        virtual TransformationData execute(TransformationData* resultLastStage,
                        TransformationPipe* currentPipe);

protected:
        bool useLocalUser;
}; // ApplyUserTransformationModifier


/******************************************************************************
 * @class ApplyUserTransformationModifierFactory
 * @brief Factory for ApplyUserTransformationModifier
 */
class INVRS_SYSTEMCORE_API ApplyUserTransformationModifierFactory :
                public TransformationModifierFactory {
public:
        /**
         *
         */
        ApplyUserTransformationModifierFactory();

protected:

        virtual TransformationModifier* createInternal(ArgumentVector* args = NULL);

        virtual bool needInstanceForEachPipe();
}; // ApplyUserTransformationModifierFactory

#endif // _APPLYUSERTRANSFORMATIONMODIFIER_H


