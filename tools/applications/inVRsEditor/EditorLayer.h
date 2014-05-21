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

#ifndef _EDITORLAYER_H
#define _EDITORLAYER_H

#include "EditorBase.h"
#include "Widgets/OpenSGWidget.h"
#include "EditorDataTypes.h"

#include <QtGui>

Q_DECLARE_METATYPE(Selection*)

class EditorLayer : public QObject {
	Q_OBJECT

public:
  EditorLayer();
  virtual ~EditorLayer();
	
  bool loadConfiguration(std::string filename);
  void saveConfiguration();

	void processSelection(QModelIndex index);
  void deleteSelection();
  bool isSelectionEntity();
  bool isSelectionEntityType();
  bool isSelectionEnvironment();

  QModelIndex newEntity(QString id);
  QModelIndex newEntityType(QString name);
  QModelIndex newEnvironment(int spacingX, int spacingZ, int sizeX, int sizeZ);

	void showAllPreview();
  void showAllMain();

  OpenSGWidget* getOpenSGWidgetMain();
  OpenSGWidget* getOpenSGWidgetPreview();	
  
  QAbstractItemModel *getModelVirtualEnvironment();
  QAbstractItemModel *getModelProperties();
	QAbstractItemModel *getModelEntryPoints();
  
	double getTranslationX();
	double getTranslationY();
	double getTranslationZ();
	double getRotationX();
	double getRotationY();
	double getRotationZ();
	double getRotationA();
	double getScaleX();
	double getScaleY();
	double getScaleZ();

  bool getFixed();
  bool getRepCopy();
	QString getName();
  int getPositionX();
  int getPositionZ();

  QString getModelFilename();
  QStringList getModelFilenames();
	QString getXmlFilename();

	QStringList getEnvironmentIds();

public slots:
  void newConfiguration(QString invrsPath, QString projectPath);
	
	void setTranslationX(double value);
	void setTranslationY(double value);
	void setTranslationZ(double value);
	void setRotationX(double value);
	void setRotationY(double value);
	void setRotationZ(double value);
	void setRotationA(double value);
	void setScaleX(double value);
	void setScaleY(double value);
	void setScaleZ(double value);
	
	void setPositionX(int value);
	void setPositionZ(int value);

	void setFixed(bool value);
	void setRepCopy(bool value);
	void setName(QString value);
	void setModelFilename(QString value);
	void setXmlFilename(QString value);
	void addEntryPoint(std::vector<double> values);
	void deleteEntryPoint(unsigned index);

protected:
  void initModelVirtualEnvironment();
  void updateModelProperties();
	void updateModelEntryPoints();
	
	OpenSGWidget *mOpenSGWidgetMain;	
  OpenSGWidget *mOpenSGWidgetPreview;
  
  EditorBase *mEditorBase;	
  QStandardItemModel *mModelVirtualEnvironment;
  QStandardItemModel *mModelProperties;
	QStandardItemModel *mModelEntryPoints;

  std::map<int, QTreeWidgetItem *> items;

  QTableWidget *mTableWidgetProperties;

  QModelIndex mSelectedIndex;
};

#endif
