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

#ifndef CAMERATRANSFORMATIONWRITER
#define CAMERATRANSFORMATIONWRITER

#include "TransformationModifierFactory.h"

/******************************************************************************  
 * The transformation of the user's camera is set. This user could either be the
 * local user or a remote user. The attribute cameraTrasnformation is changed in
 * the corresponding user object. On the entry value several modifications can
 * be applied. The height can be altered by an additional configurable parameter
 * and the the global Y Axis can be used. The altered transformation is passed
 * on.
 */

class INVRS_SYSTEMCORE_API CameraTransformationWriter : public TransformationModifier {
public:
	CameraTransformationWriter(float cameraHeight, bool useLocalUser, bool useGlobalYAxis);
	virtual TransformationData execute(TransformationData* resultLastStage,
			TransformationPipe* currentPipe);
protected:
	float cameraHeight;
	bool useLocalUser;
	bool useGlobalYAxis;
};

class INVRS_SYSTEMCORE_API CameraTransformationWriterFactory : public TransformationModifierFactory {
public:
	CameraTransformationWriterFactory();

protected:
	virtual TransformationModifier* createInternal(ArgumentVector* args);
	virtual bool needInstanceForEachPipe();
}; // UserTransformationModifierFactory

#endif
