/*
 * PhysicsSoundManager.h
 *
 *  Created on: 27.02.2009
 *      Author: roland
 */

#ifndef PHYSICSSOUNDMANAGER_H_
#define PHYSICSSOUNDMANAGER_H_

#include <inVRs/SystemCore/ComponentInterfaces/ModuleInterface.h>
#include <inVRs/Modules/3DPhysics/SimulationStepListenerInterface.h>
#include "oops/Joints.h"

class Sound;

class JointSoundData {
public:
	JointSoundData();
	JointSoundData(Sound* sound, float gainFactor, float maxGain, float pitchFactor,
			float maxPitch);
	~JointSoundData();

	Sound* sound;
	bool soundRunning;
	float gainFactor;
	float maxGain;
	float pitchFactor;
	float maxPitch;
}; // JointSoundData

class JointSoundDefinition {
public:
	JointSoundDefinition();
	~JointSoundDefinition();

protected:
	friend class PhysicsSoundManager;

	float lastMovementPosition;
	JointSoundData* movementData;
	JointSoundData* lowerLimitEndData;
	JointSoundData* upperLimitEndData;
	JointSoundData* lowerLimitStartData;
	JointSoundData* upperLimitStartData;
};

class PhysicsSoundManager: public ModuleInterface, public SimulationStepListenerInterface {
public:
	PhysicsSoundManager();
	virtual ~PhysicsSoundManager();

	virtual bool loadConfig(std::string configFile);

	virtual void cleanup();

	virtual std::string getName();

//	virtual Event* createSyncEvent();

	virtual void step(float dt, unsigned simulationTime);

protected:

	std::map<oops::SliderJoint*, JointSoundDefinition*> sliderJointSounds;
	std::map<oops::HingeJoint*, JointSoundDefinition*> hingeJointSounds;

	void manageSliderJoints(float dt);
	void manageHingeJoints(float dt);

	std::vector<oops::Joint*> getJoints(std::string entityTypeName,	std::string jointName);

/*
	Sound* sound;
	oops::SliderJoint* sliderJoint;
	float lastPosition;
	bool soundStarted;

	oops::SliderJoint* getSliderJoint();
*/
};

#endif /* PHYSICSSOUNDMANAGER_H_ */
