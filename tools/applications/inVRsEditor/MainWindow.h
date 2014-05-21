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

#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

#include "Widgets/OpenSGWidget.h"
#include "Widgets/EntityWidget.h"
#include "Widgets/EntityTypeWidget.h"
#include "Widgets/EnvironmentWidget.h"
#include "Dialogs/NewVirtualEnvironmentDialog.h"
#include "Dialogs/NewEnvironmentDialog.h"
#include "EditorLayer.h"

class QAction;
class QMenu;
class QDockWidget;
class QWidget;
class QPushButton;
class QString;
class QLabel;

class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  MainWindow();
  ~MainWindow();

protected slots:
  void open();
  void save();
  void about();

  void processSelection(QModelIndex current, QModelIndex previous);
  
  void showAllMain();
  void showAllPreview();

  void newEntity();
  void newEntityType();
  void newEnvironment(int spacingX, int spacingZ, int sizeX, int sizeZ);
  void deleteSelection();
protected:
  void createConnections();
  void createActions();
  void createMenus();
  void createStatusBar();
  void createDockWindows();

  void updateStatusBar();
		
  EditorLayer *mEditorLayer;

	NewVirtualEnvironmentDialog *mNewVirtualEnvironmentDialog;
	NewEnvironmentDialog *mNewEnvironmentDialog;

  QAction *mActionNew;
  QAction *mActionOpen;
  QAction *mActionSave;
  QAction *mActionExit;
  QAction *mActionNewEntityType;
  QAction *mActionNewEnvironment;
  QAction *mActionAbout;
  QAction *mActionAboutQt;
  QAction *mActionShowVirtualEnvironment;
  QAction *mActionShowProperties;
  QAction *mActionShowPreview;

  QMenu *mMenuFile;
  QMenu *mMenuEdit;
  QMenu *mMenuView;
  QMenu *mMenuHelp;

  QDockWidget *mDockWidgetVirtualEnvironment;
  QDockWidget *mDockWidgetPreview;
  QDockWidget *mDockWidgetProperties;

  QTreeView *mTreeViewVirtualEnvironment;

	EntityWidget *mEntityProperties;
	EntityTypeWidget *mEntityTypeProperties;
	EnvironmentWidget *mEnvironmentProperties;
	QWidget *mEmptyProperties;
	
  QPushButton *mButtonVirtualEnvAdd;
  QPushButton *mButtonVirtualEnvDelete;
  QPushButton *mButtonShowAllMain;
  QPushButton *mButtonShowAllPreview;

	QString fileName;
	QLabel *mLabelFileName;
};
#endif
