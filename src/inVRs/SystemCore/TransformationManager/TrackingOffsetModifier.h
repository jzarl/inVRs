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

#ifndef _TRACKINGOFFSETMODIFIER_H
#define _TRACKINGOFFSETMODIFIER_H

#include "TransformationModifierFactory.h"

class INVRS_SYSTEMCORE_API TrackingOffsetModifier : public TransformationModifier {
public:
	TrackingOffsetModifier(bool useHeadSensor = true, bool removeYAxis = false,
			bool removeOrientation = false, bool userLocalUser = false);
	virtual TransformationData execute(TransformationData* resultLastStage,
			TransformationPipe* currentPipe);

protected:
	bool useLocalUser;
	bool useHead; // otherwise use Hand
	bool removeYAxis;
	bool removeOrientation;
};

class INVRS_SYSTEMCORE_API TrackingOffsetModifierFactory : public TransformationModifierFactory {
public:
	TrackingOffsetModifierFactory();

protected:
	virtual TransformationModifier* createInternal(ArgumentVector *args = NULL);
	virtual bool needInstanceForEachPipe();
}; // TrackingOffsetModifier

#endif // _TRACKINGOFFSETMODIFIER_H
