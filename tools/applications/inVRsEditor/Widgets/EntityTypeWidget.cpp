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

#include "EntityTypeWidget.h"

#include <QtGui>

EntityTypeWidget::EntityTypeWidget() {
	mTitleLabel->setText("EntityType Properties");
	mTransformationWidget = new TransformationWidget();
	mPropertiesLayout->addWidget(mTransformationWidget);

	QLabel *labelName = new QLabel("Name");
	QLabel *labelModelFilename = new QLabel("Model Filename");
	QLabel *labelXmlFilename = new QLabel("Xml Filename");

	mCheckBoxFixed = new QCheckBox("fixed");
	mCheckBoxRepCopy = new QCheckBox("Representation Copy");
	mComboBoxModelFilename = new QComboBox();
	mLineEditName = new QLineEdit();
	mLineEditXmlFilename = new QLineEdit();
	
#if QT_VERSION >= 0x040400
	// QFormLayout was introduced in Qt 4.4
	QFormLayout *formLayout = new QFormLayout();
	formLayout->addRow(labelName, mLineEditName);
	formLayout->addRow(labelModelFilename, mComboBoxModelFilename);
	formLayout->addRow(labelXmlFilename, mLineEditXmlFilename);
#else
	// a more-or-less equal replacement for QFormLayout can be done with QGridLayout:
	QGridLayout *formLayout = new QGridLayout();
	labelName->setBuddy(mLineEditName);
	formLayout->addWidget(labelName,0,0);
	formLayout->addWidget(mLineEditName,0,1);
	labelModelFilename->setBuddy(mComboBoxModelFilename);
	formLayout->addWidget(labelModelFilename,1,0);
	formLayout->addWidget(mComboBoxModelFilename,1,1);
	labelXmlFilename->setBuddy(mLineEditXmlFilename);
	formLayout->addWidget(labelXmlFilename,2,0);
	formLayout->addWidget(mLineEditXmlFilename,2,1);
#endif

	mPropertiesLayout->addLayout(formLayout);
	mPropertiesLayout->addWidget(mCheckBoxFixed);
	mPropertiesLayout->addWidget(mCheckBoxRepCopy);

	connect(mTransformationWidget, SIGNAL(translationXChanged(double)), 
			this, SIGNAL(translationXChanged(double)));
	connect(mTransformationWidget, SIGNAL(translationYChanged(double)), 
			this, SIGNAL(translationYChanged(double)));
	connect(mTransformationWidget, SIGNAL(translationZChanged(double)), 
			this, SIGNAL(translationZChanged(double)));
	connect(mTransformationWidget, SIGNAL(rotationXChanged(double)), 
			this, SIGNAL(rotationXChanged(double)));
	connect(mTransformationWidget, SIGNAL(rotationYChanged(double)), 
			this, SIGNAL(rotationYChanged(double)));
	connect(mTransformationWidget, SIGNAL(rotationZChanged(double)), 
			this, SIGNAL(rotationZChanged(double)));
	connect(mTransformationWidget, SIGNAL(rotationAChanged(double)), 
			this, SIGNAL(rotationAChanged(double)));
	connect(mTransformationWidget, SIGNAL(scaleXChanged(double)), 
			this, SIGNAL(scaleXChanged(double)));
	connect(mTransformationWidget, SIGNAL(scaleYChanged(double)), 
			this, SIGNAL(scaleYChanged(double)));
	connect(mTransformationWidget, SIGNAL(scaleZChanged(double)), 
			this, SIGNAL(scaleZChanged(double)));
	connect(mCheckBoxFixed, SIGNAL(toggled(bool)),
			this, SIGNAL(fixedChanged(bool)));
	connect(mCheckBoxRepCopy, SIGNAL(toggled(bool)),
			this, SIGNAL(repCopyChanged(bool)));
	connect(mComboBoxModelFilename, SIGNAL(activated(QString)),
			this, SIGNAL(modelFilenameChanged(QString)));
	connect(mLineEditName, SIGNAL(textChanged(QString)),
			this, SIGNAL(nameChanged(QString)));
	connect(mLineEditXmlFilename, SIGNAL(textChanged(QString)),
			this, SIGNAL(xmlFilenameChanged(QString)));
} // EntityTypeWidget

void EntityTypeWidget::updateModelFilenames(QStringList names, QString current) {
	mComboBoxModelFilename->clear();
	mComboBoxModelFilename->addItems(names);
	mComboBoxModelFilename->setCurrentIndex(mComboBoxModelFilename->findText(current));
} // updateFilenames

void EntityTypeWidget::setTranslationX(double value) {
	mTransformationWidget->setTranslationX(value);
} // setTranslationX

void EntityTypeWidget::setTranslationY(double value) {
	mTransformationWidget->setTranslationY(value);
} // setTranslationY

void EntityTypeWidget::setTranslationZ(double value) {
	mTransformationWidget->setTranslationZ(value);
} // setTranslationZ

void EntityTypeWidget::setRotationX(double value) {
	mTransformationWidget->setRotationX(value);
} // setRotationX

void EntityTypeWidget::setRotationY(double value) {
	mTransformationWidget->setRotationY(value);
} // setRotationY

void EntityTypeWidget::setRotationZ(double value) {
	mTransformationWidget->setRotationZ(value);
} // setRotationZ

void EntityTypeWidget::setRotationA(double value) {
	mTransformationWidget->setRotationA(value);
} // setRotationA

void EntityTypeWidget::setScaleX(double value) {
	mTransformationWidget->setScaleX(value);
} // setScaleX

void EntityTypeWidget::setScaleY(double value) {
	mTransformationWidget->setScaleY(value);
} // setScaleY

void EntityTypeWidget::setScaleZ(double value) {
	mTransformationWidget->setScaleZ(value);
} // setScaleZ

void EntityTypeWidget::setFixed(bool value) {
	if(value) {
		mCheckBoxFixed->setCheckState(Qt::Checked);
	}else {
		mCheckBoxFixed->setCheckState(Qt::Unchecked);
	}// else
} // setFixed

void EntityTypeWidget::setRepCopy(bool value) {
	if(value) {
		mCheckBoxRepCopy->setCheckState(Qt::Checked);
	}else {
		mCheckBoxRepCopy->setCheckState(Qt::Unchecked);
	} // else
} // setRepCopy

void EntityTypeWidget::setName(QString value) {
	mLineEditName->setText(value);
} // setName

void EntityTypeWidget::setXmlFilename(QString value) {
	mLineEditXmlFilename->setText(value);
} // setXmlFilename
