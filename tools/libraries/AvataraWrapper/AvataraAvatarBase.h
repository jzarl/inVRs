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

#ifndef _AVATARAAVATARBASE_H
#define _AVATARAAVATARBASE_H

#include "AvataraWrapperExports.h"

#include <string>

#include <avatara/Avatar.h>

#include <inVRs/SystemCore/DataTypes.h>
#include <inVRs/SystemCore/XmlConfigurationConverter.h>
#include <inVRs/SystemCore/ComponentInterfaces/AvatarInterface.h>


/******************************************************************************
 * Wrapper class to the avatara library to provide avatara avatars in inVRs.
 * This class allows to integrate an avatar from the avatara library into
 * inVRs.
 */
class INVRS_AVATARAWRAPPER_API AvataraAvatarBase : public AvatarInterface {
public:

	/**
	 * Constructor
	 */
	AvataraAvatarBase();

	/**
	 * Destructor
	 */
	~AvataraAvatarBase();

	/**
	 * Loads the avatar from the passed xml stream
	 */
	virtual bool loadConfig(std::string configFile);

	/**
	 * Returns the model of the Avatar
	 */
	virtual ModelInterface* getModel();

	/**
	 * Sets the visibility of the avatar
	 */
	virtual void showAvatar(bool active);

	/**
	 * Returns if the avatar is visible or not
	 */
	virtual bool isVisible();

	/**
	 * Sets the transformation of the avatar
	 */
	virtual void setTransformation(TransformationData trans);

	/**
	 * Updates the avatar model by calculating the bone transformations and
	 * correcting the model mesh.
	 */
	virtual void update(float dt);

	/**
	 * Starts an animation loop using the animation with the passed name.
	 */
	bool startAnimation(std::string animationType, int transitionTime = 0);

	/**
	 * Starts the animation once with the passed name.
	 */
	bool runAnimationOnce(std::string animationType, int transitionTime = 0);

	/**
	 * Stops the current animation.
	 */
	void stopAnimation();

protected:
	virtual Avatara::Avatar* buildAvatar() = 0;
	virtual void updateAvatarGeo(Avatara::Avatar* avatar) = 0;
	virtual ModelInterface* buildAvatarModel() = 0;
	virtual void setAvatarOffset(gmtl::Matrix44f m) = 0;
	virtual void setAvatarTransformation(gmtl::Matrix44f m) = 0;

protected:
	std::string imageFile;			// url to the avatar texture
	/// Avatar itself;
	Avatara::Avatar* avatar;

private:
	/**
	 * Sets the user the avatar corresponds to
	 */
	virtual void setOwner(User* user);

	/**
	 * Loads the configuration from the passed config file
	 */
//	bool loadConfig(std::string configFile);

	/**
	 * Sets the avatar's hand transformation
	 */
	void setHandTransformation(TransformationData trans);

	/**
	 * Sets the avatar's head transformation
	 */
	void setHeadTransformation(TransformationData trans);

	/**
	 * Sets the avatar's view direction
	 */
	void setViewDirection(gmtl::Vec3f dir);

	void checkAvatarBuilt();

	static const int defaultTransitionTime;	// defines the default transition time to reach idle

	bool smoothAnimation;			// defines if the animations should be smooth
	float speed;					// sets the speed of the animations
	bool singleRunAnimation;		// defines if a single animation is running
	int singleAnimationTime;		// current time of the animation
	std::string defaultAnimation;	// name of the default avatar animation
	float avatarScale;				// scale of the avatar
	bool avatarInitialized;			// stores if the avatar is initialized
	bool avatarGeometryBuilt;		// stores if the avatar's geometry is built

	TransformationData modelTransform;	// model transformation (from config file)

	/// Avatar model.
	Avatara::Model model;
	/// Avatar animation.
	std::vector<Avatara::Animation*> animations;
	/// Animation set for all avatar animations.
	Avatara::AnimationSet animationSet;

	ModelInterface* avatarModel;

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
}; // AvataraAvatar


#endif //_AVATARAAVATAR_H
