/*
 * OpenSGCollisionLineFactory.cpp
 *
 *  Created on: 29.11.2010
 *      Author: sam
 */

#include "OpenSGCollisionLineSetFactory.h"
#include "CheckCollisionModifier.h"

#include <OpenSG/OSGSceneFileHandler.h>
#include <OpenSG/OSGPrimitiveIterator.h>

#include <inVRs/SystemCore/DebugOutput.h>


CollisionLineSet* OpenSGCollisionLineSetFactory::create(std::string className, std::string vrmlFile) {
#if OSG_MAJOR_VERSION >= 2
	OSG::NodeRefPtr root = OSG::SceneFileHandler::the()->read(vrmlFile.c_str());
#else
	OSG::NodeRefPtr root;
	root = OSG::SceneFileHandler::the().read(vrmlFile.c_str());
#endif
	assert(root);
	printd(INFO, "OpenSGCollisionLineFactory::create: Loaded Collision File %s\n", vrmlFile.c_str());

	std::vector<CollisionLineSet::CollisionLine *> collisionLines;
	readCollisionLines(root, collisionLines);

	return new CollisionLineSet(collisionLines);
}


void OpenSGCollisionLineSetFactory::readCollisionLines(OSG::NodeRefPtr n, std::vector<CollisionLineSet::CollisionLine *> &collisionLines) {
	int i, nChildren, size;
	OSG::PrimitiveIterator it;
	OSG::GeometryRefPtr geo;
	OSG::Pnt3f p1, p2;
	OSG::Vec3f vec1, vec2;
	CollisionLineSet::CollisionLine* line;

	std::string coretype = n->getCore()->getTypeName();
	if (coretype == "Geometry") {
		it = OSG::PrimitiveIterator(n);
		for (it.seek(0); !it.isAtEnd(); ++it) {
			size = it.getLength();
			if (size % 2 != 0)
				printd(WARNING, "WARNING: ERROR WITH COLLISION-LINE-INDICES!\n");
			for (i = 0; i < size; i += 2) {
				p1 = it.getPosition(i);
				p2 = it.getPosition(i + 1);
				//addCollLine(Vec2f(p1[0], p1[2]), Vec2f(p2[0], p2[2]));
				vec1 = p1.subZero();
				vec2 = p2.subZero();
				line = new CollisionLineSet::CollisionLine;
				line->startPoint = gmtl::Vec2f(vec1[0], vec1[2]);
				line->endPoint = gmtl::Vec2f(vec2[0], vec2[2]);
				collisionLines.push_back(line);
			} // for

		} // for
	} // if
	nChildren = n->getNChildren();
	for (i = 0; i < nChildren; i++)
	{
		OSG::NodeRefPtr child;
		child = n->getChild(i);
		readCollisionLines(child, collisionLines);
	}
}

CheckCollisionModifierFactory::CheckCollisionModifierFactory() : CheckCollisionModifierFactoryBase(new OpenSGCollisionLineSetFactory) {

}
