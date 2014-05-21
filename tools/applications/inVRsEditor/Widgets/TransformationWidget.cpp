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

#include "TransformationWidget.h"

#include <QtGui>

TransformationWidget::TransformationWidget() {
	QFrame *line;
	
	QVBoxLayout *vBoxLayout = new QVBoxLayout();

#if QT_VERSION >= 0x040400
	// QFormLayout was introduced in Qt 4.4
	QFormLayout *formLayoutTranslation = new QFormLayout();
	QFormLayout *formLayoutRotation = new QFormLayout();
	QFormLayout *formLayoutScale = new QFormLayout();
#else
	// a more-or-less equal replacement for QFormLayout can be done with QGridLayout:
	QGridLayout *formLayoutTranslation = new QGridLayout();
	QGridLayout *formLayoutRotation = new QGridLayout();
	QGridLayout *formLayoutScale = new QGridLayout();
#endif

	QLabel *labelTranslation = new QLabel("Translation");
	QLabel *labelTranslationX = new QLabel("x");
	QLabel *labelTranslationY = new QLabel("y");
	QLabel *labelTranslationZ = new QLabel("z");

	QLabel *labelRotation = new QLabel("Rotation");
	QLabel *labelRotationX = new QLabel("x");
	QLabel *labelRotationY = new QLabel("y");
	QLabel *labelRotationZ = new QLabel("z");
	QLabel *labelRotationA = new QLabel("a");

	QLabel *labelScale = new QLabel("Scale");
	QLabel *labelScaleX = new QLabel("x");
	QLabel *labelScaleY = new QLabel("y");
	QLabel *labelScaleZ = new QLabel("z");

	mDSpinBoxTranslationX = new QDoubleSpinBox();
	mDSpinBoxTranslationX->setRange(-1000, 1000);
	mDSpinBoxTranslationY = new QDoubleSpinBox();
	mDSpinBoxTranslationY->setRange(-1000, 1000);
	mDSpinBoxTranslationZ = new QDoubleSpinBox();
	mDSpinBoxTranslationZ->setRange(-1000, 1000);

	mDSpinBoxRotationX = new QDoubleSpinBox();
	mDSpinBoxRotationX->setRange(-1000, 1000);
	mDSpinBoxRotationY = new QDoubleSpinBox();
	mDSpinBoxRotationY->setRange(-1000, 1000);
	mDSpinBoxRotationZ = new QDoubleSpinBox();
	mDSpinBoxRotationZ->setRange(-1000, 1000);
	mDSpinBoxRotationA = new QDoubleSpinBox();
	mDSpinBoxRotationA->setRange(0, 360);

	mDSpinBoxScaleX = new QDoubleSpinBox();
	mDSpinBoxScaleX->setSingleStep(0.1);
	mDSpinBoxScaleY = new QDoubleSpinBox();
	mDSpinBoxScaleY->setSingleStep(0.1);
	mDSpinBoxScaleZ = new QDoubleSpinBox();
	mDSpinBoxScaleZ->setSingleStep(0.1);

#if QT_VERSION >= 0x040400
	// QFormLayout was introduced in Qt 4.4
	formLayoutTranslation->addRow(labelTranslationX, mDSpinBoxTranslationX);
	formLayoutTranslation->addRow(labelTranslationY, mDSpinBoxTranslationY);
	formLayoutTranslation->addRow(labelTranslationZ, mDSpinBoxTranslationZ);
	formLayoutRotation->addRow(labelRotationX, mDSpinBoxRotationX);
	formLayoutRotation->addRow(labelRotationY, mDSpinBoxRotationY);
	formLayoutRotation->addRow(labelRotationZ, mDSpinBoxRotationZ);
	formLayoutRotation->addRow(labelRotationA, mDSpinBoxRotationA);
	formLayoutScale->addRow(labelScaleX, mDSpinBoxScaleX);
	formLayoutScale->addRow(labelScaleY, mDSpinBoxScaleY);
	formLayoutScale->addRow(labelScaleZ, mDSpinBoxScaleZ);
#else
	labelTranslationX->setBuddy(mDSpinBoxTranslationX);
	labelTranslationY->setBuddy(mDSpinBoxTranslationY);
	labelTranslationZ->setBuddy(mDSpinBoxTranslationZ);
	labelRotationX->setBuddy(mDSpinBoxRotationX);
	labelRotationY->setBuddy(mDSpinBoxRotationY);
	labelRotationZ->setBuddy(mDSpinBoxRotationZ);
	labelRotationA->setBuddy(mDSpinBoxRotationA);
	labelScaleX->setBuddy(mDSpinBoxScaleX);
	labelScaleY->setBuddy(mDSpinBoxScaleY);
	labelScaleZ->setBuddy(mDSpinBoxScaleZ);
	// a more-or-less equal replacement for QFormLayout can be done with QGridLayout:
	formLayoutTranslation->addWidget(labelTranslationX,0,0);
	formLayoutTranslation->addWidget(mDSpinBoxTranslationX,0,1);
	formLayoutTranslation->addWidget(labelTranslationY,1,0);
	formLayoutTranslation->addWidget(mDSpinBoxTranslationY,1,1);
	formLayoutTranslation->addWidget(labelTranslationZ,2,0);
	formLayoutTranslation->addWidget(mDSpinBoxTranslationZ,2,1);
	formLayoutRotation->addWidget(labelRotationX,0,0);
	formLayoutRotation->addWidget(mDSpinBoxRotationX,0,1);
	formLayoutRotation->addWidget(labelRotationY,1,0);
	formLayoutRotation->addWidget(mDSpinBoxRotationY,1,1);
	formLayoutRotation->addWidget(labelRotationZ,2,0);
	formLayoutRotation->addWidget(mDSpinBoxRotationZ,2,1);
	formLayoutRotation->addWidget(labelRotationA,3,0);
	formLayoutRotation->addWidget(mDSpinBoxRotationA,3,1);
	formLayoutScale->addWidget(labelScaleX,0,0);
	formLayoutScale->addWidget(mDSpinBoxScaleX,0,1);
	formLayoutScale->addWidget(labelScaleY,1,0);
	formLayoutScale->addWidget(mDSpinBoxScaleY,1,1);
	formLayoutScale->addWidget(labelScaleZ,2,0);
	formLayoutScale->addWidget(mDSpinBoxScaleZ,2,1);
#endif

	line = new QFrame();
	line->setFrameShape(QFrame::HLine);
	vBoxLayout->addWidget(line);

	vBoxLayout->addWidget(labelTranslation);
	vBoxLayout->addLayout(formLayoutTranslation);

	line = new QFrame();
	line->setFrameShape(QFrame::HLine);
	vBoxLayout->addWidget(line);

	vBoxLayout->addWidget(labelRotation);
	vBoxLayout->addLayout(formLayoutRotation);

	line = new QFrame();
	line->setFrameShape(QFrame::HLine);
	vBoxLayout->addWidget(line);
	
	vBoxLayout->addWidget(labelScale);
	vBoxLayout->addLayout(formLayoutScale);

	line = new QFrame();
	line->setFrameShape(QFrame::HLine);
	vBoxLayout->addWidget(line);

	setLayout(vBoxLayout);

	connect(mDSpinBoxTranslationX, SIGNAL(valueChanged(double)), this, SIGNAL(translationXChanged(double)));
	connect(mDSpinBoxTranslationY, SIGNAL(valueChanged(double)), this, SIGNAL(translationYChanged(double)));
	connect(mDSpinBoxTranslationZ, SIGNAL(valueChanged(double)), this, SIGNAL(translationZChanged(double)));
	connect(mDSpinBoxRotationX, SIGNAL(valueChanged(double)), this, SIGNAL(rotationXChanged(double)));
	connect(mDSpinBoxRotationY, SIGNAL(valueChanged(double)), this, SIGNAL(rotationYChanged(double)));
	connect(mDSpinBoxRotationZ, SIGNAL(valueChanged(double)), this, SIGNAL(rotationZChanged(double)));
	connect(mDSpinBoxRotationA, SIGNAL(valueChanged(double)), this, SIGNAL(rotationAChanged(double)));
	connect(mDSpinBoxScaleX, SIGNAL(valueChanged(double)), this, SIGNAL(scaleXChanged(double)));
	connect(mDSpinBoxScaleY, SIGNAL(valueChanged(double)), this, SIGNAL(scaleYChanged(double)));
	connect(mDSpinBoxScaleZ, SIGNAL(valueChanged(double)), this, SIGNAL(scaleZChanged(double)));
} // TransformationWidget

void TransformationWidget::setTranslationX(double value) {
	mDSpinBoxTranslationX->setValue(value);
} // setTranslationX

void TransformationWidget::setTranslationY(double value) {
	mDSpinBoxTranslationY->setValue(value);
} // setTranslationY

void TransformationWidget::setTranslationZ(double value) {
	mDSpinBoxTranslationZ->setValue(value);
} // setTranslationZ

void TransformationWidget::setRotationX(double value) {
	mDSpinBoxRotationX->setValue(value);
} // setRotationX

void TransformationWidget::setRotationY(double value) {
	mDSpinBoxRotationY->setValue(value);
} // setRotationY

void TransformationWidget::setRotationZ(double value) {
	mDSpinBoxRotationZ->setValue(value);
} // setRotationZ

void TransformationWidget::setRotationA(double value) {
	mDSpinBoxRotationA->setValue(value);
} // setRotationA

void TransformationWidget::setScaleX(double value) {
	mDSpinBoxScaleX->setValue(value);
} // setScaleX

void TransformationWidget::setScaleY(double value) {
	mDSpinBoxScaleY->setValue(value);
} // setScaleY

void TransformationWidget::setScaleZ(double value) {
	mDSpinBoxScaleZ->setValue(value);
} // setScaleZ
