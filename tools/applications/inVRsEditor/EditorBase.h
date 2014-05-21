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
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#ifndef _EDITORBASE_H
#define _EDITORBASE_H

#include "EditorDataTypes.h"
#include "ConfigCreator.h"

#include <sstream>
#include <string>
#include <vector>

#include <gmtl/AxisAngle.h>

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleSceneManager.h>

#include <inVRs/SystemCore/SystemCore.h>
#include <inVRs/SystemCore/WorldDatabase/WorldDatabase.h>
#include <inVRs/SystemCore/TransformationManager/TransformationManager.h>
#include <inVRs/SystemCore/UtilityFunctions.h>

class EditorBase{
public:
  EditorBase();
  ~EditorBase();
		
  bool loadConfiguration(std::string filename);
  
	void newConfiguration(std::string invrsPath, std::string projectPath);
  void newEntity(int id);
  void newEntityType(std::string name);
  bool newEnvironment(int spacingX, int spacingZ, int sizeX, int sizeZ);

	void deleteSelection();

  void saveConfiguration();

	void addEntryPoint(gmtl::Vec3f position, gmtl::Vec3f direction);
	void deleteEntryPoint(unsigned index);

	void setSelection(Selection *value);

  void setSelectionTranslation(gmtl::Vec3f value);
  void setSelectionRotation(gmtl::AxisAnglef value);
  void setSelectionScale(gmtl::Vec3f value);

  void setSimpleSceneManagerMain(osg::SimpleSceneManager* simpleSceneManager);
  void setSimpleSceneManagerPreview(osg::SimpleSceneManager* simpleSceneManager);

	void setPositionX(int value);
	void setPositionZ(int value);
	void setFixed(bool value);
	void setRepCopy(bool value);
	void setName(std::string value);
	void setModelFilename(std::string value);
	void setXmlFilename(std::string value);

  osg::NodePtr getRootNode();

	std::vector<Tile*> getTileList();
  std::vector<EntityType*> getEntityTypeList();
  std::vector<Environment*> getEnvironmentList();
  
	EntityType *getEntityType(unsigned short id);
  Entity *getEntity(unsigned short id);
	Environment *getEnvironment(unsigned short id);

  Selection* getSelection();
  
	gmtl::Vec3f getSelectionTranslation();
  gmtl::AxisAnglef getSelectionRotation();
  gmtl::Vec3f getSelectionScale();
 
	bool getFixed();
	bool getRepCopy();
	std::string getName();
	int getPositionX();
	int getPositionZ();
	std::string getModelFilename();
	std::vector<std::string> getModelFilenames(std::string kind);
	std::string getXmlFilename();
protected:
  void update();

  void highlightObject();
  void highlightChanged();
  void updateHighlight();
  void clearHighlight();

  void showPreview();

	osg::NodePtr mRoot;
  osg::SimpleSceneManager *mSimpleSceneManagerMain;
  osg::SimpleSceneManager *mSimpleSceneManagerPreview;
  Selection *mSelection;

  std::vector<osg::NodePtr> highlight;
  std::vector<osg::NodePtr> highlightNode;
#if OSG_MAJOR_VERSION >= 2
  std::vector<osg::GeoPnt3fPropertyRecPtr> highlightPoints;
#else
  std::vector<osg::GeoPositions3fPtr> highlightPoints;
#endif
  osg::SimpleMaterialPtr highlightMaterial;

  float lastTimeStamp;
  std::string version;
};

#endif
