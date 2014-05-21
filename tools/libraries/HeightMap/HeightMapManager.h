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

#ifndef _HEIGHTMAPMANAGER_H
#define _HEIGHTMAPMANAGER_H

#include "OpenSGHeightMapManager.h"

/**
 * @deprecated use an instance
 */
class INVRS_HEIGHTMAP_API HeightMapManager {
	friend class HeightMapModifierFactory;

public:
	/**
	 * @deprecated use an instance
	 */
	static void cleanup();
	/**
	 * @deprecated use an instance
	 */
	static void generateTileHeightMaps();

	/**
	 * @deprecated use an instance
	 */
	static void setFilterProperties(float sigma, unsigned kernelSize);

	/**
	 * @deprecated use an instance
	 */
	static HeightMapInterface* getHeightMapOfTile(unsigned tileId);
	/**
	 * @deprecated use an instance
	 */
	static float getHeightAtWorldPos(float worldX, float worldZ);

private:
	static OpenSGHeightMapManager* instance;

	static OpenSGHeightMapManager* getLazy();

}; // HeightMapManager

#endif // _HEIGHTMAPMANAGER_H
