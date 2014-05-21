/*
 * AbstractHeightMapManager.h
 *
 *  Created on: 29.11.2010
 *      Author: sam
 */

#ifndef ABSTRACTHEIGHTMAPMANAGER_H_
#define ABSTRACTHEIGHTMAPMANAGER_H_

#include "HeightMapInterface.h"
#include <inVRs/SystemCore/WorldDatabase/Tile.h>
#include <map>

class INVRS_HEIGHTMAP_API AbstractHeightMapManager {
public:
	AbstractHeightMapManager();
	virtual ~AbstractHeightMapManager();

	void generateTileHeightMaps();

	float getHeightAtWorldPos(float worldX, float worldZ);

	HeightMapInterface* getHeightMapOfTile(unsigned tileId);
	HeightMapInterface* getHeightMapOfTile(Tile* tile);

	//uncached loading
	HeightMapInterface* load(std::string fileName);

protected:
	virtual std::string generateFileName(Tile* tile) const = 0;
	virtual HeightMapInterface* createImpl(Tile* tile) const = 0;
	virtual HeightMapInterface* loadImpl(std::string fileName) const = 0;
	virtual bool saveImpl(HeightMapInterface* heightMap, std::string fileName) const = 0;

private:
	std::map<unsigned, HeightMapInterface*> tileMap;

};

#endif /* ABSTRACTHEIGHTMAPMANAGER_H_ */
