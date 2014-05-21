/*
 * OpenSGHeightMapManager.h
 *
 *  Created on: 29.11.2010
 *      Author: sam
 */

#ifndef OPENSGHEIGHTMAPMANAGER_H_
#define OPENSGHEIGHTMAPMANAGER_H_

#include "AbstractHeightMapManager.h"

class INVRS_HEIGHTMAP_API OpenSGHeightMapManager: public AbstractHeightMapManager {
public:
	OpenSGHeightMapManager();
	virtual ~OpenSGHeightMapManager();

	void setFilterProperties(float sigma, unsigned kernelSize) {
		filterKernelSigma = sigma;
		filterKernelSize= kernelSize;
	}

protected:
	virtual std::string generateFileName(Tile* tile) const;
	virtual HeightMapInterface* createImpl(Tile* tile) const;
	virtual HeightMapInterface* loadImpl(std::string fileName) const;
	virtual bool saveImpl(HeightMapInterface* heightMap, std::string fileName) const;

private:
	unsigned filterKernelSize;
	float filterKernelSigma;
};

#endif /* OPENSGHEIGHTMAPMANAGER_H_ */
