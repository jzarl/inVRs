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

#include "AvataraAvatarBase.h"

#include <cassert>
#include <memory>

#include <gmtl/MatrixOps.h>

#include <irrXML.h>

#include "avatara/Vector.h"

#include <inVRs/SystemCore/XMLTools.h>
#include <inVRs/SystemCore/DebugOutput.h>
#include <inVRs/SystemCore/Configuration.h>
#include <inVRs/SystemCore/UserDatabase/User.h>
#include <inVRs/SystemCore/UtilityFunctions.h>

using namespace io;

const int AvataraAvatarBase::defaultTransitionTime = 250;
XmlConfigurationLoader AvataraAvatarBase::xmlConfigLoader;

AvataraAvatarBase::AvataraAvatarBase() :
	smoothAnimation(true),
	speed(1.f),
	singleRunAnimation(false),
	singleAnimationTime(-1),
	imageFile(),
	defaultAnimation(),
	avatarScale(1),
	avatarInitialized(false),
	avatarGeometryBuilt(false),
	avatar(NULL),
	modelTransform(IdentityTransformation) {
	if (!xmlConfigLoader.hasConverters()) {
		xmlConfigLoader.registerConverter(new ConverterToV1_0a4);
	}
}

AvataraAvatarBase::~AvataraAvatarBase() {
	std::vector<Avatara::Animation*>::iterator it;
	for (it = animations.begin(); it != animations.end(); ++it) {
		delete *it;
	} // for
	animations.clear();

	delete avatar;
	//TODO delete also the model?

} // ~AvataraAvatarBase

bool AvataraAvatarBase::loadConfig(std::string configFile) {
	bool success = true;

	std::string configFileConcatenatedPath = getConcatenatedPath(configFile, "AvatarConfiguration");
	printd(INFO, "AvataraAvatarBase::loadConfig(): loading configuration %s\n", configFile.c_str());

	// load xml document via xmlConfigLoader
	std::auto_ptr<const XmlDocument> document(
			xmlConfigLoader.loadConfiguration(configFileConcatenatedPath));
	if (!document.get()) {
		printd(ERROR,
				"AvataraAvatarBase::loadConfig(): error at loading of configuration file %s!\n",
				configFile.c_str());
		return false;
	} // if

	// load avatar name
	if (document->hasAttribute("avataraAvatar.name.value")) {
		this->avatarName = document->getAttributeValue("avataraAvatar.name.value");
	} // if

	// load avatar model
	std::string avatarPath = Configuration::getPath("Avatars");
	std::string modelFile = document->getAttributeValue("avataraAvatar.representation.file.name");
	if (!model.Load((avatarPath + modelFile).c_str())) {
		printd(ERROR, "AvatatarAvatar::loadConfig(): error loading model %s!\n", modelFile.c_str());
		success = false;
	} // if

	// set avatar model transformation
	const XmlElement* transformationElement =
		document->getElement("avataraAvatar.representation.transformation");
	if (transformationElement) {
		success = readTransformationDataFromXmlElement(modelTransform, transformationElement)
				&& success;
	} // if

	// read url to texture file
	imageFile = document->getAttributeValue("avataraAvatar.texture.file");

	// read animation configurations
	if (document->hasAttribute("avataraAvatar.animations.smooth")) {
		std::string smoothString = document->getAttributeValue("avataraAvatar.animations.smooth");
		convert(smoothString, smoothAnimation);
	} // if
	if (document->hasAttribute("avataraAvatar.animations.speed")) {
		speed = document->getAttributeValueAsFloat("avataraAvatar.animations.speed");
	} // if
	defaultAnimation = document->getAttributeValue("avataraAvatar.animations.default");

	// read animations
	std::vector<const XmlElement*> animationElements =
		document->getElements("avataraAvatar.animations.animation");
	std::vector<const XmlElement*>::iterator animIt;
	Avatara::Animation* anim = NULL;
	std::string animationFile;
	std::string animationName;
	for (animIt = animationElements.begin(); animIt != animationElements.end(); ++animIt) {
		anim = new Avatara::Animation();
		animationFile = (*animIt)->getAttributeValue("file");
		printd(INFO, "AvataraAvatarBase::loadConfig(): Trying to load animation %s\n", (avatarPath + animationFile).c_str());
		if (!anim->Load((avatarPath+animationFile).c_str())) {
			printd(ERROR, "AvataraAvatarBase::loadConfig(): error loading animation %s!\n", animationFile.c_str());
			success = false;
			break;
		} // if
		animationName = (*animIt)->getAttributeValue("name");
		animationSet.Add(animationName, anim);
	} // for

	if (success)
		avatarInitialized = true;

	return success;
} // loadConfig



void AvataraAvatarBase::setHandTransformation(TransformationData trans) {
	checkAvatarBuilt();
	avatar->MoveArm(true);

	TransformationData invModelTransform;
	TransformationData avatarHandTrans;

	// scale to avatar position;
	trans.position *= 1.f / avatarScale;

	invert(invModelTransform, modelTransform);
	multiply(avatarHandTrans, invModelTransform, trans);

	Algebra::Vector position = Algebra::Vector(avatarHandTrans.position[0],
			avatarHandTrans.position[1], avatarHandTrans.position[2]);

	avatar->ArmTarget(position);
} // setHandTransformation

void AvataraAvatarBase::setHeadTransformation(TransformationData trans) {
	checkAvatarBuilt();
	gmtl::Vec3f worldForward(0, 0, -1);
	gmtl::Vec3f worldDirection;
	gmtl::Vec4f avatarDirection;
	gmtl::Vec4f tempDirection;
	TransformationData invModelTransform;

	invert(invModelTransform, modelTransform);

	gmtl::xform(worldDirection, trans.orientation, worldForward);

	tempDirection = gmtl::Vec4f(worldDirection[0], worldDirection[1], worldDirection[2], 0);
	transformVector(avatarDirection, invModelTransform, tempDirection);

	setViewDirection(gmtl::Vec3f(avatarDirection[0], avatarDirection[1], avatarDirection[2]));
} // setHeadTransformation

void AvataraAvatarBase::setTransformation(TransformationData trans) {
	checkAvatarBuilt();

	gmtl::Matrix44f matrix;
	transformationDataToMatrix(trans, matrix);

	setAvatarTransformation(matrix);
} // setTransformation

void AvataraAvatarBase::setOwner(User* user) {
	AvatarInterface::setOwner(user);

	gmtl::Matrix44f matrix;
	TransformationData scaledModelTransform;

	// should be read from user if there is a user scale value
	avatarScale = 1;

	if (avatarGeometryBuilt) {
		scaledModelTransform = modelTransform;
		scaledModelTransform.scale *= avatarScale;
		transformationDataToMatrix(scaledModelTransform, matrix);

		setAvatarOffset(matrix);
	} // if
} // setOwner

void AvataraAvatarBase::checkAvatarBuilt() {
	if (avatarGeometryBuilt)
		return;

	assert(avatarInitialized);

	avatar = buildAvatar();

	avatar->SetModel(&model);
	avatar->SetAnimations(&animationSet);
	avatar->SmoothAnimation(smoothAnimation);
	avatar->SetSpeed(speed);

	avatarModel = buildAvatarModel();

	TransformationData scaledModelTransform = modelTransform;
	scaledModelTransform.scale *= avatarScale;
	gmtl::Matrix44f matrix;
	transformationDataToMatrix(scaledModelTransform, matrix);
	setAvatarOffset(matrix);

	avatarGeometryBuilt = true;

	if (defaultAnimation != "")
		this->startAnimation(defaultAnimation);
}

void AvataraAvatarBase::setViewDirection(gmtl::Vec3f dir) {
	checkAvatarBuilt();
	avatar->TurnHead(true);
	Algebra::Vector direction = Algebra::Vector(dir[0], dir[1], dir[2]);
	avatar->LookVector(direction);
} // setViewDirection

ModelInterface* AvataraAvatarBase::getModel() {
	checkAvatarBuilt();
	return avatarModel;
}

void AvataraAvatarBase::showAvatar(bool active) {
	checkAvatarBuilt();
	if (avatarModel != NULL && avatarModel->isVisible() != active) {
		avatarModel->setVisible(active);
	} // if
} // showAvatar

bool AvataraAvatarBase::isVisible() {
	if (avatarModel != NULL)
		return avatarModel->isVisible();
	return false;
} // isVisible

void AvataraAvatarBase::update(float dt) {
	int animTime;

	if (avatarGeometryBuilt)
		updateAvatarGeo(avatar);

	if (singleRunAnimation) {
		animTime = avatar->GetAnimationTime();
		if (animTime < singleAnimationTime && singleAnimationTime > 0) {
			avatar->StopAnimation();
			singleRunAnimation = false;
			if (defaultAnimation.length() > 0) {
				startAnimation(defaultAnimation, defaultTransitionTime);
			} // if
		} // if
		else
			singleAnimationTime = animTime;
	} // if

	setHeadTransformation(owner->getUserHeadTransformation());
	setHandTransformation(owner->getUserHandTransformation());
} // update

bool AvataraAvatarBase::startAnimation(std::string animationType, int transitionTime) {
	printd(INFO, "AvataraAvatarBase::startAnimation(): Trying to start Animation %s\n",
			animationType.c_str());
	avatar->MoveArm(false);
	avatar->TurnHead(false);
	avatar->SetAnimation(animationType, transitionTime);
	avatar->StartAnimation();
	singleRunAnimation = false;
	return true;
} // startAnimation

bool AvataraAvatarBase::runAnimationOnce(std::string animationType, int transitionTime) {
	printd(INFO, "AvataraAvatarBase::runAnimationOnce(): Trying to start Animation %s\n",
			animationType.c_str());
	avatar->MoveArm(false);
	avatar->TurnHead(false);
	avatar->SetAnimation(animationType, transitionTime);
	avatar->StartAnimation();
	singleRunAnimation = true;
	singleAnimationTime = -1;
	return true;
} // runAnimationOnce

void AvataraAvatarBase::stopAnimation() {
	avatar->StopAnimation();
	singleRunAnimation = false;
} // stopAnimation



//*****************************************************************************
// Configuration loading
//*****************************************************************************
AvataraAvatarBase::ConverterToV1_0a4::ConverterToV1_0a4() :
	XmlConfigurationConverter("0.0", "1.0a4") {
} // ConverterToV1_0a4

bool AvataraAvatarBase::ConverterToV1_0a4::convert(XmlDocument* document, const Version& version,
		std::string configFile) {
	if (document->getRootElement()->getName() != "avatar" &&
			document->getRootElement()->getName() != "config") {
		printd(ERROR,
				"AvataraAvatarBase::ConverterToV1_0a4::convert(): could not convert file because of invalid root node <%s> in Avatar configuration file! Please upgrade to a current xml file version\n",
				document->getRootElement()->getName().c_str());
		return false;
	} // if

	XmlElement* avatarElement = NULL;

	// replace root node (if existing) and make <avatar> node as root
	if (document->getRootElement()->getName() != "avatar") {
		avatarElement = document->getRootElement()->getSubElement("avatar");
		if (!avatarElement) {
			printd(ERROR,
					"AvataraAvatarBase::ConverterToV1_0a4::convert(): could not convert file because of misssing node <avatar>! Please upgrade to a current xml file version!\n");
			return false;
		} // if
		document->getRootElement()->removeSubElement(avatarElement);
		XmlElement* oldRootElement = document->replaceRootElement(avatarElement);
		delete oldRootElement;
	} // if
	else {
		avatarElement = document->getRootElement();
	} // else

	// replace avatar node with type based node
	// get type of avatar and use it as new node name, afterwards delete all attributes
	std::string avatarType = avatarElement->getAttributeValue("type");
	if (avatarType != "AvataraAvatar") {
		printd(ERROR,
				"AvataraAvatarBase::ConverterToV1_0a4::convert(): expected type AvataraAvatar, found type %s!\n",
				avatarType.c_str());
		return false;
	} // if
	avatarElement->deleteAllAttributes();

	updateDocument(document, XmlConfigurationLoader::XmlDtdUrl + "avataraAvatar_v1.0a4.dtd",
			destinationVersion, "avataraAvatar");

	// remove image attribute -> is moved into texture element!
	std::string texture = avatarElement->getAttributeValue("model.image");
	XmlElement* modelElement = avatarElement->getSubElement("model");
	if (modelElement && modelElement->hasAttribute("image")) {
		XmlAttribute* imageAttribute = modelElement->removeAttribute("image");
		assert(imageAttribute);
		delete imageAttribute;
	} // if

	// convert the model and modeltransformation node into an representation node
	bool success = XmlRepresentationLoader::get().updateXmlElement(avatarElement,
			version, destinationVersion, configFile);

	if (success) {
		// add file type to file element of representation node
		XmlElement* fileElement = avatarElement->getSubElement("representation.file");
		assert(fileElement);
		XmlAttribute* typeAttribute = new XmlAttribute("type", "Avatara MDL");
		fileElement->addAttribute(typeAttribute);

		std::vector<std::string> attributes;
		attributes.push_back("type");
		attributes.push_back("name");
		fileElement->fixAttributeOrder(attributes);
	} // if

	// add texture information in new texture element
	XmlElement* textureElement = new XmlElement("texture");
	XmlAttribute* textureAttribute = new XmlAttribute("file", texture);
	textureElement->addAttribute(textureAttribute);
	avatarElement->addSubElement(textureElement);

	// update all <animation> elements to name/file
	avatarElement->renameAttributes("animations.animation.key", "name");
	avatarElement->renameAttributes("animations.animation.path", "file");

	std::vector<std::string> subElements;
	subElements.push_back("name");
	subElements.push_back("representation");
	subElements.push_back("texture");
	subElements.push_back("animations");

	avatarElement->fixSubElementOrder(subElements);

	return success;
} // convert
