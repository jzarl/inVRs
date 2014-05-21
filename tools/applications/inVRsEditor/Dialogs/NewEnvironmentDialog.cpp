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

#include "NewEnvironmentDialog.h"

#include <QtGui>

NewEnvironmentDialog::NewEnvironmentDialog(QWidget *parent) : QDialog(parent) {
	setWindowTitle("New Environment");

	mSpinBoxSpacingX = new QSpinBox();
	mSpinBoxSpacingZ = new QSpinBox();
	mSpinBoxSizeX = new QSpinBox();
	mSpinBoxSizeZ = new QSpinBox();

	mButtonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

	QLabel *labelSpacingX = new QLabel("xSpacing");
	QLabel *labelSpacingZ = new QLabel("zSpacing");
	QLabel *labelSizeX = new QLabel("initialXSize");
	QLabel *labelSizeZ = new QLabel("initialZSize");


	QVBoxLayout *vBoxLayoutMain = new QVBoxLayout();
	QHBoxLayout *hBoxLayoutButtons = new QHBoxLayout();
#if QT_VERSION >= 0x040400
	// QFormLayout was introduced in Qt 4.4
	QFormLayout *formLayout = new QFormLayout();
	
	formLayout->addRow(labelSpacingX, mSpinBoxSpacingX);
	formLayout->addRow(labelSpacingZ, mSpinBoxSpacingZ);
	formLayout->addRow(labelSizeX, mSpinBoxSizeX);
	formLayout->addRow(labelSizeZ, mSpinBoxSizeZ);
#else
	// a more-or-less equal replacement for QFormLayout can be done with QGridLayout:
	QGridLayout *formLayout = new QGridLayout();
	labelSpacingX->setBuddy(mSpinBoxSpacingX);
	labelSpacingZ->setBuddy(mSpinBoxSpacingZ);
	labelSizeX->setBuddy(mSpinBoxSizeX);
	labelSizeZ->setBuddy(mSpinBoxSizeZ);
	
	formLayout->addWidget(labelSpacingX,0,0);
	formLayout->addWidget(mSpinBoxSpacingX,0,1);
	formLayout->addWidget(labelSpacingZ,1,0);
	formLayout->addWidget(mSpinBoxSpacingZ,1,1);
	formLayout->addWidget(labelSizeX,2,0);
	formLayout->addWidget(mSpinBoxSizeX,2,1);
	formLayout->addWidget(labelSizeZ,3,0);
	formLayout->addWidget(mSpinBoxSizeZ,3,1);
#endif

	hBoxLayoutButtons->addStretch();
	hBoxLayoutButtons->addWidget(mButtonBox);

	vBoxLayoutMain->addLayout(formLayout);
	vBoxLayoutMain->addLayout(hBoxLayoutButtons);

	setLayout(vBoxLayoutMain);
	setFixedSize(sizeHint());

	connect(mButtonBox, SIGNAL(rejected()),
			this, SLOT(close()));
	connect(mButtonBox, SIGNAL(accepted()),
			this, SLOT(acceptValues()));
	
} // NewEnvironmentDialog

void NewEnvironmentDialog::acceptValues() {
	emit values(mSpinBoxSpacingX->value(), mSpinBoxSpacingZ->value(),
			mSpinBoxSizeX->value(), mSpinBoxSizeZ->value());
	close();
} // acceptValues
