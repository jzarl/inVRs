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

/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 * rlander(rlander@inVRs.org):                                               *
 *  Added statements for dll-support under windows                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

//_____________________________________________________________
//
/// \file Texture.h
/// Texture definition.
//_____________________________________________________________

#ifndef _DR_TEXTURE_H
  #define _DR_TEXTURE_H

#include <string>

#include "avatara/AvataraDllExports.h"

namespace Avatara
{

  //-------------------------------------------------------
  /// Contains Texture.
  /// Texture class can load uncompressed TGA images.
  //-------------------------------------------------------
  class AVATARA_API Texture
  {
  public:
    Texture();
    ~Texture();

    unsigned int GetTextureID() const;
    bool LoadUncompressedTGA(std::string fileName);
    void BuildGLTexture();
    void UnLoad();

  private:
    int width;                      ///< Width of image in pixels.
    int height;                     ///< Height of image in pixels.
    unsigned char* pData;           ///< Pointer to pixel data.
    unsigned int textureID;         ///< OpenGL texture id.
  };

} // namespace Avatara

#endif
