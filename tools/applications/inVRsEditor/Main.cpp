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

#include "MainWindow.h"

#include <QApplication>
#include <QSplashScreen>

int main(int  argc,  char*  argv[]){
	QApplication app(argc, argv);
	 
	QSplashScreen *splash = new QSplashScreen;
	splash->setPixmap(QPixmap(":/images/inVRs_logo_300.png"));
	splash->show();

	Qt::Alignment topRight = Qt::AlignRight | Qt::AlignTop;
#if OSG_MAJOR_VERSION >= 2 && WIN32
	//preload shared object needed to be able to load things
	OSG::preloadSharedObject("OSGFileIO");
	OSG::preloadSharedObject("OSGImageFileIO");
#endif
	splash->showMessage(QObject::tr("Initialize OpenSG ..."),
			topRight, Qt::black);
	osg::osgInit(argc, argv);
	
	splash->showMessage(QObject::tr("Setting up the main window ..."),
			topRight, Qt::black);
	MainWindow mainWin;
	mainWin.showMaximized();
	
	splash->finish(&mainWin);
	delete splash;

	return  app.exec();
}
