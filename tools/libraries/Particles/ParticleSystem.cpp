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

#include "ParticleSystem.h"
#include <OpenSG/OSGGroup.h>

OSG_USING_NAMESPACE

CParticleSystem::CParticleSystem() :
  m_rTimeLeft( 0.0 ), m_rTotalTime( 0.0 ),
  m_rDesiredUpdatesPerSecond( 0.0f ), m_rRadius( 0.0f ),
  m_iNumProcessedUpdates( 0 ), m_iRandomSeed( 0 ), m_iForwardThreshold( 10 ),
  m_rSynchronisationInterval( 1.0f ), m_bProcessedFirstUpdate( false )
{
  m_rCurrentUpdatesPerSecond = m_rDesiredUpdatesPerSecond;

  m_pNode = Node::create();
  addRefCP( m_pNode );
}

CParticleSystem::~CParticleSystem( void )
{
  for(vector<CParticleEmitter *>::iterator ppChild = m_pChildren.begin();
    ppChild != m_pChildren.end(); ++ppChild )
  {
    delete *ppChild;
  }

  subRefCP( m_pNode );
}

bool CParticleSystem::bInitialize( const char *i_pConfigFile, Real32 i_rDesiredUpdatesPerSecond,
  UInt32 i_iRandomSeed, Real32 i_rSynchronisationInterval, UInt32 i_iForwardThreshold )
{
  if( !CParticleSystemConfig::bLoadParticleEmitters( i_pConfigFile, this ) )
    return false;

  m_rDesiredUpdatesPerSecond = m_rCurrentUpdatesPerSecond = i_rDesiredUpdatesPerSecond;
  m_iRandomSeed = i_iRandomSeed;
  m_rSynchronisationInterval = i_rSynchronisationInterval;
  m_iForwardThreshold = i_iForwardThreshold;

  beginEditCP( m_pNode );
  {
    m_pNode->setCore( Group::create() );
    for(vector<CParticleEmitter *>::iterator ppChild = m_pChildren.begin();
      ppChild != m_pChildren.end(); ++ppChild )
    {
      m_pNode->addChild( (*ppChild)->pGetNodePtr() );
    }
  }
  endEditCP( m_pNode );

  return true;
}

Pnt3f CParticleSystem::RandomSpawnPoint( void )
{
  if( m_rRadius > 0.0f )
  {
    const Real32 r1_2pi = 2.0f * Pi * (0.5f + 0.5f * rRandomReal()), r2 = (0.5f + 0.5f * rRandomReal());
    const Real32 scl = 2.0f * sqrtf( r2 * ( 1.0f - r2 ) ); // circle emission
    return Pnt3f( cosf( r1_2pi ) * scl, 0.0f, 1.0f - 2.0f * r2 ) * m_rRadius;
  }
  else
    return Pnt3f( 0.0f, 0.0f, 0.0f ); // point emission
}

void CParticleSystem::NextFrame( void )
{
  if( !m_bProcessedFirstUpdate )
  {
  	timer.start();
    m_bProcessedFirstUpdate = true;
    return;
  }

  double dt = timer.getTime();
  timer.reset();

  const Real64 rElapsedTime = dt + m_rTimeLeft;
  const Real64 rSecondsPerUpdate = 1.0 / m_rDesiredUpdatesPerSecond; // not current, to keep time in sync
  Real64 rNumUpdates = rElapsedTime * m_rCurrentUpdatesPerSecond;
  if( rNumUpdates <= 0.0 ) rNumUpdates = 0.0f;
  const UInt32 iNumUpdates = (UInt32)rNumUpdates; 

  for( UInt32 i = 0; i < iNumUpdates; ++i )
  {
    for(vector<CParticleEmitter *>::iterator ppChild = m_pChildren.begin();
      ppChild != m_pChildren.end(); ++ppChild )
    {
      (*ppChild)->Update( m_rTotalTime, rSecondsPerUpdate, false );
    }
    m_rTotalTime += rSecondsPerUpdate;
  }

  m_iNumProcessedUpdates += iNumUpdates;
  m_rTimeLeft = rElapsedTime - (Real64)iNumUpdates * rSecondsPerUpdate;

//  dStopwatchReset( &m_Timer );
//  dStopwatchStart( &m_Timer );
}

void CParticleSystem::ForwardTo( UInt32 i_iUpdateCount )
{
  if( i_iUpdateCount <= m_iNumProcessedUpdates )
    return;

  const Real64 rSecondsPerUpdate = 1.0 / m_rDesiredUpdatesPerSecond; // not current, to keep time in sync
  const UInt32 iNumUpdates = i_iUpdateCount - m_iNumProcessedUpdates;

  for( UInt32 i = 0; i < iNumUpdates; ++i )
  {
    for(vector<CParticleEmitter *>::iterator ppChild = m_pChildren.begin();
      ppChild != m_pChildren.end(); ++ppChild )
    {
      (*ppChild)->Update( m_rTotalTime, rSecondsPerUpdate, true );
    }
    m_rTotalTime += rSecondsPerUpdate;
  }

  m_iNumProcessedUpdates += iNumUpdates;
}

void CParticleSystem::SynchronizeTo( UInt32 i_iUpdateCount )
{
  if( i_iUpdateCount >= m_iNumProcessedUpdates + m_iForwardThreshold )
  {
    ForwardTo( i_iUpdateCount );
    m_rCurrentUpdatesPerSecond = m_rDesiredUpdatesPerSecond;
  }
  else
  {
    const Int32 iUpdateDelta = i_iUpdateCount - m_iNumProcessedUpdates;
    m_rCurrentUpdatesPerSecond += ((Real32)iUpdateDelta) / m_rSynchronisationInterval;
    if( m_rCurrentUpdatesPerSecond < 0.0f )
      m_rCurrentUpdatesPerSecond = 0.0f;
  }
}

void CParticleSystem::MoveParticles( const Vec3f &i_movement )
{
  for(vector<CParticleEmitter *>::iterator ppChild = m_pChildren.begin();
    ppChild != m_pChildren.end(); ++ppChild )
  {
    (*ppChild)->MoveParticles( i_movement );
  }
}

Vec3f CParticleSystem::GetSize()
{
	Vec3f result(0,0,0);
	float length;
	for(vector<CParticleEmitter *>::iterator ppChild = m_pChildren.begin();
		ppChild != m_pChildren.end(); ++ppChild )
	{
		length = (*ppChild)->GetParticleLength();
// 		printd("Found length %f\n", length);
		if (length > result[0])
		{
			result[0] = length;
		} // if
	} // for
	result[1] = result[0];
	result[2] = result[0];
	return result;
} // GetSize

IParticles *pCreateParticleSystem( const ps_info &i_particleSystemInfo )
{
  CParticleSystem *pParticleSystem = new CParticleSystem();
  if( !pParticleSystem )
    return 0;

  if( !pParticleSystem->bInitialize( i_particleSystemInfo.pConfigFile,
    i_particleSystemInfo.rUpdatesPerSecond, i_particleSystemInfo.iRandomSeed,
    i_particleSystemInfo.rSynchronisationInterval, i_particleSystemInfo.iForwardThreshold ) )
  {
    delete pParticleSystem;
    return 0;
  }

  return pParticleSystem;
}

