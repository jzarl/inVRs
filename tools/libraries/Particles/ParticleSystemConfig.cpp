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

#include "ParticleSystemConfig.h"
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGGeoProperty.h>

OSG_USING_NAMESPACE

#include <png.h>

#include <iostream>
using namespace std;

// #ifdef BIGENDIAN
// // MeJ for strtof
// // MeJ  #include <stdlib.h> is not sufficient
// extern "C" float strtof (const char *nptr, char **endptr);
// #endif

#include <inVRs/SystemCore/DataTypes.h>
#include <inVRs/SystemCore/XMLTools.h>
#include <inVRs/SystemCore/DebugOutput.h>
#include "ParticleSystem.h"
#include "ParticleEmitter.h"
#include "ParticleType.h"

static char g_szCurrentSubDir[256];
static char effectPath[256];
#define NUM_REAL32_TIMELINES 10

/******* Forward Declarations *******/

Real32 calculateSceneLength( CParticleEmitter *i_pEmitter );
bool convertImageToColorTimeline( CTimeline<Vec3f> &o_ColorTimeline, char *i_pFilename, Real32 i_rLength );
static int check_if_png(char *file_name, FILE **fp);
static png_byte *pReadPNGFile( char *i_pFilename, UInt32 &o_iWidth, UInt32 &o_iHeight );

/******* Particle-System Config *******/

bool CParticleSystemConfig::parseEmission(IrrXMLReader* xml, CParticleEmitter* pEmitter)
{
	bool finished = false;
	float timestampTime;
	float value, variation;
	Vec3f vector;
	bool colorLinked;

	while(!finished && xml && xml->read()) {
		switch (xml -> getNodeType()) {
			case EXN_ELEMENT:
				if (!strcmp("timestamp", xml->getNodeName()))
				{
					timestampTime = xml->getAttributeValueAsFloat("time");
				} // if
				else if (!strcmp("direction", xml->getNodeName()))
				{
					vector[0] = xml->getAttributeValueAsFloat("x");
					vector[1] = xml->getAttributeValueAsFloat("y");
					vector[2] = xml->getAttributeValueAsFloat("z");
					pEmitter->GetEmissionDirection().SetValue( timestampTime, vector);
				} // if
				else if (!strcmp("dispersion", xml->getNodeName()))
				{
					value = 0;
					if (xml->getAttributeValue("value"))
						value = xml->getAttributeValueAsFloat("value");
					pEmitter->GetEmissionDirectionVariation().SetValue(timestampTime, value);
				} // if
				else if (!strcmp("colorvar", xml->getNodeName()))
				{
					colorLinked = false;
					vector[0] = xml->getAttributeValueAsFloat("R");
					vector[1] = xml->getAttributeValueAsFloat("G");
					vector[2] = xml->getAttributeValueAsFloat("B");
					pEmitter->GetColorVariation().SetValue(timestampTime, vector);
					if (xml->getAttributeValue("linked"))
					{
						convert (xml->getAttributeValue("linked"), colorLinked);
					} // if
					pEmitter->GetColorVariationLink().SetValue(timestampTime, colorLinked ? 1.0f : 0.0f);
				} // if
				else if (!strcmp("lifetime", xml->getNodeName()))
				{
					value = 0;
					variation = 0;
					if (xml->getAttributeValue("value"))
						value = xml->getAttributeValueAsFloat("value");
					if (xml->getAttributeValue("variation"))
						variation = xml->getAttributeValueAsFloat("variation");
					pEmitter->GetLifeTime().SetValue(timestampTime, value);
					pEmitter->GetLifeTimeVariation().SetValue(timestampTime, variation);
				} // else if
				else if (!strcmp("rate", xml->getNodeName()))
				{
					value = 0;
					variation = 0;
					if (xml->getAttributeValue("value"))
						value = xml->getAttributeValueAsFloat("value");
					if (xml->getAttributeValue("variation"))
						variation = xml->getAttributeValueAsFloat("variation");
					pEmitter->GetEmissionRate().SetValue(timestampTime, value);
					pEmitter->GetEmissionRateVariation().SetValue(timestampTime, variation);
				} // else if
				else if (!strcmp("size", xml->getNodeName()))
				{
					value = 0;
					variation = 0;
					if (xml->getAttributeValue("value"))
						value = xml->getAttributeValueAsFloat("value");
					if (xml->getAttributeValue("variation"))
						variation = xml->getAttributeValueAsFloat("variation");
					pEmitter->GetSize().SetValue(timestampTime, value);
					pEmitter->GetSizeVariation().SetValue(timestampTime, variation);
				} // else if
				else if (!strcmp("velocity", xml->getNodeName()))
				{
					value = 0;
					variation = 0;
					if (xml->getAttributeValue("value"))
						value = xml->getAttributeValueAsFloat("value");
					if (xml->getAttributeValue("variation"))
						variation = xml->getAttributeValueAsFloat("variation");
					pEmitter->GetVelocity().SetValue(timestampTime, value);
					pEmitter->GetVelocityVariation().SetValue(timestampTime, variation);
				} // else if
				break;
			case EXN_ELEMENT_END:
				if (!strcmp("emission", xml->getNodeName()))
				{
					finished = true;
				} // if
				break;
			default:
				break;
		} // switch
	} // while
	
	return finished;
} // parseEmission

CParticleType* CParticleSystemConfig::parseParticle(IrrXMLReader* xml, CParticleSystem *o_pParent, CParticleType* particle)
{
	bool finished = false;
	float timestampTime, timestampSize, timestampVelocity;

	while(!finished && xml && xml->read()) {
		switch (xml -> getNodeType()) {
			case EXN_ELEMENT:
				if (!strcmp("timestamp", xml->getNodeName()))
				{
					timestampTime = xml->getAttributeValueAsFloat("time");
				} // if
				else if (!strcmp("size", xml->getNodeName()))
				{
					timestampSize = xml->getAttributeValueAsFloat("value");
					particle->GetSizeChange().SetValue(timestampTime, timestampSize);
				} // else if
				else if (!strcmp("velocity", xml->getNodeName()))
				{
					timestampVelocity = xml->getAttributeValueAsFloat("value");
					particle->GetVelocityChange().SetValue(timestampTime, timestampVelocity);
				} // else if
				break;
			case EXN_ELEMENT_END:
				if (!strcmp("particle", xml->getNodeName()))
				{
					finished = true;
				} // if
				break;
			default:
				break;
		} // switch
	} // while
	
	return particle;
} // parseParticle


CParticleEmitter* CParticleSystemConfig::parseEmitter(IrrXMLReader* xml, CParticleSystem *o_pParent, bool emitterLoop)
{
	bool finished = false;
	std::string colorBase, colorCourse;
	CParticleEmitter* emitter = NULL;
	CParticleType* particle = NULL;
	bool particleAdditive;
	std::string particleTexture;

	while(!finished && xml && xml->read()) {
		switch (xml -> getNodeType()) {
			case EXN_ELEMENT:
				if (!strcmp("color", xml->getNodeName()))
				{
					colorBase = xml->getAttributeValue("base");
					colorCourse = xml->getAttributeValue("course");
				} // if
				else if (!strcmp("particle", xml->getNodeName()))
				{
					particleTexture = xml->getAttributeValue("texture");
					convert(xml->getAttributeValue("additive"), particleAdditive);
					particle = new CParticleType();
					particle->SetAdditive(particleAdditive);
					particle->SetTexture((char*)(effectPath + particleTexture).c_str());
					parseParticle(xml, o_pParent, particle);
					if (!particle)
					{
						printd(ERROR,"CParticleSystemConfig::parseEmitter(): ERROR WHILE PARSING PARTICLE!\n");
						return NULL;
					} // if
					emitter = new CParticleEmitter(o_pParent, particle);
					emitter->SetLoop(emitterLoop);
				} // else if
				else if (!strcmp("emission", xml->getNodeName()))
				{
					parseEmission(xml, emitter);
				} // else if
				break;
			case EXN_ELEMENT_END:
				if (!strcmp("emitter", xml->getNodeName()))
				{
					emitter->SetLength(calculateSceneLength(emitter));
					assert(convertImageToColorTimeline(emitter->GetColor(), (char*)(effectPath + colorBase).c_str(), emitter->rGetLength()));
					assert(convertImageToColorTimeline(emitter->pGetParticleType()->GetColorChange(), (char*)(effectPath + colorCourse).c_str(), 1.0f));
					assert(emitter->bInitialize());
					finished = true;
				} // if
				break;
			default:
				break;
		} // switch
	} // while
	
	return emitter;	
} // parseEmitter

bool CParticleSystemConfig::parseParticleOrigin(IrrXMLReader* xml, CParticleSystem *o_pParent)
{
	bool finished = false;
	CParticleEmitter* emitter = NULL;
	std::string emitterType;
	bool emitterLoop;

	while(!finished && xml && xml->read()) {
		switch (xml -> getNodeType()) {
			case EXN_ELEMENT:
				if (!strcmp("emitter", xml->getNodeName()))
				{
					emitterType = xml->getAttributeValue("type");
					convert(xml->getAttributeValue("loop"), emitterLoop);
					emitter = parseEmitter(xml, o_pParent, emitterLoop);
					if (!emitter)
					{
						printd(ERROR, "CParticleSystemConfig::parseParticleOrigin(): ERROR WHILE PARSING PARTICLE EMITTER!\n");
						return false;
					} // if
					o_pParent->AddChild(emitter);
				} // if
				break;
			case EXN_ELEMENT_END:
				if (!strcmp("origin", xml->getNodeName()))
				{
					finished = true;
				} // if
				break;
			default:
				break;
		} // switch
	} // while
	
	return finished;
} // parseParticleOrigin


bool CParticleSystemConfig::bLoadParticleEmitters( const char *i_pConfigFile, CParticleSystem *o_pParent )
{
	IrrXMLReader* xml;
	bool result = true;
	strcpy(effectPath, i_pConfigFile);
	char* lastSlash = strrchr(effectPath, '/');
	*(lastSlash+1) = '\0';
	std::string cfgFile(i_pConfigFile);

	printd(INFO, "CParticleSystemConfig::bLoadParticleEmitters(): Trying to load Particles of file %s\n", i_pConfigFile);
	if (!fileExists(cfgFile))
	{
		printd(ERROR, "CParticleSystemConfig::bLoadParticleEmitters(): ERROR: could not find config-file %s\n", i_pConfigFile);
		return false;
	} // if

	xml = createIrrXMLReader(i_pConfigFile);

	while(xml && xml->read()) {
		switch (xml -> getNodeType()) {
			case EXN_ELEMENT:
				if (!strcmp("origin", xml->getNodeName())) 
				{
					o_pParent->SetRadius(xml->getAttributeValueAsFloat("size"));
					result = result | parseParticleOrigin(xml, o_pParent);
				} // if
				break;
			case EXN_ELEMENT_END:
				break;
			default:
				break;
		} // switch
	} // while
	delete xml;

	return true;
}

Real32 calculateSceneLength( CParticleEmitter *i_pEmitter )
{
  CTimeline<Real32> *pReal32Timelines[NUM_REAL32_TIMELINES] = {
    // Real32 Emitter Timelines
    &i_pEmitter->GetEmissionDirectionVariation(),
    &i_pEmitter->GetEmissionRate(),
    &i_pEmitter->GetEmissionRateVariation(),
    &i_pEmitter->GetLifeTime(),
    &i_pEmitter->GetLifeTimeVariation(),
    &i_pEmitter->GetColorVariationLink(),
    &i_pEmitter->GetVelocity(),
    &i_pEmitter->GetVelocityVariation(),
    &i_pEmitter->GetSize(),
    &i_pEmitter->GetSizeVariation()
  };

  Real32 fSceneLength = 0, fCurrent;

  for (int i = 0; i < NUM_REAL32_TIMELINES; i++)
  {
    if ((fCurrent = pReal32Timelines[i]->rGetLength()) > fSceneLength)
      fSceneLength = fCurrent;
  }

  if ((fCurrent = i_pEmitter->GetEmissionDirection().rGetLength()) > fSceneLength)
    fSceneLength = fCurrent;
  if ((fCurrent = i_pEmitter->GetColorVariation().rGetLength()) > fSceneLength)
    fSceneLength = fCurrent;

  return fSceneLength;
}

bool convertImageToColorTimeline( CTimeline<Vec3f> &o_ColorTimeline, char *i_pFilename, Real32 i_rLength )
{
  UInt32 iWidth, iHeight;
  png_byte *pImage = pReadPNGFile( i_pFilename, iWidth, iHeight );
  if( !pImage || iWidth == 0 || iHeight == 0 )
    return false;

  Real32 rTimeStep = (iWidth == 1) ? 0.0f : i_rLength / (Real32)(iWidth - 1);

  Real32 rCurTime = 0.0f; png_byte *pCurPixel = pImage;
  for( UInt32 x = 0; x < iWidth; ++x, rCurTime += rTimeStep )
  {
    Vec3f vColor;
    vColor[0] = (Real32)*pCurPixel++ / 255.0f;
    vColor[1] = (Real32)*pCurPixel++ / 255.0f;
    vColor[2] = (Real32)*pCurPixel++ / 255.0f;
    o_ColorTimeline.SetValue( rCurTime, vColor );
  }

  delete[] pImage;
  return true;
}

/******************/

#define PNG_BYTES_TO_CHECK 4
static int check_if_png(char *file_name, FILE **fp)
{
   png_byte buf[PNG_BYTES_TO_CHECK];

   /* Open the prospective PNG file. */
   if ((*fp = fopen(file_name, "rb")) == NULL)
      return 0;

   /* Read in some of the signature bytes */
   if (fread(buf, 1, PNG_BYTES_TO_CHECK, *fp) != PNG_BYTES_TO_CHECK)
      return 0;

   /* Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
      Return nonzero (true) if they match */

   return(!png_sig_cmp(buf, (png_size_t)0, PNG_BYTES_TO_CHECK));
}

static png_byte *pReadPNGFile( char *i_pFilename, UInt32 &o_iWidth, UInt32 &o_iHeight )
{
  png_structp pPNG = png_create_read_struct( PNG_LIBPNG_VER_STRING, 0, 0, 0 );
  if( !pPNG )
    return 0;

  png_infop pInfo = png_create_info_struct( pPNG );
  if( !pInfo )
  {
    png_destroy_read_struct( &pPNG, (png_infopp)0, (png_infopp)0 );
    return 0;
  } 

  if( setjmp( png_jmpbuf( pPNG ) ) )
  {
    png_destroy_read_struct( &pPNG, &pInfo, (png_infopp)0 );
    return 0;
  } 

  FILE *pFile = 0;
  if( !check_if_png( i_pFilename, &pFile ) )
  {
    if( pFile != 0 ) fclose( pFile );
    png_destroy_read_struct( &pPNG, &pInfo, (png_infopp)0 );
    return 0;
  }

  png_init_io( pPNG, pFile );
  png_set_sig_bytes( pPNG, PNG_BYTES_TO_CHECK );
  png_read_info( pPNG, pInfo );

  // the following code converts the image to 24-bit RGB format
  {
    if( png_get_bit_depth( pPNG, pInfo ) == 16 )
      png_set_strip_16( pPNG ); // force 8-bits per color channel

    png_set_expand( pPNG ); // expand to 24-bit RGB / 32-bit RGBA format

    Int32 iColorType = png_get_color_type( pPNG, pInfo );
    if( iColorType == PNG_COLOR_TYPE_GRAY || iColorType == PNG_COLOR_TYPE_GRAY_ALPHA )
      png_set_gray_to_rgb( pPNG ); // convert grayscale to color format

    if( iColorType & PNG_COLOR_MASK_ALPHA )
         png_set_strip_alpha( pPNG ); // eliminate alpha channel

    png_read_update_info( pPNG, pInfo );
  }

  o_iWidth = png_get_image_width( pPNG, pInfo );
  o_iHeight = png_get_image_height( pPNG, pInfo );

  png_byte *pData = new png_byte[ o_iHeight * png_get_rowbytes( pPNG, pInfo ) ];
  if( pData )
  {
    png_byte *pCurData = pData;
    png_byte **ppRows = new png_byte *[o_iHeight];
    if( ppRows )
    {
      for( UInt32 i = 0; i < o_iHeight; ++i )
      {
        ppRows[i] = pCurData;
        pCurData += png_get_rowbytes( pPNG, pInfo );
      }

      png_read_image( pPNG, ppRows );
      delete[] ppRows;
    }
  }
  
  png_destroy_read_struct( &pPNG, &pInfo, (png_infopp)0 );

  fclose( pFile );
  return pData;
}

/*********************************************************************EOF***/
