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

#include "NewVirtualEnvironmentDialog.h"

#include <QtGui>

NewVirtualEnvironmentDialog::NewVirtualEnvironmentDialog(QWidget *parent) : QDialog(parent) {
	setWindowTitle("New VirtualEnvironment");

	mLineEditInvrsPath = new QLineEdit();
	mLineEditInvrsPath->setFixedWidth(350);
	mLineEditProjectPath = new QLineEdit();
	mLineEditProjectPath->setFixedWidth(350);

	mPushButtonBrowseInvrs = new QPushButton("Browse...");
	mPushButtonBrowseProject = new QPushButton("Browse...");

	mButtonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

	QLabel *labelInvrsPath = new QLabel("inVRs Path");
	QLabel *labelProjectPath = new QLabel("Project Path");

	QVBoxLayout *vBoxLayoutMain = new QVBoxLayout();
	QHBoxLayout *hBoxLayoutInvrs = new QHBoxLayout();
	QHBoxLayout *hBoxLayoutProject = new QHBoxLayout();
	QHBoxLayout *hBoxLayoutButtons = new QHBoxLayout();

	hBoxLayoutInvrs->addWidget(mLineEditInvrsPath);
	hBoxLayoutInvrs->addWidget(mPushButtonBrowseInvrs);

	hBoxLayoutProject->addWidget(mLineEditProjectPath);
	hBoxLayoutProject->addWidget(mPushButtonBrowseProject);

	hBoxLayoutButtons->addStretch();
	hBoxLayoutButtons->addWidget(mButtonBox);

	vBoxLayoutMain->addWidget(labelProjectPath);
	vBoxLayoutMain->addLayout(hBoxLayoutProject);	
	vBoxLayoutMain->addWidget(labelInvrsPath);
	vBoxLayoutMain->addLayout(hBoxLayoutInvrs);
	vBoxLayoutMain->addLayout(hBoxLayoutButtons);

	setLayout(vBoxLayoutMain);
	setFixedSize(sizeHint());

	connect(mButtonBox, SIGNAL(rejected()),
			this, SLOT(close()));
	connect(mButtonBox, SIGNAL(accepted()),
			this, SLOT(acceptPaths()));
	connect(mPushButtonBrowseInvrs, SIGNAL(clicked()),
			this, SLOT(browseInvrsPath()));
	connect(mPushButtonBrowseProject, SIGNAL(clicked()),
			this, SLOT(browseProjectPath()));
} // NewVirtualEnvironmentDialog

void NewVirtualEnvironmentDialog::acceptPaths() {
	emit paths(mLineEditInvrsPath->text(), mLineEditProjectPath->text());
	close();
} // acceptPaths

void NewVirtualEnvironmentDialog::browseInvrsPath() {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"." ,
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if(!dir.isEmpty() && !dir.isNull()) {
		mLineEditInvrsPath->setText(dir);
	} // if
} // browseInvrsPath

void NewVirtualEnvironmentDialog::browseProjectPath() {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"." ,
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if(!dir.isEmpty() && !dir.isNull()) {
		mLineEditProjectPath->setText(dir);
	} // if
} // browseProjectPath
