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

#include "EntryPointDialog.h"

#include <QtGui>

EntryPointDialog::EntryPointDialog(QWidget *parent) : QDialog(parent) {
	setWindowTitle("New EntryPoint");

	mDSpinBoxXPos = new QDoubleSpinBox();
	mDSpinBoxXPos->setRange(-1000, 1000);
	mDSpinBoxYPos = new QDoubleSpinBox();
	mDSpinBoxYPos->setRange(-1000, 1000);
	mDSpinBoxZPos = new QDoubleSpinBox();
	mDSpinBoxZPos->setRange(-1000, 1000);
	mDSpinBoxXDir = new QDoubleSpinBox();
	mDSpinBoxXDir->setRange(-1000, 1000);
	mDSpinBoxXDir->setSingleStep(0.1);
	mDSpinBoxYDir = new QDoubleSpinBox();
	mDSpinBoxYDir->setRange(-1000, 1000);
	mDSpinBoxYDir->setSingleStep(0.1);
	mDSpinBoxZDir = new QDoubleSpinBox();
	mDSpinBoxZDir->setRange(-1000, 1000);
	mDSpinBoxZDir->setSingleStep(0.1);

	QLabel *labelXPos = new QLabel("xPos");
	QLabel *labelYPos = new QLabel("YPos");
	QLabel *labelZPos = new QLabel("ZPos");
	QLabel *labelXDir = new QLabel("xDir");
	QLabel *labelYDir = new QLabel("yDir");
	QLabel *labelZDir = new QLabel("zDir");

	QVBoxLayout *vBoxLayoutMain = new QVBoxLayout();
	QGridLayout *gridLayout = new QGridLayout;
	QHBoxLayout *hBoxLayoutButtons = new QHBoxLayout();

	// 2010-11-02 ZaJ: layout in a 2x6 (rows x cols) grid:
	gridLayout->addWidget(labelXPos,0,0);
	gridLayout->addWidget(mDSpinBoxXPos,0,1);
	gridLayout->addWidget(labelYPos,0,2);
	gridLayout->addWidget(mDSpinBoxYPos,0,3);
	gridLayout->addWidget(labelZPos,0,4);
	gridLayout->addWidget(mDSpinBoxZPos,0,5);
	
	gridLayout->addWidget(labelXDir,1,0);
	gridLayout->addWidget(mDSpinBoxXDir,1,1);
	gridLayout->addWidget(labelYDir,1,2);
	gridLayout->addWidget(mDSpinBoxYDir,1,3);
	gridLayout->addWidget(labelZDir,1,4);
	gridLayout->addWidget(mDSpinBoxZDir,1,5);

	mButtonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

	hBoxLayoutButtons->addStretch();
	hBoxLayoutButtons->addWidget(mButtonBox);

	vBoxLayoutMain->addLayout(gridLayout);
	vBoxLayoutMain->addLayout(hBoxLayoutButtons);

	setLayout(vBoxLayoutMain);

	setFixedSize(sizeHint());

	connect(mButtonBox, SIGNAL(rejected()), this, SLOT(close()));
	connect(mButtonBox, SIGNAL(accepted()), this, SLOT(acceptEntryPoint()));
} // EntryPointDialog

void EntryPointDialog::acceptEntryPoint() {
	std::vector<double> val;
	val.push_back(mDSpinBoxXPos->value());
	val.push_back(mDSpinBoxYPos->value());
	val.push_back(mDSpinBoxZPos->value());
	val.push_back(mDSpinBoxXDir->value());
	val.push_back(mDSpinBoxYDir->value());
	val.push_back(mDSpinBoxZDir->value());
	emit values(val);
	
	mDSpinBoxXPos->setValue(0.0);
	mDSpinBoxYPos->setValue(0.0);
	mDSpinBoxZPos->setValue(0.0);
	mDSpinBoxXDir->setValue(0.0);
	mDSpinBoxYDir->setValue(0.0);
	mDSpinBoxZDir->setValue(0.0);
	close();
} // acceptEntryPoint
