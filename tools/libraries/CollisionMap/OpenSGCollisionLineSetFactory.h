/*
 * OpenSGCollisionLineSetFactory.h
 *
 *  Created on: 29.11.2010
 *      Author: sam
 */

#ifndef OPENSGCOLLISIONLINESETFACTORY_H_
#define OPENSGCOLLISIONLINESETFACTORY_H_

#include "CollisionLineSet.h"
#include "CollisionMapExports.h"

#include <OpenSG/OSGConfig.h>
#if OSG_MAJOR_VERSION >= 2
#include <OpenSG/OSGNode.h>
#else
#include <OpenSG/OSGNodePtr.h>
#endif

class INVRS_COLLISIONMAP_API OpenSGCollisionLineSetFactory: public CollisionLineSetFactory {
public:
	virtual CollisionLineSet* create(std::string className, std::string vrmlFile);

private:
	/** Reads the collision lines from the passed OpenSG-tree.
	 * The method reads the lines from each Geometry core found in the passed
	 * OpenSG-tree and stores this lines in the collisionLines-list.
	 * @param n OpenSG-Tree containing the collision lines
	 */
	void readCollisionLines(OSG::NodeRefPtr n, std::vector<CollisionLineSet::CollisionLine *> &collisionLines);
};

#endif /* OPENSGCOLLISIONLINESETFACTORY_H_ */
