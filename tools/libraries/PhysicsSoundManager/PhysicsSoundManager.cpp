/*
 * PhysicsSoundManager.cpp
 *
 *  Created on: 27.02.2009
 *      Author: roland
 */

#include "PhysicsSoundManager.h"

#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/Modules/3DPhysics/Physics.h>
#include "oops/Simulation.h"
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/Modules/3DPhysics/PhysicsEntity.h>
#include "Sound/Sound.h"
#include <inVRs/SystemCore/UtilityFunctions.h>

#include <assert.h>
#include <limits>

JointSoundData::JointSoundData() {
	sound = NULL;
	soundRunning = false;
	gainFactor = 1.0f;
	maxGain = std::numeric_limits<float>::max();
	pitchFactor = 0;
	maxPitch = std::numeric_limits<float>::max();
} // JointSoundData

JointSoundData::JointSoundData(Sound* sound, float gainFactor, float maxGain, float pitchFactor,
		float maxPitch) {
	this->sound = sound;
	this->soundRunning = false;
	this->gainFactor = gainFactor;
	this->maxGain = maxGain;
	this->pitchFactor = pitchFactor;
	this->maxPitch = maxPitch;
} // JointSoundData

JointSoundData::~JointSoundData() {
	if (sound != NULL)
		delete sound;
	sound = NULL;
} // ~JointSoundData

JointSoundDefinition::JointSoundDefinition() {
	lastMovementPosition = 0;
	movementData = NULL;
	lowerLimitEndData = NULL;
	upperLimitEndData = NULL;
	lowerLimitStartData = NULL;
	upperLimitStartData = NULL;
} // JointSoundDefinition

JointSoundDefinition::~JointSoundDefinition() {
	if (movementData != NULL) {
		delete movementData;
		movementData = NULL;
	} // if
	if (lowerLimitEndData != NULL) {
		delete lowerLimitEndData;
		lowerLimitEndData = NULL;
	} // if
	if (upperLimitEndData != NULL) {
		delete upperLimitEndData;
		upperLimitEndData = NULL;
	} // if
	if (lowerLimitStartData != NULL) {
		delete lowerLimitStartData;
		lowerLimitStartData = NULL;
	} // if
	if (upperLimitStartData != NULL) {
		delete upperLimitStartData;
		upperLimitStartData = NULL;
	} // if
} // ~JointSoundDefinition


PhysicsSoundManager::PhysicsSoundManager() {
	Physics* physics = (Physics*)SystemCore::getModuleByName("Physics");
	physics->addSimulationStepListener(this);
}

PhysicsSoundManager::~PhysicsSoundManager() {
	JointSoundDefinition* soundDefinition;
	std::map<oops::HingeJoint*, JointSoundDefinition*>::iterator hingeIt;
	std::map<oops::SliderJoint*, JointSoundDefinition*>::iterator sliderIt;

	for (hingeIt = hingeJointSounds.begin(); hingeIt != hingeJointSounds.end(); ++hingeIt) {
		soundDefinition = hingeIt->second;
		delete soundDefinition;
	} // for
	hingeJointSounds.clear();

	for (sliderIt = sliderJointSounds.begin(); sliderIt != sliderJointSounds.end(); ++sliderIt) {
		soundDefinition = sliderIt->second;
		delete soundDefinition;
	} // for
	sliderJointSounds.clear();
} // ~PhysicsSoundManager

bool PhysicsSoundManager::loadConfig(std::string configFile) {
	IrrXMLReader* xml;
	JointSoundDefinition* jointSoundDef = NULL;
	std::string entityTypeName;
	std::string jointName;
	std::string movementSoundFile = "";
	std::string lowerLimitEndSoundFile = "";
	std::string lowerLimitStartSoundFile = "";
	std::string upperLimitEndSoundFile = "";
	std::string upperLimitStartSoundFile = "";
	std::vector<oops::Joint*> jointList;
	std::vector<oops::Joint*>::iterator it;
	oops::Joint* joint;
	oops::SliderJoint* sliderJoint;
	oops::HingeJoint* hingeJoint;
	Sound* sound;
	float movementGainFactor = 1.0;
	float movementMaxGain = std::numeric_limits<float>::max();
	float movementPitchFactor = 0;
	float movementMaxPitch = std::numeric_limits<float>::max();
	float lowerLimitEndGainFactor = 1.0;
	float lowerLimitEndMaxGain = std::numeric_limits<float>::max();
	float lowerLimitEndPitchFactor = 0;
	float lowerLimitEndMaxPitch = std::numeric_limits<float>::max();
	float upperLimitEndGainFactor = 1.0;
	float upperLimitEndMaxGain = std::numeric_limits<float>::max();
	float upperLimitEndPitchFactor = 0;
	float upperLimitEndMaxPitch = std::numeric_limits<float>::max();
	float lowerLimitStartGainFactor = 1.0;
	float lowerLimitStartMaxGain = std::numeric_limits<float>::max();
	float lowerLimitStartPitchFactor = 0;
	float lowerLimitStartMaxPitch = std::numeric_limits<float>::max();
	float upperLimitStartGainFactor = 1.0;
	float upperLimitStartMaxGain = std::numeric_limits<float>::max();
	float upperLimitStartPitchFactor = 0;
	float upperLimitStartMaxPitch = std::numeric_limits<float>::max();

	printd(INFO, "PhysicsSoundManager::loadConfig(): loading PhysicsSoundManager config!\n");

	configFile = Configuration::getPath("PhysicsSoundConfiguration") + configFile;

	if(!fileExists(configFile))
	{
		printd(ERROR, "PhysicsSoundManager::loadConfig(): cannot read from file %s\n", configFile.c_str());
		return false;
	} // if

	xml = createIrrXMLReader(configFile.c_str());

	if (!xml)
	{
		printd(ERROR, "PhysicsSoundManager::loadConfig(): ERROR: could not find config-file %s\n", configFile.c_str());
		return false;
	} // if

	printd(INFO, "PhysicsSoundManager::loadConfig(): loading file %s\n", configFile.c_str());

	while(xml && xml->read())
	{
		switch (xml -> getNodeType())
		{
			case EXN_TEXT:
				break;
			case EXN_ELEMENT:
				if (!strcmp("jointSound", xml->getNodeName())) {
				} // if
				else if (!strcmp("joint", xml->getNodeName())) {
					entityTypeName = xml->getAttributeValue("entityTypeName");
					jointName = xml->getAttributeValue("name");
					jointList = getJoints(entityTypeName, jointName);
				} // else if
				else if (!strcmp("movementSound", xml->getNodeName())) {
					movementSoundFile = xml->getAttributeValue("file");
					if (xml->getAttributeValue("gainFactor") != NULL)
						movementGainFactor = xml->getAttributeValueAsFloat("gainFactor");
					if (xml->getAttributeValue("maxGain") != NULL)
						movementMaxGain = xml->getAttributeValueAsFloat("maxGain");
					if (xml->getAttributeValue("pitchFactor") != NULL)
						movementPitchFactor = xml->getAttributeValueAsFloat("pitchFactor");
					if (xml->getAttributeValue("maxPitch") != NULL)
						movementMaxPitch = xml->getAttributeValueAsFloat("maxPitch");
				} // else if
				else if (!strcmp("lowerLimitEndSound", xml->getNodeName())) {
					lowerLimitEndSoundFile = xml->getAttributeValue("file");
					if (xml->getAttributeValue("gainFactor") != NULL)
						lowerLimitEndGainFactor = xml->getAttributeValueAsFloat("gainFactor");
					if (xml->getAttributeValue("maxGain") != NULL)
						lowerLimitEndMaxGain = xml->getAttributeValueAsFloat("maxGain");
					if (xml->getAttributeValue("pitchFactor") != NULL)
						lowerLimitEndPitchFactor = xml->getAttributeValueAsFloat("pitchFactor");
					if (xml->getAttributeValue("maxPitch") != NULL)
						lowerLimitEndMaxPitch = xml->getAttributeValueAsFloat("maxPitch");
				} // else if
				else if (!strcmp("lowerLimitStartSound", xml->getNodeName())) {
					lowerLimitStartSoundFile = xml->getAttributeValue("file");
					if (xml->getAttributeValue("gainFactor") != NULL)
						lowerLimitStartGainFactor = xml->getAttributeValueAsFloat("gainFactor");
					if (xml->getAttributeValue("maxGain") != NULL)
						lowerLimitStartMaxGain = xml->getAttributeValueAsFloat("maxGain");
					if (xml->getAttributeValue("pitchFactor") != NULL)
						lowerLimitStartPitchFactor = xml->getAttributeValueAsFloat("pitchFactor");
					if (xml->getAttributeValue("maxPitch") != NULL)
						lowerLimitStartMaxPitch = xml->getAttributeValueAsFloat("maxPitch");
				} // else if
				else if (!strcmp("upperLimitEndSound", xml->getNodeName())) {
					upperLimitEndSoundFile = xml->getAttributeValue("file");
					if (xml->getAttributeValue("gainFactor") != NULL)
						upperLimitEndGainFactor = xml->getAttributeValueAsFloat("gainFactor");
					if (xml->getAttributeValue("maxGain") != NULL)
						upperLimitEndMaxGain = xml->getAttributeValueAsFloat("maxGain");
					if (xml->getAttributeValue("pitchFactor") != NULL)
						upperLimitEndPitchFactor = xml->getAttributeValueAsFloat("pitchFactor");
					if (xml->getAttributeValue("maxPitch") != NULL)
						upperLimitEndMaxPitch = xml->getAttributeValueAsFloat("maxPitch");
				} // else if
				else if (!strcmp("upperLimitStartSound", xml->getNodeName())) {
					upperLimitStartSoundFile = xml->getAttributeValue("file");
					if (xml->getAttributeValue("gainFactor") != NULL)
						upperLimitStartGainFactor = xml->getAttributeValueAsFloat("gainFactor");
					if (xml->getAttributeValue("maxGain") != NULL)
						upperLimitStartMaxGain = xml->getAttributeValueAsFloat("maxGain");
					if (xml->getAttributeValue("pitchFactor") != NULL)
						upperLimitStartPitchFactor = xml->getAttributeValueAsFloat("pitchFactor");
					if (xml->getAttributeValue("maxPitch") != NULL)
						upperLimitStartMaxPitch = xml->getAttributeValueAsFloat("maxPitch");
				} // else if
				else {
//					printd(WARNING,
//						"PhysicsSoundManager::loadConfig(): unsupported element %s found!\n",
//						xml->getNodeName());
				}
				break;
			case EXN_ELEMENT_END:
				if (!strcmp("jointSound", xml->getNodeName())) {
					jointList = getJoints(entityTypeName, jointName);
					for (it = jointList.begin(); it != jointList.end(); ++it) {
						joint = *it;
						jointSoundDef = new JointSoundDefinition();
						if (movementSoundFile.length() > 0) {
							sound = new Sound();
							movementSoundFile = getFullPath(movementSoundFile, "Sounds");
							if (sound->init(movementSoundFile)) {
								jointSoundDef->movementData = new JointSoundData(sound,
										movementGainFactor, movementMaxGain, movementPitchFactor,
										movementMaxPitch);
							} // if
						} // if
						if (lowerLimitEndSoundFile.length() > 0) {
							sound = new Sound();
							lowerLimitEndSoundFile = getFullPath(lowerLimitEndSoundFile, "Sounds");
							if (sound->init(lowerLimitEndSoundFile)) {
								jointSoundDef->lowerLimitEndData = new JointSoundData(sound,
										lowerLimitEndGainFactor, lowerLimitEndMaxGain,
										lowerLimitEndPitchFactor, lowerLimitEndMaxPitch);
							} // if
						} // if
						if (lowerLimitStartSoundFile.length() > 0) {
							sound = new Sound();
							lowerLimitStartSoundFile = getFullPath(lowerLimitStartSoundFile, "Sounds");
							if (sound->init(lowerLimitStartSoundFile)) {
								jointSoundDef->lowerLimitStartData = new JointSoundData(sound,
										lowerLimitStartGainFactor, lowerLimitStartMaxGain,
										lowerLimitStartPitchFactor, lowerLimitStartMaxPitch);
							} // if
						} // if
						if (upperLimitEndSoundFile.length() > 0) {
							sound = new Sound();
							upperLimitEndSoundFile = getFullPath(upperLimitEndSoundFile, "Sounds");
							if (sound->init(upperLimitEndSoundFile)) {
								jointSoundDef->upperLimitEndData = new JointSoundData(sound,
										upperLimitEndGainFactor, upperLimitEndMaxGain,
										upperLimitEndPitchFactor, upperLimitEndMaxPitch);
							} // if
						} // if
						if (upperLimitStartSoundFile.length() > 0) {
							sound = new Sound();
							upperLimitStartSoundFile = getFullPath(upperLimitStartSoundFile, "Sounds");
							if (sound->init(upperLimitStartSoundFile)) {
								jointSoundDef->upperLimitStartData = new JointSoundData(sound,
										upperLimitStartGainFactor, upperLimitStartMaxGain,
										upperLimitStartPitchFactor, upperLimitStartMaxPitch);
							} // if
						} // if
						sliderJoint = dynamic_cast<oops::SliderJoint*>(joint);
						hingeJoint = dynamic_cast<oops::HingeJoint*>(joint);
						if (sliderJoint)
							sliderJointSounds[sliderJoint] = jointSoundDef;
						else if (hingeJoint)
							hingeJointSounds[hingeJoint] = jointSoundDef;
						else {
							printd(ERROR, "PhysicsSoundManager::loadConfig(): unsupported joint type found!\n");
							delete jointSoundDef;
						} // else
					} // for
					movementGainFactor = 1.0;
					movementMaxGain = std::numeric_limits<float>::max();
					movementPitchFactor = 0;
					movementMaxPitch = std::numeric_limits<float>::max();
					lowerLimitEndGainFactor = 1.0;
					lowerLimitEndMaxGain = std::numeric_limits<float>::max();
					lowerLimitEndPitchFactor = 0;
					lowerLimitEndMaxPitch = std::numeric_limits<float>::max();
					upperLimitEndGainFactor = 1.0;
					upperLimitEndMaxGain = std::numeric_limits<float>::max();
					upperLimitEndPitchFactor = 0;
					upperLimitEndMaxPitch = std::numeric_limits<float>::max();
					lowerLimitStartGainFactor = 1.0;
					lowerLimitStartMaxGain = std::numeric_limits<float>::max();
					lowerLimitStartPitchFactor = 0;
					lowerLimitStartMaxPitch = std::numeric_limits<float>::max();
					upperLimitStartGainFactor = 1.0;
					upperLimitStartMaxGain = std::numeric_limits<float>::max();
					upperLimitStartPitchFactor = 0;
					upperLimitStartMaxPitch = std::numeric_limits<float>::max();

					movementSoundFile.clear();
					lowerLimitEndSoundFile.clear();
					lowerLimitStartSoundFile.clear();
					upperLimitEndSoundFile.clear();
					upperLimitStartSoundFile.clear();
					entityTypeName.clear();
					jointName.clear();
				} // if
				break;
			default:
				break;
		} // switch
	} // while
	delete xml;

	return true;
} // loadConfig

void PhysicsSoundManager::cleanup() {

} // cleanup

std::string PhysicsSoundManager::getName() {
	return "PhysicsSoundManager";
} // getName

void PhysicsSoundManager::step(float dt, unsigned simulationTime) {
	manageSliderJoints(dt);
	manageHingeJoints(dt);
} // step

void PhysicsSoundManager::manageSliderJoints(float dt) {

	std::map<oops::SliderJoint*, JointSoundDefinition*>::iterator it;
	oops::SliderJoint* sliderJoint;
	JointSoundDefinition* soundDefinition;
	JointSoundData* soundData;
	float soundGain;
	float soundPitch;

	for (it = sliderJointSounds.begin(); it != sliderJointSounds.end(); ++it) {
		sliderJoint = it->first;
		soundDefinition = it->second;

		float currentPosition = sliderJoint->getPosition();
		float diff = fabs(currentPosition - soundDefinition->lastMovementPosition) / dt;
//		float pitchFactor = 0.1;

		if (soundDefinition->movementData != NULL) {
			soundData = soundDefinition->movementData;
			if (diff == 0 && soundData->soundRunning) {
				soundData->sound->stop();
				soundData->soundRunning = false;
			} // if
			else if (diff > 0 && !soundData->soundRunning) {
				soundData->sound->play(true);
				soundData->soundRunning = true;
			} // else if
			if (diff > 0) {
				soundGain = diff * soundData->gainFactor;
				if (soundGain > soundData->maxGain)
					soundGain = soundData->maxGain;
				soundData->sound->setGain(soundGain);
				soundPitch = 1 + diff * soundData->pitchFactor;
				if (soundPitch > soundData->maxPitch)
					soundPitch = soundData->maxPitch;
				soundData->sound->setPitch(soundPitch);
			} // if
		} // if
		if (soundDefinition->lowerLimitEndData != NULL) {
			soundData = soundDefinition->lowerLimitEndData;
			if (diff > 0 && currentPosition < sliderJoint->getMinBoundary() &&
					!soundData->soundRunning) {
				soundData->sound->play(false);
				soundGain = diff * soundData->gainFactor;
				if (soundGain > soundData->maxGain)
					soundGain = soundData->maxGain;
				soundData->sound->setGain(soundGain);
				soundPitch = 1 + diff * soundData->pitchFactor;
				if (soundPitch > soundData->maxPitch)
					soundPitch = soundData->maxPitch;
				soundData->sound->setPitch(soundPitch);
				soundData->soundRunning = true;
			} // if
			else if (currentPosition >= sliderJoint->getMinBoundary())
				soundData->soundRunning = false;
		} // if
		if (soundDefinition->upperLimitEndData != NULL) {
			soundData = soundDefinition->upperLimitEndData;
			if (diff > 0 && currentPosition > sliderJoint->getMaxBoundary() &&
					!soundData->soundRunning) {
				soundData->sound->play(false);
				soundGain = diff * soundData->gainFactor;
				if (soundGain > soundData->maxGain)
					soundGain = soundData->maxGain;
				soundData->sound->setGain(soundGain);
				soundPitch = 1 + diff * soundData->pitchFactor;
				if (soundPitch > soundData->maxPitch)
					soundPitch = soundData->maxPitch;
				soundData->sound->setPitch(soundPitch);
				soundData->soundRunning = true;
			} // if
			else if (currentPosition <= sliderJoint->getMaxBoundary())
				soundData->soundRunning = false;
		} // if
		//TODO: lowerLimitStartData / upperLimitStartData

		soundDefinition->lastMovementPosition = currentPosition;
	} // for
} // manageSliderJoint

void PhysicsSoundManager::manageHingeJoints(float dt) {

	std::map<oops::HingeJoint*, JointSoundDefinition*>::iterator it;
	oops::HingeJoint* hingeJoint;
	JointSoundDefinition* soundDefinition;
	JointSoundData* soundData;
	float soundGain;
	float soundPitch;

	for (it = hingeJointSounds.begin(); it != hingeJointSounds.end(); ++it) {
		hingeJoint = it->first;
		soundDefinition = it->second;

		float currentPosition = hingeJoint->getAngle();
		float diff = fabs(currentPosition - soundDefinition->lastMovementPosition) / dt;

		if (soundDefinition->movementData != NULL) {
			soundData = soundDefinition->movementData;
			if (diff == 0 && soundData->soundRunning) {
				soundData->sound->stop();
				soundData->soundRunning = false;
			} // if
			else if (diff > 0 && !soundData->soundRunning) {
				soundData->sound->play(true);
				soundData->soundRunning = true;
			} // else if
			if (diff > 0) {
				soundGain = diff * soundData->gainFactor;
				if (soundGain > soundData->maxGain)
					soundGain = soundData->maxGain;
				soundData->sound->setGain(soundGain);
				soundPitch = 1 + diff * soundData->pitchFactor;
				if (soundPitch > soundData->maxPitch)
					soundPitch = soundData->maxPitch;
				soundData->sound->setPitch(soundPitch);
			} // if
		} // if
		if (soundDefinition->lowerLimitEndData != NULL) {
			soundData = soundDefinition->lowerLimitEndData;
			if (diff > 0 && currentPosition < hingeJoint->getMinAngle() && !soundData->soundRunning) {
				soundData->sound->play(false);
				soundGain = diff * soundData->gainFactor;
				if (soundGain > soundData->maxGain)
					soundGain = soundData->maxGain;
				soundData->sound->setGain(soundGain);
				soundPitch = 1 + diff * soundData->pitchFactor;
				if (soundPitch > soundData->maxPitch)
					soundPitch = soundData->maxPitch;
				soundData->sound->setPitch(soundPitch);
				soundData->soundRunning = true;
			} // if
			else if (currentPosition >= hingeJoint->getMinAngle())
				soundData->soundRunning = false;
		} // if
		if (soundDefinition->upperLimitEndData != NULL) {
			soundData = soundDefinition->upperLimitEndData;
			if (diff > 0 && currentPosition > hingeJoint->getMaxAngle() &&
					!soundData->soundRunning) {
				soundData->sound->play(false);
				soundGain = diff * soundData->gainFactor;
				if (soundGain > soundData->maxGain)
					soundGain = soundData->maxGain;
				soundData->sound->setGain(soundGain);
				soundPitch = 1 + diff * soundData->pitchFactor;
				if (soundPitch > soundData->maxPitch)
					soundPitch = soundData->maxPitch;
				soundData->sound->setPitch(soundPitch);
				soundData->soundRunning = true;
			} // if
			else if (currentPosition <= hingeJoint->getMaxAngle())
				soundData->soundRunning = false;
		} // if
		//TODO: lowerLimitStartData / upperLimitStartData

		soundDefinition->lastMovementPosition = currentPosition;
	} // for
} // manageHingeJoint

std::vector<oops::Joint*> PhysicsSoundManager::getJoints(std::string entityTypeName,
		std::string jointName) {

	std::vector<oops::Joint*> result;
	Entity* entity;
	PhysicsEntity* physicsEntity;
	std::vector<oops::Joint*> jointList;
	std::vector<oops::Joint*>::iterator jointListIt;
	oops::Joint* joint;

	EntityType* entityType = WorldDatabase::getEntityTypeWithName(entityTypeName);
	if (!entityType) {
		printd(ERROR, "PhysicsSoundManager::getJoints(): no entityType with name %s found!\n",
				entityTypeName.c_str());
		return result;
	} // if
	const std::vector<Entity*>& instanceList = entityType->getInstanceList();
	for (int i=0; i < (int)instanceList.size(); i++) {
		entity = instanceList.at(i);
		physicsEntity = dynamic_cast<PhysicsEntity*>(entity);
		if (!physicsEntity) {
			printd(ERROR, "PhysicsSoundManager::getJoints(): entityType %s is no PhysicsEntity!\n",
					entityTypeName.c_str());
		} // if
		else {
			jointList.clear();
			physicsEntity->getJoints(jointList);
			jointListIt = jointList.begin();
			while (jointListIt != jointList.end()) {
				joint = *jointListIt;
				if (joint->getName() == jointName) {
					result.push_back(joint);
				} // if
				++jointListIt;
			} // for
		} // else
	} // for
	return result;
} // getJoints

//TODO: register module in OutputInterface instead of SystemCore
MAKEMODULEPLUGIN(PhysicsSoundManager, SystemCore)
