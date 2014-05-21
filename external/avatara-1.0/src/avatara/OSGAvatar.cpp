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
/// \file OSGAvatar.cpp
/// Implementation of avatar for OpenSG.
//_____________________________________________________________

#include "avatara/OSGAvatar.h"
#include "avatara/Debug.h"
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGBaseFunctions.h>
#include <OpenSG/OSGSimpleMaterial.h>
#include <OpenSG/OSGSimpleTexturedMaterial.h>

#if OSG_MAJOR_VERSION >= 2
#include <OpenSG/OSGTypedGeoVectorProperty.h>
#include <OpenSG/OSGTypedGeoIntegralProperty.h>
#endif

OSG_USING_NAMESPACE;
using namespace Algebra;


namespace Avatara
{

	//-------------------------------------------------------
	/// Initializes empty avatar object.
	//-------------------------------------------------------
	OSGAvatar::OSGAvatar()
	{
	}


	//-------------------------------------------------------
	/// Cleans up resources.
	//-------------------------------------------------------
	OSGAvatar::~OSGAvatar()
	{
	}


	//-------------------------------------------------------
	/// Builds an OpenSG geometry of the avatar.
	///
	/// \param image IN: Avatar texture.
	/// \return OpenSG geometry.
	//-------------------------------------------------------
#if OSG_MAJOR_VERSION >= 2
	OSG::GeometryRecPtr OSGAvatar::MakeAvatarGeo(OSG::ImageRecPtr image)
#else //OpenSG1:
		OSG::GeometryPtr OSGAvatar::MakeAvatarGeo(OSG::ImagePtr image)
#endif
		{
			ASSERT(pModel != NULL);

			//
			// ----- type and length attributes
			//
			// Avatar consists of triangles only.
			// 3 vertices per face (triangle).
#if OSG_MAJOR_VERSION >= 2
			OSG::GeoUInt8PropertyRecPtr type = OSG::GeoUInt8Property::create();
			OSG::GeoUInt32PropertyRecPtr lens = OSG::GeoUInt32Property::create();

			type->addValue(GL_TRIANGLES);
			lens->addValue(3*pModel->GetNoOfFaces());
#else //OpenSG1:
			GeoPTypesPtr type = GeoPTypesUI8::create();
			GeoPLengthsPtr lens = GeoPLengthsUI32::create();
			beginEditCP(type, GeoPTypesUI8::GeoPropDataFieldMask);
			beginEditCP(lens, GeoPLengthsUI32::GeoPropDataFieldMask);
			{
				type->push_back(GL_TRIANGLES);
				lens->push_back(3*pModel->GetNoOfFaces());
			}
			endEditCP  (lens, GeoPLengthsUI32::GeoPropDataFieldMask);
			endEditCP  (type, GeoPTypesUI8::GeoPropDataFieldMask);
#endif

			//
			// ----- vertex positions
			//
			// One position per vertex is stored.
#if OSG_MAJOR_VERSION >= 2
			GeoPnt3fPropertyRecPtr pnts = GeoPnt3fProperty::create();
#else //OpenSG1:
			GeoPositions3fPtr pnts = GeoPositions3f::create();
			beginEditCP(pnts, GeoPositions3f::GeoPropDataFieldMask);
#endif
			{
				for(int i=0; i<pModel->GetNoOfVertices(); i++)
				{
					Algebra::Vector pnt = pModel->GetVertex(i).Position();
#if OSG_MAJOR_VERSION >= 2
					pnts->addValue(Pnt3f(pnt.X(), pnt.Y(), pnt.Z()));
#else //OpenSG1:
					pnts->push_back(Pnt3f(pnt.X(), pnt.Y(), pnt.Z()));
#endif
				}
			}
#if OSG_MAJOR_VERSION >= 2
#else //OpenSG1:
			endEditCP  (pnts, GeoPositions3f::GeoPropDataFieldMask);
#endif

			//
			// ----- color, normal vectors, texture coordinates
			//
#if OSG_MAJOR_VERSION >= 2
			GeoColor3fPropertyRecPtr colors = GeoColor3fProperty::create();
			GeoVec3fPropertyRecPtr norms = GeoVec3fProperty::create();
			GeoVec2fPropertyRecPtr tex = GeoVec2fProperty::create();
#else //OpenSG1:
			GeoColors3fPtr colors = GeoColors3f::create();
			GeoNormals3fPtr norms = GeoNormals3f::create();
			GeoTexCoords2fPtr tex = GeoTexCoords2f::create();
			beginEditCP(colors, GeoColors3f::GeoPropDataFieldMask);
			beginEditCP(norms, GeoNormals3f::GeoPropDataFieldMask);
			beginEditCP(tex, GeoTexCoords2f::GeoPropDataFieldMask);
#endif
			{
				for(int i=0; i<pModel->GetNoOfFaces(); i++)
				{
					Face face = pModel->GetFace(i);
					for (int j=0; j<3; j++)
					{
						Color col = face.pColors[j];
#if OSG_MAJOR_VERSION >= 2
						colors->addValue(Color3f(col.r, col.g, col.b));
#else //OpenSG1:
						colors->push_back(Color3f(col.r, col.g, col.b));
#endif

						Algebra::Vector normal;
						if(face.shading == Smooth)
							normal = pModel->GetVertex(face.pVertexIndices[j]).Normal();
						else
							normal = face.normal;
						normal.Normalize();
#if OSG_MAJOR_VERSION >= 2
						norms->addValue(Vec3f(normal.X(), normal.Y(), normal.Z()));
						tex->addValue(Vec2f(face.pTextureCoords[j].u, face.pTextureCoords[j].v));
#else //OpenSG1:
						norms->push_back(Vec3f(normal.X(), normal.Y(), normal.Z()));
						tex->push_back(Vec2f(face.pTextureCoords[j].u, face.pTextureCoords[j].v));
#endif
					}
				}
			}
#if OSG_MAJOR_VERSION >= 2
#else //OpenSG1:
			endEditCP  (tex, GeoTexCoords2f::GeoPropDataFieldMask);
			endEditCP  (norms, GeoNormals3f::GeoPropDataFieldMask);
			endEditCP  (colors, GeoPositions3f::GeoPropDataFieldMask);
#endif

			//
			// ----- index list
			//
			// 2 indices per vertex. First index is for positions. Second index is for
			// color, normal vectors and texture coords.
			// 3 vertices per face.
#if OSG_MAJOR_VERSION >= 2
			GeoUInt32PropertyRecPtr index1 = GeoUInt32Property::create();
			GeoUInt32PropertyRecPtr index2 = GeoUInt32Property::create();
#else //OpenSG1:
			GeoIndicesUI32Ptr indices = GeoIndicesUI32::create();
			beginEditCP(indices, GeoIndicesUI32::GeoPropDataFieldMask);
#endif
			{
				int i=0;
				for (i=0; i<pModel->GetNoOfFaces(); i++)
				{
					Face face = pModel->GetFace(i);
					int j=0;
					for (j=0; j<3; j++)
					{
#if OSG_MAJOR_VERSION >= 2
						index1->addValue(face.pVertexIndices[j]); // position index
						index2->addValue(i*3+j);                  // color, normal, texture index
#else //OpenSG1:
						indices->push_back(face.pVertexIndices[j]); // position index
						indices->push_back(i*3+j);                  // color, normal, texture index
#endif
					}
				}
			}
#if OSG_MAJOR_VERSION >= 2
#else //OpenSG1:
			endEditCP  (indices, GeoIndicesUI32::GeoPropDataFieldMask);
#endif

			//
			// ----- Textured material
			//
#if OSG_MAJOR_VERSION >= 2
			SimpleTexturedMaterialRecPtr mat = SimpleTexturedMaterial::create();
#else //OpenSG1:
			SimpleTexturedMaterialPtr mat = SimpleTexturedMaterial::create();
			beginEditCP(mat);
			{
#endif
				mat->setAmbient      (Color3f(0.3,0.3,0.3));
				mat->setDiffuse      (Color3f(0.8,0.8,0.8));
				mat->setEmission     (Color3f(0.0,0.0,0.0));
				mat->setSpecular     (Color3f(0.8,0.8,0.8));
				mat->setShininess    (20);
				mat->setTransparency (0);
				mat->setColorMaterial(GL_NONE);
				mat->setImage        (image);
				mat->setMinFilter    (GL_LINEAR);
				mat->setMagFilter    (GL_LINEAR);
				mat->setEnvMode      (GL_MODULATE);
				mat->setEnvMap       (false);
#if OSG_MAJOR_VERSION >= 2
#else //OpenSG1:
			}
			endEditCP  (mat);
#endif

			//
			// ----- create geometry
			//
			geo = Geometry::create();
#if OSG_MAJOR_VERSION >= 2
#else //OpenSG1:
			beginEditCP(geo);
#endif
			{
				geo->setTypes    (type);
				geo->setLengths  (lens);
#if OSG_MAJOR_VERSION >= 2
				geo->setProperty(pnts,OSG::Geometry::PositionsIndex);
				geo->setIndex(index1,Geometry::PositionsIndex);

				geo->setProperty(colors,OSG::Geometry::ColorsIndex);
				geo->setIndex(index2,Geometry::ColorsIndex);

				geo->setProperty(norms,OSG::Geometry::NormalsIndex);
				geo->setIndex(index2,Geometry::NormalsIndex);

				geo->setProperty(tex,OSG::Geometry::TexCoordsIndex);
				geo->setIndex(index2,Geometry::TexCoordsIndex);
#else //OpenSG1:
				geo->setPositions(pnts);
				geo->setColors   (colors);
				geo->setNormals  (norms);
				geo->setTexCoords(tex);
				geo->setIndices  (indices);
				geo->getIndexMapping().push_back(Geometry::MapPosition);
				geo->getIndexMapping().push_back(Geometry::MapColor  |
						Geometry::MapNormal |
						Geometry::MapTexCoords);
#endif
				//geo->setMaterial (getDefaultMaterial());
				geo->setMaterial (mat);

				// turn of display list caching
				geo->setDlistCache(false);
			}
#if OSG_MAJOR_VERSION >= 2
			OSG::commitChanges();
#else //OpenSG1:
			endEditCP  (geo);
#endif

			return geo;
		}


	//-------------------------------------------------------
	/// Updates avatar geometry to a new pose.
	//-------------------------------------------------------
	void OSGAvatar::UpdateAvatarGeo()
	{
		// get position vector from the geometry
#if OSG_MAJOR_VERSION >= 2
		GeoVectorProperty* pos = geo->getPositions();
#else //OpenSG1:
		GeoPositionsPtr pos = geo->getPositions();
#endif

#if OPENSG_GEOPROPERTY_HAVE_SIZE_FUNCTION
		ASSERT(pModel->GetNoOfVertices() == pos->size());
#else
		ASSERT(pModel->GetNoOfVertices() == pos->getSize());
#endif

		// Calculate the new pose of the animation
		CalculatePose();

		// discard the old positions and add the new skinned positions
#if OSG_MAJOR_VERSION >= 2
#else //OpenSG1:
		beginEditCP(pos);
#endif
#if OPENSG_GEOPROPERTY_HAVE_SIZE_FUNCTION
		for(UInt32 i = 0; i < pos->size(); i++)
#else
		for(UInt32 i = 0; i < pos->getSize(); i++)
#endif
		{
			Algebra::Vector pnt = SkinVertex(pModel->GetVertex(i));
			Pnt3f p(pnt.X(), pnt.Y(), pnt.Z());
			pos->setValue(p, i);
		}
#if OSG_MAJOR_VERSION >= 2
		OSG::commitChanges();
#else //OpenSG1:
		endEditCP  (pos);

		// right now the geometry doesn't notice changes to the properties, it has
		// to be notified explicitly
		beginEditCP(geo, Geometry::PositionsFieldMask);
		endEditCP  (geo, Geometry::PositionsFieldMask);
#endif
	}
}

