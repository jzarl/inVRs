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

#include "EditorLayer.h"

#include <string.h>
#include <sstream>
#include <iostream>

#include <QtGui>

EditorLayer::EditorLayer() {
	mOpenSGWidgetMain = new OpenSGWidget();
	mOpenSGWidgetPreview = new OpenSGWidget();
	mEditorBase = new EditorBase();

	mModelVirtualEnvironment = new QStandardItemModel(0, 1);
	QStringList tmpStringList;
	tmpStringList.append("Virtual Environment");
	mModelVirtualEnvironment->setHorizontalHeaderLabels(tmpStringList);

	mModelProperties = new QStandardItemModel(0, 1);
	mModelEntryPoints = new QStandardItemModel(0, 1);

	mEditorBase->setSimpleSceneManagerMain(mOpenSGWidgetMain->getSimpleSceneManager());
	mEditorBase->setSimpleSceneManagerPreview(mOpenSGWidgetPreview->getSimpleSceneManager());
} // EditorLayer

EditorLayer::~EditorLayer() {
	delete mEditorBase;
} // ~EditorLayer

bool EditorLayer::loadConfiguration(std::string filename) {
	if (mEditorBase->loadConfiguration(filename)) {
		mOpenSGWidgetMain->setRootNode(mEditorBase->getRootNode());

		mOpenSGWidgetMain->update();
		mOpenSGWidgetPreview->update();
		updateModelProperties();
		initModelVirtualEnvironment();
		return true;
	}
	return false;
} // loadConfiguration

void EditorLayer::saveConfiguration() {
	mEditorBase->saveConfiguration();
} // saveConfiguration

void EditorLayer::processSelection(QModelIndex index) {
	mSelectedIndex = index;

	QStandardItem *tmpItem = mModelVirtualEnvironment->itemFromIndex(index);
	mEditorBase->setSelection(tmpItem->data().value<Selection*> ());

	updateModelProperties();
	updateModelEntryPoints();
	mOpenSGWidgetMain->update();
	mOpenSGWidgetPreview->update();
} // processSelection

void EditorLayer::deleteSelection() {
	if (isSelectionEntity()) {
		mEditorBase->deleteSelection();
		mModelVirtualEnvironment->removeRow(mSelectedIndex.row(), mSelectedIndex.parent());

		if (mEditorBase->getSelection()->type == ENTITYTYPE && !mModelVirtualEnvironment->hasChildren(mSelectedIndex)) {
			mModelVirtualEnvironment->removeRow(mSelectedIndex.row(), mSelectedIndex.parent());
		} // if
	} else if (isSelectionEntityType()) {
		mEditorBase->deleteSelection();
		std::string tmpString = toString(mEditorBase->getSelection()->entityType->getId()) + " - " + mEditorBase->getSelection()->entityType->getName();

		QList<QStandardItem *> lIEntityTypes = mModelVirtualEnvironment->findItems(QString::fromLocal8Bit(tmpString.c_str()), Qt::MatchExactly | Qt::MatchRecursive);

		foreach (QStandardItem* item, lIEntityTypes)
			{
				mModelVirtualEnvironment->removeRow(item->index().row(), item->index().parent());
			} // foreach
	} // else if

	mOpenSGWidgetMain->update();
	mOpenSGWidgetPreview->update();
	updateModelProperties();
} // deleteSelection

bool EditorLayer::isSelectionEntity() {
	return mEditorBase->getSelection()->type == ENTITY;
} // isSelectionEntity

bool EditorLayer::isSelectionEntityType() {
	return mEditorBase->getSelection()->type == ENTITYTYPE;
} // isSelectionEntityType

bool EditorLayer::isSelectionEnvironment() {
	return mEditorBase->getSelection()->type == ENVIRONMENT;
} // isSelectionEnvironment

QModelIndex EditorLayer::newEntity(QString id) {
	QStandardItem *iEntity;

	mEditorBase->newEntity(id.toUInt());
	Selection *tmpSelection = mEditorBase->getSelection();
	if (tmpSelection->type == ENTITY) {
		std::string tmpString;

		tmpString = toString(tmpSelection->entity->getEntityType()->getId()) + " - ";
		tmpString += tmpSelection->entity->getEntityType()->getName();

		QList<QStandardItem *> lIEntityTypes = mModelVirtualEnvironment->findItems(QString::fromLocal8Bit(tmpString.c_str()), Qt::MatchExactly | Qt::MatchRecursive);

		bool addItem = false;
		foreach (QStandardItem* item, lIEntityTypes)
			{
				if (item->data().value<Selection *> ()->location == mEditorBase->getSelection()->location) {
					iEntity = new QStandardItem();
					iEntity->setEditable(false);
					tmpString = "Entity ";
					tmpString += toString(mEditorBase->getSelection()->entity->getEnvironmentBasedId() & 65535);
					iEntity->setText(QString::fromLocal8Bit(tmpString.c_str()));
					QVariant tmpVariant;
					tmpVariant.setValue(mEditorBase->getSelection());
					iEntity->setData(tmpVariant);
					item->appendRow(iEntity);
					addItem = true;
				} // if
			} // foreach

		if (!addItem) {
			tmpString = "Entity Types";
			QList<QStandardItem *> lIEntityTypes = mModelVirtualEnvironment->findItems(QString::fromLocal8Bit(tmpString.c_str()), Qt::MatchExactly | Qt::MatchRecursive);
			foreach (QStandardItem *item, lIEntityTypes)
				{
					if (item->data().value<Selection *> ()->location == mEditorBase->getSelection()->location) {
						QStandardItem *iEntityType = new QStandardItem();
						iEntityType->setEditable(false);
						tmpString = toString(tmpSelection->entity->getEntityType()->getId()) + " - ";
						tmpString += tmpSelection->entity->getEntityType()->getName();
						iEntityType->setText(QString::fromLocal8Bit(tmpString.c_str()));
						QVariant tmpVariant;
						Selection *selectionEntityType = new Selection();
						selectionEntityType->type = ENTITYTYPE;
						selectionEntityType->entityType = tmpSelection->entity->getEntityType();
						selectionEntityType->location = mEditorBase->getSelection()->location;
						tmpVariant.setValue(selectionEntityType);
						iEntityType->setData(tmpVariant);
						item->appendRow(iEntityType);
						iEntity = new QStandardItem();
						iEntity->setEditable(false);
						tmpString = "Entity ";
						tmpString += toString(mEditorBase->getSelection()->entity->getEnvironmentBasedId() & 65535);
						iEntity->setText(QString::fromLocal8Bit(tmpString.c_str()));
						tmpVariant.setValue(mEditorBase->getSelection());
						iEntity->setData(tmpVariant);
						iEntityType->appendRow(iEntity);
					} // if
				} // foreach
		} // if

		updateModelProperties();
		mOpenSGWidgetMain->update();
		mOpenSGWidgetPreview->update();

		return iEntity->index();
	} // if
	return QModelIndex();
} // newEntity

QModelIndex EditorLayer::newEntityType(QString name) {
	mEditorBase->newEntityType(qPrintable(name));
	QList<QStandardItem *> lIDatabaseET = mModelVirtualEnvironment->findItems("Entity Types", Qt::MatchExactly | Qt::MatchRecursive);

	foreach (QStandardItem* item, lIDatabaseET)
		{
			if (item->data().value<Selection *> ()->location == -1) {
				QStandardItem *iEntityType = new QStandardItem();
				iEntityType->setEditable(false);
				Selection *tmpSelection = mEditorBase->getSelection();
				std::string tmpString = toString(tmpSelection->entityType->getId()) + " - ";
				tmpString += tmpSelection->entityType->getName();
				iEntityType->setText(QString::fromLocal8Bit(tmpString.c_str()));
				QVariant tmpVariant;
				tmpVariant.setValue(tmpSelection);
				iEntityType->setData(tmpVariant);
				item->appendRow(iEntityType);

				updateModelProperties();
				return iEntityType->index();
			} // if
		} // foreach

	return QModelIndex();
} // newEntityType

QModelIndex EditorLayer::newEnvironment(int spacingX, int spacingZ, int sizeX, int sizeZ) {
	if (!mEditorBase->newEnvironment(spacingX, spacingZ, sizeX, sizeZ)) {
		QModelIndex index;
		return index;
	} // if

	QList<QStandardItem *> lIDatabaseEnv = mModelVirtualEnvironment->findItems("Environment Layout", Qt::MatchExactly | Qt::MatchRecursive);

	foreach (QStandardItem* item, lIDatabaseEnv)
		{
			QStandardItem *iEnvironment = new QStandardItem();
			iEnvironment->setEditable(false);
			Selection *tmpSelection = mEditorBase->getSelection();
			std::string tmpString = "Environment " + toString(tmpSelection->environment->getId());
			iEnvironment->setText(QString::fromLocal8Bit(tmpString.c_str()));
			QVariant tmpVariant;
			tmpVariant.setValue(tmpSelection);
			iEnvironment->setData(tmpVariant);
			item->appendRow(iEnvironment);

			updateModelProperties();

			QStandardItem *iEntityTypes = new QStandardItem();
			iEntityTypes->setEditable(false);
			iEntityTypes->setText("Entity Types");

			int id = tmpSelection->environment->getId();
			tmpSelection = new Selection();
			tmpSelection->type = ENTITYTYPES;
			tmpSelection->location = id;
			tmpVariant.setValue(tmpSelection);
			iEntityTypes->setData(tmpVariant);
			iEnvironment->appendRow(iEntityTypes);

			return iEnvironment->index();
		} //foreach
	return QModelIndex();
} // new Environment

void EditorLayer::showAllMain() {
	mOpenSGWidgetMain->showAll();
	mOpenSGWidgetMain->update();
} // showAllMain

void EditorLayer::showAllPreview() {
	mOpenSGWidgetPreview->showAll();
	mOpenSGWidgetPreview->update();
} // showAllPreview

OpenSGWidget* EditorLayer::getOpenSGWidgetMain() {
	return mOpenSGWidgetMain;
} // getOpenSGWidgetMain

OpenSGWidget* EditorLayer::getOpenSGWidgetPreview() {
	return mOpenSGWidgetPreview;
} // getOpenSGWidgetPreview

QAbstractItemModel* EditorLayer::getModelVirtualEnvironment() {
	return mModelVirtualEnvironment;
} // getModelVirtualEnvironment

QAbstractItemModel *EditorLayer::getModelProperties() {
	return mModelProperties;
} // getModelProperties

QAbstractItemModel *EditorLayer::getModelEntryPoints() {
	return mModelEntryPoints;
} // getModelEntryPoints

double EditorLayer::getTranslationX() {
	return mEditorBase->getSelectionTranslation()[0];
} // getTranslationX

double EditorLayer::getTranslationY() {
	return mEditorBase->getSelectionTranslation()[1];
} // getTranslationY

double EditorLayer::getTranslationZ() {
	return mEditorBase->getSelectionTranslation()[2];
} // getTranslationZ

double EditorLayer::getRotationX() {
	return mEditorBase->getSelectionRotation().getAxis()[0];
} // getRotationX

double EditorLayer::getRotationY() {
	return mEditorBase->getSelectionRotation().getAxis()[1];
} // getRotationY

double EditorLayer::getRotationZ() {
	return mEditorBase->getSelectionRotation().getAxis()[2];
} // getRotationZ

double EditorLayer::getRotationA() {
	return gmtl::Math::rad2Deg(mEditorBase->getSelectionRotation().getAngle());
} // getRotationA

double EditorLayer::getScaleX() {
	return mEditorBase->getSelectionScale()[0];
} // getScaleX

double EditorLayer::getScaleY() {
	return mEditorBase->getSelectionScale()[1];
} //getScaleY

double EditorLayer::getScaleZ() {
	return mEditorBase->getSelectionScale()[2];
} //getScaleZ

bool EditorLayer::getFixed() {
	return mEditorBase->getFixed();
} // getFixed

bool EditorLayer::getRepCopy() {
	return mEditorBase->getRepCopy();
} // getRepCopy

QString EditorLayer::getName() {
	return QString::fromLocal8Bit(mEditorBase->getName().c_str());
} // getName

int EditorLayer::getPositionX() {
	return mEditorBase->getPositionX();
} //getPositionX

int EditorLayer::getPositionZ() {
	return mEditorBase->getPositionZ();
} // getPositionZ

QString EditorLayer::getModelFilename() {
	return QString::fromLocal8Bit(mEditorBase->getModelFilename().c_str());
} // getModelFilename

QStringList EditorLayer::getModelFilenames() {
	std::vector < std::string > strings = mEditorBase->getModelFilenames("entities");
	QStringList stringList;
	for (std::vector<std::string>::iterator stringIt = strings.begin(); stringIt != strings.end(); ++stringIt) {
		stringList.insert(0, QString::fromLocal8Bit(stringIt->c_str()));
	} // for

	return stringList;
} // getModelFilenames

QString EditorLayer::getXmlFilename() {
	return QString::fromLocal8Bit(mEditorBase->getXmlFilename().c_str());
} //getXmlFilename

QStringList EditorLayer::getEnvironmentIds() {
	QStringList envList;

	std::vector<Environment *> environments = mEditorBase->getEnvironmentList();

	for (std::vector<Environment *>::iterator environmentIt = environments.begin(); environmentIt != environments.end(); ++environmentIt) {
		envList << QString::number((*environmentIt)->getId());
	} // for
	return envList;
} // getEnvironmentIds

void EditorLayer::newConfiguration(QString invrsPath, QString projectPath) {
	mEditorBase->newConfiguration(qPrintable(invrsPath), qPrintable(projectPath));
	std::ostringstream ss;
	ss << qPrintable(projectPath) << "/config/general.xml";
	loadConfiguration(ss.str());
} // newConfiguration

void EditorLayer::setTranslationX(double value) {
	gmtl::Vec3f tmpVector = mEditorBase->getSelectionTranslation();
	tmpVector[0] = value;
	mEditorBase->setSelectionTranslation(tmpVector);
	mOpenSGWidgetMain->update();
	mOpenSGWidgetPreview->update();
} // setTranslationX

void EditorLayer::setTranslationY(double value) {
	gmtl::Vec3f tmpVector = mEditorBase->getSelectionTranslation();
	tmpVector[1] = value;
	mEditorBase->setSelectionTranslation(tmpVector);
	mOpenSGWidgetMain->update();
	mOpenSGWidgetPreview->update();
} // setTranslationY

void EditorLayer::setTranslationZ(double value) {
	gmtl::Vec3f tmpVector = mEditorBase->getSelectionTranslation();
	tmpVector[2] = value;
	mEditorBase->setSelectionTranslation(tmpVector);
	mOpenSGWidgetMain->update();
	mOpenSGWidgetPreview->update();
} // setTranslationZ

void EditorLayer::setRotationX(double value) {
	gmtl::AxisAnglef tmpAxisAngle = mEditorBase->getSelectionRotation();
	gmtl::Vec3f tmpVector = tmpAxisAngle.getAxis();
	tmpVector[0] = value;
	tmpAxisAngle.setAxis(tmpVector);
	mEditorBase->setSelectionRotation(tmpAxisAngle);
	mOpenSGWidgetMain->update();
	mOpenSGWidgetPreview->update();
} // setRotationX

void EditorLayer::setRotationY(double value) {
	gmtl::AxisAnglef tmpAxisAngle = mEditorBase->getSelectionRotation();
	gmtl::Vec3f tmpVector = tmpAxisAngle.getAxis();
	tmpVector[1] = value;
	tmpAxisAngle.setAxis(tmpVector);
	mEditorBase->setSelectionRotation(tmpAxisAngle);
	mOpenSGWidgetMain->update();
	mOpenSGWidgetPreview->update();
} // setRotationY

void EditorLayer::setRotationZ(double value) {
	gmtl::AxisAnglef tmpAxisAngle = mEditorBase->getSelectionRotation();
	gmtl::Vec3f tmpVector = tmpAxisAngle.getAxis();
	tmpVector[2] = value;
	tmpAxisAngle.setAxis(tmpVector);
	mEditorBase->setSelectionRotation(tmpAxisAngle);
	mOpenSGWidgetMain->update();
	mOpenSGWidgetPreview->update();
} // setRotationZ

void EditorLayer::setRotationA(double value) {
	gmtl::AxisAnglef tmpAxisAngle = mEditorBase->getSelectionRotation();
	tmpAxisAngle.setAngle(gmtl::Math::deg2Rad(value));
	mEditorBase->setSelectionRotation(tmpAxisAngle);
	mOpenSGWidgetMain->update();
	mOpenSGWidgetPreview->update();
} // setRotationA

void EditorLayer::setScaleX(double value) {
	gmtl::Vec3f tmpVector = mEditorBase->getSelectionScale();
	tmpVector[0] = value;
	mEditorBase->setSelectionScale(tmpVector);
	mOpenSGWidgetMain->update();
	mOpenSGWidgetPreview->update();
} // setTranslationX

void EditorLayer::setScaleY(double value) {
	gmtl::Vec3f tmpVector = mEditorBase->getSelectionScale();
	tmpVector[1] = value;
	mEditorBase->setSelectionScale(tmpVector);
	mOpenSGWidgetMain->update();
	mOpenSGWidgetPreview->update();
} // setTranslationY

void EditorLayer::setScaleZ(double value) {
	gmtl::Vec3f tmpVector = mEditorBase->getSelectionScale();
	tmpVector[2] = value;
	mEditorBase->setSelectionScale(tmpVector);
	mOpenSGWidgetMain->update();
	mOpenSGWidgetPreview->update();
} // setTranslationZ

void EditorLayer::setPositionX(int value) {
	mEditorBase->setPositionX(value);
	mOpenSGWidgetMain->update();
} // setPositionX

void EditorLayer::setPositionZ(int value) {
	mEditorBase->setPositionZ(value);
	mOpenSGWidgetMain->update();
} // setPositionZ

void EditorLayer::setFixed(bool value) {
	mEditorBase->setFixed(value);
} // setFixed

void EditorLayer::setRepCopy(bool value) {
	mEditorBase->setRepCopy(value);
} // setRepCopy

void EditorLayer::setName(QString value) {
	Selection *tmpSelection = mEditorBase->getSelection();
	if (tmpSelection->type == ENTITYTYPE) {
		std::string tmpString;

		tmpString = toString(tmpSelection->entityType->getId()) + " - ";
		tmpString += tmpSelection->entityType->getName();

		QList<QStandardItem *> lIEntityTypes = mModelVirtualEnvironment->findItems(QString::fromLocal8Bit(tmpString.c_str()), Qt::MatchExactly | Qt::MatchRecursive);

		mEditorBase->setName(qPrintable(value));

		foreach (QStandardItem* item, lIEntityTypes)
			{
				tmpString = toString(tmpSelection->entityType->getId());
				tmpString += " - " + tmpSelection->entityType->getName();
				item->setText(QString::fromLocal8Bit(tmpString.c_str()));
			} // foreach
	} // if
} // setName

void EditorLayer::setModelFilename(QString value) {
	mEditorBase->setModelFilename(qPrintable(value));

	mOpenSGWidgetMain->update();
	mOpenSGWidgetPreview->update();
} // setModelFilename

void EditorLayer::setXmlFilename(QString value) {
	mEditorBase->setXmlFilename(qPrintable(value));
} // setXmlFilename

void EditorLayer::addEntryPoint(std::vector<double> values) {
	if (values.size() == 6) {
		gmtl::Vec3f position;
		gmtl::Vec3f direction;
		position[0] = values[0];
		position[1] = values[1];
		position[2] = values[2];
		direction[0] = values[3];
		direction[1] = values[4];
		direction[2] = values[5];
		mEditorBase->addEntryPoint(position, direction);
		updateModelEntryPoints();
	} // if
} // addEntryPoint

void EditorLayer::deleteEntryPoint(unsigned index) {
	mEditorBase->deleteEntryPoint(index);
	updateModelEntryPoints();
} // deleteEntryPoint

void EditorLayer::initModelVirtualEnvironment() {
	std::string tmpString;
	Selection *tmpSelection;
	QVariant tmpVariant;

	std::vector<Environment *> environments = mEditorBase->getEnvironmentList();
	std::vector<EntityType *> entityTypes = mEditorBase->getEntityTypeList();

	mModelVirtualEnvironment->clear();

	//create environment layout
	QStandardItem *iEnvironmentLayout = new QStandardItem();
	iEnvironmentLayout->setEditable(false);
	iEnvironmentLayout->setText("Environment Layout");
	tmpSelection = new Selection();
	tmpSelection->type = NONE;
	tmpSelection->location = 0;
	tmpVariant.setValue(tmpSelection);
	iEnvironmentLayout->setData(tmpVariant);

	for (std::vector<Environment *>::iterator environmentIt = environments.begin(); environmentIt != environments.end(); ++environmentIt) {

		QStandardItem *iEnvironment = new QStandardItem();
		iEnvironment->setEditable(false);
		tmpString = "Environment " + toString((*environmentIt)->getId());
		iEnvironment->setText(QString::fromLocal8Bit(tmpString.c_str()));
		tmpSelection = new Selection();
		tmpSelection->type = ENVIRONMENT;
		tmpSelection->environment = (*environmentIt);
		tmpSelection->location = (*environmentIt)->getId();
		tmpVariant.setValue(tmpSelection);
		iEnvironment->setData(tmpVariant);

		QStandardItem *iEntityTypes = new QStandardItem();
		iEntityTypes->setEditable(false);
		iEntityTypes->setText("Entity Types");
		tmpSelection = new Selection();
		tmpSelection->type = ENTITYTYPES;
		tmpSelection->location = (*environmentIt)->getId();
		tmpVariant.setValue(tmpSelection);
		iEntityTypes->setData(tmpVariant);

		for (std::vector<EntityType *>::iterator entityTypeIt = entityTypes.begin(); entityTypeIt != entityTypes.end(); ++entityTypeIt) {

			QStandardItem *iEntityType = new QStandardItem();
			iEntityType->setEditable(false);
			tmpString = toString((*entityTypeIt)->getId()) + " - " + (*entityTypeIt)->getName();
			iEntityType->setText(QString::fromLocal8Bit(tmpString.c_str()));
			tmpSelection = new Selection();
			tmpSelection->type = ENTITYTYPE;
			tmpSelection->entityType = (*entityTypeIt);
			tmpSelection->location = (*environmentIt)->getId();
			tmpVariant.setValue(tmpSelection);
			iEntityType->setData(tmpVariant);

			std::vector<Entity *> entities = (*entityTypeIt)->getInstanceList();
			for (std::vector<Entity *>::iterator entityIt = entities.begin(); entityIt != entities.end(); ++entityIt) {

				if ((*entityIt)->getEnvironment()->getId() == (*environmentIt)->getId()) {
					QStandardItem *iEntity = new QStandardItem();
					iEntity->setEditable(false);
					tmpString = "Entity " + toString((*entityIt)->getEnvironmentBasedId() & 65535);
					iEntity->setText(QString::fromLocal8Bit(tmpString.c_str()));
					tmpSelection = new Selection();
					tmpSelection->type = ENTITY;
					tmpSelection->entity = (*entityIt);
					tmpSelection->location = (*environmentIt)->getId();
					tmpVariant.setValue(tmpSelection);
					iEntity->setData(tmpVariant);
					iEntityType->appendRow(iEntity);
				} // if
			} // for

			if (iEntityType->hasChildren()) {
				iEntityTypes->appendRow(iEntityType);
			} // if
		} // for

		iEnvironment->appendRow(iEntityTypes);
		iEnvironmentLayout->appendRow(iEnvironment);
	} // for

	//create database
	QStandardItem *iDatabase = new QStandardItem();
	iDatabase->setEditable(false);
	iDatabase->setText("Database");
	tmpSelection = new Selection();
	tmpSelection->type = NONE;
	tmpSelection->location = -1;
	tmpVariant.setValue(tmpSelection);
	iDatabase->setData(tmpVariant);

	QStandardItem *iEntityTypes = new QStandardItem();
	iEntityTypes->setEditable(false);
	iEntityTypes->setText("Entity Types");
	tmpSelection = new Selection();
	tmpSelection->type = NONE;
	tmpSelection->location = -1;
	tmpVariant.setValue(tmpSelection);
	iEntityTypes->setData(tmpVariant);
	iDatabase->appendRow(iEntityTypes);

	for (std::vector<EntityType *>::iterator entTypeIt = entityTypes.begin(); entTypeIt != entityTypes.end(); ++entTypeIt) {

		QStandardItem *iEntityType = new QStandardItem();
		iEntityType->setEditable(false);
		tmpString = toString((*entTypeIt)->getId()) + " - " + (*entTypeIt)->getName();
		iEntityType->setText(QString::fromLocal8Bit(tmpString.c_str()));
		tmpSelection = new Selection();
		tmpSelection->type = ENTITYTYPE;
		tmpSelection->entityType = (*entTypeIt);
		tmpSelection->location = -1;
		tmpVariant.setValue(tmpSelection);
		iEntityType->setData(tmpVariant);
		iEntityTypes->appendRow(iEntityType);
	} // for

	QStandardItem *iVirtualEnvironment = new QStandardItem("Virtual Environment");
	mModelVirtualEnvironment->setHorizontalHeaderItem(0, iVirtualEnvironment);
	mModelVirtualEnvironment->appendRow(iEnvironmentLayout);
	mModelVirtualEnvironment->appendRow(iDatabase);
} // initModelVirtualEnvironment

void EditorLayer::updateModelProperties() {
	mModelProperties->clear();

	QStringList tmpStringList;
	QStandardItem *tmpItem;

	if (isSelectionEntity()) {
		tmpStringList.append("InstanceId");
		tmpItem = new QStandardItem(QString::fromLocal8Bit(toString(mEditorBase->getSelection()->entity->getInstanceId()).c_str()));
		tmpItem->setEditable(false);
		mModelProperties->appendRow(tmpItem);

		tmpStringList.append("TypeBasedId");
		tmpItem = new QStandardItem(QString::fromLocal8Bit(toString(mEditorBase->getSelection()->entity->getTypeBasedId() & 65535).c_str()));
		tmpItem->setEditable(false);
		mModelProperties->appendRow(tmpItem);

		tmpStringList.append("EnvironmentBasedId");
		tmpItem = new QStandardItem(QString::fromLocal8Bit(toString(mEditorBase->getSelection()->entity->getEnvironmentBasedId() & 65535).c_str()));
		tmpItem->setEditable(false);
		mModelProperties->appendRow(tmpItem);

		mModelProperties->setVerticalHeaderLabels(tmpStringList);
	} else if (isSelectionEntityType()) {
		tmpStringList.append("id");
		tmpItem = new QStandardItem(QString::fromLocal8Bit(toString(mEditorBase->getSelection()->entityType->getId()).c_str()));
		tmpItem->setEditable(false);
		mModelProperties->appendRow(tmpItem);

		tmpStringList.append("File Path");
		tmpItem = new QStandardItem(QString::fromLocal8Bit(toString(mEditorBase->getSelection()->entityType->getModel()->getFilePath()).c_str()));
		tmpItem->setEditable(false);
		mModelProperties->appendRow(tmpItem);

		tmpStringList.append("File Type");
		std::string tmpString = mEditorBase->getSelection()->entityType->getModel()->getFilePath();
		tmpString = tmpString.substr(tmpString.find_last_of(".") + 1);
		if (tmpString == "wrl")
			tmpString = "VRML";
		tmpItem = new QStandardItem(QString::fromLocal8Bit(tmpString.c_str()));
		tmpItem->setEditable(false);
		mModelProperties->appendRow(tmpItem);

		mModelProperties->setVerticalHeaderLabels(tmpStringList);
	} else if (isSelectionEnvironment()) {
		tmpStringList.append("Id");
		tmpItem = new QStandardItem(QString::fromLocal8Bit(toString(mEditorBase->getSelection()->environment->getId()).c_str()));
		tmpItem->setEditable(false);
		mModelProperties->appendRow(tmpItem);

		tmpStringList.append("XScacing");
		tmpItem = new QStandardItem(QString::fromLocal8Bit(toString(mEditorBase->getSelection()->environment->getXSpacing()).c_str()));
		tmpItem->setEditable(false);
		mModelProperties->appendRow(tmpItem);

		tmpStringList.append("ZScacing");
		tmpItem = new QStandardItem(QString::fromLocal8Bit(toString(mEditorBase->getSelection()->environment->getZSpacing()).c_str()));
		tmpItem->setEditable(false);
		mModelProperties->appendRow(tmpItem);

		tmpStringList.append("XSize");
		tmpItem = new QStandardItem(QString::fromLocal8Bit(toString(mEditorBase->getSelection()->environment->getXSize()).c_str()));
		tmpItem->setEditable(false);
		mModelProperties->appendRow(tmpItem);

		tmpStringList.append("ZSize");
		tmpItem = new QStandardItem(QString::fromLocal8Bit(toString(mEditorBase->getSelection()->environment->getZSize()).c_str()));
		tmpItem->setEditable(false);
		mModelProperties->appendRow(tmpItem);
		mModelProperties->setVerticalHeaderLabels(tmpStringList);
	} // else if

	tmpStringList.clear();
	tmpStringList.append("Value");
	mModelProperties->setHorizontalHeaderLabels(tmpStringList);
} // updateModelProperties


void EditorLayer::updateModelEntryPoints() {
	mModelEntryPoints->clear();

	QStandardItem *tmpItem;
	QList<QStandardItem *> tmpRow;
	QStringList tmpStringList;

	if (isSelectionEnvironment()) {
		int cnt = mEditorBase->getSelection()->environment->getNumberOfStartTransformations();

		for (int i = 0; i < cnt; ++i) {
			TransformationData transData = mEditorBase->getSelection()-> environment->getStartTransformation(i);

			gmtl::Vec3f direction;
			gmtl::Vec3f front(0, 0, -1);
			direction = transData.orientation * front;

			tmpRow.clear();
			tmpItem = new QStandardItem(QString::fromLocal8Bit(toString(transData.position[0]).c_str()));
			tmpItem->setEditable(false);
			tmpRow.append(tmpItem);
			tmpItem = new QStandardItem(QString::fromLocal8Bit(toString(transData.position[1]).c_str()));
			tmpItem->setEditable(false);
			tmpRow.append(tmpItem);
			tmpItem = new QStandardItem(QString::fromLocal8Bit(toString(transData.position[2]).c_str()));
			tmpItem->setEditable(false);
			tmpRow.append(tmpItem);
			tmpItem = new QStandardItem(QString::fromLocal8Bit(toString(direction[0]).c_str()));
			tmpItem->setEditable(false);
			tmpRow.append(tmpItem);
			tmpItem = new QStandardItem(QString::fromLocal8Bit(toString(direction[1]).c_str()));
			tmpItem->setEditable(false);
			tmpRow.append(tmpItem);
			tmpItem = new QStandardItem(QString::fromLocal8Bit(toString(direction[2]).c_str()));
			tmpItem->setEditable(false);
			tmpRow.append(tmpItem);

			mModelEntryPoints->appendRow(tmpRow);

		} // for
	} // if

	tmpStringList.append("xPos");
	tmpStringList.append("yPos");
	tmpStringList.append("zPos");
	tmpStringList.append("XDir");
	tmpStringList.append("yDir");
	tmpStringList.append("zDir");
	mModelEntryPoints->setHorizontalHeaderLabels(tmpStringList);

} // updateModelEntryPoints
