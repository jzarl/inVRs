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

#ifndef _ENVIRONMENTWIDGET_H
#define _ENVIRONMENTWIDGET_H

#include "PropertiesWidget.h"
#include "../Dialogs/EntryPointDialog.h"

#include <QObject>

class QSpinBox;
class QLineEdit;
class QListWidget;
class QPushButton;
class QAbstractItemModel;

class EnvironmentWidget : public PropertiesWidget {
	Q_OBJECT
public:
	EnvironmentWidget();

	void setPositionX(int value);
	void setPositionZ(int value);
	void setXmlFilename(QString value);

	void setModelEntryPoints(QAbstractItemModel *model);
signals:
	void positionXChanged(int value);
	void positionZChanged(int value);

	void xmlFilenameChanged(QString value);

	void entryPointAdded(std::vector<double> values);
	void entryPointDeleted(unsigned index);
protected:
	QSpinBox *mSpinBoxPositionX;
	QSpinBox *mSpinBoxPositionZ;

	QLineEdit *mLineEditXmlFilename;

	QTableView *mTableViewEP;

	QPushButton *mPushButtonAddEP;
	QPushButton *mPushButtonDeleteEP;

	EntryPointDialog *mEntryPointDialog;

	int indexEP;

protected slots:
	void deleteEntryPoint();
};

#endif
