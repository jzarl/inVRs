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

/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#ifndef _SIMPLEAVATAR_H
#define _SIMPLEAVATAR_H

#include "../ComponentInterfaces/AvatarInterface.h"
#include "../XmlConfigurationLoader.h"

/******************************************************************************
 * @class SimpleAvatar
 * @brief
 *
 * This class represents a simple avatar which consists of a simple OpenSG-model.
 * @author rlander
 */
class INVRS_SYSTEMCORE_API SimpleAvatar : public AvatarInterface {
public:
	SimpleAvatar();

	virtual ~SimpleAvatar();

	/**
	 * This method loads the configuration of the Avatar. Therefore it parses the
	 * file until it is finished.
	 * @param configFile url to the avatar config file
	 * @return true if no error occured while parsing the file
	 */
	virtual bool loadConfig(std::string configFile);

	/**
	 * This method shows or hides the Avatar. It does so by activating or
	 * deactivating the OpenSG-Node
	 * @param active defines if the avatar should be visible
	 */
	virtual void showAvatar(bool active);

	/**
	 * This method sets the Transformation of the avatar in world coordinates
	 * @param trans Transformation of the avatar in world coordinates
	 */
	virtual void setTransformation(TransformationData trans);

	/**
	 * This method returns the avatar-model
	 * @return model of the avatar
	 */
	virtual ModelInterface* getModel();

	/**
	 * The method returns if the avatar is currently visible
	 * @return bool if avatar is visible
	 */
	virtual bool isVisible();

protected:
	/**
	 * This method loads the avatar model. It therefore opens the passed modelFile
	 * in the Avatars-path which should be set in the Configuration. It also
	 * applies the configured model Transformation to the model.
	 * @param modelFile model which should be used as Avatar
	 */
	bool loadModel(std::string fileType, std::string fileName);

	TransformationData modelTransform;
	TransformationData avatarTransformation;
	ModelInterface* avatarModel;
	SceneGraphInterface* sgIF;

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

};

/******************************************************************************
 * @class SimpleAvatarFactory
 * @brief This class is used to create a SimpleAvatar object
 */
class INVRS_SYSTEMCORE_API SimpleAvatarFactory : public AvatarFactory {
public:
	/**
	 * This method create a SimpleAvatar object. It builds the new object and
	 * passes the XML-reader which has to be passed as argument to the
	 * loadConfig-method of the SimpleAvatar-object
	 * @param configFile url to the avatar config file
	 * @return new SimpleAvatar object
	 */
	virtual AvatarInterface* create(std::string className, std::string configFile);
}; // SimpleAvatarFactory

#endif // _SIMPLEAVATAR_H
