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

#ifndef _USER_H
#define _USER_H

#include <string>
#include <vector>

#include "../ComponentInterfaces/ModuleInterface.h"
#include "../DataTypes.h"
#include "../ComponentInterfaces/AvatarInterface.h"
#include "../WorldDatabase/CameraTransformation.h"
#include "CursorTransformationModel.h"
#include "UserTransformationModel.h"
#include "../ComponentInterfaces/CursorRepresentationInterface.h"
#include "../ComponentInterfaces/NetworkInterface.h"
#include "../XmlConfigurationLoader.h"

class EntityTransform;
class ModuleInterface;
class Entity;
//class UserTransformationModel;
//class UserTransformationModelFactory;

static const int MAX_NUM_SENSORS = 256;

/******************************************************************************
 * @class UserSetupData
 * @brief This class stores information about the User configuration
 *
 * The class is used to store the configuration information of a User object.
 * This information is sent to other Users when the user connects to the system
 * in order to add this User to the UserDatabase.
 */
class INVRS_SYSTEMCORE_API UserSetupData {
public:
	/**
	 * The constructor initializes the name, id and avatarFile members and sets
	 * the initialCursorModel-string to "VirtualHandCursorModel"
	 */
	UserSetupData();

	std::string name;
	std::string avatarFile;
	std::string cursorRepresentationFile;
	std::string initialCursorModel;
	ArgumentVector* cursorModelArguments;
	std::string initialUserTransformationModel;
	ArgumentVector* userTransformationModelArguments;
	unsigned id;
	NetworkIdentification networkId;
//	float physicalToWorldScale;
}; // UserSetupData

/******************************************************************************
 * @class AssociatedEntity
 * @brief The class sores an offset from the curso to the picking point
 *
 * This class is used to store the offset transformation of the users' cursor
 * transformation to the picking point of a carried entity.
 */
class INVRS_SYSTEMCORE_API AssociatedEntity {
public:
	TransformationData pickingOffset;
	Entity* entity;
}; // AssociatedEntity

/******************************************************************************
 * @class User
 * @brief This class is used to store all user-specific information in the VE.
 *
 * The User class stores transformations of the user, the user's cursor, hand
 * and head (in world coordinates). These transformations are also made
 * available in user coordinates (relative to the user's transformation).
 *
 * The class holds a pointer to the Avatar representing the User, and to the
 * CameraTransformation used for rendering. If the User carries objects, it also
 * contains a list of AssociatedEntity elements.
 *
 * <h4>Some notes on the transformations</h4>
 * <table>
 * <tr><td>navigatedTransformation</td>
 * <td> This transformation is the transformation of the
 *		User in the virtual world that arises from the navigation.
 *		It is used to obtain all world transformation of the user,
 *		for example the world transformation of the user's hand or head
 * </td></tr>
 * <tr><td>trackedUserTransformation</td>
 * <td> This transformation is the user's transformation relative to the
 *		center point of the tracking system. It is calculated by the
 *		UserTransformationModel, even if it is not measured by the
 *		tracking system.
 * </td></tr>
 * <tr><td>trackedHeadTransformation</td>
 * <td> This transformation is the data from the head sensor of
 *		the tracking system (index 0) scaled with the physicalToWorld
 *		factor.
 * </td></tr>
 * <tr><td>trackedHandTransformation</td>
 * <td> This transformation is the data from the wand sensor of
 *		the tracking system (index 1) scaled with the physicalToWorld
 *		factor.
 * </td></tr>
 * <tr><td>trackedSensorTransformation</td>
 * <td> This transformation is the data from any sensor of the tracking
 *		system scaled with the physicalToWorld factor.
 * </td></tr>
 * <tr><td>worldUserTransformation</td>
 * <td> This transformation is the tracked user transformation multiplied
 * 		to the navigatedTransformation (resulting in the transformation of
 * 		the user in the virtual world). The tracked user transformation is
 * 		therefore calculated by the UserTransformationModel (even if it is
 * 		not directly measured by the tracking system).
 * 		This transformation can be used for the avatar for example.
 * </td></tr>
 * <tr><td>worldHeadTransformation</td>
 * <td> This transformation is the tracked head transformation multiplied
 *		to the navigatedTransformation (resulting in the transformation of
 *		the user's head in the virtual world).
 * </td></tr>
 * <tr><td>worldHandTransformation</td>
 * <td> This transformation is the tracked hand transformation multiplied
 * 		to the navigatedTransformation (resulting in the transformation of
 *		the user's hand in the virtual world).
 * </td></tr>
 * <tr><td>worldSensorTransformation</td>
 * <td> This is the tracked transformation of any sensor from the tracking
 * 		system multiplied to the navigatedTransformation (resulting in the
 * 		transformation of the sensor in the virtual world).
 * </td></tr>
 * <tr><td>cursorTransformation</td>
 * <td> This transformation is calculated by the CursorTransformationModel
 * 				 and is the transformation of the User's cursor. For VirtualHand
 * 				 this transformation is equal to the worldHand transformation.
 * </table>
 *
 * <h4>Used coordinate spaces</h4>
 * <table>
 * <tr><td>world coordinates</td>	<td>space of a virtual environment</td></tr>
 * <tr><td>tracked coordinates</td>	<td>space measured in real environment (scaled with physicalToWorldScale)</td></tr>
 * <tr><td>user coordinates</td>	<td>same scale and orientation as world coordinates, but space has its origin at the user position</td></tr> * </table>
 *
 * @see UserDatabase
 */
class INVRS_SYSTEMCORE_API User {
public:
	/**
	 * The constructor creates a new UserSetupData, initializes the Id of the user
	 * with a random number and calls the commonInit-method
	 */
	User();

	/**
	 * The constructor calls the commonInit-method
	 * @param setupData UserSetupData to initializes the class with
	 */
	User(UserSetupData* setupData);

	/**
	 * The destructor deletes the CameraTransformation-object, calls the
	 * removeAvatar-method from the WorldDatabase, deletes the avatar and deletes
	 * all associatedEntities-entries in the associatedEntities-list.
	 */
	~User();

	/**
	 * The method loads the configuration of the User object from the passed
	 * config file. It parses the URL of the config-files for each module and
	 * calls the loadConfig-method of these modules to initialize them. It also
	 * parses the URL of the avatar config file and calls the
	 * WorldDatabase::loadAvatar-method and reads the name of the used
	 * CursorTransformationModel and calls the
	 * setCursorTransformationModel-method.
	 * @param configFile URL of the User-config file
	 * @return true if the config-file parsing was successfull, false otherwise
	 */
	bool loadConfig(std::string userCfgFilePath);

	/**
	 * The method deletes all registered CursorTransformationModelFactories
	 * for static data
	 */
	static void cleanup();

	/**
	 * The method registers a CursorTransformationModelFactory to create the
	 * CursorTransformationModel configured in the User config-file
	 * @param factory CursorTransformationModelFactory for the desired
	 * CursorTransformationModel
	 */
	static void registerCursorTransformationModelFactory(CursorTransformationModelFactory* factory);

	/**
	 * The method registers a UserTransformationModelFactory to create the
	 * UserTransformationModel configured in the User config-file.
	 * @param factory UserTransformationModelFactory for the desired
	 *        UserTransformationModel
	 */
	static void registerUserTransformationModelFactory(UserTransformationModelFactory* factory);

	/**
	 * The method adds the passed Entity with the passed picking-offset to the
	 * list of associated Entities.
	 * @param entity Entity that should be associated to the User
	 * @param offset offset of the picking point to the center of the Entity
	 */
	void pickUpEntity(Entity* entity, TransformationData offset);

	/**
	 * The method removes the Entity with the passed entityId
	 * from the list of associated Entities. The passed Id has to be
	 * the typeBasedId of the Entity!!!
	 * @param entityId typeBasedId of the Entity which should be released
	 * @return true if the Entity with the Id was found in the
	 * associatedEntities-list, false otherwise
	 */
	bool dropEntity(unsigned int entityId);

	/**
	 * The method creates the CursorTransformationModel with the passed name. It
	 * therefore iterates over every registered CursorTransformationModelFactory
	 * and calls their create-method which returns NULL if it cannot create the
	 * Model or the new Model if the factory matches to the Model. If there is
	 * already a CursorTransformationModel created then this one will be deleted
	 * and replaced by the new one. If the new Model could not be created an
	 * Error-message is printed.
	 * @param modelName name of the desired CursorTransformationModel
	 */
	void setCursorTransformationModel(std::string modelName, ArgumentVector* arguments);

	/**
	 * The method creates the UserTransformationModel with the passed name. It
	 * therefore iterates over every registered UserTransformationModelFactory
	 * and calls their create-method which returns NULL if it cannot create the
	 * Model or the new Model if the factory matches to the Model. If there is
	 * already a UserTransformationModel created then this one will be deleted
	 * and replaced by the new one. If the new Model could not be created an
	 * Error-message is printed.
	 * @param modelName name of the desired UserTransformationModel
	 */
	void setUserTransformationModel(std::string modelName, ArgumentVector* arguments);

	/**
	 * The method sets the transformation of the User's head relative to the
	 * navigatedTransformation of the User. If you have a tracking system then this
	 * transformation is equal to the value of the head-sensor.
	 * @param head transformation of the User's head relative to the
	 * navigatedTransformation of the User
	 */
	void setHeadTransformation(TransformationData head);

	/**
	 * The method sets the transformation of the User's hand relative to the
	 * navigatedTransformation of the User. If an avatar is registered then the method
	 * also calls the setHandTransformation-method of the Avatar with the
	 * return-value of the getUserHandTransformation-method as parameter.
	 * @param hand transformation of the User's hand relative to the User's position
	 */
	void setHandTransformation(TransformationData hand);

	/**
	 * The method sets the transformation of the user's sensor with the passed
	 * index. By default index 0 corresponds to the Head-sensor and index 1 to
	 * the Hand-sensor.
	 * @param sensorIndex index of the passed sensor
	 * @param transformation transformation value of the sensor
	 */
	void setSensorTransformation(unsigned short sensorIndex, TransformationData transformation);

	/**
	 * The method sets the navigated transformation of the User in world
	 * coordinates. This transformation can be used to setthe camera if a tracking
	 * system is used.
	 * @param navigatedTrans navigated transformation of the user
	 */
	void setNavigatedTransformation(TransformationData navigated); // transformation from navigated space to world space

	/**
	 * The method sets the transformation of the User's cursor in
	 * world-coordinates.
	 * @param trans transformation of the User's cursor in world coordinates
	 */
	void setCursorTransformation(TransformationData &trans);

//	/**
//	 * Sets the scale factor from physical units to world units
//	 */
//	void setPhysicalToWorldScale(float scale);

	/**
	 * The method sets if the User is currently moving or not
	 * @param moving true if the User is currently moving, false otherwise
	 */
	void setMoving(bool moving);

	void setNetworkId(NetworkIdentification* netId);

	/**
	 * The method returns the Id of the User
	 * @return Id of the User
	 */
	unsigned getId();

	/**
	 * The method returns the name of the User
	 * @return name of the User
	 */
	std::string getName();

	/**
	 * The method returns the URL of the configuration-file of the Avatar
	 * @return URL of the configuration-file of the Avatar
	 */
	std::string getAvatarConfigFile();

	/**
	 * The method returns the used CursorTransformationModel
	 * @return the used CursorTransformationModel
	 */
	CursorTransformationModel* getCursorTransformationModel();

	/**
	 * The method returns the arguments which are used for the used
	 * CursorTransformationModel
	 * @return a list of model-specific arguments
	 */
	ArgumentVector* getCursorTransformationModelArguments();

	/**
	 * The method returns the used UserTransformationModel
	 * @return the used UserTransformationModel
	 */
	UserTransformationModel* getUserTransformationModel();

	/**
	 * The method returns the arguments which are used for the used
	 * UserTransformationModel
	 * @return a list of model-specific arguments
	 */
	ArgumentVector* getUserTransformationModelArguments();

//	/**
//	 * Returns the scale factor from physical units to world units
//	 */
//	float getPhysicalToWorldScale();

	/**
	 * The method returns the navigated transformation of the User in world
	 * coordinates.
	 * @return navigated transformation of the User in world coordinates
	 */
	TransformationData getNavigatedTransformation();

	/**
	 * The method returns the transformation of the User's cursor in world
	 * coordinates. The transformation is calculated from the
	 * CursorTransformationModel.
	 * @return transformation of the User's cursor in world coordinates
	 */
	TransformationData getCursorTransformation();

	/**
	 * The method returns the transformation of the User's head in world
	 * coordinates. (navigated * tracked head transformation)
	 * @return transformation of the User's head in world coordinates
	 */
	TransformationData getWorldHeadTransformation();

	/**
	 * The method returns the transformation of the User's hand in world
	 * coordinates. (navigated * tracked hand transformation)
	 * @return transformation of the User's hand in world coordinates
	 */
	TransformationData getWorldHandTransformation();

	/**
	 * The method returns the transformation of the user's sensor with
	 * the passed index in world coordinates. (navigated * tracked sensor
	 * transformation)
	 * @param index index of the requested sensor
	 * @return transformation of the sensor in world coordinates
	 */
	TransformationData getWorldSensorTransformation(unsigned short index);

	/**
	 * The method returns the transformation of the User in world coordinates
	 * (navigated * tracked user transformation). The needed tracked user
	 * transformation is therefore calculated by the UserTransformationModel,
	 * even if no tracking sensor is measuring the user's transformation.
	 * This transformation is usually used to display the user's avatar.
	 * @return the User's transformation in world coordinates
	 */
	TransformationData getWorldUserTransformation();

	/**
	 * The method returns the hand transformation relative to the
	 * navigatedTransformation. If you use a tracking system this transformation
	 * is equal to the data of your wand sensor converted into world coordinates
	 * (by multiplication with physicalToWorldScale).
	 * @return hand transformation of the User relative to the navigatedTransformation
	 */
	TransformationData getTrackedHandTransformation();

	/**
	 * The method returns the head transformation relative to the
	 * navigatedTransformation. If you use a tracking system this transformation is
	 * equal to the data of your head sensor converted into world coordinates
	 * (by multiplication with physicalToWorldScale).
	 * @return head transformation of the User relative to the navigatedTransformation
	 */
	TransformationData getTrackedHeadTransformation();

	/**
	 * The method returns the transformation of the sensor with the passed index
	 * relative to the center point of your tracking system in world coordinates
	 * (= scaled by physicalToWorldScale).
	 * @param index index of the requested sensor
	 * @return transformation of the sensor relative to the tracking system center
	 */
	TransformationData getTrackedSensorTransformation(unsigned short index);

	/**
	 * The method returns the transformation of the user relative to the center
	 * point of your tracking system in world coordinates (= scaled by
	 * physicalToWorldScale). This value is calculated by the
	 * UserTransformationModel.
	 * @return transformation of the user relative to the tracking system center
	 */
	TransformationData getTrackedUserTransformation();

	/**
	 * The method returns the transformation of the user's head relative to the
	 * user's transformation. The user's transformation is therefore calculated
	 * by the UserTransformationModel.
	 * @return head transformation relative to the user transformation
	 */
	TransformationData getUserHeadTransformation();

	/**
	 * The method returns the transformation of the user's hand relative to the
	 * user's transformation. The user's transformation is therefore calculated
	 * by the UserTransformationModel.
	 * @return hand transformation relative to the user transformation
	 */
	TransformationData getUserHandTransformation();

	/**
	 * The method returns the transformation of the sensor with the passed
	 * index relative to the user's transformation. The user's transformation
	 * is therefore calculated by the UserTransformationModel.
	 * @param index index of the requested sensor
	 * @return sensor transformation relative to the user transformation
	 */
	TransformationData getUserSensorTransformation(unsigned short index);

	/**
	 * The method returns a pointer to the User's avatar
	 * @return pointer to the User's avatar
	 */
	AvatarInterface* getAvatar();

	/**
	 * The method returns the used CursorRepresentation
	 * @return CursorRepresentation which is used
	 */
	CursorRepresentationInterface* getCursor();

	/**
	 * The method returns the CameraTransformation-object which stores the
	 * transformation of the User's camera
	 * @return CameraTransformation of the User's camera
	 */
	CameraTransformation* getCamera();

	/**
	 * The method returns the ammount of Entities currently associated with the
	 * user.
	 * @return the amount of Entities
	 */
	unsigned getNumberOfAssociatedEntities();

	/**
	 * The method returns the Entity with the passed entityId if it is found in
	 * the associatedEntity-list. The passed Id has to be equal to the typeBasedId
	 * of the Entity.
	 * @param entityId typeBasedId of the searched Entity
	 * @return the Entity with the passed Id if found in the associatedEntities-list, NULL otherwise
	 */
	Entity* getAssociatedEntity(unsigned int entityId);

	/**
	 * The method returns the Entity at a certain position in the associatedEntity
	 * -list.
	 * @param index the position of the Entity to be returned
	 * @return the Entity with the passed Id if found in the associatedEntities-list, NULL otherwise
	 */
	Entity* getAssociatedEntityByIndex(unsigned index);

	/**
	 * The method returns the picking-offset of the Entity with the passed Id if
	 * it is in the associatedEntities-list. The passed Id has to be the
	 * typeBasedId of the Entity
	 * @param entityId typeBasedId of the Entity
	 * @return picking offset of the picked Entity if found, identityTransformation if not found
	 */
	TransformationData getAssociatedEntityOffset(unsigned int entityId);

  /**
	 * The method returns the NetworkIdentification of the user
	 * @return The netID
	 */
	NetworkIdentification getNetworkId();

	/**
	 * The method returns wether the User is moving or not
	 * @return true if the user if moving currently, false otherwise
	 */
	bool isMoving();

protected:
	friend class UserDatabase;

	/**
	 * The method initializes the class with the passed data. If the
	 * avatar-configuration file is set in the setupData then it calls the
	 * loadAvatar-method from the WorldDatabase. It initializes all
	 * transformations, calls the setCursorTransformationModel-method to set the
	 * CursorTransformationModel and creates a new CursorRepresentation-object.
	 */
	void commonInit(UserSetupData* setupData);

	/**
	 * The method updates the user-transformation which is the transformation
	 * of the user relative to the center point of the tracking system.
	 */
	void updateUserTransformation();

	/**
	 *
	 */
	void attachCursorRepresentation();

	/**
	 *
	 */
	void detachCursorRepresentation();

	/**
	 *
	 */
	void updateCursor(float dt);

	std::string name;
	std::string avatarConfigFile;
	std::string cursorRepresentationConfigFile;
	bool moving;
	unsigned id;
	NetworkIdentification networkId;
	std::vector<AssociatedEntity*> associatedEntities;
	CameraTransformation* camera;
	AvatarInterface* avatar;
	CursorRepresentationInterface* cursor;

	TransformationData sensorTransformation[MAX_NUM_SENSORS];
	TransformationData userTransformation;
	TransformationData navigatedTransformation;
	TransformationData cursorTransformation;

	CursorTransformationModel* cursorTransformationModel;
	ArgumentVector* cursorTransformationModelArguments;
	UserTransformationModel* userTransformationModel;
	ArgumentVector* userTransformationModelArguments;

	static std::vector<CursorTransformationModelFactory*> cursorTransformationModelFactories;
	static std::vector<UserTransformationModelFactory*> userTransformationModelFactories;

//*****************************************************************************
// Configuration loading
//*****************************************************************************
private:
	static XmlConfigurationLoader userXmlConfigLoader;
	static XmlConfigurationLoader cursorTransXmlConfigLoader;
	static XmlConfigurationLoader userTransXmlConfigLoader;

	static bool loadCursorTransformationModelConfig(std::string configFile, User* user);
	static bool loadUserTransformationModelConfig(std::string configFile, User* user);

	class UserConverterToV1_0a4 : public XmlConfigurationConverter {
	public:
		UserConverterToV1_0a4();
		virtual bool convert(XmlDocument* document, const Version& version, std::string configFile);
	}; // UserConverterToV1_0a4

	class CursorTransConverterToV1_0a4 : public XmlConfigurationConverter {
	public:
		CursorTransConverterToV1_0a4();
		virtual bool convert(XmlDocument* document, const Version& version, std::string configFile);
	}; // CursorTransConverterToV1_0a4

	class UserTransConverterToV1_0a4 : public XmlConfigurationConverter {
	public:
		UserTransConverterToV1_0a4();
		virtual bool convert(XmlDocument* document, const Version& version, std::string configFile);
	}; // UserTransConverterToV1_0a4

}; // User

#endif
