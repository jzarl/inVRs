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

#ifndef _ENTRYPOINTDIALOG_H
#define _ENTRYPOINTDIALOG_H

#include <QDialog>

class QDoubleSpinBox;
class QDialogButtonBox;

class EntryPointDialog : public QDialog {
	Q_OBJECT

public:
	EntryPointDialog(QWidget *parent = 0);
signals:
	void values(const std::vector<double> &values);
protected:
	QDoubleSpinBox *mDSpinBoxXPos;
	QDoubleSpinBox *mDSpinBoxYPos;
	QDoubleSpinBox *mDSpinBoxZPos;
	QDoubleSpinBox *mDSpinBoxXDir;
	QDoubleSpinBox *mDSpinBoxYDir;
	QDoubleSpinBox *mDSpinBoxZDir;

	QDialogButtonBox *mButtonBox;
	
private slots:
	void acceptEntryPoint();
};
#endif
