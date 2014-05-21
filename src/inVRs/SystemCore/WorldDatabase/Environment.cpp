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

#include "Environment.h"

#include <assert.h>

#include "WorldDatabase.h"
#include "WorldDatabaseEvents.h"
#include "../DataTypes.h"
#include "../DebugOutput.h"
#include "../EventManager/EventManager.h"
#include "../IdPoolManager.h"
//#include "../SystemCore.h"
#include "../../OutputInterface/OutputInterface.h"

Environment::Environment(int xSpacing, int zSpacing, int initialXSize, int initialZSize,
		unsigned environmentId) :
	globalEnvironmentIdPool(0, 0xFFFF) {
	this->xSpacing = xSpacing;
	this->zSpacing = zSpacing;
	this->sizeX = initialXSize;
	this->sizeZ = initialZSize;
	this->environmentId = environmentId;

	posX = 0;
	posZ = 0;
	xOffset = 0;
	zOffset = 0;

	sgIF = OutputInterface::getSceneGraphInterface();
	assert(sgIF);

	tileGrid = new Tile*[sizeX * sizeZ];
	memset(tileGrid, 0, sizeof(Tile*) * sizeX * sizeZ);

	envTransformation = identityTransformation();

	localEnvironmentIdPool = NULL;
	loadTimeEnvironmentIdPool = globalEnvironmentIdPool.allocSubPool(0, 4096);
	if (!loadTimeEnvironmentIdPool)
		printd(ERROR, "Environment::Environment(): failed to allocate load time id pool\n");

	char nameBuffer[256];
	sprintf(nameBuffer, "Env%u_pool", environmentId);
	globalEnvironmentIdPoolName = std::string(nameBuffer);
	localIdPoolManager.registerIdPool(globalEnvironmentIdPoolName, &globalEnvironmentIdPool);
	printd(INFO, "Environment::Environment(): registered id pool %s\n",
			globalEnvironmentIdPoolName.c_str());

	sceneGraphAttachment = NULL;
} // Environment

Environment::~Environment() {
	int i, j;
	Entity* entity;
	std::map<unsigned, AbstractEntityCreationCB*>::iterator it;

	while (entityList.size() > 0) {
		entity = entityList[0];
		removeEntity(entity);
		delete entity;
	} // for

	for (i = 0; i < sizeZ; i++) {
		for (j = 0; j < sizeX; j++) {
			if (tileGrid[i * sizeX + j] != NULL) {
				delete removeTileAtGridPosition(j, i);
			} // if
		} // for
	} // for
	delete[] tileGrid;

	if (sgIF)
		sgIF->detachEnvironment(this);

	if (sceneGraphAttachment != NULL) {
		printd(ERROR, "Environment::~Environment(): detach me from scene graph interface first!\n");
	} // if

	for (it = entityCreationCallback.begin(); it != entityCreationCallback.end(); ++it) {
		if ((it->second) != NULL)
			delete (it->second);
	} // for

} // ~Environment

void Environment::addEntryPoint(gmtl::Vec3f position, gmtl::Vec3f direction) {
	float angle;
	gmtl::Vec3f axis;
	gmtl::Vec3f front(0, 0, -1);
	gmtl::Vec3f up(0, 1, 0);
	gmtl::AxisAnglef axisAngle;
	TransformationData trans = identityTransformation();

	gmtl::normalize(direction);
	trans.position = position;
	gmtl::cross(axis, front, direction);
	gmtl::normalize(axis);
	if (gmtl::length(axis) < 0.9f)
		axis = gmtl::Vec3f(0, 1, 0);
	angle = acos(gmtl::dot(front, direction));
	axisAngle = gmtl::AxisAnglef(angle, axis);

	gmtl::set(trans.orientation, axisAngle);
	gmtl::Vec3f testUp = trans.orientation * up;
	if (testUp[1] < 0) {
		gmtl::AxisAnglef fixRotation(M_PI, front);
		gmtl::Quatf fixRotationQuat;
		gmtl::set(fixRotationQuat, fixRotation);
		trans.orientation = trans.orientation * fixRotationQuat;
	}

	entryPoints.push_back(trans);
} // addEntryPoint

void Environment::deleteEntryPoint(unsigned index) {
	if(index < entryPoints.size()) {
		entryPoints.erase(entryPoints.begin() + index);
	} // if
} // deleteEntryPoint

bool Environment::setTileAtGridPosition(int x, int z, Tile* tile) {
	Tile* oldTile;
	int i, j, index;
	int xPoints, zPoints;
	char* newtilename;
	char* oldtilename;
	TransformationData trans;

	if (!tile) {
		printd(WARNING, "Environment::setTileAtGridPosition(): NULL tile passed at %dx%d\n", x, z);
		return false;
	} // if

	xPoints = tile->getXSize() / xSpacing;
	zPoints = tile->getZSize() / zSpacing;
	printd(
			INFO,
			"Environment::setTileAtGridPosition(): setting tile with size %i %i in %i %i grid points!\n",
			tile->getXSize(), tile->getZSize(), xPoints, zPoints);

	if (!((x < sizeX) && (z < sizeZ) && (x + xPoints <= sizeX) && (z + zPoints <= sizeZ))) {
		newtilename
				= tile->getName().size() > 0 ? (char*)tile->getName().c_str() : (char*)"unnamed";
		printd(
				ERROR,
				"Environment::setTileAtGridPosition(): tile %s cannot be placed at %dx%d\n\tgrid size is %dx%d and spacing is %dx%d\n\ttile size is %dx%d\n",
				newtilename, x, z, sizeX, sizeZ, xSpacing, zSpacing, tile->getXSize(),
				tile->getZSize());
		delete tile;
		return false;
	} // if

	// check if tiles overlapp:
	for (i = 0; i < zPoints; i++) {
		for (j = 0; j < xPoints; j++) {
			index = (z + i) * sizeX + (x + j);
			if (tileGrid[index] != NULL) {
				oldTile = tileGrid[index];
				newtilename
						= tile->getName().size() > 0 ? (char*)tile->getName().c_str() : (char*)"unnamed";
				oldtilename
						= oldTile->getName().size() > 0 ? (char*)tile->getName().c_str() : (char*)"unnamed";

				printd(
						ERROR,
						"Environment::setTileAtGridPosition(): tile %s (new) conflicts with %s (existing) at %dx%d\n\tgrid size is %dx%d and spacing is %dx%d\n\tnew tile size is %dx%d\n\tsize of existing tile ist %dx%d\n",
						newtilename, oldtilename, z + i, x + j, sizeX, sizeZ, xSpacing, zSpacing,
						xPoints, zPoints, oldTile->getXSize() / xSpacing, oldTile->getZSize()
								/ zSpacing);
				dumpTileGrid();
				delete tile;
				return false;
			} // if
		} // for
	} // for

	// everything ok, put tile instance into tileGrid
	for (i = 0; i < zPoints; i++) {
		for (j = 0; j < xPoints; j++) {
			index = (z + i) * sizeX + (x + j);
			tileGrid[index] = tile;
		} // for
	} // for

	// put into scene graph
	trans = identityTransformation();
	trans.position = gmtl::Vec3f((tile->getXSize() / 2.f) + (x) * xSpacing, tile->getHeight(), (z)
			* zSpacing + (tile->getZSize() / 2.f));
	setQuatfAsAxisAngleDeg(trans.orientation, 0, 1, 0, tile->getYRotation());

	if (sgIF)
		sgIF->attachTile(this, tile, trans);

	return true;
} // setTileAtGridPosition

Tile* Environment::removeTileAtGridPosition(int x, int z) {
	Tile* ret;
	int i, j, xPoints, zPoints;

	if (x < 0 || z < 0 || x >= sizeX || z >= sizeZ) {
		printd(
				ERROR,
				"Environment::removeTileAtGridPosition(): invalid positions X=%i Y=%i (environmentSize = X=%i Y=%i)!\n",
				x, z, sizeX, sizeZ);
		return NULL;
	} // if

	ret = tileGrid[z * sizeX + x];
	if (ret == NULL)
		return NULL;

	// remove from scenegraph:
	if (sgIF)
		sgIF->detachTile(this, ret);

	xPoints = ret->getXSize() / xSpacing;
	zPoints = ret->getZSize() / zSpacing;

	// remove from tileGrid:
	for (i = 0; i < zPoints; i++) {
		for (j = 0; j < xPoints; j++) {
			if (tileGrid[(z + i) * sizeX + (x + j)] != ret) {
				printd(
						ERROR,
						"Environment::removeTileInstanceAt(): internal error: removing tile %d at (%d,%d):\n cannot remove part of tile at (%d,%d)\n\ttile has size %dx%d\n\toffset is (%d,%d)\n",
						ret->getId(), x, z, x + j, z + i, xPoints, zPoints, xOffset, zOffset);
				assert(false);
			} // if
			tileGrid[(z + i) * sizeX + (x + j)] = NULL;
		} // for
	} // for

	return ret;
} // removeTileAtGridPosition

unsigned Environment::createEntity(EntityType* entType, TransformationData initialTransfInEnv,
		AbstractEntityCreationCB* callback) {
	Event * event;
	unsigned short idInEnv;
	unsigned environmentBasedId;

	assert(entType);

	idInEnv = getFreeEntityId();
	environmentBasedId = join(environmentId, idInEnv);

	// TODO: think about how to determine srcModuleId for Event
	// Event is created by EntityType (so that this one can send a specific event if needed!)
	event = entType->createEntityCreationEvent(this, environmentBasedId, initialTransfInEnv);

	EventManager::sendEvent(event, EventManager::EXECUTE_GLOBAL);

	if (callback != NULL)
		entityCreationCallback[environmentBasedId] = callback;

	return environmentBasedId;
} // createEntity

bool Environment::destroyEntity(Entity* entity, AbstractEntityDeletionCB* callback) {
	Event * event;

	if (!entity) {
		// TODO: check if we have to delete callback!!!
		if (callback)
			delete callback;
		return false;
	}

	event = new WorldDatabaseDestroyEntityEvent(entity, WORLD_DATABASE_ID);
	EventManager::sendEvent(event, EventManager::EXECUTE_GLOBAL);

	if (callback != NULL)
		entityDeletionCallback[entity->getEnvironmentBasedId()] = callback;

	return true;
} // destroyEntity

bool Environment::unregisterEntityCreationCallback(unsigned environmentBasedId) {
	bool success = false;
	std::map<unsigned, AbstractEntityCreationCB*>::iterator it;

	it = entityCreationCallback.find(environmentBasedId);
	if (it != entityCreationCallback.end()) {
		delete (it->second);
		entityCreationCallback.erase(it);
		success = true;
	} // if
	else {
		printd(
				WARNING,
				"Environment::unregisterEntityCreationCallback(): no callback for entity with ID %u found! IGNORING!\n",
				environmentBasedId);
	} // else
	return success;
} // unregisterEntityCreationCallback

bool Environment::unregisterEntityDeletionCallback(unsigned environmentBasedId) {
	bool success = false;
	std::map<unsigned, AbstractEntityDeletionCB*>::iterator it;

	it = entityDeletionCallback.find(environmentBasedId);
	if (it != entityDeletionCallback.end()) {
		delete (it->second);
		entityDeletionCallback.erase(it);
		success = true;
	} // if
	else {
		printd(
				WARNING,
				"Environment::unregisterEntityDeletionCallback(): no callback for entity with ID %u found! IGNORING!\n",
				environmentBasedId);
	} // else

	return success;
} // unregisterEntityDeletionCallback

void Environment::enlargeEnvironment(int xDirection, int zDirection) {
	Tile** newTileGrid;
	int absXDirection, absZDirection;
	int i, j;
	int newSizeX, newSizeZ;
	int movX, movZ;

	if ((xDirection == 0) && (zDirection == 0))
		return;

	if (xDirection < 0) {
		absXDirection = -xDirection;
		movX = -xDirection;
	} // if
	else {
		absXDirection = xDirection;
		movX = 0;
	} // else

	if (zDirection < 0) {
		absZDirection = -zDirection;
		movZ = -zDirection;
	} // if
	else {
		absZDirection = zDirection;
		movZ = 0;
	} // else

	newSizeX = sizeX + absXDirection;
	newSizeZ = sizeZ + absZDirection;
	newTileGrid = new Tile*[newSizeX * newSizeZ];
	memset(newTileGrid, 0, sizeof(Tile*) * newSizeX * newSizeZ);

	for (i = 0; i < sizeZ; i++) {
		for (j = 0; j < sizeX; j++) {
			newTileGrid[(i + movZ) * newSizeX + (j + movX)] = tileGrid[i * sizeX + j];
		} // for
	} // for

	delete[] tileGrid;
	tileGrid = newTileGrid;
	sizeX = newSizeX;
	sizeZ = newSizeZ;
	xOffset = movX;
	zOffset = movZ;

	envTransformation.position += gmtl::Vec3f(-xOffset * xSpacing, 0, -zOffset * zSpacing);
} // enlargeEnvironment

//void shrinkEnvironment(int xDirection, int yDirection, std::vector<Entity*>* lostEntities, std::vector<TileInstance*>* lostTileInstances)
//{
//}

TransformationData Environment::convertWorldToEnvironment(TransformationData worldTransformation) {
	TransformationData result, invWorldTransformation;

	invWorldTransformation = getWorldTransformation();
	invert(invWorldTransformation);
	multiply(result, invWorldTransformation, worldTransformation);
	return result;
} // convertWorldToEnvironment

TransformationData Environment::convertEnvironmentToWorld(
		TransformationData environmentTransformation) {
	TransformationData result = environmentTransformation;
	result.position += envTransformation.position;
	return result;
} // convertEnvironmentToWorld

bool Environment::rayIntersect(gmtl::Vec3f position, gmtl::Vec3f direction, float* intDist,
		Entity** intersectedEntity) {
	int i;
	bool ret, res;
	Entity* ent;
	float minDist;
	int minIdx;
	float d;
	ModelInterface* entityModel;

	if (!sgIF) {
		printd(
				ERROR,
				"Environment::rayIntersect(): no ScenegraphInterface set, ray intersection test cannot be performed\n");
		return false;
	} // if

	/*
	 TransformationData worldToEnv, envToEnt;
	 gmtl::Vec4f directionEnv, positionEnv;
	 gmtl::Vec4f direction4Comp, position4Comp;
	 gmtl::Vec4f directionEnt, positionEnt;
	 worldToEnv = getWorldTransformation();
	 invert(worldToEnv);

	 for(i=0;i<3;i++)
	 {
	 position4Comp[i] = position[i];
	 direction4Comp[i] = direction[i];
	 } // for

	 direction4Comp[3] = 0;
	 position4Comp[3] = 1;
	 transformVector(directionEnv, worldToEnv, direction4Comp);
	 transformVector(positionEnv, worldToEnv, position4Comp);
	 */

	minIdx = -1;
	minDist = 400000.0f;
	ret = false;

	for (i = 0; i < (int)entityList.size(); i++) {
		ent = entityList[i];
		if (ent->isFixed())
			continue;
		/*
		 envToEnt = ent->getEnvironmentTransformation();
		 invert(envToEnt);
		 transformVector(directionEnt, envToEnt, directionEnv);
		 transformVector(positionEnt, envToEnt, positionEnv);

		 position = gmtl::Vec3f(positionEnt[0], positionEnt[1], positionEnt[2]);
		 direction = gmtl::Vec3f(directionEnt[0], directionEnt[1], directionEnt[2]);
		 */

		// TODO: transform into model space
		entityModel = ent->getVisualRepresentation();
		res = sgIF->rayIntersect(entityModel, position, direction, &d, 0); // 0 -> no max dist
		// TODO: check what this assertion was for! Failed on execution on prism!
		//		assert(d<=400000.0f);
		if (res) {
			// 			printd("Environment::rayIntersect(): hit entity of type %s at %0.03f\n", ent->getEntityType()->getName().c_str(), d);
			ret = true;
			if (d <= minDist) {
				minDist = d;
				minIdx = i;
			} // if
		} // if
	} // for

	if (ret) {
		if (intDist)
			*intDist = minDist;
		if (intersectedEntity)
			*intersectedEntity = entityList[minIdx];
	} // if

	return ret;
} // rayIntersect

void Environment::setVisible(bool visible) {
	if (sgIF)
		sgIF->showEnvironment(this, visible);
} // setVisible

void Environment::setXPosition(int xPosition) {
	posX = xPosition;
	updatePosition();
} // setXPosition

void Environment::setZPosition(int zPosition) {
	posZ = zPosition;
	updatePosition();
} // setZPosition

void Environment::setXmlFilename(std::string filename) {
	this->xmlFilename = filename;
} // setXmlFileName

unsigned short Environment::getId() {
	return environmentId;
} // getId

TransformationData Environment::getStartTransformation(unsigned index) {
	if (index >= entryPoints.size()) {
		printd(WARNING, "Environment::getStartTrans(): invalid index %u!\n", index);
		return identityTransformation();
	} // if

	return entryPoints[index];
} // getStartTransformation

TransformationData Environment::getWorldStartTransformation(unsigned index) {
	TransformationData result;
	TransformationData entryPoint = getStartTransformation(index);
	multiply(result, envTransformation, entryPoint);
	return result;
} // getStartTransformation

unsigned Environment::getNumberOfStartTransformations() {
	return entryPoints.size();
}

int Environment::getXSpacing() {
	return xSpacing;
} // getXSpacing

int Environment::getZSpacing() {
	return zSpacing;
} // getYSpacing

int Environment::getXSize() {
	return sizeX;
} // getXSize

int Environment::getZSize() {
	return sizeZ;
} // getYSize

int Environment::getXPosition() {
	return posX;
} // getXPosition

int Environment::getZPosition() {
	return posZ;
} // getYPosition

TransformationData Environment::getWorldTransformation() {
	return envTransformation;
} // getWorldTransformation

Tile* Environment::getTileAtGridPosition(int x, int z) {
	// when enlarging an Environment then 0,0 should be the upper left tile again and not
	// the tile which was at position 0,0 original
	// TODO: add extra method for getting the tile that was inital at position 0,0
	//	x += xOffset;
	//	y += yOffset;
	assert(x >= 0 && x < sizeX);
	assert(z >= 0 && z < sizeZ);
	return tileGrid[z * sizeX + x];
} // getTileAtGridPosition

Tile* Environment::getTileAtWorldPosition(float x, float z) {
	int gridPosX, gridPosZ;
	gridPosX = (int)((x / xSpacing) - posX);
	gridPosZ = (int)((z / zSpacing) - posZ);
	if (gridPosX < 0 || gridPosZ < 0 || gridPosX >= sizeX || gridPosZ >= sizeZ)
		return NULL;
	return tileGrid[gridPosZ * sizeX + gridPosX];
} // getTileAtWorldPosition

gmtl::Vec3f Environment::getWorldPositionOfTile(Tile* tle) {
	int i, j = 0;
	Tile *gridTle = NULL;
	gmtl::Vec3f result;

	for (i = 0; i < sizeZ; i++) {
		for (j = 0; j < sizeX; j++) {
			gridTle = tileGrid[i * sizeX + j];
			if (gridTle == tle)
				break;
		} // for
		if (gridTle == tle)
			break;
	} // for
	if (gridTle != tle) {
		printd(
				WARNING,
				"Environment::getTileWorldPosition(): Tile with id %i not found in Environment with id %u\n",
				tle->getId(), this->environmentId);
		return gmtl::Vec3f(0, 0, 0);
	} // if
	result = this->envTransformation.position;
	result[0] += j * xSpacing;
	result[2] += i * zSpacing;
	result[0] += tle->getXSize() / 2.f;
	result[2] += tle->getZSize() / 2.f;
	return result;
} // getWorldPositionOfTile


Entity* Environment::getEntityByEnvironmentBasedId(unsigned envBasedId) {
	int i;
	Entity* result = NULL;
	unsigned short environmentId, entityId;

	split(envBasedId, environmentId, entityId);
	if (environmentId == this->environmentId)
		result = entityMap[entityId];

	if (result)
		return result;

	for (i = 0; i < (int)entityList.size(); i++) {
		if (entityList[i]->getEnvironmentBasedId() == envBasedId) {
			result = entityList[i];
			break;
		} // if
	} // for

	return result;
} // getEntityByEnvironmentBasedId

Tile** Environment::getTileGrid() {
	return tileGrid;
} // getTileGrid

const std::vector<Entity*>& Environment::getEntityList() {
	return entityList;
} // getEntityList

std::string Environment::getXmlFilename() {
	return xmlFilename;
} // getXmlFilename

void Environment::dump() {
	int i;
	printd(INFO, "--[dumpEnvironment]-------------------------------------\n");
	printd(INFO, "Environment::dump(): ID=%u SIZE: %ix%i POS: %i %i\n", this->environmentId,
			this->sizeX, this->sizeZ, this->posX, this->posZ);
	// 	std::cout << "  Environment - SIZE: "<<this->sizeX<<"x"<<this->sizeY<<" POS: "<<this->posX<<" "<<this->posY << std::endl;
	dumpTileGrid();

	printd(INFO, "--[dumpEntities]-------------------------------------\n");
	for (i = 0; i < (int)this->entityList.size(); i++)
		entityList[i]->dump();
	printd(INFO, "--[dumpEntities end]-------------------------------------\n");

	printd(INFO, "--[dumpEnvironment end]-------------------------------------\n");
} // dump

void Environment::dumpTileGrid() {
	int i, j;
	char *output = new char[(sizeX * 5 + 2) * (sizeZ) + 1];
	char *line = new char[(5 + 2)];
	output[0] = '\0';
	for (j = 0; j < sizeZ; j++) {
		for (i = 0; i < sizeX; i++) {
			if (!tileGrid[j * sizeX + i])
				strcat(output, "NL ");
			else {
				sprintf(line, "%2i ", tileGrid[j * sizeX + i]->getId());
				strcat(output, line);
			}
		} // for
		strcat(output, "\n");
	} // for
	printd(INFO, "Environment::dumpTileGrid():\n%s", output);
	delete[] output;
	delete[] line;
} // dumpTileGrid


bool Environment::addEntity(Entity* entity) {
	assert(entity);
	entity->setEnvironment(this);
	entityList.push_back(entity);
	if (sgIF)
		sgIF->attachEntity(entity);
	else
		printd("OHNO, no scenegraphinterface set!\n");

	return true;
} // addEntity

bool Environment::removeEntity(Entity* entity) {
	int i;
	unsigned environmentBasedId;
	unsigned short environmentId, entityId;
	std::map<unsigned, AbstractEntityDeletionCB*>::iterator it;

	if (!entity)
		return false;

	environmentBasedId = entity->getEnvironmentBasedId();
	split(environmentBasedId, environmentId, entityId);

	for (i = 0; i < (int)entityList.size(); i++) {
		if (entityList[i] == entity)
			break;
	} // for

	if (i == (int)entityList.size()) {
		printd(
				WARNING,
				"Environment::removeEntity(): Entity with environmentId %u and entityId %u not found in this Environment!\n",
				environmentId, entityId);
		return false;
	} // if

	if (sgIF)
		sgIF->detachEntity(entity);

	entityList.erase(entityList.begin() + i);

	return true;
} // removeEntity


std::string Environment::getGlobalEnvironmentIdPoolName() {
	return globalEnvironmentIdPoolName;
} // getGlobalEnvironmentIdPoolName

void Environment::setLocalEnvironmentIdPool(IdPool* newlocalPool) {
	localEnvironmentIdPool = newlocalPool;
} // setLocalEnvironmentIdPool

void Environment::updatePosition() {
	envTransformation.position[0] = posX * xSpacing;
	envTransformation.position[2] = posZ * zSpacing;

	if (sgIF)
		sgIF->updateEnvironment(this);
} // updatePosition

bool Environment::addNewEntity(Entity* entity) {
	unsigned int environmentBasedId;
	unsigned short environmentId, entityId;
	std::map<unsigned, AbstractEntityCreationCB*>::iterator it;

	assert(entity);

	entity->setEnvironment(this);
	entityList.push_back(entity);

	environmentBasedId = entity->getEnvironmentBasedId();
	split(environmentBasedId, environmentId, entityId);

	if (environmentId == this->environmentId)
		entityMap[entityId] = entity;
	else {
		printd(ERROR,
				"Environment::addNewEntity(): Entity for different Environment can not be added!\n");
		return false;
	} // else

	if (sgIF)
		sgIF->attachEntity(entity);
	else
		printd("OHNO, no scenegraphinterface set!\n");

	it = entityCreationCallback.find(environmentBasedId);
	if (it != entityCreationCallback.end()) {
		(it->second)->call(entity);
		delete (it->second);
		entityCreationCallback.erase(it);
	} // if

	return true;
} // addNewEntity

void Environment::removeDeletedEntity(Entity* entity) {
	unsigned environmentBasedId;
	unsigned short entityId, environmentId;
	std::map<unsigned, AbstractEntityDeletionCB*>::iterator it;

	environmentBasedId = entity->getEnvironmentBasedId();
	split(environmentBasedId, environmentId, entityId);

	assert(environmentId == this->environmentId);

	if (entityMap.find(entityId) != entityMap.end())
		entityMap.erase(entityMap.find(entityId));

	it = entityDeletionCallback.find(environmentBasedId);
	if (it != entityDeletionCallback.end()) {
		(it->second)->call(entity);
		delete (it->second);
		entityDeletionCallback.erase(it);
	} // if

	if (localEnvironmentIdPool)
		localEnvironmentIdPool->freeEntry(entityId);
} // removeDeletedEntity

unsigned short Environment::getFreeEntityId() {
	if (!localEnvironmentIdPool) {
		printd(INFO, "Environment::getFreeEntityId(): allocating local pool\n");
		localEnvironmentIdPool = globalEnvironmentIdPool.allocSubPool(4096);
		assert(localEnvironmentIdPool);
	}
	return (unsigned short)localEnvironmentIdPool->allocEntry();
} // getFreeEntityId
