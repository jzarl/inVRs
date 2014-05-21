#include "LimitRayCastSelectionChangeModel.h"

#include <limits>
#include <gmtl/gmtl.h>
#include <gmtl/Intersection.h>

#include <inVRs/SystemCore/UserDatabase/User.h>
#include <inVRs/SystemCore/UserDatabase/UserDatabase.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>

LimitRayCastSelectionChangeModel::LimitRayCastSelectionChangeModel(float rayDistance) :
	RayCastSelectionChangeModel(true),
	lastEnvironmentId(0) {
	this->maxRayDistance = rayDistance;
}

LimitRayCastSelectionChangeModel::~LimitRayCastSelectionChangeModel() {

}

void LimitRayCastSelectionChangeModel::unselect(std::vector<PICKEDENTITY>* selectedEntities,
		std::vector<PICKEDENTITY>* entitiesToUnselect) {
	Entity* ent;
	PICKEDENTITY pickedEntity;

	assert(selectedEntities);
	assert(entitiesToUnselect);
	assert(selectedEntities->size()<=1);

	entitiesToUnselect->clear();
	ent = getEntityCursorIsPointingAt();

	// 	if(ent) fprintf(stderr, "RayCastSelectionChangeModel: pointing at %u\n", ent->getTypeBasedId());

	if (selectedEntities->size() == 1) {
		if ((*selectedEntities)[0].entity == ent) {
			// selected entity remains selected
			triggerUnselectionStateChange = false;
		} else if (ent == NULL) {
			// remove selected entity from selection and trigger changeState
			entitiesToUnselect->push_back((*selectedEntities)[0]);
			triggerUnselectionStateChange = true;
		} else {
			// unselect entity previousely selected (no statechange will occure, if the same model is choosen for selecting)
			entitiesToUnselect->push_back((*selectedEntities)[0]);
			triggerUnselectionStateChange = true;
		}
	} else {
		// empty selection, suggest state change
		triggerUnselectionStateChange = true;
	}
}

void LimitRayCastSelectionChangeModel::select(std::vector<PICKEDENTITY>* selectedEntities,
		std::vector<PICKEDENTITY>* entitiesToSelect, std::vector<PICKEDENTITY>* entitiesUnselected) {

	Entity* ent;
	PICKEDENTITY pickedEntity;

	assert(selectedEntities);
	assert(entitiesToSelect);
	assert(entitiesUnselected);
	assert(selectedEntities->size()<=1);

	entitiesToSelect->clear();

	ent = getEntityCursorIsPointingAt();

	// 	if(ent) fprintf(stderr, "RayCastSelectionChangeModel: pointing at %u\n", ent->getTypeBasedId());

	if (selectedEntities->size() == 0) {

		if (!ent) {
			// TODO: Not sure if this completly fixes #43, but this helps
			// leaving the state change variable in a defined state (false, as
			// we guess it should be) LANDI (PA)
			triggerSelecetionStateChange = false;
			return; // nothing selected, stay in idle state
		}
		// we are in idle state, switch to selected state
		if (usePickingOffset)
			pickedEntity.pickingOffset = pickingOffset;
		else
			pickedEntity.pickingOffset = identityTransformation();
		pickedEntity.entity = ent;
		entitiesToSelect->push_back(pickedEntity);
		triggerSelecetionStateChange = true;
		assert(entitiesUnselected->size() == 0); // unselect() should not write on selectedEntities
	} else {
		// already in selection state
		if ((*selectedEntities)[0].entity != ent) {
			assert(contains(entitiesUnselected, (*selectedEntities)[0].entity));

			if (ent != NULL) {
				if (usePickingOffset)
					pickedEntity.pickingOffset = pickingOffset;
				else
					pickedEntity.pickingOffset = identityTransformation();

				pickedEntity.entity = ent;
				entitiesToSelect->push_back(pickedEntity);
			}
		} else {
			// selected the same entity again
			if (usePickingOffset) {
				(*selectedEntities)[0].pickingOffset = pickingOffset;
			}
			removeEntityFromList(entitiesUnselected, ent);
		}
		triggerSelecetionStateChange = false;
	}

}

std::string LimitRayCastSelectionChangeModel::getName() {
	return "LimitRayCastSelectionChangeModel";
}

Entity* LimitRayCastSelectionChangeModel::getEntityCursorIsPointingAt() {
	Entity* result = NULL;
	User* localUser = UserDatabase::getLocalUser();
	TransformationData cursorTransf = localUser->getCursorTransformation();
	gmtl::Rayf ray = gmtl::Rayf();
	gmtl::Vec3f stdDir(0, 0, -1);
	gmtl::Vec3f rayDir = cursorTransf.orientation * stdDir;
	gmtl::Point3f origin(cursorTransf.position[0], cursorTransf.position[1],
			cursorTransf.position[2]);
	ray.setOrigin(origin);
	ray.setDir(rayDir);

	std::vector<Entity*>::iterator iter;
#ifdef max
#undef max
#endif
	float minDistance = std::numeric_limits<float>::max();
	Entity* ent;
	gmtl::AABoxf aabb;
	gmtl::Point3f minPnt, maxPnt;
	unsigned int numHits;
	float distIn, distOut;

	Environment* env = WorldDatabase::getEnvironmentAtWorldPosition(cursorTransf.position[0],
			cursorTransf.position[2]);

	if (!env) {
		env = WorldDatabase::getEnvironmentWithId(lastEnvironmentId);
	} // if

	if (!env) {
		return NULL;
	}
	lastEnvironmentId = env->getId();
	std::vector<Entity*> entities = env->getEntityList();

	for (iter = entities.begin(); iter != entities.end(); ++iter) {
		ent = *iter;
		if (ent && !ent->isFixed()) {
			AABB modelBox = ent->getVisualRepresentation()->getAABB();
			minPnt = modelBox.p0;
			maxPnt = modelBox.p1;
			aabb.setMin(minPnt);
			aabb.setMax(maxPnt);

			if (gmtl::intersect(ray, aabb, numHits, distIn, distOut) &&
					distIn < maxRayDistance &&
					minDistance > distIn) {
				TransformationData entTrans = ent->getWorldTransformation();
				TransformationData rayCursorTrans = identityTransformation();
				rayCursorTrans.position = cursorTransf.position + rayDir * distIn;
				rayCursorTrans.orientation = cursorTransf.orientation;
				rayDestination = rayCursorTrans.position;
				entTrans.scale = gmtl::Vec3f(1, 1, 1);
				invert(rayCursorTrans);
				multiply(pickingOffset, rayCursorTrans, entTrans);

				result = ent;
				minDistance = distIn;
			} // if
		}
//		else {
//			printd(ERROR, "LimitRayCastSelectionChangeModel: invalid entity %d\n", *iter);
//		}
	} // for

	return result;
}

void LimitRayCastSelectionChangeModel::setMaxRayDistance(float rayDistance) {
	this->maxRayDistance = rayDistance;
}

float LimitRayCastSelectionChangeModel::getRayDistance() {
	return rayDistance;
}
gmtl::Vec3f LimitRayCastSelectionChangeModel::getRayDirection() {
	return rayDirection;
}

gmtl::Vec3f LimitRayCastSelectionChangeModel::getRayDestination() {
	return rayDestination;
}

SelectionChangeModel* LimitRayCastSelectionChangeModelFactory::create(std::string className,
		void* args) {
	if (className == "LimitRayCastSelectionChangeModel") {
		float rayDistance;
		ArgumentVector* argVec = (ArgumentVector*)args;

		if (argVec && argVec->keyExists("rayDistanceThreshold")) {
			argVec->get("rayDistanceThreshold", rayDistance);
			return new LimitRayCastSelectionChangeModel(rayDistance);
		} // if

		return new LimitRayCastSelectionChangeModel(1.0f);
	}
	return NULL;
}
