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

#include "EnvironmentWidget.h"

#include <iostream>

#include <QtGui>

EnvironmentWidget::EnvironmentWidget() {
	mEntryPointDialog = new EntryPointDialog(this);

	mTitleLabel->setText("Environment Properties");
	
	QLabel *labelPositionX = new QLabel("xPosition");
	QLabel *labelPositionZ = new QLabel("zPosition");
	QLabel *labelXmlFilename = new QLabel("XML filename");
	QLabel *labelEntryPoints = new QLabel("Entry Points:");

	mSpinBoxPositionX = new QSpinBox();
	mSpinBoxPositionZ = new QSpinBox();
	
	mLineEditXmlFilename = new QLineEdit();

#if QT_VERSION >= 0x040400
	// QFormLayout was introduced in Qt 4.4
	QFormLayout *formLayout = new QFormLayout();
	formLayout->addRow(labelPositionX, mSpinBoxPositionX);
	formLayout->addRow(labelPositionZ, mSpinBoxPositionZ);
	formLayout->addRow(labelXmlFilename, mLineEditXmlFilename);
#else
	// a more-or-less equal replacement for QFormLayout can be done with QGridLayout:
	QGridLayout *formLayout = new QGridLayout();
	labelPositionX->setBuddy(mSpinBoxPositionX);
	formLayout->addWidget(labelPositionX,0,0);
	formLayout->addWidget(mSpinBoxPositionX,0,1);
	labelPositionZ->setBuddy(mSpinBoxPositionZ);
	formLayout->addWidget(labelPositionZ,1,0);
	formLayout->addWidget(mSpinBoxPositionZ,1,1);
	labelXmlFilename->setBuddy(mLineEditXmlFilename);
	formLayout->addWidget(labelXmlFilename,2,0);
	formLayout->addWidget(mLineEditXmlFilename,2,1);
#endif
	mPropertiesLayout->addLayout(formLayout);

	mTableViewEP = new QTableView();
	mTableViewEP->setSelectionBehavior(QAbstractItemView::SelectRows);

	mPushButtonAddEP = new QPushButton("Add");
	mPushButtonDeleteEP = new QPushButton("Delete");

	
	mPropertiesLayout->addWidget(labelEntryPoints);
	mPropertiesLayout->addWidget(mTableViewEP);
	QHBoxLayout *hBoxLayout = new QHBoxLayout();
	hBoxLayout->addWidget(mPushButtonAddEP);
	hBoxLayout->addWidget(mPushButtonDeleteEP);
	mPropertiesLayout->addLayout(hBoxLayout);

	connect(mSpinBoxPositionX, SIGNAL(valueChanged(int)),
			this, SIGNAL(positionXChanged(int)));
	connect(mSpinBoxPositionZ, SIGNAL(valueChanged(int)),
			this, SIGNAL(positionZChanged(int)));
	connect(mLineEditXmlFilename, SIGNAL(textChanged(QString)),
			this, SIGNAL(xmlFilenameChanged(QString)));
	connect(mPushButtonAddEP, SIGNAL(clicked()),
			mEntryPointDialog, SLOT(show()));
	connect(mEntryPointDialog, SIGNAL(values(std::vector<double>)),
			this, SIGNAL(entryPointAdded(std::vector<double>)));
	connect(mPushButtonDeleteEP, SIGNAL(clicked()),
			this, SLOT(deleteEntryPoint()));
} // EnvironmentWidget

void EnvironmentWidget::setPositionX(int value) {
	mSpinBoxPositionX->setValue(value);
} // setPositionX

void EnvironmentWidget::setPositionZ(int value) {
	mSpinBoxPositionZ->setValue(value);
} // setPositionZ

void EnvironmentWidget::setXmlFilename(QString value) {
	mLineEditXmlFilename->setText(value);
} // setXmlFilename

void EnvironmentWidget::setModelEntryPoints(QAbstractItemModel *model) {
	mTableViewEP->setModel(model);
} // setModelEntryPoints

void EnvironmentWidget::deleteEntryPoint() {
	unsigned index = (unsigned) mTableViewEP->selectionModel()->currentIndex().row();
	emit entryPointDeleted(index);
} // deleteEntryPoint
