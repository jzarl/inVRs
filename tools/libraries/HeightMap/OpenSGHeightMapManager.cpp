/*
 * OpenSGHeightMapManager.cpp
 *
 *  Created on: 29.11.2010
 *      Author: sam
 */

#include "OpenSGHeightMapManager.h"
#include "HeightMap.h"

#include <inVRs/SystemCore/Configuration.h>
#include <inVRs/OutputInterface/OpenSGSceneGraphInterface/OpenSGSceneGraphInterface.h>
#include <inVRs/SystemCore/DebugOutput.h>


OSG_USING_NAMESPACE

OpenSGHeightMapManager::OpenSGHeightMapManager() :
	filterKernelSize(3), filterKernelSigma(0.01) {
}

OpenSGHeightMapManager::~OpenSGHeightMapManager() {

}

std::string OpenSGHeightMapManager::generateFileName(Tile* tile) const {
	assert(tile);
	std::string path = Configuration::getPath("HeightMaps");
	return path + tile->getName() + ".htmp";
}

HeightMapInterface* OpenSGHeightMapManager::loadImpl(std::string fileName) const {
	HeightMap* heightMap = new HeightMap;
	if (heightMap->setup(fileName.c_str()))
		return heightMap;
	else
		return NULL;
}

HeightMapInterface* OpenSGHeightMapManager::createImpl(Tile* tile) const {
	if (!tile)
		return NULL;

	std::string fileName = generateFileName(tile);

	NodeRefPtr tileRoot;
	{ //check model is correct
		OpenSGModel* tileModel;
		tileModel = (OpenSGModel*) tile->getModel();
		if (!tileModel) {
			PRINTD(ERROR, "invalid tile model can't create a height map\n");
			return NULL;
		}
		tileRoot = tileModel->getNodePtr();
	}

	PRINTD(INFO, "generating HeightMap for tile with name %s and size %i %i!\n", tile->getName().c_str(), tile->getXSize(), tile->getZSize());

	NodeRefPtr rotNode;
	rotNode = Node::create();
	{ //setup view on the tile
		TransformRefPtr rotTrans;
		rotTrans = Transform::create();
		Quaternion q;
		Matrix m;

		q.setValueAsAxisDeg(Vec3f(0, 1, 0), tile->getYRotation());
		m.setIdentity();
		m.setRotate(q);
#if OSG_MAJOR_VERSION < 2
		beginEditCP(rotTrans, Transform::MatrixFieldMask);
#endif
		{
			rotTrans->setMatrix(m);
		}
#if OSG_MAJOR_VERSION < 2
		endEditCP(rotTrans, Transform::MatrixFieldMask);
		beginEditCP(rotNode, Node::CoreFieldMask | Node::ChildrenFieldMask);
#endif
		{
			rotNode->setCore(rotTrans);
			rotNode->addChild(tileRoot);
		}
#if OSG_MAJOR_VERSION < 2
		endEditCP(rotNode, Node::CoreFieldMask | Node::ChildrenFieldMask);
#endif
	}

	HeightMap* heightMap = new HeightMap;

	//TODO: configurable
	heightMap->setup(rotNode, tile->getXSize(), tile->getZSize(), true);
	//TODO configurable
	heightMap->setGaussFilterCoefficients(filterKernelSize, filterKernelSigma);
	heightMap->generateHeightMap(rotNode);
	heightMap->writeToFile(fileName.c_str());

	//subchild again so that OpenSG can cleanup the rest but not the tile itself
#if OSG_MAJOR_VERSION < 2
	beginEditCP(rotNode, Node::ChildrenFieldMask);
#endif
	{
		rotNode->subChild(tileRoot);
	}
#if OSG_MAJOR_VERSION < 2
	endEditCP(rotNode, Node::ChildrenFieldMask);
#endif

	return heightMap;
}


bool OpenSGHeightMapManager::saveImpl(HeightMapInterface* heightMap, std::string fileName) const {
	HeightMap* h = dynamic_cast<HeightMap*>(heightMap);
	if(!h) {
		PRINTD(WARNING, "invalid height map type\n");
		return false;
	}

	return h->writeToFile(fileName.c_str());
}
