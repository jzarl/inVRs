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

#include "HeightMapModifierBase.h"
#include <inVRs/SystemCore/DebugOutput.h>

HeightMapModifier::HeightMapModifier(HeightMapInterface* heightmap) :
	TransformationModifier(), heightmap(heightmap), manager(NULL) {
}

HeightMapModifier::HeightMapModifier(AbstractHeightMapManager* manager) :
	TransformationModifier(), heightmap(NULL), manager(manager) {
}

HeightMapModifier::~HeightMapModifier() {
	delete heightmap;
	//don't the delete the manager just set to null
	manager = NULL;
}

TransformationData HeightMapModifier::execute(TransformationData* resultLastStage,
		TransformationPipe* currentPipe) {

	// TODO: Check if there is some sort of scale!!!
	TransformationData result = *resultLastStage;

	if (heightmap != NULL)
		result.position[1] = heightmap->getHeight(result.position[0], result.position[2]);
	else
		result.position[1] = manager->getHeightAtWorldPos(result.position[0], result.position[2]);

	return result;
} // execute

HeightMapModifierFactoryBase::HeightMapModifierFactoryBase(AbstractHeightMapManager* manager): manager(manager) {
	className = "HeightMapModifier";
}

HeightMapModifierFactoryBase::~HeightMapModifierFactoryBase() {
	//don't delete just set to NULL
	manager = NULL;
}

TransformationModifier* HeightMapModifierFactoryBase::createInternal(ArgumentVector* args) {
	if (!args || args->size() == 0)
		return new HeightMapModifier(manager);

	std::string fileName;
	if (!args->get("fileName", fileName)) {
		printd(ERROR, "HeightMapModifierFactory::create(): missing argument \"fileName\"!\n");
		return NULL;
	} // if

	HeightMapInterface* heightmap = manager->load(fileName);
	if(!heightmap) {
		printd(ERROR, "HeightMapModifier::create(): failed to load heightmap %s\n", fileName.c_str());
		return NULL;
	}

	float scale = 1;
	float offset = 0;

	args->get("scale", scale);
	args->get("offset", offset);

	heightmap->adjustZ(scale, offset);

	return new HeightMapModifier(heightmap);
}

bool HeightMapModifierFactoryBase::needInstanceForEachPipe() {
	return true;
}

