/*---------------------------------------------------------------------------*\
 *           interactive networked Virtual Reality system (inVRs)            *
 *                                                                           *
 *    Copyright (C) 2005-2009 by the Johannes Kepler University, Linz        *
 *                                                                           *
 *                            www.inVRs.org                                  *
 *                                                                           *
 *              contact: canthes@inVRs.org, rlander@inVRs.org                *
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This library is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU Library General Public License as published    *
 * by the Free Software Foundation, version 2.                               *
 *                                                                           *
 * This library is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public         *
 * License along with this library; if not, write to the Free Software       *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *                           Project: Avatara                                *
 *                                                                           *
 * The Avatara library was developed during a practical at the Johannes      *
 * Kepler University, Linz in 2005 by Helmut Garstenauer                     *
 * (helmut@digitalrune.com) and Martin Garstenauer (martin@digitalrune.com)  *
 *                                                                           *
\*---------------------------------------------------------------------------*/

//_____________________________________________________________
//
/// \file Texture.cpp
/// Texture Implementation.
///
/// Comments:
///
/// Header of TGA-File:
///
///   struct TGAHeader
///   {
///     char  idlength;
///     char  colourmaptype;
///     char  datatypecode;
///     short int colourmaporigin;
///     short int colourmaplength;
///     char  colourmapdepth;
///     short int x_origin;
///     short int y_origin;
///     short width;
///     short height;
///     char  bitsperpixel;
///     char  imagedescriptor;
///   };
//_____________________________________________________________
//


#include "avatara/Texture.h"
#include "avatara/Debug.h"
#include <stdio.h>
#include <iostream>
#include <memory.h>
#include <cstdlib>
#ifdef WIN32
  #include <windows.h>
#endif
#if defined(MACOS_X) || defined(__APPLE_CC__)
    #include <OpenGL/gl.h>
#else
    #include <GL/gl.h>
#endif

#ifdef CAVE
  #include <cave_ogl.h>
#endif

using namespace std;

namespace Avatara
{
  //-------------------------------------------------------
  /// Initializes empty texture object.
  //-------------------------------------------------------
  Texture::Texture()
  {
    width = 0;
    height = 0;
    pData = 0;
    textureID = 0;
  }


  //-------------------------------------------------------
  /// Frees resources.
  //-------------------------------------------------------
  Texture::~Texture()
  {
    UnLoad();
  }


  //-------------------------------------------------------
  /// Returns texture id.
  /// A texture id of 0 means that no texture was loaded
  /// in OpenGL.
  //-------------------------------------------------------
  unsigned int Texture::GetTextureID() const
  {
    return textureID;
  }


  //-------------------------------------------------------
  /// Loads texture from file.
  /// Foramt: uncompressed tga.
  ///
  /// \param fileName Path of texture file.
  /// \return True if success, otherwise false.
  //-------------------------------------------------------
  bool Texture::LoadUncompressedTGA(string fileName)
  {
    PRINT("Texture::LoadUncompressedTGA(\"" + fileName + "\") ");

    ASSERT(!fileName.empty());

    UnLoad();

    bool hasAlpha = false;

    //
    // open file
    //
    FILE* file = fopen(fileName.c_str(), "rb");
    if (file == 0)
      return false;

    //
    // read header
    //
    unsigned char pBuffer[18]; memset(pBuffer, 0, 18);
    fread(pBuffer, sizeof(unsigned char), 18, file);

    //
    // check DataTypeCode (2 = uncompressed RGB(A))
    //
    if (pBuffer[2] != 2)
    {
      cerr << "TGA file is not uncompressed RGB." << endl;
      fclose(file);
      return false;
    }

    //
    // get width and height
    //
    width = pBuffer[13]*256 + pBuffer[12];
    height = pBuffer[15]*256 + pBuffer[14];
    DEBUG(width)
    DEBUG(height)

    //
    // check BitsPerPixel (should be 24 for RGB or 32 for RGBA)
    //
    if(pBuffer[16] != 24 && pBuffer[16] != 32)
    {
      cerr << "BitsPerPixel is wrong." << endl;
      fclose(file);
      return false;
    }
    if(pBuffer[16] == 32) hasAlpha = true;

    //
    // allocate memory for Image (4 Bytes per Pixel)
    //
    pData = (unsigned char*) malloc(sizeof(unsigned char)*width*height*4);

    //
    // read image data
    //
    for (int i=0; i<width*height; i++)
    {
      //
      // read pixel (Pixel is stored in RGB(A) format in tga)
      //
      fread(&pData[i*4+0], sizeof(unsigned char), 1, file);
      fread(&pData[i*4+2], sizeof(unsigned char), 1, file);
      fread(&pData[i*4+1], sizeof(unsigned char), 1, file);
      if(hasAlpha)
        fread(&pData[i*4+3], sizeof(unsigned char), 1, file);
      else
        pData[i*4+3] = 255;
    }

    // close file
    fclose(file);

    PRINT("Texture::LoadUncompressedTGA() finished. ")
    return true;
  }


  //-------------------------------------------------------
  /// Builds texture in OpenGL.
  /// texture ID will be generated and stored.
  /// Min-Filter/Mag-Filter = linear.
  //-------------------------------------------------------
  void Texture::BuildGLTexture()
  {
    ASSERT(pData != 0);

    // Generate 1 texture id
    glGenTextures(1, &textureID);
    // Create OpenGL texture object
    glBindTexture(GL_TEXTURE_2D, textureID);
    // set filtering
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Set texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                GL_UNSIGNED_BYTE, pData);
  }


  //-------------------------------------------------------
  /// Frees resources.
  //-------------------------------------------------------
  void Texture::UnLoad()
  {
    if (pData != 0)
    {
      free(pData);
    }
  }

} // namespace Avatara
