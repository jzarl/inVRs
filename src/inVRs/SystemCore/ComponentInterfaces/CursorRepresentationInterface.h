/*
 * CursorRepresentationInterface.h
 *
 *  Created on: Jul 24, 2009
 *      Author: rlander
 */

#ifndef CURSORREPRESENTATIONINTERFACE_H_
#define CURSORREPRESENTATIONINTERFACE_H_

#include <string>

#include "../ClassFactory.h"
#include <inVRs/OutputInterface/ModelInterface.h>

/******************************************************************************
 *
 */
class CursorRepresentationInterface {
public:

	/**
	 *
	 */
	virtual bool loadConfig(std::string configFile) = 0;

	/**
	 *
	 */
	virtual void setTransformation(const TransformationData& trans) = 0;

	/**
	 *
	 */
	virtual void update(float dt) = 0;

	/**
	 *
	 */
	virtual ModelInterface* getModel() = 0;

	virtual ~CursorRepresentationInterface() { }
}; // CursorRepresentationInterface

typedef ClassFactory<CursorRepresentationInterface, std::string> CursorRepresentationFactory;

#endif /* CURSORREPRESENTATIONINTERFACE_H_ */
