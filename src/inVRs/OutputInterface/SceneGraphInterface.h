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

#ifndef _SCENEGRAPHINTERFACE_H
#define _SCENEGRAPHINTERFACE_H

#include "OutputInterfaceSharedLibraryExports.h"
#include "ModelInterface.h"
#include "SceneGraphAttachment.h"
#include "SceneGraphNodeInterface.h"
#include "GeometrySceneGraphNodeInterface.h"
#include "GroupSceneGraphNodeInterface.h"
#include "TransformationSceneGraphNodeInterface.h"

#include "../SystemCore/ComponentInterfaces/ModuleInterface.h"
#include "../SystemCore/DataTypes.h"
#include "../SystemCore/XMLTools.h"

class Environment;
class Entity;
class Tile;

/*****************************************************************************
 * The SceneGraphInterface abstracts operations on the visual representation of
 * all kinds of objects (tiles, entities and so on). It allows adding and
 * removing of models (of type ModelInterface), entities and tiles. Hirarchical
 * scenes can be built up by attaching entities and tiles to environments.
 * A few notes regarding its usage: first of all the visualization concept is
 * completely orhogonal to the rest of the WorldDatabase. That means if you
 * attach or detach something to an environment it only affects the graphical
 * output.
 * However entities and tiles can be attached only to environments they reside
 * in. In order to add an entity (as an example, but the same is true for tiles)
 * you must first add it to the environment and then you can attach it to the
 * SceneGraphInterface. When removing an entity (or tile) detachEntity() must be
 * called before the entity is actualy removed from the environment (via
 * removeEntity() or whatever). Similary if an entity is entering another
 * environment it has to be detached from the scene graph interface first and
 * later on attached to the environment it is moving to. Not all implementations
 * will generate an error message in case of an inappropriate use.
 * Some good news finally: If you want to make an environment invisible you
 * don't need to detach everything, you can simply use
 * <code>showEnvironment()</code>.
 * There exists only one instance of a SceneGraphInterface for the whole
 * lifetime of the application. It can be obtained by the SystemCore.
 */
class INVRS_OUTPUTINTERFACE_API SceneGraphInterface : public ModuleInterface {
public:
	SceneGraphInterface();
	virtual ~SceneGraphInterface();

	/**
	 * Must be invoked before any of the other methods can be called
	 * @return returns true on success and false on failure
	 */
	virtual bool init() = 0;

	/**
	 * Instruct the module to read its configuration from a file.
	 * @return true on success.
	 * @param configFile path of the XML-file.
	 */
	virtual bool loadConfig(std::string configFile) = 0;

	/**
	 * Instruct the module to free all memory allocated internally.
	 */
	virtual void cleanup() = 0;

	/**
	 * Return the unique name of the module.
	 */
	virtual std::string getName() = 0;

	/**
	 * \todo write doku
	 */
	virtual Event* createSyncEvent() {
		return NULL;
	}

	/**
	 * Loads a model from the filesystem
	 * @param url complete path pointing to the modelfile
	 * @return returns the newly created instance
	 */
	virtual ModelInterface* loadModel(std::string fileType, std::string url) = 0;

	/**
	 * Attaches an environment to the scene graph.
	 * Entities and tiles can only be attached to environments which in turn have been attached already
	 * @param env environment which should be attached.
	 * @return true on success and false on failure
	 */
	virtual bool attachEnvironment(Environment* env) = 0;

	/**
	 * Attaches an entity (more precisely its model) to the environment it resides on. This is not necessarily the environment identified by
	 * the environment id of the entity. The environment must be attached already.
	 * @param ent entity which should be attached
	 * @return true on success and false on failure
	 */
	virtual bool attachEntity(Entity* ent) = 0;

	/**
	 * Attaches te model of a tile to scene graph.
	 * @param env the environment the tile is attached to. Must be in the scene graph already
	 * @param tle the tile whos model should be attached
	 * @param trans the transformation of the tile (necessary because the tile itself doesn't know where it is located within the environmet)
	 * @return true on success and false on failure
	 */
	virtual bool attachTile(Environment* env, Tile* tle, const TransformationData& trans) = 0;

	/**
	 * Attaches a model. The model is not attached to a specific environment but directly to the root of the scene graph.
	 * Additionally it can undergo a transformation which doesn't affect any other instances created via clone()
	 * @param model model which should be attached
	 * @param trans additional transformation applied to the model (after the modeltransformation)
	 * @return true on success and false on failure
	 */
	virtual bool attachModel(ModelInterface* model, const TransformationData& trans) = 0;

	/**
	 * Attaches a model to an entity.
	 * @param ent entity the model is attached to (must be already attached to the scenegraph)
	 * @param model model which should be attached
	 * @param trans transformation relative to the entity the model is attached to
	 * @return true on success and false on failure
	 */
	virtual bool
			attachModelToEntity(Entity* ent, ModelInterface* model, const TransformationData& trans) = 0;

	/**
	 * Detaches an environment form the scene graph. The environment must not contain any attached tiles or entities.
	 * @param env environment which should be detached from the scene graph
	 * @return true on success and false on failure
	 */
	virtual bool detachEnvironment(Environment* env) = 0;

	/**
	 * Detaches an entity. This only affects the visual representation of the entity.
	 * @param ent entity which should be detached
	 * @return true on success and false on failure
	 */
	virtual bool detachEntity(Entity* ent) = 0;

	/**
	 * Detaches a tile from a environment
	 * @param env environment where the tle is attached to
	 * @param tle tile which should be detached
	 * @return true on success and false on failure
	 */
	virtual bool detachTile(Environment* env, Tile* tle) = 0;

	/**
	 * Detaches a model.
	 * @param model model which should be detached
	 * @return true on success and false on failure
	 */
	virtual bool detachModel(ModelInterface* model) = 0;

	/**
	 * Intersects a model (also considering the modeltransformation) with a ray given by <code>position</code> and direction
	 * @param position in model space
	 * @param direction in model space
	 * @param dist position + dist*direction is the intersection point
	 * @return returns true if model is hit by the ray within maxDist
	 */
	virtual bool rayIntersect(ModelInterface* model, gmtl::Vec3f position, gmtl::Vec3f direction,
			float* dist, float maxDist) = 0;

	/**
	 * Sets the transformation of the model in the scene graph (applied after the modeltransformation) to trans
	 * @param trans the new transformation of the model
	 */
	virtual void updateModel(ModelInterface* model, const TransformationData& trans) = 0;

	/**
	 * Updates the transformation of the model of the entity relative to its environment
	 * @param ent entity which will be updated
	 */
	virtual void updateEntity(Entity* ent) = 0;

	/**
	 * Adjusts the scene graph such that it matches again to the transformation of the environment relative to the world
	 * @param env environment
	 */
	virtual void updateEnvironment(Environment* env) = 0;

	/**
	 * Sets the transformation of the model belonging to a tile relative to its environment to trans
	 * @param tile tile in the scene graph
	 * @param trans new transformation of the model of the tile (applied after the modeltransformation)
	 */
	virtual void updateTile(Tile* tle, const TransformationData& trans) = 0;

	/**
	 * Shows or hides an environment and all objects which are attached to it.
	 * @param env environment which should become either visible or invisible
	 * @param showOrHide if set to true the env will be shown and vice versa
	 */
	virtual void showEnvironment(Environment* env, bool showOrHide) = 0;

	/**
	 * Shows or hides a model
	 * @param model model whos visibility state is being set
	 * @param showOrHide if set to true the env will be shown and vice versa
	 */
	virtual void showModel(ModelInterface* model, bool showOrHide) = 0;

}; // SceneGraphInterface

#endif // _SCENEGRAPHINTERFACE_H
