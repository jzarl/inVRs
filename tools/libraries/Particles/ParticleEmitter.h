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

#ifndef __PARTICLEEMITTER_H__
#define __PARTICLEEMITTER_H__

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGBaseTypes.h>
#include <OpenSG/OSGParticles.h>

#include "ParticleType.h"

struct particle
{
  inline bool bIsDead() const { return rTimeLived >= rLifeTime; }

  OSG::Real32  rLifeTime, rTimeLived;

  OSG::Real32  rSize0;
  OSG::Vec3f   velocity0;
  OSG::Vec3f   color0;

  OSG::Pnt3f   position;
};

class CParticleEmitter
{
public:
  CParticleEmitter( class CParticleSystem *i_pParent, CParticleType *i_pParticleType );
  ~CParticleEmitter();

  bool bInitialize();

  void Update( OSG::Real32 i_rTime, OSG::Real32 i_rDeltaTime, bool i_bSimulate );

  OSG::Real32 rGetEmittedParticles( OSG::Real32 i_rTime ) const;
  OSG::UInt32 iEstimateMaximumParticles() const;

  bool bSpawnParticle( struct particle &o_particle, OSG::Real32 i_rTime, OSG::Pnt3f i_position ) const;
  float GetParticleLength();

private:
  OSG::Vec3f GetRandomDirection( OSG::Real32 i_rTime ) const;

public:
  inline CParticleType *pGetParticleType() { return m_pParticleType; }
  inline OSG::NodePtr pGetNodePtr() { return m_pNode; }

  inline void SetLength( OSG::Real32 i_rLength ) { m_rLength = i_rLength; }
  inline OSG::Real32 rGetLength() const { return m_rLength; }

  inline CTimeline<OSG::Vec3f>  &GetEmissionDirection() { return m_emissionDirection; }
  inline CTimeline<OSG::Real32> &GetEmissionDirectionVariation() { return m_emissionDirectionVariation; }
  inline CTimeline<OSG::Real32> &GetEmissionRate() { return m_emissionRate; }
  inline CTimeline<OSG::Real32> &GetEmissionRateVariation() { return m_emissionRateVariation; }
  inline CTimeline<OSG::Real32> &GetLifeTime() { return m_lifeTime; }
  inline CTimeline<OSG::Real32> &GetLifeTimeVariation() { return m_lifeTimeVariation; }
  inline CTimeline<OSG::Vec3f>  &GetColor() { return m_color; }
  inline CTimeline<OSG::Vec3f>  &GetColorVariation() { return m_colorVariation; }
  inline CTimeline<OSG::Real32> &GetColorVariationLink() { return m_colorVariationLink; }
  inline CTimeline<OSG::Real32> &GetVelocity() { return m_velocity; }
  inline CTimeline<OSG::Real32> &GetVelocityVariation() { return m_velocityVariation; }
  inline CTimeline<OSG::Real32> &GetSize() { return m_size; }
  inline CTimeline<OSG::Real32> &GetSizeVariation() { return m_sizeVariation; }

  inline void SetLoop( bool i_bLoop ) { m_bLoop = i_bLoop; }
  inline bool bGetLoop() const { return m_bLoop; }

  inline void MoveParticles( const OSG::Vec3f &i_movement ) { m_nextMovement += i_movement; }

private:
  class CParticleSystem *m_pParent;
  CParticleType         *m_pParticleType;

  OSG::UInt32       m_iNumParticles;
  particle     *m_pParticles;
  OSG::ParticlesPtr m_pDrawableParticles;
  OSG::Real32       m_rEmissionRemainder;

  OSG::NodePtr m_pNode;
  bool    m_bLoop;
  OSG::Real32  m_rLength;
  OSG::Vec3f   m_nextMovement;

  CTimeline<OSG::Vec3f>  m_emissionDirection;
  CTimeline<OSG::Real32> m_emissionDirectionVariation, m_emissionRate, m_emissionRateVariation;
  CTimeline<OSG::Real32> m_lifeTime, m_lifeTimeVariation;
  CTimeline<OSG::Real32> m_size, m_sizeVariation;
  CTimeline<OSG::Real32> m_velocity, m_velocityVariation;
  CTimeline<OSG::Vec3f>  m_color, m_colorVariation;
  CTimeline<OSG::Real32> m_colorVariationLink;
};

#endif // __PARTICLEEMITTER_H__
