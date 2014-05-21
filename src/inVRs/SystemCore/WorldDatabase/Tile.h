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

#ifndef _TILE_H
#define _TILE_H

#include "Entity.h"
#include "../DataTypes.h"
#include "../../OutputInterface/SceneGraphInterface.h"


/******************************************************************************
 * The class Tile is used for structuring VEs, for optimized culling and manual
 * control over editing VEs.
 * @author canthes
 */
class INVRS_SYSTEMCORE_API Tile {
public:

	/**
	 * Constructor initializes Tile.
	 * The constructor initializes the member variables with default values.
	 */
	Tile();

	/**
	 * Destructor.
	 * The destructor checks if the Tile is already removed from the
	 * SceneGraphInterface and prints an ERROR-message if not
	 */
	~Tile();

	/**
	 * Creates a clone of the current Tile.
	 * The method creates and returns a copy of the current Tile. Therefore all
	 * member variables are cloned and the model is cloned or deep-cloned
	 * according to the configuration.
	 * @return copy of the current Tile
	 */
	Tile* clone();

	/**
	 * Sets the Tile's ID.
	 * @param id ID of the Tile
	 */
	void setId(unsigned int id);

	/**
	 * Sets the name of the Tile.
	 * @param name name of the Tile
	 */
	void setName(std::string name);

	/**
	 * Sets the horizontal size of the Tile.
	 * The passed size is in world coordinates and not in spacing units. This
	 * means that the passed argument must be a multiple of the horizontal
	 * spacing defined in the EnvironmentLayout config-file which is parsed by
	 * the WorldDatabase.
	 * @param sizeX horizontal size of Tile in world coordinates
	 */
	void setXSize(int sizeX);

	/**
	 * Sets the vertical size of the Tile
	 * The passed size is in world coordinates and not in spacing units. This
	 * means that the passed argument must be a multiple of the vertical
	 * spacing defined in the EnvironmentLayout config-file which is parsed by
	 * the WorldDatabase.
	 * @param sizeZ vertical size of Tile in world coordinates
	 */
	void setZSize(int sizeZ);

	/**
	 * Sets the orientation of the tile along Y-axis.
	 * The orientation can be used to match the Tile into the Environment. The
	 * passed rotation is in degrees and should be either 0, 90, 180 or 270
	 * degrees.
	 * @param yRotation rotation in degrees along the Y-axis
	 */
	void setYRotation(int yRotation);

	/**
	 * Sets the height of the Tile.
	 * The method sets the offset of the Tile along the global Y-axis. This can
	 * be used to place a Tile in an arbitrary height.
	 * @param height height of the Tile
	 */
	void setHeight(float height);

	/**
	 * Sets the graphical representation of the Tile.
	 * @param mdl model of the Tile
	 */
	void setModel(ModelInterface* mdl);

	/**
	 * Sets the name of the XML-file where the Tile should be stored.
	 * @param filename of the XML-file
	 */
	void setXmlFilename(std::string filename);

	/**
	 * Returns the ID of the Tile.
	 * @return id of the tile
	 */
	unsigned int getId();

	/**
	 * Returns the name of the Tile.
	 * @return name of the Tile
	 */
	std::string getName();

	/**
	 * Returns the horizontal size of the Tile in world coordinates.
	 * @return horizontal size of the Tile in world coordinates
	 */
	int getXSize();

	/**
	 * Returns the vertical size of the Tile in world coordinates.
	 * @return vertical size of the Tile in world coordinates
	 */
	int getZSize();

	/**
	 * Returns the rotation of the Tile along the Y-Axis.
	 * @return rotation of the Tile along the Y-axis in degrees
	 */
	int getYRotation();

	/**
	 * Returns the height of the Tile in world coordinates.
	 * @return height of the Tile in world coordinates
	 */
	float getHeight();

	/**
	 * Returns the graphical representation of the Tile.
	 * @return model of the Tile
	 */
	ModelInterface* getModel();

	/**
	 * Returns the name of the XML-file where the Tile is stored.
	 * @return filname of the XML-File
	 */
	std::string getXmlFilename();

	/**
	 * Prints debugging information about the Tile.
	 */
	void dump();

	/// Pointer to data relevant for the SceneGraphInterface
	SceneGraphAttachment* sceneGraphAttachment;

private:
	/**
	 * Copy-constructor
	 * The copy-constructor initializes the new Tile with the data from the
	 * passed Tile. Therefore all member variables are copied and the model
	 * is cloned or deep-cloned according to the configuration.
	 * @param src Tile which should be cloned
	 */
	Tile(Tile* src);

	/// ID of the Tile
	unsigned int id;
	/// Name of the Tile
	std::string name;
	/// Horizontal and vertical size and rotation along the y-Axis
	int sizeX, sizeZ, yRotation;
	/// Height of the Tile
	float height;

	/// Graphical representation of the Tile
	ModelInterface* model;

	/// name of the XML-file where the Tile is stored
	std::string xmlFilename;
}; // Tile

#endif // _TILE_H
