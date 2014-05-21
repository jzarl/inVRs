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

#ifndef __PARTICLESYSTEMCONFIG_H__
#define __PARTICLESYSTEMCONFIG_H__

#include <irrXML.h>

using namespace irr;
using namespace io;

class CParticleSystem;
class CParticleEmitter;
class CParticleType;

namespace CParticleSystemConfig
{
	bool bLoadParticleEmitters( const char *i_pConfigFile, CParticleSystem *o_pParent );
	bool parseParticleOrigin(IrrXMLReader* xml, CParticleSystem *o_pParent);
	CParticleEmitter* parseEmitter(IrrXMLReader* xml, CParticleSystem *o_pParent, bool emitterLoop);
	CParticleType* parseParticle(IrrXMLReader* xml, CParticleSystem *o_pParent, CParticleType* particle);
	bool parseEmission(IrrXMLReader* xml, CParticleEmitter* pEmitter);
// 	float calculateSceneLength( CParticleEmitter *i_pEmitter );
// 	bool convertImageToColorTimeline( CTimeline<Vec3f> &o_ColorTimeline, char *i_pFilename, float i_rLength );
}

#endif // __PARTICLESYSTEMCONFIG_H__
