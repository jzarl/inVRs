/*
 * AbstractHeightMapManager.cpp
 *
 *  Created on: 29.11.2010
 *      Author: sam
 */

#include "AbstractHeightMapManager.h"

#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/SystemCore/DebugOutput.h>

AbstractHeightMapManager::AbstractHeightMapManager() {

}

AbstractHeightMapManager::~AbstractHeightMapManager() {
	std::map<unsigned, HeightMapInterface*>::iterator it;
	for (it = tileMap.begin(); it != tileMap.end(); ++it) {
		if ((*it).second)
			delete (*it).second;
	}
	tileMap.clear();
}

void AbstractHeightMapManager::generateTileHeightMaps() {
	const std::vector<Tile*>& tiles = WorldDatabase::getTileList();
	for (std::vector<Tile*>::const_iterator it = tiles.begin(); it != tiles.end(); ++it) {
		getHeightMapOfTile(*it);
	}
}

float AbstractHeightMapManager::getHeightAtWorldPos(float worldX, float worldZ) {
	HeightMapInterface* h = NULL;
	gmtl::Vec3f tilePos;

	{
		Environment* env = WorldDatabase::getEnvironmentAtWorldPosition(worldX, worldZ);
		if (env) {
			Tile* tile = env->getTileAtWorldPosition(worldX, worldZ);
			if (tile) {
				tilePos = env->getWorldPositionOfTile(tile);
				h = getHeightMapOfTile(tile);
			}
		}
	}

	if (!h) {
		PRINTD_W("can't find heightmap for world pos: (%f,%f)\n", worldX, worldZ);
		return 0;
	}

	return h->getHeight(worldX - tilePos[0], worldZ - tilePos[2]);
}

HeightMapInterface* AbstractHeightMapManager::load(std::string fileName) {
	return loadImpl(fileName);
}

HeightMapInterface* AbstractHeightMapManager::getHeightMapOfTile(Tile* tile) {
	if (tileMap.find(tile->getId()) != tileMap.end()) {
		return tileMap[tile->getId()]; //cache hit
	}

	//load and save
	std::string fileName = generateFileName(tile);
	HeightMapInterface* h = loadImpl(fileName);
	if(h) {
		PRINTD(INFO, "found saved heightmap at %s\n",fileName.c_str());
	} else {
		PRINTD(INFO, "creating heightmap for tile %s\n",tile->getName().c_str());
		h = createImpl(tile);
		if(h) { //save to disk
			if(!saveImpl(h, fileName))
				PRINTD(WARNING, "can't save heightmap of tile %s to %s\n",tile->getName().c_str(), fileName.c_str());
		}
	}

	if(h) { //save in cache -> TODO what about locks?
		tileMap[tile->getId()] = h;
		return h;
	} else {
		PRINTD(WARNING, "can't neighter load or create heightmap for tile %s\n",tile->getName().c_str());
		return NULL;
	}
}

HeightMapInterface* AbstractHeightMapManager::getHeightMapOfTile(unsigned tileId) {
	if (tileMap.find(tileId) != tileMap.end()) {
		return tileMap[tileId]; //cache hit
	}
	Tile* tile = WorldDatabase::getTileWithId(tileId);
	if (!tile) {
		PRINTD(ERROR, "can't find tile with id %i\n", tileId);
		return NULL;
	}
	return getHeightMapOfTile(tile);
}
