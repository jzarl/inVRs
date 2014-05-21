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

#include "ParticleEmitter.h"
#include "ParticleSystem.h"

OSG_USING_NAMESPACE

CParticleEmitter::CParticleEmitter( CParticleSystem *i_pParent, CParticleType *i_pParticleType ) :
  m_pParent( i_pParent ), m_pParticleType( i_pParticleType ), m_rEmissionRemainder( 0.0f ),
  m_nextMovement( 0.0f, 0.0f, 0.0f )
{
  // set timeline defaults (which are not 0)
  m_emissionDirection.SetDefaultValue( Vec3f( 0.0f, 1.0f, 0.0f ) );
  m_emissionRate.SetDefaultValue( 10.0f );
  m_lifeTime.SetDefaultValue( 2.0f );
  m_size.SetDefaultValue( 1.0f );
  m_velocity.SetDefaultValue( 1.0f );
  m_color.SetDefaultValue( Vec3f( 1.0f, 0.0f, 1.0f ) );

  m_pDrawableParticles = Particles::create();
  beginEditCP( m_pDrawableParticles );
  {
    m_pDrawableParticles->setMode( Particles::ViewerQuads );
    m_pDrawableParticles->setDrawOrder( m_pParticleType->bGetAdditive() ? Particles::Any : Particles::BackToFront );
    m_pDrawableParticles->setDynamic( true );
    m_pDrawableParticles->setMaterial( m_pParticleType->pGetMaterial() );
    m_pDrawableParticles->setPositions( GeoPositions3f::create() );
    m_pDrawableParticles->setColors( GeoColors3f::create() );
  }
  endEditCP( m_pDrawableParticles );

  m_pNode = Node::create();
  addRefCP( m_pNode );
  beginEditCP( m_pNode );
  {
    m_pNode->setCore( m_pDrawableParticles );
	m_pNode->updateVolume();
    // set volume static to prevent constant update
    Volume &v = m_pNode->getVolume( false ).getInstance();
    v.setEmpty();
    v.extendBy( Pnt3f( 0, 0, 0 ) );
    v.extendBy( Pnt3f( 1, 1, 1 ) );
    v.setStatic();
    ((DynamicVolume&)m_pNode->getVolume()).instanceChanged();
  }
  endEditCP( m_pNode );
}

CParticleEmitter::~CParticleEmitter()
{
  subRefCP( m_pNode );

  if( m_pParticles )
    delete[] m_pParticles;

  if( m_pParticleType )
    delete m_pParticleType;
}

bool CParticleEmitter::bInitialize()
{
  m_iNumParticles = iEstimateMaximumParticles();

 if( m_iNumParticles <= 0 )
    return false;

  m_pParticles = new particle[m_iNumParticles];
  return m_pParticles != 0;
}

void CParticleEmitter::Update( Real32 i_rTime, Real32 i_rDeltaTime, bool i_bSimulate )
{
  const Real64 rInvDesiredUpdatesPerSecond = 1.0f / (Real64)m_pParent->rGetDesiredUpdatesPerSecond();
  m_rEmissionRemainder += rGetEmittedParticles( i_rTime ) * rInvDesiredUpdatesPerSecond;
  UInt32 iEmittedParticles = (UInt32)m_rEmissionRemainder;
  m_rEmissionRemainder -= iEmittedParticles;

  GeoPositionsPtr pPos = NullFC; GeoColorsPtr pCols = NullFC; MFVec3f *pSizes = 0;
  if( !i_bSimulate )
  {
    beginEditCP( m_pDrawableParticles );
    pPos = m_pDrawableParticles->getPositions(); beginEditCP( pPos );
    pCols = m_pDrawableParticles->getColors(); beginEditCP( pCols );
    pSizes = m_pDrawableParticles->getMFSizes();
    pPos->clear(); pCols->clear(); pSizes->clear();
  }

  particle *pCurParticle = m_pParticles;
  for( UInt32 i = 0; i < m_iNumParticles; ++i, ++pCurParticle )
  {
    pCurParticle->position += m_nextMovement;

    bool bParticleOkay = m_pParticleType->bUpdateParticle( *pCurParticle,
      i_rDeltaTime, pPos, pCols, pSizes );
    if( !bParticleOkay && iEmittedParticles > 0 )
    {
      bParticleOkay = bSpawnParticle( *pCurParticle, i_rTime, m_pParent->RandomSpawnPoint() );
      if( bParticleOkay )
      {
        m_pParticleType->bUpdateParticle( *pCurParticle, i_rDeltaTime, pPos, pCols, pSizes );
        --iEmittedParticles;
      }
    }
  }

  m_nextMovement = Vec3f( 0.0f, 0.0f, 0.0f ); // reset movement vector

  if( !i_bSimulate )
  {
    endEditCP( pCols ); endEditCP( pPos );
    endEditCP( m_pDrawableParticles );
  }
}

Vec3f CParticleEmitter::GetRandomDirection( Real32 i_rTime ) const
{
  const Vec3f baseDir = m_emissionDirection.GetValue( i_rTime );
  Real32 rPhi = atan2f( baseDir[1], baseDir[0] );
  Real32 rTheta = atan2f( sqrtf( baseDir[0] * baseDir[0] + baseDir[1] * baseDir[1] ), baseDir[2] );
  const Real32 rVariation = m_emissionDirectionVariation.GetValue( i_rTime ) * 0.5f * (Pi / 180.0f);

  rPhi += rVariation * m_pParent->rRandomReal();
  rTheta += rVariation * m_pParent->rRandomReal();

  return Vec3f( sinf( rTheta ) * cosf( rPhi ), sinf( rTheta ) * sinf( rPhi ), cosf( rTheta ) );
}

Real32 CParticleEmitter::rGetEmittedParticles( Real32 i_rTime ) const
{
  if( i_rTime > m_rLength )
  {
    if( !m_bLoop )
      return 0;
    else
      i_rTime = fmod( i_rTime, m_rLength );
  }
  return m_emissionRate.GetValue( i_rTime ) + m_emissionRateVariation.GetValue( i_rTime );
}

UInt32 CParticleEmitter::iEstimateMaximumParticles() const
{
  Real32 rMaxLifeTime = m_lifeTime.GetMaximum() + m_lifeTimeVariation.GetMaximum(); // secs
  Real32 rMaxEmissionRate = m_emissionRate.GetMaximum() + m_emissionRateVariation.GetMaximum(); // parts / sec
  return (UInt32)ceilf( rMaxEmissionRate * rMaxLifeTime ) * 2; // * 2 for looping animations
}

bool CParticleEmitter::bSpawnParticle( particle &o_particle, Real32 i_rTime, Pnt3f i_position ) const
{
  if( i_rTime > m_rLength )
  {
    if( !m_bLoop )
      return false;
    else
      i_rTime = fmod( i_rTime, m_rLength );
  }

  o_particle.rLifeTime = m_lifeTime.GetValue( i_rTime ) + m_lifeTimeVariation.GetValue( i_rTime ) * m_pParent->rRandomReal();
  o_particle.rTimeLived = 0.0f;

  o_particle.rSize0 = m_size.GetValue( i_rTime ) + m_sizeVariation.GetValue( i_rTime ) * m_pParent->rRandomReal();
  o_particle.velocity0 = GetRandomDirection( i_rTime ) * (m_velocity.GetValue( i_rTime ) + m_velocityVariation.GetValue( i_rTime ) * m_pParent->rRandomReal());

  o_particle.color0 = m_color.GetValue( i_rTime );
  Real32 rLink = m_colorVariationLink.GetValue( i_rTime );
  Vec3f variation = m_colorVariation.GetValue( i_rTime );
  if( rLink <= 0.0f )
  {
    o_particle.color0 += Vec3f( variation[0] * m_pParent->rRandomReal(), variation[1] * m_pParent->rRandomReal(), variation[2] * m_pParent->rRandomReal() );
  }
  else if( rLink < 1.0f )
  {
    Vec3f componentVariation( variation[0] * m_pParent->rRandomReal(), variation[1] * m_pParent->rRandomReal(), variation[2] * m_pParent->rRandomReal() );
    Vec3f linkedVariation = variation * m_pParent->rRandomReal();
    o_particle.color0 += componentVariation * (1.0f - rLink) + linkedVariation * rLink;
  }
  else
    o_particle.color0 += variation * m_pParent->rRandomReal();

  o_particle.position = i_position;

  return true;
}

float CParticleEmitter::GetParticleLength()
{
	return (m_velocity.GetMaximum()+m_velocityVariation.GetMaximum()) * (m_lifeTime.GetMaximum()+m_lifeTimeVariation.GetMaximum()) + (m_size.GetMaximum()+m_sizeVariation.GetMaximum())/2.0f;
} // getSize
