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

#ifndef _USERTRANSFORMATIONWRITER_H
#define _USERTRANSFORMATIONWRITER_H

#include "TransformationModifierFactory.h"

/****************************************************************************** 
 * The user transformation of the pipe owner is set. This user could either be
 * the local user or a remote user. The attribute navigatedTransformation is set
 * in the corresponding user object in the user database. The input
 * transformation is passed on.
 */
class INVRS_SYSTEMCORE_API UserTransformationWriter : public TransformationModifier {
protected:
	bool useLocalUser;

public:
	UserTransformationWriter(bool useLocalUser = false);
	virtual TransformationData execute(TransformationData* resultLastStage,
			TransformationPipe* currentPipe);
};

class INVRS_SYSTEMCORE_API UserTransformationWriterFactory : public TransformationModifierFactory {
public:
	UserTransformationWriterFactory();

protected:
	virtual TransformationModifier* createInternal(ArgumentVector *args = NULL);
	virtual bool needInstanceForEachPipe();
};

#endif // _USERTRANSFORMATIONWRITER_H
