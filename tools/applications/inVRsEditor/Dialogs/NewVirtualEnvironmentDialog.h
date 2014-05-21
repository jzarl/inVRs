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

#ifndef _NEWVIRTUALENVIRONMENTDIALOG_H
#define _NEWVIRTUALENVIRONMENTDIALOG_H

#include <QDialog>

class QLineEdit;
class QPushButton;
class QDialogButtonBox;
class QAbstractButton;

class NewVirtualEnvironmentDialog : public QDialog {
	Q_OBJECT

public:
	NewVirtualEnvironmentDialog(QWidget *parent = 0);

signals:
	void paths(const QString &invrsPath, const QString &projectPath);

protected:
	QLineEdit *mLineEditInvrsPath;
	QLineEdit *mLineEditProjectPath;

	QPushButton *mPushButtonBrowseInvrs;
	QPushButton *mPushButtonBrowseProject;
	
	QDialogButtonBox *mButtonBox;

private slots:
	void acceptPaths();
	void browseInvrsPath();
	void browseProjectPath();
}; // NewVirtualEnvironmentDialog
#endif
