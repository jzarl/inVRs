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

#ifndef __PARTICLE_TYPE_H__
#define __PARTICLE_TYPE_H__

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGBaseTypes.h>
#include <OpenSG/OSGGeoProperty.h>
#include <OpenSG/OSGSimpleTexturedMaterial.h>

#include "TimeLine.h"
#include <string>
using namespace std;

class CParticleType
{
public:
  CParticleType();
  ~CParticleType();

  bool bUpdateParticle( struct particle &io_particle, OSG::Real32 i_rDeltaTime,
    OSG::GeoPositionsPtr i_pPos, OSG::GeoColorsPtr i_pCols, OSG::MFVec3f *i_pSizes ) const;

private:
  void RecreateTexture();

public:
  inline void SetTexture( char *i_pTexture ) { m_bTextureChanged = true; m_sTexture = i_pTexture; }
  inline void SetAdditive( bool i_bAdditive ) { m_bTextureChanged = true; m_bAdditive = i_bAdditive; }

  inline const char *pGetTexture() const { return m_sTexture.c_str(); }
  inline bool bGetAdditive() const { return m_bAdditive; }

  inline OSG::SimpleTexturedMaterialPtr pGetMaterial() { if( m_bTextureChanged ) RecreateTexture(); return m_pMaterial; }

  inline CTimeline<OSG::Real32> &GetSizeChange() { return m_sizeChange; }
  inline CTimeline<OSG::Real32> &GetVelocityChange() { return m_velocityChange; }
  inline CTimeline<OSG::Vec3f>  &GetColorChange() { return m_colorChange; }

private:
  string  m_sTexture;
  bool    m_bAdditive, m_bTextureChanged;

  OSG::SimpleTexturedMaterialPtr m_pMaterial;

  CTimeline<OSG::Real32> m_sizeChange;
  CTimeline<OSG::Real32> m_velocityChange;
  CTimeline<OSG::Vec3f>  m_colorChange;
};

#endif // __PARTICLE_TYPE_H__
