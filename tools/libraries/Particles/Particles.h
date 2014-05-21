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

#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include <string>
#include <OpenSG/OSGQuaternion.h>
#include <OpenSG/OSGVector.h>
#include <OpenSG/OSGNode.h>
#include <OpenSG/OSGTransform.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGBaseTypes.h>
#include <OpenSG/OSGSwitch.h>
#include <inVRs/SystemCore/DataTypes.h>
#include "ParticleSystemParticles.h"

class Particles
{
	public:
		Particles(std::string fileName, bool makeClone=true);
		Particles(Particles* src);
		~Particles();
		
		void update();
		OSG::NodePtr getNodePtr();
		void setPosition(OSG::Vec3f pos);
		void setPosition(gmtl::Vec3f startPos, gmtl::Vec3f endPos, float offset = 0.0f);
		void setRotation(OSG::Quaternion q);
		void setScale(OSG::Real32 factor);
		void setScale(gmtl::Vec3f scale);
		void setModelTransformation(TransformationData trans);
		void createSymmetricEmitterLine(OSG::Vec3f dir, OSG::Real32 dist, OSG::UInt16 num);
		bool isActive();
		void activate();
		void deactivate();
		void reset(bool makeClone=true);
		void duplicate(OSG::Vec3f deltaPos, OSG::Quaternion deltaRot);

	private:
		bool cloned;
		ps_info particleSystemInfo;
		IParticles *pParticles;
		OSG::TransformPtr particleTrans;
		OSG::TransformPtr particleModelTrans;
/*		OSG::SwitchPtr particleSwitch;*/
// 		OSG::NodePtr particleSwitchNode;
		OSG::NodePtr particleNode;
		OSG::NodePtr particleModelNode;
		OSG::NodePtr particleTransNode;
}; // Particles

#endif //_PARTICLES_H_
