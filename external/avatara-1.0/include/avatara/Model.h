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
/// \file Model.h
/// Definitions for Vertex, Color, TextureCoord, Face, Model.
//_____________________________________________________________

#ifndef _DR_MODEL_H
  #define _DR_MODEL_H

#include "avatara/Vertex.h"
#include "avatara/Skeleton.h"


namespace Avatara
{
  //-------------------------------------------------------
  /// Type of shading: flat or smooth.
  //-------------------------------------------------------
  enum ShadingStyle { Flat = 0, Smooth = 1 };


  //-------------------------------------------------------
  /// Color (RGBA).
  //-------------------------------------------------------
  struct Color
  {
    unsigned char r,	///< red (0...255)
		g,				///< green (0...255)
		b,				///< blue (0...255)
		a;				///< alpha (0...255)
  };


  //-------------------------------------------------------
  /// Texture coordinates.
  //-------------------------------------------------------
  struct TextureCoord
  {
    float u,			///< texture coordinate u
		v;				///< texture coordinate v
  };


  //-------------------------------------------------------
  /// Trilateral face structure.
  /// A face is always a triangle.
  //-------------------------------------------------------
  struct AVATARA_API Face
  {
    int pVertexIndices[3];          ///< Indices of vertices.
    ShadingStyle shading;           ///< Type of shading.
    Algebra::Vector normal;         ///< Normal vector.
    Color pColors[3];               ///< Vertex colors.
    TextureCoord pTextureCoords[3]; ///< Texture coordinates of vertices.
  };


  //-------------------------------------------------------
  /// Contains model (vertices and faces) with skeleton.
  /// The skeleton is optional.
  //-------------------------------------------------------
  class AVATARA_API Model
  {
  public:
    Model();
    ~Model();

    bool Load(std::string fileName);

    int GetNoOfVertices() const;
    const Vertex& GetVertex(int index) const;
    int GetNoOfFaces() const;
    const Face& GetFace(int index) const;
    const Skeleton& GetSkeleton() const;
    int GetVersion() const;

  private:
    Model(const Model&);            // forbid use of copy constructor
    Model& operator=(const Model&); // forbid use of assignment operator

    bool LoadBones(FILE* file, int noOfBones);
    void UnLoad();

    int version;          ///< Version of model file format.
    int noOfVertices;     ///< Number of vertices.
    Vertex* pVertices;    ///< Array with vertices.
    int noOfFaces;        ///< Number of faces.
    Face* pFaces;         ///< Array of faces.
    Skeleton skeleton;    ///< Skeleton object.
  };

} // namespace Avatara

#endif
