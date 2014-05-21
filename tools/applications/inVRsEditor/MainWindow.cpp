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

#include "MainWindow.h"

MainWindow::MainWindow() {
mEditorLayer = new EditorLayer();

	mNewVirtualEnvironmentDialog = new NewVirtualEnvironmentDialog(this);
	mNewEnvironmentDialog = new NewEnvironmentDialog(this);
setCentralWidget(mEditorLayer->getOpenSGWidgetMain());

createDockWindows();
createActions();
createMenus();
createStatusBar();
createConnections();

setWindowIcon(QIcon(":/images/inVRs_logo_300.png"));
} // MainWindow

MainWindow::~MainWindow() {
delete mEditorLayer;
} // ~MainWindow

void MainWindow::open() {
	fileName = QFileDialog::getOpenFileName(this, "Open config.xml", ".",
			"XML Files(*.xml)");

	// if a filename was chosen, try to load it:
if (! fileName.isEmpty()
		&& ! mEditorLayer->loadConfiguration(qPrintable(fileName)))
{
	QMessageBox::warning(this,"inVrs Editor",
			QString("The configuration file <em>%1</em> could not be loaded!").arg(fileName));
}
updateStatusBar();
} //open

void MainWindow::save(){
mEditorLayer->saveConfiguration();
} // save

void MainWindow::about() {
QMessageBox::about(this, "About inVRsEditor",
	"<h2>inVRsEditor</h2>"
	"<p><a href=\"http://www.invrs.org\">www.invrs.org</a>"
		"<p><a href=\"mailto:gwoess@invrs.org\">gwoess@invrs.org</a>");
} // about

void MainWindow::processSelection(QModelIndex current, QModelIndex previous) {
mEditorLayer->processSelection(current);

if(mEditorLayer->isSelectionEntity()){
		mEntityProperties->setTranslationX(mEditorLayer->getTranslationX());
		mEntityProperties->setTranslationY(mEditorLayer->getTranslationY());
		mEntityProperties->setTranslationZ(mEditorLayer->getTranslationZ());
		mEntityProperties->setRotationX(mEditorLayer->getRotationX());
		mEntityProperties->setRotationY(mEditorLayer->getRotationY());
		mEntityProperties->setRotationZ(mEditorLayer->getRotationZ());
		mEntityProperties->setRotationA(mEditorLayer->getRotationA());
		mEntityProperties->setScaleX(mEditorLayer->getScaleX());
		mEntityProperties->setScaleY(mEditorLayer->getScaleY());
		mEntityProperties->setScaleZ(mEditorLayer->getScaleZ());

		mEntityProperties->setFixed(mEditorLayer->getFixed());
		mEntityProperties->updateTableView();
		mDockWidgetProperties->setWidget(mEntityProperties);
}else if(mEditorLayer->isSelectionEntityType()){
		mEntityTypeProperties->updateModelFilenames(mEditorLayer->getModelFilenames(),
				mEditorLayer->getModelFilename());

		mEntityTypeProperties->setTranslationX(mEditorLayer->getTranslationX());
		mEntityTypeProperties->setTranslationY(mEditorLayer->getTranslationY());
		mEntityTypeProperties->setTranslationZ(mEditorLayer->getTranslationZ());
		mEntityTypeProperties->setRotationX(mEditorLayer->getRotationX());
		mEntityTypeProperties->setRotationY(mEditorLayer->getRotationY());
		mEntityTypeProperties->setRotationZ(mEditorLayer->getRotationZ());
		mEntityTypeProperties->setRotationA(mEditorLayer->getRotationA());
		mEntityTypeProperties->setScaleX(mEditorLayer->getScaleX());
		mEntityTypeProperties->setScaleY(mEditorLayer->getScaleY());
		mEntityTypeProperties->setScaleZ(mEditorLayer->getScaleZ());

		mEntityTypeProperties->setFixed(mEditorLayer->getFixed());
		mEntityTypeProperties->setRepCopy(mEditorLayer->getRepCopy());
		mEntityTypeProperties->setName(mEditorLayer->getName());
		mEntityTypeProperties->setXmlFilename(mEditorLayer->getXmlFilename());
		mEntityTypeProperties->updateTableView();
	mDockWidgetProperties->setWidget(mEntityTypeProperties);
}else if(mEditorLayer->isSelectionEnvironment()){
		mEnvironmentProperties->setPositionX(mEditorLayer->getPositionX());
		mEnvironmentProperties->setPositionZ(mEditorLayer->getPositionZ());
		mEnvironmentProperties->setXmlFilename(mEditorLayer->getXmlFilename());

		mEnvironmentProperties->updateTableView();
	mDockWidgetProperties->setWidget(mEnvironmentProperties);
}else{
	mDockWidgetProperties->setWidget(mEmptyProperties);
} // else
} // processSelection

void MainWindow::showAllMain(){
mEditorLayer->showAllMain();
} // showAllMain

void MainWindow::showAllPreview(){
mEditorLayer->showAllPreview();
} // showAllPreview

void MainWindow::newEntity(){
	if(mEditorLayer->isSelectionEntity() || mEditorLayer->isSelectionEntityType()) {
		bool ok = false;
		QString item = QInputDialog::getItem(this, "Select Environment", "Select an Environment",
				mEditorLayer->getEnvironmentIds(), 0, false, &ok);

		if(ok && !item.isEmpty()) {
			QModelIndex index = mEditorLayer->newEntity(item);
			if(index.isValid()) {
				mTreeViewVirtualEnvironment->setCurrentIndex(index);
			} // if
		} // if
	} // if
} // newEntity

void MainWindow::newEntityType(){
bool ok = false;
	QString name = QInputDialog::getText(this, "New EntityType", "Name of new EntityType",
			QLineEdit::Normal, "", &ok);

	if(ok) {
		mTreeViewVirtualEnvironment->setCurrentIndex(mEditorLayer->newEntityType(name));
	} // if
} // newEntityType

void MainWindow::newEnvironment(int spacingX, int spacingZ, int sizeX, int sizeZ) {
	QModelIndex index = mEditorLayer->newEnvironment(spacingX, spacingZ, sizeX, sizeZ);

	if(index.isValid()){
		mTreeViewVirtualEnvironment->setCurrentIndex(index);
	}else{
	QMessageBox::warning(this,"inVrs Editor", "Create new Environment failed!");
	} // else
} // newEnvironment

void MainWindow::deleteSelection() {
mEditorLayer->deleteSelection();
} // deleteSelection

void MainWindow::createConnections() {
connect(mActionNew, SIGNAL(triggered()), mNewVirtualEnvironmentDialog, SLOT(show()));
connect(mActionOpen, SIGNAL(triggered()), this, SLOT(open()));
connect(mActionSave, SIGNAL(triggered()), this, SLOT(save()));
connect(mActionExit, SIGNAL(triggered()), this, SLOT(close()));
connect(mActionAbout, SIGNAL(triggered()), this, SLOT(about()));
connect(mActionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

connect(mActionShowVirtualEnvironment, SIGNAL(toggled(bool)),
	mDockWidgetVirtualEnvironment, SLOT(setVisible(bool)));
connect(mDockWidgetVirtualEnvironment, SIGNAL(visibilityChanged(bool)),
	mActionShowVirtualEnvironment, SLOT(setChecked(bool)));

connect(mActionShowProperties, SIGNAL(toggled(bool)),
	mDockWidgetProperties, SLOT(setVisible(bool)));
connect(mDockWidgetProperties, SIGNAL(visibilityChanged(bool)),
	mActionShowProperties, SLOT(setChecked(bool)));

connect(mActionShowPreview, SIGNAL(toggled(bool)),
	mDockWidgetPreview, SLOT(setVisible(bool)));
connect(mDockWidgetPreview, SIGNAL(visibilityChanged(bool)),
	mActionShowPreview, SLOT(setChecked(bool)));

connect(mTreeViewVirtualEnvironment->selectionModel(),
			SIGNAL(currentChanged(QModelIndex, QModelIndex)),
			this, SLOT(processSelection(QModelIndex, QModelIndex)));

	connect(mEntityProperties, SIGNAL(translationXChanged(double)),
			mEditorLayer, SLOT(setTranslationX(double)));
	connect(mEntityProperties, SIGNAL(translationYChanged(double)),
			mEditorLayer, SLOT(setTranslationY(double)));
	connect(mEntityProperties, SIGNAL(translationZChanged(double)),
			mEditorLayer, SLOT(setTranslationZ(double)));
	connect(mEntityProperties, SIGNAL(rotationXChanged(double)),
			mEditorLayer, SLOT(setRotationX(double)));
	connect(mEntityProperties, SIGNAL(rotationYChanged(double)),
			mEditorLayer, SLOT(setRotationY(double)));
	connect(mEntityProperties, SIGNAL(rotationZChanged(double)),
			mEditorLayer, SLOT(setRotationZ(double)));
	connect(mEntityProperties, SIGNAL(rotationAChanged(double)),
			mEditorLayer, SLOT(setRotationA(double)));
	connect(mEntityProperties, SIGNAL(scaleXChanged(double)),
			mEditorLayer, SLOT(setScaleX(double)));
	connect(mEntityProperties, SIGNAL(scaleYChanged(double)),
			mEditorLayer, SLOT(setScaleY(double)));
	connect(mEntityProperties, SIGNAL(scaleZChanged(double)),
			mEditorLayer, SLOT(setScaleZ(double)));
	connect(mEntityProperties, SIGNAL(fixedChanged(bool)),
			mEditorLayer, SLOT(setFixed(bool)));

connect(mEntityTypeProperties, SIGNAL(translationXChanged(double)),
			mEditorLayer, SLOT(setTranslationX(double)));
	connect(mEntityTypeProperties, SIGNAL(translationYChanged(double)),
			mEditorLayer, SLOT(setTranslationY(double)));
	connect(mEntityTypeProperties, SIGNAL(translationZChanged(double)),
			mEditorLayer, SLOT(setTranslationZ(double)));
	connect(mEntityTypeProperties, SIGNAL(rotationXChanged(double)),
			mEditorLayer, SLOT(setRotationX(double)));
	connect(mEntityTypeProperties, SIGNAL(rotationYChanged(double)),
			mEditorLayer, SLOT(setRotationY(double)));
	connect(mEntityTypeProperties, SIGNAL(rotationZChanged(double)),
			mEditorLayer, SLOT(setRotationZ(double)));
	connect(mEntityTypeProperties, SIGNAL(rotationAChanged(double)),
			mEditorLayer, SLOT(setRotationA(double)));
	connect(mEntityTypeProperties, SIGNAL(scaleXChanged(double)),
			mEditorLayer, SLOT(setScaleX(double)));
	connect(mEntityTypeProperties, SIGNAL(scaleYChanged(double)),
			mEditorLayer, SLOT(setScaleY(double)));
	connect(mEntityTypeProperties, SIGNAL(scaleZChanged(double)),
			mEditorLayer, SLOT(setScaleZ(double)));
	connect(mEntityTypeProperties, SIGNAL(fixedChanged(bool)),
	  mEditorLayer, SLOT(setFixed(bool)));
connect(mEntityTypeProperties, SIGNAL(repCopyChanged(bool)),
			mEditorLayer, SLOT(setRepCopy(bool)));
	connect(mEntityTypeProperties, SIGNAL(modelFilenameChanged(QString)),
			mEditorLayer, SLOT(setModelFilename(QString)));
	connect(mEntityTypeProperties, SIGNAL(xmlFilenameChanged(QString)),
			mEditorLayer, SLOT(setXmlFilename(QString)));
	connect(mEntityTypeProperties, SIGNAL(nameChanged(QString)),
			mEditorLayer, SLOT(setName(QString)));

	connect(mEnvironmentProperties, SIGNAL(positionXChanged(int)),
			mEditorLayer, SLOT(setPositionX(int)));
	connect(mEnvironmentProperties, SIGNAL(positionZChanged(int)),
			mEditorLayer, SLOT(setPositionZ(int)));
	connect(mEnvironmentProperties, SIGNAL(xmlFilenameChanged(QString)),
			mEditorLayer, SLOT(setXmlFilename(QString)));
	connect(mEnvironmentProperties, SIGNAL(entryPointAdded(std::vector<double>)),
			mEditorLayer, SLOT(addEntryPoint(std::vector<double>)));
	connect(mEnvironmentProperties, SIGNAL(entryPointDeleted(unsigned)),
			mEditorLayer, SLOT(deleteEntryPoint(unsigned)));

connect(mButtonShowAllMain, SIGNAL(clicked()), this, SLOT(showAllMain()));
connect(mButtonShowAllPreview, SIGNAL(clicked()), this, SLOT(showAllPreview()));

connect(mButtonVirtualEnvAdd, SIGNAL(clicked()), this, SLOT(newEntity()));
connect(mButtonVirtualEnvDelete, SIGNAL(clicked()), this, SLOT(deleteSelection()));

connect(mActionNewEntityType, SIGNAL(triggered()), this, SLOT(newEntityType()));
connect(mActionNewEnvironment, SIGNAL(triggered()),
			mNewEnvironmentDialog, SLOT(show()));

	connect(mNewVirtualEnvironmentDialog, SIGNAL(paths(QString, QString)),
			mEditorLayer, SLOT(newConfiguration(QString, QString)));
	connect(mNewEnvironmentDialog, SIGNAL(values(int, int, int, int)),
			this, SLOT(newEnvironment(int, int, int, int)));
} // createConnections

void MainWindow::createActions() {
mActionNew = new QAction("&New", this);
mActionNew->setShortcut(QKeySequence::New);
mActionNew->setStatusTip("Create a new file");

mActionOpen = new QAction("&Open", this);
mActionOpen->setShortcut(QKeySequence::Open);
mActionOpen->setStatusTip("Open a file");

mActionSave = new QAction("&Save", this);
mActionSave->setShortcut(QKeySequence::Save);
mActionSave->setStatusTip("Save file");

mActionExit = new QAction("E&xit", this);
#if QT_VERSION >= 0x040600
// QKeySequence::Quit was introduced in qt 4.6:
mActionExit->setShortcut(QKeySequence::Quit);
#endif
mActionExit->setStatusTip("Exit the application");

mActionNewEntityType = new QAction("New EntityType", this);
mActionNewEntityType->setStatusTip("Create a new EntityType");

mActionNewEnvironment = new QAction("New Environment", this);
mActionNewEnvironment->setStatusTip("Create a new Environment");

mActionAbout = new QAction("About", this);
mActionAbout->setStatusTip("About inVRsEditor");

mActionAboutQt = new QAction("About &Qt", this);
mActionAboutQt->setStatusTip("Show the Qt library's About box");

mActionShowVirtualEnvironment = new QAction("Show Virtual Environment", this);
mActionShowVirtualEnvironment->setCheckable(true);
mActionShowVirtualEnvironment->setChecked(true);
mActionShowVirtualEnvironment->setStatusTip("Show TreeView of the inVRs Scene and Database");

mActionShowProperties = new QAction("Show Properties", this);
mActionShowProperties->setCheckable(true);
mActionShowProperties->setChecked(true);
mActionShowProperties->setStatusTip("Show Properties");

mActionShowPreview = new QAction("Show Preview", this);
mActionShowPreview->setCheckable(true);
mActionShowPreview->setChecked(true);
mActionShowPreview->setStatusTip("Show Preview");
} // createActions

void MainWindow::createMenus() {
mMenuFile = menuBar()->addMenu("&File");
mMenuFile->addAction(mActionNew);
mMenuFile->addAction(mActionOpen);
mMenuFile->addAction(mActionSave);
mMenuFile->addSeparator();
mMenuFile->addAction(mActionExit);

menuBar()->addSeparator();

mMenuEdit = menuBar()->addMenu("&Edit");
mMenuEdit->addAction(mActionNewEntityType);
mMenuEdit->addAction(mActionNewEnvironment);

mMenuView = menuBar()->addMenu("&View");
mMenuView->addAction(mActionShowVirtualEnvironment);
mMenuView->addAction(mActionShowProperties);
mMenuView->addAction(mActionShowPreview);

mMenuHelp = menuBar()->addMenu("&Help");
mMenuHelp->addAction(mActionAboutQt);
mMenuHelp->addAction(mActionAbout);
} // createMenus

void MainWindow::createStatusBar() {
mLabelFileName = new QLabel("new file");
mLabelFileName->setAlignment(Qt::AlignHCenter);
mLabelFileName->setMinimumSize(mLabelFileName->sizeHint());

statusBar()->addWidget(mLabelFileName);

updateStatusBar();
} // createStatusBar

void MainWindow::createDockWindows() {
// Virtual Environment (TreeView)
	mDockWidgetVirtualEnvironment = new QDockWidget("Virtual Environment");
QWidget *widgetTrees = new QWidget();
QVBoxLayout *vBoxLayoutTrees = new QVBoxLayout();
mButtonVirtualEnvAdd = new QPushButton("Add");
mButtonVirtualEnvDelete = new QPushButton("Delete");
mButtonShowAllMain = new QPushButton("Show all MainWindow");

QHBoxLayout *hBoxLayoutVirtualEnv = new QHBoxLayout();
hBoxLayoutVirtualEnv->addWidget(mButtonVirtualEnvAdd);
hBoxLayoutVirtualEnv->addWidget(mButtonVirtualEnvDelete);

mTreeViewVirtualEnvironment = new QTreeView();
mTreeViewVirtualEnvironment->setModel(mEditorLayer->getModelVirtualEnvironment());

vBoxLayoutTrees->addWidget(mTreeViewVirtualEnvironment);
vBoxLayoutTrees->addItem(hBoxLayoutVirtualEnv);
vBoxLayoutTrees->addWidget(mButtonShowAllMain);

widgetTrees->setLayout(vBoxLayoutTrees);
mDockWidgetVirtualEnvironment->setWidget(widgetTrees);
mDockWidgetVirtualEnvironment->setAllowedAreas(Qt::LeftDockWidgetArea|
	Qt::RightDockWidgetArea);
addDockWidget(Qt::LeftDockWidgetArea, mDockWidgetVirtualEnvironment);

	// Preview
mDockWidgetPreview = new QDockWidget("Preview");
	QWidget *widgetPreview = new QWidget();
	QVBoxLayout *vBoxLayoutPreview = new QVBoxLayout();
mButtonShowAllPreview = new QPushButton("Show all Preview");

	vBoxLayoutPreview->addWidget(mEditorLayer->getOpenSGWidgetPreview());
	vBoxLayoutPreview->addWidget(mButtonShowAllPreview);
	widgetPreview->setLayout(vBoxLayoutPreview);

	mDockWidgetPreview->setWidget(widgetPreview);
mDockWidgetPreview->setAllowedAreas(Qt::LeftDockWidgetArea|
	  Qt::RightDockWidgetArea);
addDockWidget(Qt::LeftDockWidgetArea, mDockWidgetPreview);

	// Properties
mDockWidgetProperties = new QDockWidget("Properties");
mDockWidgetProperties->setAllowedAreas(Qt::LeftDockWidgetArea |
	Qt::RightDockWidgetArea);
addDockWidget(Qt::RightDockWidgetArea, mDockWidgetProperties);
mEmptyProperties = new QWidget();
mDockWidgetProperties->setWidget(mEmptyProperties);

	mEntityProperties = new EntityWidget();
	mEntityTypeProperties = new EntityTypeWidget();
	mEnvironmentProperties = new EnvironmentWidget();

	mEntityProperties->setModelProperties(mEditorLayer->getModelProperties());
	mEntityTypeProperties->setModelProperties(mEditorLayer->getModelProperties());
	mEnvironmentProperties->setModelProperties(mEditorLayer->getModelProperties());
	mEnvironmentProperties->setModelEntryPoints(mEditorLayer->getModelEntryPoints());
} // createDockWindows

void MainWindow::updateStatusBar() {
mLabelFileName->setText(fileName);
} // updateStatusBar
