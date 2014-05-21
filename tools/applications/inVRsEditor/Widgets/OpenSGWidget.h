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

#ifndef _OPENSGWIDGET_H
#define _OPENSGWIDGET_H

#include <OpenSG/OSGSimpleSceneManager.h>
#include <OpenSG/OSGPassiveWindow.h>

#include <Qt/qgl.h>
#include <QMouseEvent>
#include <QWheelEvent>

class OpenSGWidget : public QGLWidget
{
public:
  OpenSGWidget(QWidget *parent = 0);
  
  void initializeGL();
  void paintGL();
  void resizeGL(int width, int height);
  
	void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);
	
  void update();
  void showAll();
  
	void setRootNode(osg::NodePtr node);
 
	osg::SimpleSceneManager* getSimpleSceneManager();
protected:
  osg::PassiveWindowPtr mPwin;
  osg::NodePtr root;
#if OSG_MAJOR_VERSION >= 2
  osg::SimpleSceneManager::ObjRefPtr mSimpleSceneManager;
#else
  osg::SimpleSceneManager* mSimpleSceneManager;
#endif
};

#endif
