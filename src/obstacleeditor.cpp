#include "obstacleeditor.h"

/*!
 *
 *
 *	obstacleEditor
 *
 *
 */

obstacleEditor::obstacleEditor(obstacleModel *model, QWidget *parent) : QWidget(parent) {
	// store obstacle model
	_model = model;

	// set size properties
	this->setFixedWidth(256);

	// set up mapper
	_mapper = new QDataWidgetMapper(this);
	_mapper->setModel(_model);
	_mapper->setItemDelegate(new obstacleEditorDelegate(this));
	_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

	// set up editor pages
	_pages = new QStackedWidget;
	_pages->addWidget(new boxEditor(_mapper));
	_pages->addWidget(new cylinderEditor(_mapper));
	_pages->addWidget(new sphereEditor(_mapper));

	// set up buttons
	_deleteButton = new QPushButton(tr("Delete"));
	_deleteButton->setEnabled(true);
	_deleteButton->setFixedWidth(75);
	_nextButton = new QPushButton(tr("Next"));
	_nextButton->setEnabled(false);
	_nextButton->setFixedWidth(75);
	_previousButton = new QPushButton(tr("Previous"));
	_previousButton->setEnabled(false);
	_previousButton->setFixedWidth(75);

	// create signal connections
	QWidget::connect(_deleteButton, SIGNAL(clicked()), this, SLOT(deleteCurrentIndex()));
	QWidget::connect(_nextButton, SIGNAL(clicked()), _mapper, SLOT(toNext()));
	QWidget::connect(_nextButton, SIGNAL(clicked()), this, SLOT(buttonPressed()));
	QWidget::connect(_previousButton, SIGNAL(clicked()), _mapper, SLOT(toPrevious()));
	QWidget::connect(_previousButton, SIGNAL(clicked()), this, SLOT(buttonPressed()));

	// set default units
	_units = false;	// us

	// lay out grid
	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(_pages);
	QHBoxLayout *hbox7 = new QHBoxLayout();
	hbox7->addWidget(_previousButton, 0, Qt::AlignCenter);
	hbox7->addWidget(_deleteButton, 1, Qt::AlignCenter);
	hbox7->addWidget(_nextButton, 0, Qt::AlignCenter);
	vbox->addLayout(hbox7);
	this->setLayout(vbox);

	// go to first item
	this->setCurrentIndex(_mapper->model()->index(0,0));
}

void obstacleEditor::dataChanged(QModelIndex/*topLeft*/, QModelIndex bottomRight) {
	this->setCurrentIndex(bottomRight);
}

void obstacleEditor::setCurrentIndex(const QModelIndex &index) {
	if (index.isValid()) {
		// disable current mappings
		_mapper->clearMapping();

		// load appropriate page
		int form = _model->data(_model->index(index.row(), rsObstacleModel::FORM), Qt::EditRole).toInt();
		switch (form) {
			case rs::BOX:
				_pages->setCurrentIndex(0);
				this->setUnits(_units);
				dynamic_cast<boxEditor *>(_pages->currentWidget())->nullIndex(false);
				break;
			case rs::CYLINDER:
				_pages->setCurrentIndex(1);
				this->setUnits(_units);
				dynamic_cast<cylinderEditor *>(_pages->currentWidget())->nullIndex(false);
				break;
			case rs::SPHERE:
				_pages->setCurrentIndex(2);
				this->setUnits(_units);
				dynamic_cast<sphereEditor *>(_pages->currentWidget())->nullIndex(false);
				break;
		}

		// set new index for mapper
		_mapper->setCurrentIndex(index.row());

		// update button states
		_deleteButton->setEnabled(true);
		_nextButton->setEnabled(index.row() < _mapper->model()->rowCount() - 1);
		_previousButton->setEnabled(index.row() > 0);
	}
	else {
		// disable current page
		if (dynamic_cast<boxEditor *>(_pages->currentWidget()))
			dynamic_cast<boxEditor *>(_pages->currentWidget())->nullIndex(true);
		else if (dynamic_cast<cylinderEditor *>(_pages->currentWidget()))
			dynamic_cast<cylinderEditor *>(_pages->currentWidget())->nullIndex(true);
		else if (dynamic_cast<sphereEditor*>(_pages->currentWidget()))
			dynamic_cast<sphereEditor*>(_pages->currentWidget())->nullIndex(true);

		// disable all buttons
		_deleteButton->setDisabled(true);
		_nextButton->setDisabled(true);
		_previousButton->setDisabled(true);
	}
}

void obstacleEditor::buttonPressed(void) {
	// enable appropriate buttons
	QModelIndex index = _mapper->model()->index(_mapper->currentIndex(), 0);
	_nextButton->setEnabled(index.row() < _mapper->model()->rowCount() - 1);
	_previousButton->setEnabled(index.row() > 0);

	// signal other views that index has changed
	emit indexChanged(index);
}

void obstacleEditor::deleteCurrentIndex(void) {
	// save current index
	int index = _mapper->currentIndex();

	// remove current obstacle from model
	_mapper->model()->removeRows(_mapper->currentIndex(), 1);

	// new index is same row as last one
	_mapper->setCurrentIndex(index);

	// if it is invalid, then set the last row in the model
	if (_mapper->currentIndex() == -1) {
		this->setCurrentIndex(_mapper->model()->index(_mapper->model()->rowCount()-1, rsObstacleModel::ID));
	}

	// signal a change in current obstacle
	emit indexChanged(_mapper->model()->index(_mapper->currentIndex(), rsObstacleModel::ID));
}

void obstacleEditor::setUnits(bool si) {
	if (dynamic_cast<boxEditor *>(_pages->currentWidget()))
		dynamic_cast<boxEditor *>(_pages->currentWidget())->setUnits(si);
	else if (dynamic_cast<cylinderEditor *>(_pages->currentWidget()))
		dynamic_cast<cylinderEditor *>(_pages->currentWidget())->setUnits(si);
	else if (dynamic_cast<sphereEditor *>(_pages->currentWidget()))
		dynamic_cast<sphereEditor *>(_pages->currentWidget())->setUnits(si);
}

/*!
 *
 *
 *	Box Editor
 *
 *
 */

/*!	\brief Box Obstacle Editor.
 *
 *	Build individual box editor with relevant pieces of information.
 *
 *	\param		mapper data mapper from obstacleEditor model.
 */
boxEditor::boxEditor(QDataWidgetMapper *mapper, QWidget *parent) : QWidget(parent) {
	// save mapper
	_mapper = mapper;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Box Editor</span>"));

	// position x
	QLabel *pXLabel = new QLabel(tr("Pos X:"));
	_pXUnits = new QLabel();
	QDoubleSpinBox *pXBox = new QDoubleSpinBox();
	pXBox->setObjectName("px");
	pXBox->setMinimum(-1000000);
	pXBox->setMaximum(1000000);
	pXBox->setSingleStep(0.5);
	pXLabel->setBuddy(pXBox);
	QWidget::connect(pXBox, SIGNAL(valueChanged(double)), _mapper, SLOT(submit()));

	// position y
	QLabel *pYLabel = new QLabel(tr("Pos Y:"));
	_pYUnits = new QLabel();
	QDoubleSpinBox *pYBox = new QDoubleSpinBox();
	pYBox->setObjectName("py");
	pYBox->setMinimum(-1000000);
	pYBox->setMaximum(1000000);
	pYBox->setSingleStep(0.5);
	pYLabel->setBuddy(pYBox);
	QWidget::connect(pYBox, SIGNAL(valueChanged(double)), _mapper, SLOT(submit()));

	// position z
	QLabel *pZLabel = new QLabel(tr("Pos Z:"));
	_pZUnits = new QLabel();
	QDoubleSpinBox *pZBox = new QDoubleSpinBox();
	pZBox->setObjectName("pz");
	pZBox->setMinimum(-1000000);
	pZBox->setMaximum(1000000);
	pZBox->setSingleStep(0.5);
	pZLabel->setBuddy(pZBox);
	QWidget::connect(pZBox, SIGNAL(valueChanged(double)), _mapper, SLOT(submit()));

	// length x
	QLabel *lXLabel = new QLabel(tr("Length X:"));
	_lXUnits = new QLabel();
	QDoubleSpinBox *lXBox = new QDoubleSpinBox();
	lXBox->setObjectName("lx");
	lXBox->setMinimum(-100);
	lXBox->setMaximum(100);
	lXBox->setSingleStep(0.5);
	lXLabel->setBuddy(lXBox);
	QWidget::connect(lXBox, SIGNAL(valueChanged(double)), _mapper, SLOT(submit()));

	// length y
	QLabel *lYLabel = new QLabel(tr("Length Y:"));
	_lYUnits = new QLabel();
	QDoubleSpinBox *lYBox = new QDoubleSpinBox();
	lYBox->setObjectName("ly");
	lYBox->setMinimum(-100);
	lYBox->setMaximum(100);
	lYBox->setSingleStep(0.5);
	lYLabel->setBuddy(lYBox);
	QWidget::connect(lYBox, SIGNAL(valueChanged(double)), _mapper, SLOT(submit()));

	// length z
	QLabel *lZLabel = new QLabel(tr("Length Z:"));
	_lZUnits = new QLabel();
	QDoubleSpinBox *lZBox = new QDoubleSpinBox();
	lZBox->setObjectName("lz");
	lZBox->setMinimum(-100);
	lZBox->setMaximum(100);
	lZBox->setSingleStep(0.5);
	lZLabel->setBuddy(lZBox);
	QWidget::connect(lZBox, SIGNAL(valueChanged(double)), _mapper, SLOT(submit()));

	// color
	_colorPicker = new bodyColorPicker();
	_colorPicker->setObjectName("colorbutton");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), _mapper, SLOT(submit()));

	// lay out grid
	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *hbox0 = new QHBoxLayout();
	hbox0->addWidget(title, 5, Qt::AlignHCenter);
	layout->addLayout(hbox0);
	layout->addStretch(1);
	QHBoxLayout *hbox2 = new QHBoxLayout();
	hbox2->addWidget(pXLabel, 2, Qt::AlignRight);
	hbox2->addWidget(pXBox, 5);
	hbox2->addWidget(_pXUnits, 1, Qt::AlignLeft);
	hbox2->addWidget(lXLabel, 2, Qt::AlignRight);
	hbox2->addWidget(lXBox, 5);
	hbox2->addWidget(_lXUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox2);
	QHBoxLayout *hbox3 = new QHBoxLayout();
	hbox3->addWidget(pYLabel, 2, Qt::AlignRight);
	hbox3->addWidget(pYBox, 5);
	hbox3->addWidget(_pYUnits, 1, Qt::AlignLeft);
	hbox3->addWidget(lYLabel, 2, Qt::AlignRight);
	hbox3->addWidget(lYBox, 5);
	hbox3->addWidget(_lYUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox3);
	QHBoxLayout *hbox4 = new QHBoxLayout();
	hbox4->addWidget(pZLabel, 2, Qt::AlignRight);
	hbox4->addWidget(pZBox, 5);
	hbox4->addWidget(_pZUnits, 1, Qt::AlignLeft);
	hbox4->addWidget(lZLabel, 2, Qt::AlignRight);
	hbox4->addWidget(lZBox, 5);
	hbox4->addWidget(_lZUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox4);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(_colorPicker);
	layout->addLayout(hbox6);
	layout->addStretch(2);
	this->setLayout(layout);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void boxEditor::nullIndex(bool nullify) {
	(this->findChild<QDoubleSpinBox *>("px"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("py"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("pz"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("lx"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("ly"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("lz"))->setDisabled(nullify);
	(this->findChild<QPushButton *>("colorbutton"))->setDisabled(nullify);

	// dim color button
	QColor color = (this->findChild<QPushButton *>("colorbutton"))->palette().color(QPalette::Button);
	if (nullify) color.setAlpha(50);
	else color.setAlpha(255);
	(this->findChild<QPushButton *>("colorbutton"))->setPalette(color);

	// re-enable mapping
	if (!nullify) {
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("px"), rsObstacleModel::P_X);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("py"), rsObstacleModel::P_Y);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("pz"), rsObstacleModel::P_Z);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("lx"), rsObstacleModel::L_1);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("ly"), rsObstacleModel::L_2);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("lz"), rsObstacleModel::L_3);
		_mapper->addMapping(this->findChild<QPushButton *>("colorbutton"), rsObstacleModel::COLOR, "color");
	}
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void boxEditor::setUnits(bool si) {
	QString text;
	if (si) text = tr("cm");
	else text = tr("in");
	_pXUnits->setText(text);
	_pYUnits->setText(text);
	_pZUnits->setText(text);
	_lXUnits->setText(text);
	_lYUnits->setText(text);
	_lZUnits->setText(text);
}

/*!
 *
 *
 *	Cylinder Editor
 *
 *
 */

/*!	\brief Cylinder Obstacle Editor.
 *
 *	Build individual cylinder editor with relevant pieces of information.
 *
 *	\param		mapper data mapper from obstacleEditor model.
 */
cylinderEditor::cylinderEditor(QDataWidgetMapper *mapper, QWidget *parent) : QWidget(parent) {
	// save mapper
	_mapper = mapper;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Cylinder Editor</span>"));

	// position x
	QLabel *pXLabel = new QLabel(tr("Pos X:"));
	_pXUnits = new QLabel();
	QDoubleSpinBox *pXBox = new QDoubleSpinBox();
	pXBox->setObjectName("px");
	pXBox->setMinimum(-1000000);
	pXBox->setMaximum(1000000);
	pXBox->setSingleStep(0.5);
	pXLabel->setBuddy(pXBox);
	QWidget::connect(pXBox, SIGNAL(valueChanged(double)), _mapper, SLOT(submit()));

	// position y
	QLabel *pYLabel = new QLabel(tr("Pos Y:"));
	_pYUnits = new QLabel();
	QDoubleSpinBox *pYBox = new QDoubleSpinBox();
	pYBox->setObjectName("py");
	pYBox->setMinimum(-1000000);
	pYBox->setMaximum(1000000);
	pYBox->setSingleStep(0.5);
	pYLabel->setBuddy(pYBox);
	QWidget::connect(pYBox, SIGNAL(valueChanged(double)), _mapper, SLOT(submit()));

	// position z
	QLabel *pZLabel = new QLabel(tr("Pos Z:"));
	_pZUnits = new QLabel();
	QDoubleSpinBox *pZBox = new QDoubleSpinBox();
	pZBox->setObjectName("pz");
	pZBox->setMinimum(-1000000);
	pZBox->setMaximum(1000000);
	pZBox->setSingleStep(0.5);
	pZLabel->setBuddy(pZBox);
	QWidget::connect(pZBox, SIGNAL(valueChanged(double)), _mapper, SLOT(submit()));

	// radius
	QLabel *lXLabel = new QLabel(tr("Radius:"));
	_lXUnits = new QLabel();
	QDoubleSpinBox *lXBox = new QDoubleSpinBox();
	lXBox->setObjectName("radius");
	lXBox->setMinimum(0);
	lXBox->setMaximum(100);
	lXBox->setSingleStep(0.5);
	lXLabel->setBuddy(lXBox);
	QWidget::connect(lXBox, SIGNAL(valueChanged(double)), _mapper, SLOT(submit()));

	// length
	QLabel *lYLabel = new QLabel(tr("Length:"));
	_lYUnits = new QLabel();
	QDoubleSpinBox *lYBox = new QDoubleSpinBox();
	lYBox->setObjectName("length");
	lYBox->setMinimum(0);
	lYBox->setMaximum(100);
	lYBox->setSingleStep(0.5);
	lYLabel->setBuddy(lYBox);
	QWidget::connect(lYBox, SIGNAL(valueChanged(double)), _mapper, SLOT(submit()));

	// color
	_colorPicker = new bodyColorPicker();
	_colorPicker->setObjectName("colorbutton");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), _mapper, SLOT(submit()));

	// lay out grid
	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *hbox0 = new QHBoxLayout();
	hbox0->addWidget(title, 5, Qt::AlignHCenter);
	layout->addLayout(hbox0);
	layout->addStretch(1);
	QHBoxLayout *hbox2 = new QHBoxLayout();
	hbox2->addWidget(pXLabel, 2, Qt::AlignRight);
	hbox2->addWidget(pXBox, 5);
	hbox2->addWidget(_pXUnits, 1, Qt::AlignLeft);
	hbox2->addWidget(lXLabel, 2, Qt::AlignRight);
	hbox2->addWidget(lXBox, 5);
	hbox2->addWidget(_lXUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox2);
	QHBoxLayout *hbox3 = new QHBoxLayout();
	hbox3->addWidget(pYLabel, 2, Qt::AlignRight);
	hbox3->addWidget(pYBox, 5);
	hbox3->addWidget(_pYUnits, 1, Qt::AlignLeft);
	hbox3->addWidget(lYLabel, 2, Qt::AlignRight);
	hbox3->addWidget(lYBox, 5);
	hbox3->addWidget(_lYUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox3);
	QHBoxLayout *hbox4 = new QHBoxLayout();
	hbox4->addWidget(pZLabel, 2, Qt::AlignRight);
	hbox4->addWidget(pZBox, 5);
	hbox4->addWidget(_pZUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox4);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(_colorPicker);
	layout->addLayout(hbox6);
	layout->addStretch(2);
	this->setLayout(layout);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void cylinderEditor::nullIndex(bool nullify) {
	(this->findChild<QDoubleSpinBox *>("px"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("py"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("pz"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("radius"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("length"))->setDisabled(nullify);
	(this->findChild<QPushButton *>("colorbutton"))->setDisabled(nullify);

	// dim color button
	QColor color = (this->findChild<QPushButton *>("colorbutton"))->palette().color(QPalette::Button);
	if (nullify) color.setAlpha(50);
	else color.setAlpha(255);
	(this->findChild<QPushButton *>("colorbutton"))->setPalette(color);

	// re-enable mapping
	if (!nullify) {
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("px"), rsObstacleModel::P_X);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("py"), rsObstacleModel::P_Y);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("pz"), rsObstacleModel::P_Z);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("radius"), rsObstacleModel::L_1);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("length"), rsObstacleModel::L_2);
		_mapper->addMapping(this->findChild<QPushButton *>("colorbutton"), rsObstacleModel::COLOR, "color");
	}
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void cylinderEditor::setUnits(bool si) {
	QString text;
	if (si) text = tr("cm");
	else text = tr("in");
	_pXUnits->setText(text);
	_pYUnits->setText(text);
	_pZUnits->setText(text);
	_lXUnits->setText(text);
	_lYUnits->setText(text);
}

/*!
 *
 *
 *	Sphere Editor
 *
 *
 */

/*!	\brief Sphere Obstacle Editor.
 *
 *	Build individual sphere editor with relevant pieces of information.
 *
 *	\param		mapper data mapper from obstacleEditor model.
 */
sphereEditor::sphereEditor(QDataWidgetMapper *mapper, QWidget *parent) : QWidget(parent) {
	// save mapper
	_mapper = mapper;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Sphere Editor</span>"));

	// position x
	QLabel *pXLabel = new QLabel(tr("Pos X:"));
	_pXUnits = new QLabel();
	QDoubleSpinBox *pXBox = new QDoubleSpinBox();
	pXBox->setObjectName("px");
	pXBox->setMinimum(-1000000);
	pXBox->setMaximum(1000000);
	pXBox->setSingleStep(0.5);
	pXLabel->setBuddy(pXBox);
	QWidget::connect(pXBox, SIGNAL(valueChanged(double)), _mapper, SLOT(submit()));

	// position y
	QLabel *pYLabel = new QLabel(tr("Pos Y:"));
	_pYUnits = new QLabel();
	QDoubleSpinBox *pYBox = new QDoubleSpinBox();
	pYBox->setObjectName("py");
	pYBox->setMinimum(-1000000);
	pYBox->setMaximum(1000000);
	pYBox->setSingleStep(0.5);
	pYLabel->setBuddy(pYBox);
	QWidget::connect(pYBox, SIGNAL(valueChanged(double)), _mapper, SLOT(submit()));

	// position z
	QLabel *pZLabel = new QLabel(tr("Pos Z:"));
	_pZUnits = new QLabel();
	QDoubleSpinBox *pZBox = new QDoubleSpinBox();
	pZBox->setObjectName("pz");
	pZBox->setMinimum(-1000000);
	pZBox->setMaximum(1000000);
	pZBox->setSingleStep(0.5);
	pZLabel->setBuddy(pZBox);
	QWidget::connect(pZBox, SIGNAL(valueChanged(double)), _mapper, SLOT(submit()));

	// radius
	QLabel *lXLabel = new QLabel(tr("Radius:"));
	_lXUnits = new QLabel();
	QDoubleSpinBox *lXBox = new QDoubleSpinBox();
	lXBox->setObjectName("radius");
	lXBox->setMinimum(0);
	lXBox->setMaximum(100);
	lXBox->setSingleStep(0.5);
	lXLabel->setBuddy(lXBox);
	QWidget::connect(lXBox, SIGNAL(valueChanged(double)), _mapper, SLOT(submit()));

	// color
	_colorPicker = new bodyColorPicker();
	_colorPicker->setObjectName("colorbutton");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), _mapper, SLOT(submit()));

	// lay out grid
	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *hbox0 = new QHBoxLayout();
	hbox0->addWidget(title, 5, Qt::AlignHCenter);
	layout->addLayout(hbox0);
	layout->addStretch(1);
	QHBoxLayout *hbox2 = new QHBoxLayout();
	hbox2->addWidget(pXLabel, 2, Qt::AlignRight);
	hbox2->addWidget(pXBox, 5);
	hbox2->addWidget(_pXUnits, 1, Qt::AlignLeft);
	hbox2->addWidget(lXLabel, 2, Qt::AlignRight);
	hbox2->addWidget(lXBox, 5);
	hbox2->addWidget(_lXUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox2);
	QHBoxLayout *hbox3 = new QHBoxLayout();
	hbox3->addWidget(pYLabel, 2, Qt::AlignRight);
	hbox3->addWidget(pYBox, 5);
	hbox3->addWidget(_pYUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox3);
	QHBoxLayout *hbox4 = new QHBoxLayout();
	hbox4->addWidget(pZLabel, 2, Qt::AlignRight);
	hbox4->addWidget(pZBox, 5);
	hbox4->addWidget(_pZUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox4);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(_colorPicker);
	layout->addLayout(hbox6);
	layout->addStretch(2);
	this->setLayout(layout);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void sphereEditor::nullIndex(bool nullify) {
	(this->findChild<QDoubleSpinBox *>("px"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("py"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("pz"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("radius"))->setDisabled(nullify);
	(this->findChild<QPushButton *>("colorbutton"))->setDisabled(nullify);

	// dim color button
	QColor color = (this->findChild<QPushButton *>("colorbutton"))->palette().color(QPalette::Button);
	if (nullify) color.setAlpha(50);
	else color.setAlpha(255);
	(this->findChild<QPushButton *>("colorbutton"))->setPalette(color);

	// re-enable mapping
	if (!nullify) {
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("px"), rsObstacleModel::P_X);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("py"), rsObstacleModel::P_Y);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("pz"), rsObstacleModel::P_Z);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("radius"), rsObstacleModel::L_1);
		_mapper->addMapping(this->findChild<QPushButton *>("colorbutton"), rsObstacleModel::COLOR, "color");
	}
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void sphereEditor::setUnits(bool si) {
	QString text;
	if (si) text = tr("cm");
	else text = tr("in");
	_pXUnits->setText(text);
	_pYUnits->setText(text);
	_pZUnits->setText(text);
	_lXUnits->setText(text);
}

/*!
 *
 *
 *	bodyColorPicker
 *
 *
 */

bodyColorPicker::bodyColorPicker(QWidget *parent) : QWidget(parent) {
	QHBoxLayout *hbox = new QHBoxLayout(this);

	QLabel *label = new QLabel(tr("Color:"));
	hbox->addWidget(label, 2, Qt::AlignRight);

	_color = QColor(0, 255, 0);

	_button = new QPushButton(this);
	_button->setObjectName("colorbutton");
	_button->setPalette(QPalette(_color));
	hbox->addWidget(_button, 5);
	hbox->addStretch(1);

	label->setBuddy(_button);
	QWidget::connect(_button, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
}

QColor bodyColorPicker::color(void) const {
    return _color;
}

void bodyColorPicker::setColor(const QColor color) {
	if (color == _color)
		return;

	_color = color;
	_button->setPalette(QPalette(_color));
}

void bodyColorPicker::onButtonClicked(void) {
	const QColor color = QColorDialog::getColor(_color, this);
	if (!color.isValid())
		return;

	this->setColor(color);
	emit colorChanged(color);
}

/*!
 *
 *
 *	obstacleEditorDelegate
 *
 *
 */
obstacleEditorDelegate::obstacleEditorDelegate(QObject *parent) : QItemDelegate(parent) { }

void obstacleEditorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
	if (!strcmp(editor->metaObject()->className(), "QComboBox")) {
		editor->setProperty("currentIndex", index.data());
		return;
	}
	QItemDelegate::setEditorData(editor, index);
}

void obstacleEditorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
	if (!strcmp(editor->metaObject()->className(), "QComboBox")) {
		QVariant value = editor->property("currentIndex");
		if (value.isValid()) {
			model->setData(index, value);
			return;
		}
	}
	QItemDelegate::setModelData(editor, model, index);
}
