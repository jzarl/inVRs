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

#ifndef _HEIGHTMAPMODIFIERBASE_H
#define _HEIGHTMAPMODIFIERBASE_H

#include <inVRs/SystemCore/TransformationManager/TransformationModifierFactory.h>
#include "HeightMapInterface.h"
#include "AbstractHeightMapManager.h"

class HeightMapModifier : public TransformationModifier {
public:
	HeightMapModifier(HeightMapInterface* heightmap);
	HeightMapModifier(AbstractHeightMapManager* manager);
	~HeightMapModifier();

	virtual TransformationData execute(TransformationData* resultLastStage,
			TransformationPipe* currentPipe);

protected:
	HeightMapInterface* heightmap;
	AbstractHeightMapManager* manager;

};

class INVRS_HEIGHTMAP_API HeightMapModifierFactoryBase : public TransformationModifierFactory {
public:
	HeightMapModifierFactoryBase(AbstractHeightMapManager* manager);
	~HeightMapModifierFactoryBase();
protected:

	virtual TransformationModifier* createInternal(ArgumentVector* args = NULL);
	bool needInstanceForEachPipe();

private:
	AbstractHeightMapManager* manager;

}; // HeightmapModifierFactory

#endif // _HEIGHTMAPMODIFIER_H
