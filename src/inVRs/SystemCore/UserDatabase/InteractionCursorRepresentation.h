/*
 * InteractionCursorRepresentation.h
 *
 *  Created on: Jul 27, 2009
 *      Author: rlander
 */

#ifndef INTERACTIONCURSORREPRESENTATION_H_
#define INTERACTIONCURSORREPRESENTATION_H_

#include "../ComponentInterfaces/CursorRepresentationInterface.h"
#include "../XmlConfigurationLoader.h"

/******************************************************************************
 *
 */
class InteractionCursorRepresentation : public CursorRepresentationInterface {
public:

	/**
	 *
	 */
	InteractionCursorRepresentation();

	/**
	 *
	 */
	virtual ~InteractionCursorRepresentation();

	/**
	 *
	 */
	virtual bool loadConfig(std::string configFile);

	/**
	 *
	 */
	virtual void setTransformation(const TransformationData& trans);

	/**
	 *
	 */
	virtual void update(float dt);

	/**
	 *
	 */
	virtual ModelInterface* getModel();

private:
	ModelInterface* idleModel;
	ModelInterface* selectionModel;
	ModelInterface* manipulationModel;
	ModelInterface* currentModel;
	TransformationData cursorTrans;

//*****************************************************************************
// Configuration loading
//*****************************************************************************
private:
	static XmlConfigurationLoader xmlConfigLoader;

	class ConverterToV1_0a4 : public XmlConfigurationConverter {
	public:
		ConverterToV1_0a4();
		virtual bool convert(XmlDocument* document, const Version& version, std::string configFile);
	}; // ConverterToV1_0a4

}; // InteractionCursorRepresentation


/******************************************************************************
 *
 */
class InteractionCursorRepresentationFactory : public CursorRepresentationFactory {

	/**
	 *
	 */
	CursorRepresentationInterface* create(std::string className, std::string configFile);
}; // InteractionCursorRepresentationFactory

#endif /* INTERACTIONCURSORREPRESENTATION_H_ */
