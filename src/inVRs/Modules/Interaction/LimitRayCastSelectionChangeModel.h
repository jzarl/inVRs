#ifndef LIMITRAYCASTSELECTIONCHANGEMODEL_
#define LIMITRAYCASTSELECTIONCHANGEMODEL_

#include "RayCastSelectionChangeModel.h"
#include "SelectionChangeModel.h"

class INVRS_INTERACTION_API LimitRayCastSelectionChangeModel : public RayCastSelectionChangeModel
{
public:

	LimitRayCastSelectionChangeModel(float rayDistance);
	~LimitRayCastSelectionChangeModel();

	virtual std::string getName();

	void select(std::vector<PICKEDENTITY>* selectedEntities, std::vector<PICKEDENTITY>* entitiesToSelect, std::vector<PICKEDENTITY>* entitiesUnselected);
	void unselect(std::vector<PICKEDENTITY>* selectedEntities, std::vector<PICKEDENTITY>* entitiesToUnselect);
	void setMaxRayDistance(float rayDistance);

	float getRayDistance();
	gmtl::Vec3f getRayDirection();
	gmtl::Vec3f getRayDestination();

protected:

	Entity* getEntityCursorIsPointingAt();
	float maxRayDistance;

private:
	gmtl::Vec3f rayDirection;
	float rayDistance;
	gmtl::Vec3f rayDestination;
	unsigned short lastEnvironmentId;
};

class INVRS_INTERACTION_API LimitRayCastSelectionChangeModelFactory : public SelectionChangeModelFactory
{
public:

	SelectionChangeModel* create(std::string className, void* args = NULL);
};

#endif /*LIMITRAYCASTSELECTIONCHANGEMODEL_*/

