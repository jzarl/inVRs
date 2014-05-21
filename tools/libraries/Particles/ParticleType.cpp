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

#include "ParticleType.h"
#include "ParticleEmitter.h"
#include <OpenSG/OSGImage.h>
#include <OpenSG/OSGBlendChunk.h>

OSG_USING_NAMESPACE

CParticleType::CParticleType() : m_bTextureChanged( false )
{
  m_pMaterial = SimpleTexturedMaterial::create();
  addRefCP( m_pMaterial ); // keep internal copy

  // set timeline defaults
  m_sizeChange.SetDefaultValue( 1.0f );
  m_velocityChange.SetDefaultValue( 1.0f );
  m_colorChange.SetDefaultValue( Vec3f( 1.0f, 1.0f, 1.0f ) );
}

CParticleType::~CParticleType()
{
  subRefCP( m_pMaterial );
}

void CParticleType::RecreateTexture()
{
  beginEditCP( m_pMaterial );
  {
    BlendChunkPtr bc = BlendChunk::create();
    beginEditCP( bc );
    {
      if( m_bAdditive )
      {
        bc->setSrcFactor( GL_SRC_ALPHA );
        bc->setDestFactor( GL_ONE );
      }
      else
      {
        // TODO: nicht richtiges alpha-blending, aber gute ergebnisse bei partikel.
        // für alpha-blending COLOR mit ALPHA ersetzen.
//         bc->setSrcFactor( GL_SRC_COLOR );
//         bc->setDestFactor( GL_ONE_MINUS_SRC_COLOR );
         bc->setSrcFactor( GL_SRC_ALPHA );
         bc->setDestFactor( GL_ONE_MINUS_SRC_COLOR );
      }
    }
    endEditCP( bc );
    m_pMaterial->addChunk( bc );

    m_pMaterial->setLit( false );

    ImagePtr pTexture = Image::create();
    pTexture->read( m_sTexture.c_str() );
    m_pMaterial->setImage( pTexture );
    m_pMaterial->setEnvMode( GL_MODULATE );
  }
  endEditCP( m_pMaterial );
}

bool CParticleType::bUpdateParticle( particle &io_particle, Real32 i_rDeltaTime,
  GeoPositionsPtr i_pPos, GeoColorsPtr i_pCols, MFVec3f *i_pSizes ) const
{
  if( io_particle.bIsDead() )
    return false;

  Real32 rLivedFraction = io_particle.rTimeLived / io_particle.rLifeTime;
  io_particle.rTimeLived += i_rDeltaTime;

  if( i_pSizes != 0 )
  {
    Real32 rSize = io_particle.rSize0 * m_sizeChange.GetValue( rLivedFraction );
    i_pSizes->push_back( Vec3f( rSize, rSize, rSize ) );
  }

  Vec3f velocity = io_particle.velocity0 * m_velocityChange.GetValue( rLivedFraction );
  io_particle.position += velocity * i_rDeltaTime;
  if( i_pPos != NullFC )
    i_pPos->push_back( io_particle.position );

  if( i_pCols != NullFC )
  {
    Vec3f colorChange = m_colorChange.GetValue( rLivedFraction );
    i_pCols->push_back( Color3f( io_particle.color0[0] * colorChange[0],
      io_particle.color0[1] * colorChange[1], io_particle.color0[2] * colorChange[2] ) );
  }

  return true;
}
