/*
 * SimpleCursorRepresentation.h
 *
 *  Created on: Jul 27, 2009
 *      Author: rlander
 */

#ifndef SIMPLECURSORREPRESENTATION_H_
#define SIMPLECURSORREPRESENTATION_H_

#include "../ComponentInterfaces/CursorRepresentationInterface.h"
#include "../XmlConfigurationLoader.h"

/******************************************************************************
 *
 */
class SimpleCursorRepresentation : public CursorRepresentationInterface {
public:

	/**
	 *
	 */
	SimpleCursorRepresentation();

	/**
	 *
	 */
	virtual ~SimpleCursorRepresentation();

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
	ModelInterface* model;

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

}; // SimpleCursorRepresentation


/******************************************************************************
 *
 */
class SimpleCursorRepresentationFactory : public CursorRepresentationFactory {

	/**
	 *
	 */
	CursorRepresentationInterface* create(std::string className, std::string configFile);
}; // SimpleCursorRepresentationFactory

#endif /* SIMPLECURSORREPRESENTATION_H_ */
