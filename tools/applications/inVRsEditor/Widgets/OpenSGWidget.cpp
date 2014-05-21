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

#include "OpenSGWidget.h"

OpenSGWidget::OpenSGWidget(QWidget *parent) : QGLWidget(parent){
#if OSG_MAJOR_VERSION >= 2
  mSimpleSceneManager = osg::SimpleSceneManager::create();
#else
  mSimpleSceneManager = new osg::SimpleSceneManager();
#endif
  
  osg::NodePtr root = osg::Node::create();
  beginEditCP(root);
  root->setCore(osg::Group::create());
  endEditCP(root);

  mPwin = osg::PassiveWindow::create();
  mSimpleSceneManager->setWindow(mPwin);
  mSimpleSceneManager->setRoot(root);
  mSimpleSceneManager->showAll();
  mSimpleSceneManager->getCamera()->setNear(0.1);
}

void OpenSGWidget::initializeGL() {
	mPwin->init();
} // initializeGL

void OpenSGWidget::paintGL() {
	mSimpleSceneManager->redraw();
} // paintGL

void OpenSGWidget::resizeGL(int width, int height) {
	mSimpleSceneManager->resize(width,height);
} // resizeGL

void OpenSGWidget::mousePressEvent(QMouseEvent *event) {
	osg::UInt32 button;
	switch(event->button()){
		case Qt::LeftButton:
			button = osg::SimpleSceneManager::MouseLeft;
			break;
		case Qt::MidButton:
			button = osg::SimpleSceneManager::MouseMiddle;
			break;
		case Qt::RightButton:
			button = osg::SimpleSceneManager::MouseRight;
			break;
		default:
			return;
	} // switch
	mSimpleSceneManager->mouseButtonPress(button, event->x(), event->y());
	updateGL();
} // mousePressEvent

void OpenSGWidget::mouseReleaseEvent(QMouseEvent *event) {
	osg::UInt32 button;
	switch(event->button()){
		case Qt::LeftButton:
			button = osg::SimpleSceneManager::MouseLeft;
			break;
		case Qt::MidButton:
			button = osg::SimpleSceneManager::MouseMiddle;
			break;
		case Qt::RightButton:
			button = osg::SimpleSceneManager::MouseRight;
			break;
		default:
			return;
	} // switch
	mSimpleSceneManager->mouseButtonRelease(button, event->x(), event->y());
	updateGL();
} // mouseReleaseEvent

void OpenSGWidget::mouseMoveEvent(QMouseEvent *event) {
	mSimpleSceneManager->mouseMove(event->x(), event->y());
	updateGL();
} // mouseMoveEvent

void OpenSGWidget::wheelEvent(QWheelEvent *event) {
	if(event->delta() > 0){
		mSimpleSceneManager->mouseButtonRelease(osg::SimpleSceneManager::MouseUp, 
			event->x(), event->y());
	}else{
		mSimpleSceneManager->mouseButtonRelease(osg::SimpleSceneManager::MouseDown, 
			event->x(), event->y());		
	} // else
} // wheelEvent

void OpenSGWidget::update() {
  updateGL();
} // udpate

void OpenSGWidget::showAll() {
  mSimpleSceneManager->showAll();
} // showAll

void OpenSGWidget::setRootNode(osg::NodePtr node) {
	root = node;
	mSimpleSceneManager->setRoot(root);
	mSimpleSceneManager->showAll(); 
} // setRootNode

osg::SimpleSceneManager* OpenSGWidget::getSimpleSceneManager() {
  return mSimpleSceneManager;
} // getSimpleSceneManager
