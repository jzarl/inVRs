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

#ifndef __PARTICLESYSTEM_H__
#define __PARTICLESYSTEM_H__

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGBaseTypes.h>

#include "ParticleSystemParticles.h"
//#include "ODETimer/timer.h"
#include <inVRs/SystemCore/Timer.h>
#include "ParticleEmitter.h"
#include "ParticleSystemConfig.h"

class CParticleSystem : public IParticles
{
public:
  CParticleSystem( void );
  ~CParticleSystem( void );

  bool bInitialize( const char *i_pConfigFile, OSG::Real32 i_rDesiredUpdatesPerSecond,
    OSG::UInt32 i_iRandomSeed, OSG::Real32 i_rSynchronisationInterval, OSG::UInt32 i_iForwardThreshold );

  void NextFrame( void );

  void ForwardTo( OSG::UInt32 i_iUpdateCount );
  void SynchronizeTo( OSG::UInt32 i_iUpdateCount );

  void MoveParticles( const OSG::Vec3f &i_movement );
  OSG::Vec3f GetSize();

private:

public:
  inline OSG::NodePtr pGetNodePtr( void ) { return m_pNode; }
  inline OSG::Real32 rGetDesiredUpdatesPerSecond() const { return m_rDesiredUpdatesPerSecond; }

  OSG::Pnt3f RandomSpawnPoint( void ); // used by emitter

  // used during configuration
  inline void AddChild( CParticleEmitter *i_pChild ) { if( i_pChild ) m_pChildren.push_back( i_pChild ); }
  inline void SetRadius( OSG::Real32 i_rRadius ) { m_rRadius = i_rRadius; }

  /// Returns an uniformly distributed random floating-point number in the range ]-1;1[.
  inline OSG::Real32 rRandomReal()
  {
    m_iRandomSeed = m_iRandomSeed * 172790621 + 1;
    return ((OSG::Real32)m_iRandomSeed / (OSG::Real32)0xffffffff) * 2.0f - 1.0f;
  }

  inline OSG::UInt32 iGetUpdateCount() const { return m_iNumProcessedUpdates; }

private:
  OSG::NodePtr    m_pNode;
//  dStopwatch m_Timer;
  inVRsUtilities::Timer timer;
  OSG::Real64     m_rTimeLeft, m_rTotalTime;
  OSG::Real32     m_rDesiredUpdatesPerSecond, m_rCurrentUpdatesPerSecond, m_rRadius;
  OSG::UInt32     m_iNumProcessedUpdates, m_iRandomSeed, m_iForwardThreshold;
  OSG::Real32     m_rSynchronisationInterval;
  bool       m_bProcessedFirstUpdate;

  vector<CParticleEmitter *> m_pChildren;
};

#endif // __PARTICLESYSTEM_H__
