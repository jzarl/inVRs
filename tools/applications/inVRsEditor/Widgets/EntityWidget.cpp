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

#include "EntityWidget.h"

#include <QtGui>

EntityWidget::EntityWidget() {
	mTitleLabel->setText("Entity Properties");
	mTransformationWidget = new TransformationWidget();
	mPropertiesLayout->addWidget(mTransformationWidget);

	mCheckBoxFixed = new QCheckBox("fixed");
	mPropertiesLayout->addWidget(mCheckBoxFixed);

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
} // EntityWidget

void EntityWidget::setTranslationX(double value) {
	mTransformationWidget->setTranslationX(value);
} // setTranslationX

void EntityWidget::setTranslationY(double value) {
	mTransformationWidget->setTranslationY(value);
} // setTranslationY

void EntityWidget::setTranslationZ(double value) {
	mTransformationWidget->setTranslationZ(value);
} // setTranslationZ

void EntityWidget::setRotationX(double value) {
	mTransformationWidget->setRotationX(value);
} // setRotationX

void EntityWidget::setRotationY(double value) {
	mTransformationWidget->setRotationY(value);
} // setRotationY

void EntityWidget::setRotationZ(double value) {
	mTransformationWidget->setRotationZ(value);
} // setRotationZ

void EntityWidget::setRotationA(double value) {
	mTransformationWidget->setRotationA(value);
} // setRotationA

void EntityWidget::setScaleX(double value) {
	mTransformationWidget->setScaleX(value);
} // setScaleX

void EntityWidget::setScaleY(double value) {
	mTransformationWidget->setScaleY(value);
} // setScaleY

void EntityWidget::setScaleZ(double value) {
	mTransformationWidget->setScaleZ(value);
} // setScaleZ

void EntityWidget::setFixed(bool value) {
	if(value) {
		mCheckBoxFixed->setCheckState(Qt::Checked);
	}else {
		mCheckBoxFixed->setCheckState(Qt::Unchecked);
	}// else
} // setFixed
