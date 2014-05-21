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

#ifndef MODELINTERFACE_H_
#define MODELINTERFACE_H_

#include "OutputInterfaceSharedLibraryExports.h"
#include "SceneGraphNodeInterface.h"

#include "../SystemCore/DataTypes.h"

/**
 * ModelInterface is an abstract class for a visual representation of all kinds of objects
 * (entities, tiles, avatars ...)
 */
class INVRS_OUTPUTINTERFACE_API ModelInterface {
public:
	ModelInterface();
	virtual ~ModelInterface();

	/**
	 * Produces a new instance of the same derived ModelInterface-class which should have
	 * identical visual properties. Wheter the datastructures for rendering will be copied
	 * or just referenced by the new instance depends on the current clone mode.
	 * The newly created instance will have the same model transformation
	 * @return pointer to the newly created instance
	 */
	virtual ModelInterface* clone() = 0;

	/**
	 * Sets the clone mode for instances created via <code>clone()</code>.
	 * @param deepClone	if true <code>clone()</code> produces an deep copy of <code>this</code>
	 *					otherwise both instances will share the same datastructures for visualization ("shallow copy")
	 * The default clone mode is the "shallow copy" clone mode.
	 */
	virtual void setDeepCloneMode(bool deepClone);

	/**
	 * Returns the current clone mode.
	 */
	virtual bool getDeepCloneMode();

	/**
	 * @todo: document
	 */
	virtual SceneGraphNodeInterface* getSubNodeByName(std::string nodeName) = 0;

	/**
	 * Sets the model transformation which is applied on the model before any other transformations.
	 * The main purpose of the model transformation is to adjust models in such a way that they fit
	 * to the rest of models already in the world
	 */
	virtual void setModelTransformation(TransformationData& trans) = 0;

	/**
	 * Getter for the model transformation.
	 */
	virtual TransformationData getModelTransformation() = 0;

	/**
	 * Returns the AABB of the model in "model space" (but including the model transformation).
	 * @return the AABB of the model
	 */
	virtual AABB getAABB() = 0;

	/**
	 * Defines whether the model should be rendered or not
	 * @param visibility visibility state
	 */
	virtual void setVisible(bool visibility) = 0;

	/**
	 * Returns if the model will be rendered or not
	 * @return visibility status of model
	 */
	virtual bool isVisible() = 0;

	/**
	 * Sets the relative path of model in the filesystem
	 * @param path relative path of model in the filesystem
	 */
	virtual void setFilePath(std::string path) = 0;

	/**
	 * Returns the relativ path of model in the filesystem
	 * @return path of model in the filesystem
	 */
	virtual std::string getFilePath() = 0;

protected:
	bool deepClone;
}; // ModelInterface

#endif /* MODELINTERFACE_H_ */
