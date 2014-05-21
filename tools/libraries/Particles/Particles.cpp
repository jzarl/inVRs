/*
 * Copyright (c) 2005, Stephan Reiter <stephan.reiter@students.jku.at>,
 * Christian Wressnegger <christian.wressnegger@students.jku.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * The names of its contributors may not be used to endorse or promote
 * products derived from this software without specific prior written
 * permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <gmtl/Generate.h>
#include "Particles.h"

OSG_USING_NAMESPACE

Particles::Particles(std::string fileName, bool makeClone)
{
	int i;

	particleSystemInfo.pConfigFile = new char[512];
  	sprintf( (char *)particleSystemInfo.pConfigFile, "%s/effect.xml", fileName.c_str());
	particleSystemInfo.rUpdatesPerSecond = 30;
	particleSystemInfo.iRandomSeed = time( NULL );
	particleSystemInfo.rSynchronisationInterval = 1.0f;
	particleSystemInfo.iForwardThreshold = 50;
	pParticles = pCreateParticleSystem( particleSystemInfo );
//	delete particleSystemInfo.pConfigFile;

	particleNode = Node::create();
	particleModelNode = pParticles != 0 ? pParticles->pGetNodePtr() : Node::create();
// 	particleNode = pParticles != 0 ? pParticles->pGetNodePtr() : Node::create();

	Vec3f boundingBoxSize = pParticles->GetSize();
	
	particleTransNode = Node::create();
	particleTrans = Transform::create();

	particleModelTrans = Transform::create();
	
	Matrix m;
	m.setIdentity();
	beginEditCP(particleTrans, Transform::MatrixFieldMask);
		particleTrans->setMatrix(m);
	endEditCP(particleTrans, Transform::MatrixFieldMask);

	beginEditCP(particleModelTrans, Transform::MatrixFieldMask);
		particleModelTrans->setMatrix(m);
	endEditCP(particleModelTrans, Transform::MatrixFieldMask);

	beginEditCP(particleNode, Node::CoreFieldMask | Node::ChildrenFieldMask);
		particleNode->setCore(particleModelTrans);
		particleNode->addChild(particleModelNode);
	endEditCP(particleNode, Node::CoreFieldMask | Node::ChildrenFieldMask);
	
	beginEditCP(particleTransNode);
		particleTransNode->setCore(particleTrans);
		if (makeClone)
			particleTransNode->addChild(cloneTree(particleNode));
		else
			particleTransNode->addChild(particleNode);

		// set volume static to prevent constant update
		Volume &v = particleTransNode->getVolume( false ).getInstance();
		v.setEmpty();
		v.extendBy( Pnt3f( -boundingBoxSize[0], -boundingBoxSize[1], -boundingBoxSize[2] ) );
		v.extendBy( Pnt3f( boundingBoxSize[0], boundingBoxSize[1], boundingBoxSize[2] ) );
		v.setStatic();
		((DynamicVolume&)particleTransNode->getVolume()).instanceChanged();

	endEditCP(particleTransNode);

	cloned = false;
} // Particles

Particles::Particles(Particles* src)
{
	int i;

	particleSystemInfo = src->particleSystemInfo;
	pParticles = src->pParticles;

// 	particleNode = pParticles != 0 ? pParticles->pGetNodePtr() : Node::create();
	particleNode = Node::create();
	particleModelNode = pParticles->pGetNodePtr();
	assert(particleModelNode != NullFC);

	Vec3f boundingBoxSize = pParticles->GetSize();
	
	particleTransNode = Node::create();
	particleTrans = Transform::create();

	particleModelTrans = Transform::create();

	Matrix m;
	m.setIdentity();
	beginEditCP(particleTrans, Transform::MatrixFieldMask);
		particleTrans->setMatrix(m);
	endEditCP(particleTrans, Transform::MatrixFieldMask);

	beginEditCP(particleModelTrans, Transform::MatrixFieldMask);
		particleModelTrans->setMatrix(m);
	endEditCP(particleModelTrans, Transform::MatrixFieldMask);

	beginEditCP(particleNode, Node::CoreFieldMask | Node::ChildrenFieldMask);
		particleNode->setCore(particleModelTrans);
		particleNode->addChild(particleModelNode);
	endEditCP(particleNode, Node::CoreFieldMask | Node::ChildrenFieldMask);

	beginEditCP(particleTransNode);
		particleTransNode->setCore(particleTrans);
		particleTransNode->addChild(cloneTree(particleNode));

		// set volume static to prevent constant update
		Volume &v = particleTransNode->getVolume( false ).getInstance();
		v.setEmpty();
		v.extendBy( Pnt3f( -boundingBoxSize[0], -boundingBoxSize[1], -boundingBoxSize[2] ) );
		v.extendBy( Pnt3f( boundingBoxSize[0], boundingBoxSize[1], boundingBoxSize[2] ) );
		v.setStatic();
		((DynamicVolume&)particleTransNode->getVolume()).instanceChanged();

	endEditCP(particleTransNode);
	cloned = true;
} // Particles

Particles::~Particles()
{
	if (!cloned)
	{
		delete particleSystemInfo.pConfigFile;
		delete pParticles;
	} // if
} // Particles


void Particles::update()
{
	pParticles->NextFrame();
	particleNode->invalidateVolume();
} // update

NodePtr Particles::getNodePtr()
{
// 	return particleSwitchNode;
	return particleTransNode;
} // getNodePtr

void Particles::setModelTransformation(TransformationData trans)
{
	OSG::Matrix m;
	gmtl::Matrix44f mat;
	transformationDataToMatrix(trans, mat);
	m.setValue(mat.getData());
	beginEditCP(particleModelTrans, Transform::MatrixFieldMask);
		particleModelTrans->setMatrix(m);
	endEditCP(particleModelTrans, Transform::MatrixFieldMask);
} // Particles

void Particles::setPosition(Vec3f pos)
{
	Matrix mOrig = particleTrans->getMatrix();
	Matrix mNew = mOrig;
	mNew.setTranslate(pos);
	beginEditCP(particleTrans, Transform::MatrixFieldMask);
		particleTrans->setMatrix(mNew);
	endEditCP(particleTrans, Transform::MatrixFieldMask);

	mOrig.invert();
//	mNew.multLeft(mOrig);
	mNew.mult(mOrig);
	beginEditCP(particleTransNode);
		DynamicVolume &v = particleTransNode->getVolume(false);
		v.transform(mNew);
	endEditCP(particleTransNode);

} // setPosition

void Particles::setRotation(Quaternion q)
{
	int i;
	Vec3f position, scale;
	Pnt3f minPnt, maxPnt;
	Quaternion orientation, scaleOrientation;
	Matrix mOrig = particleTrans->getMatrix();
	Matrix mNew = mOrig;
	mNew.getTransform(position, orientation, scale, scaleOrientation);
	orientation = q;
	mNew.setTransform(position, orientation, scale, scaleOrientation);
	beginEditCP(particleTrans, Transform::MatrixFieldMask);
		particleTrans->setMatrix(mNew);
	endEditCP(particleTrans, Transform::MatrixFieldMask);

	mOrig.invert();
//	mNew.multLeft(mOrig);
	mNew.mult(mOrig);
	beginEditCP(particleTransNode);
		DynamicVolume &v = particleTransNode->getVolume(false);
		v.transform(mNew);
	endEditCP(particleTransNode);

} // setRotation

void Particles::setScale(Real32 factor)
{
	Vec3f position, scale;
	Quaternion orientation, scaleOrientation;
	Matrix mOrig = particleTrans->getMatrix();
	Matrix mNew = mOrig;
	mNew.getTransform(position, orientation, scale, scaleOrientation);
	scale = Vec3f(factor, factor, factor);
	mNew.setTransform(position, orientation, scale, scaleOrientation);
	beginEditCP(particleTrans, Transform::MatrixFieldMask);
		particleTrans->setMatrix(mNew);
	endEditCP(particleTrans, Transform::MatrixFieldMask);

	mOrig.invert();
//	mNew.multLeft(mOrig);
	mNew.mult(mOrig);
	beginEditCP(particleTransNode);
		DynamicVolume &v = particleTransNode->getVolume(false);
		v.transform(mNew);
	endEditCP(particleTransNode);

} // setScale

void Particles::setScale(gmtl::Vec3f scale)
{
	float maxScale;
	Vec3f position, osgScale;
	Quaternion orientation, scaleOrientation;

	beginEditCP(particleTrans);
		Matrix mOrig = particleTrans->getMatrix();
	endEditCP(particleTrans);

	Matrix mNew = mOrig;
	mNew.getTransform(position, orientation, osgScale, scaleOrientation);
	osgScale = Vec3f(scale[0], scale[1], scale[2]);
	mNew.setTransform(position, orientation, osgScale, scaleOrientation);
	beginEditCP(particleTrans, Transform::MatrixFieldMask);
		particleTrans->setMatrix(mNew);
	endEditCP(particleTrans, Transform::MatrixFieldMask);
	
	mOrig.invert();
//	mNew.multLeft(mOrig);
	mNew.mult(mOrig);
	beginEditCP(particleTransNode);
		DynamicVolume &v = particleTransNode->getVolume(false);
		v.transform(mNew);
	endEditCP(particleTransNode);

} // setScale

void Particles::setPosition(gmtl::Vec3f startPos, gmtl::Vec3f endPos, float offset)
{
	float angle;
	Matrix m, m1, mOrig, mNew;
	Quaternion q;
	gmtl::Vec3f up(0,1,0);
	gmtl::Vec3f axis;
	gmtl::Vec3f delta = endPos - startPos;
	gmtl::Vec3f dir = delta;
	gmtl::normalize(dir);
	gmtl::Vec3f newPos;

	beginEditCP(particleTrans);
		mOrig = particleTrans->getMatrix();
	endEditCP(particleTrans);
	mNew = mOrig;
	
	if (dir != up)
	{
		gmtl::cross(axis, up, dir);
		angle = acos(gmtl::dot(up, dir));
		q.setValueAsAxisRad(Vec3f(axis[0], axis[1], axis[2]), angle);
		m1.setRotate(q);
	} // if

	float len = gmtl::length(delta);
	m.setScale(1, len-offset, 1);	// 1*rad from Scooter
	m.multLeft(m1);
	newPos = startPos + dir * offset;
	m.setTranslate(Vec3f(newPos[0], newPos[1], newPos[2]));	
	beginEditCP(particleTrans, Transform::MatrixFieldMask);
		particleTrans->setMatrix(m);
	endEditCP(particleTrans, Transform::MatrixFieldMask);

// TODO: fix Bounding Box calculation!!!
	mNew.setTranslate(Vec3f(newPos[0], newPos[1], newPos[2]));
	mOrig.invert();
//	mNew.multLeft(mOrig);
	mNew.mult(mOrig);
	beginEditCP(particleTransNode);
		DynamicVolume &v = particleTransNode->getVolume(false);
		v.transform(mNew);
	endEditCP(particleTransNode);	

} // setPosition

void Particles::duplicate(Vec3f deltaPos, Quaternion deltaRot)
{
	NodePtr copyTransNode = cloneTree(particleTransNode);
	TransformPtr copyTrans = Transform::create();
	Matrix m;
	m.setIdentity();
	m.setRotate(deltaRot);
	m.setTranslate(deltaPos);
	beginEditCP(copyTrans);
		copyTrans->setMatrix(m);
	endEditCP(copyTrans);
	beginEditCP(copyTransNode);
		copyTransNode->setCore(copyTrans);
	endEditCP(copyTransNode);
	beginEditCP(particleTransNode);
		particleTransNode->addChild(copyTransNode);
	endEditCP(particleTransNode);
} // duplicate

void Particles::createSymmetricEmitterLine(Vec3f dir, Real32 dist, UInt16 num)
{
	NodePtr localTransNode;
	TransformPtr localTrans;
	Matrix m;
	dir.normalize();
	for (int i = 1; i < num; i++)
	{
		localTransNode = Node::create();
		localTrans = Transform::create();
		m.setIdentity();
		m.setTranslate(dir*(i*dist));
		beginEditCP(localTrans, Transform::MatrixFieldMask);
			localTrans->setMatrix(m);
		endEditCP(localTrans, Transform::MatrixFieldMask);
		beginEditCP(localTransNode, Node::CoreFieldMask | Node::ChildrenFieldMask);
			localTransNode->setCore(localTrans);
			localTransNode->addChild(cloneTree(particleNode));
		endEditCP(localTransNode, Node::CoreFieldMask | Node::ChildrenFieldMask);
		beginEditCP(particleTransNode, Node::ChildrenFieldMask);
			particleTransNode->addChild(localTransNode);
		endEditCP(particleTransNode, Node::ChildrenFieldMask);

		localTransNode = Node::create();
		localTrans = Transform::create();
		m.setIdentity();
		m.setTranslate(dir*(-i*dist));
		beginEditCP(localTrans, Transform::MatrixFieldMask);
			localTrans->setMatrix(m);
		endEditCP(localTrans, Transform::MatrixFieldMask);
		beginEditCP(localTransNode, Node::CoreFieldMask | Node::ChildrenFieldMask);
			localTransNode->setCore(localTrans);
			localTransNode->addChild(cloneTree(particleNode));
		endEditCP(localTransNode, Node::CoreFieldMask | Node::ChildrenFieldMask);
		beginEditCP(particleTransNode, Node::ChildrenFieldMask);
			particleTransNode->addChild(localTransNode);
		endEditCP(particleTransNode, Node::ChildrenFieldMask);
	} // for
} // createSymmetricEmitterLine

bool Particles::isActive()
{
	bool result;
	beginEditCP(particleTransNode);
		result = particleTransNode->getActive();
	endEditCP(particleTransNode);
	return result;
} // isActive

void Particles::activate()
{
	beginEditCP(particleTransNode);
		particleTransNode->setActive(true);
	endEditCP(particleTransNode);	
} // activate

void Particles::deactivate()
{
	beginEditCP(particleTransNode);
		particleTransNode->setActive(false);
	endEditCP(particleTransNode);	
} // deactivate


void Particles::reset(bool makeClone)
{
	beginEditCP(particleTransNode, Node::ChildrenFieldMask);
		particleTransNode->subChild(0);
	endEditCP(particleTransNode, Node::ChildrenFieldMask);
	
	delete pParticles;
	pParticles = pCreateParticleSystem( particleSystemInfo );
	particleNode = pParticles != 0 ? pParticles->pGetNodePtr() : Node::create();

	beginEditCP(particleTransNode, Node::ChildrenFieldMask);
		if (makeClone)
			particleTransNode->addChild(cloneTree(particleNode));
		else
			particleTransNode->addChild(particleNode);
	endEditCP(particleTransNode, Node::ChildrenFieldMask);
} // reset

