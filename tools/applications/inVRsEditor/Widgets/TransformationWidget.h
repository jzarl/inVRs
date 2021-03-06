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

#ifndef _TRANSFORMATIONWIDGET_H
#define _TRANSFORMATIONWIDGET_H

#include <QObject>
#include <QWidget>

class QDoubleSpinBox;

class TransformationWidget : public QWidget {
	Q_OBJECT

public:
	TransformationWidget();
	
	void setTranslationX(double value);
	void setTranslationY(double value);
	void setTranslationZ(double value);
	void setRotationX(double value);
	void setRotationY(double value);
	void setRotationZ(double value);
	void setRotationA(double value);
	void setScaleX(double value);
	void setScaleY(double value);
	void setScaleZ(double value);

signals:
	void translationXChanged(double value);
	void translationYChanged(double value);
	void translationZChanged(double value);
	void rotationXChanged(double value);
	void rotationYChanged(double value);
	void rotationZChanged(double value);
	void rotationAChanged(double value);
	void scaleXChanged(double value);
	void scaleYChanged(double value);
	void scaleZChanged(double value);

protected:
	QDoubleSpinBox *mDSpinBoxTranslationX;
	QDoubleSpinBox *mDSpinBoxTranslationY;
	QDoubleSpinBox *mDSpinBoxTranslationZ;
	QDoubleSpinBox *mDSpinBoxRotationX;
	QDoubleSpinBox *mDSpinBoxRotationY;
	QDoubleSpinBox *mDSpinBoxRotationZ;
	QDoubleSpinBox *mDSpinBoxRotationA;
	QDoubleSpinBox *mDSpinBoxScaleX;
	QDoubleSpinBox *mDSpinBoxScaleY;
	QDoubleSpinBox *mDSpinBoxScaleZ;
};

#endif
