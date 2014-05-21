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

#include "PropertiesWidget.h"

#include <QtGui>

PropertiesWidget::PropertiesWidget() {
	mTitleLabel = new QLabel();
	mTableView = new QTableView();
	mPropertiesLayout = new QVBoxLayout();
	
	QWidget *mainWidget = new QWidget(this);
	QVBoxLayout *vBoxLayout = new QVBoxLayout();
	QScrollArea *scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);

	vBoxLayout->addWidget(mTitleLabel);
	vBoxLayout->addWidget(mTableView);
	vBoxLayout->addLayout(mPropertiesLayout);
	vBoxLayout->addStretch();

	mainWidget->setLayout(vBoxLayout);
	mainWidget->setMinimumSize(120, 700);
	
	scrollArea->setWidget(mainWidget);
	scrollArea->setAlignment(Qt::AlignRight);
	QHBoxLayout *mainLayout = new QHBoxLayout();
	mainLayout->addWidget(scrollArea);

	setLayout(mainLayout);
} // PropertiesWidget

void PropertiesWidget::updateTableView() {
	mTableView->resizeColumnsToContents();
} // updateTableView

void PropertiesWidget::setModelProperties(QAbstractItemModel *model) {
	mTableView->setModel(model);	
} // setModelProperties
