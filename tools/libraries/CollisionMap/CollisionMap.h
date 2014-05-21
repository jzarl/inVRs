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

#ifndef _COLLISIONMAP_H
#define _COLLISIONMAP_H

#include <vector>
#include <string>
#include <map>

#include "CollisionLineSet.h"
#include <inVRs/SystemCore/DataTypes.h>

/******************************************************************************
 * @class
 * @brief Class for loading CollisionMaps for Tiles.
 *
 * The class is used to load a CollisionLineSet for each Tile in the world. It
 * therefore loads a configuration file in the <code>loadCollisionWorld</code>
 * method where the VRML-Files containing the CollisionLineSet for the Tiles are
 * defined. Since several Tiles can have the same collision shape the config
 * file defines a mapping from Tile-IDs to CollisionMaps. The
 * <code>getTileCollisionMap</code> method then allows to get the
 * CollisionLineSet for each Tile.
 * The class has also the possibility to check the collision between the User
 * and the Tiles. Therefore a collision shape for the user has to be passed to
 * the constructor of the class. This function is DEPRECATED and will eventually
 * be removed in the future.
 */
class CollisionMap {
public:

	/** Empty constructor.
	 */
	CollisionMap();

	/** Constructor stores collision object for user.
	 */
	CollisionMap(CollisionObject* user);

	/** Destructor frees all loaded CollisionObjects.
	 */
	~CollisionMap();

	/** Loads the CollisionLineSets for each Tile.
	 * The method parses the passed config file which contains the urls of the
	 * appropriate CollisionLineSet-Files for each Tile. The CollisionLineSets
	 * are then loaded and stored in a map which allows to get the matching
	 * CollisionLineSet via the ID of a Tile.
	 * @param cfgFile config file containing the urls of the CollisionLineSet-files
	 * @return true if the file could be parsed correctly, false otherwise
	 */
	bool loadCollisionWorld(std::string cfgFile, CollisionLineSetFactory* factory);

	/** DEPRECATED: Checks the collision between the user and the Tiles.
	 * The method checks for collisions between the users's shape passed in
	 * the constructor and the CollisionLineSets of the Tiles.
	 * @param trans Transformation of the User in world coordinates
	 * @param dst Vector where to write the found collisions to
	 * @return Destination vector (same as second parameter)
	 */
	std::vector<CollisionData*>& checkCollision(TransformationData trans, std::vector<
			CollisionData*> &dst);

	/** Returns the CollisionLineSet for the Tile with the passed Id.
	 * The method checks if a CollisionLineSet is loaded for the Tile with the
	 * passed ID. If so the corresponding CollisionLineSet is returned.
	 * @param tileId ID of the Tile of which the CollisionLineSet is searched
	 * @return CollisionLineSet of the Tile, NULL if not found
	 */
	CollisionObject* getTileCollisionMap(unsigned tileId);

	/** Loads the CollisionLineSet out of the passed vrmlFile and sets the
	 * Transformation of the CollisionLineSet to the passed parameter.
	 * This method is only used by the <code>loadCollisionWorld</code>
	 * method as helper method.
	 * @param vrmlFile url to the File containing the CollisionLines
	 * @param trans Transformation of the CollisionLineSet
	 * @return CollisionLineSet loaded from the passed VRML-File
	 */
	CollisionLineSet* loadCollisionLineSet(std::string vrmlFile, TransformationData trans, CollisionLineSetFactory* factory);

protected:

	/// list of all loaded CollisionLineSet for the Tiles
	std::vector<CollisionObject*> objects;

	/// Map containing all loaded CollisionMaps
	std::map<unsigned, CollisionObject*> objectMap;

	/// Map containing the CollisionMaps for each Tile
	std::map<unsigned, CollisionObject*> tileMap;

	/// DEPRECATED: CollisionObject for the user
	CollisionObject* user;

}; // CollisionMap

#endif // _COLLISIONMAP_H
