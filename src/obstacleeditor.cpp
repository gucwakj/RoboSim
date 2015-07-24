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
	this->setFixedWidth(350);

	// set up editor pages
	_pages = new QStackedWidget;
	_pages->addWidget(new emptyEditor());
	_pages->addWidget(new boxEditor(_model));
	_pages->addWidget(new cylinderEditor(_model));
	_pages->addWidget(new dotEditor(_model));
	_pages->addWidget(new lineEditor(_model));
	_pages->addWidget(new sphereEditor(_model));
	_pages->addWidget(new textEditor(_model));

	// set up buttons
	_deleteButton = new QPushButton(tr("Delete"));
	_deleteButton->setEnabled(false);
	_deleteButton->setFixedWidth(75);
	_deleteButton->setToolTip("Delete the object");
	_deleteButton->setToolTipDuration(-1);
	_nextButton = new QPushButton(tr("Next"));
	_nextButton->setEnabled(false);
	_nextButton->setFixedWidth(75);
	_nextButton->setToolTip("Edit the next object in the list");
	_nextButton->setToolTipDuration(-1);
	_previousButton = new QPushButton(tr("Previous"));
	_previousButton->setEnabled(false);
	_previousButton->setFixedWidth(75);
	_previousButton->setToolTip("Edit the previous object in the list");
	_previousButton->setToolTipDuration(-1);

	// create signal connections
	QWidget::connect(_deleteButton, SIGNAL(clicked()), this, SLOT(deleteCurrentIndex()));
	QWidget::connect(_nextButton, SIGNAL(clicked()), this, SLOT(nextButtonPressed()));
	QWidget::connect(_previousButton, SIGNAL(clicked()), this, SLOT(prevButtonPressed()));

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

	// set variables for tracking editor
	_row = -1;
}

void obstacleEditor::dataChanged(QModelIndex/*topLeft*/, QModelIndex bottomRight) {
	if (bottomRight.row() != _row)
		this->setCurrentIndex(bottomRight);
}

void obstacleEditor::setCurrentIndex(const QModelIndex &index) {
	if (index.isValid()) {
		// set new curent model row
		_row = index.row();

		// load appropriate page
		int form = _model->data(_model->index(index.row(), rsObstacleModel::FORM), Qt::EditRole).toInt();
		switch (form) {
			case rs::BOX:
				_pages->setCurrentIndex(1);
				dynamic_cast<boxEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<boxEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::CYLINDER:
				_pages->setCurrentIndex(2);
				dynamic_cast<cylinderEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<cylinderEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::DOT:
				_pages->setCurrentIndex(3);
				dynamic_cast<dotEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<dotEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::LINE:
				_pages->setCurrentIndex(4);
				dynamic_cast<lineEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<lineEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::SPHERE:
				_pages->setCurrentIndex(5);
				dynamic_cast<sphereEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<sphereEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::TEXT:
				_pages->setCurrentIndex(6);
				dynamic_cast<textEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<textEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
		}

		// update button states
		_deleteButton->setEnabled(true);
		_nextButton->setEnabled(index.row() < _model->rowCount() - 1);
		_previousButton->setEnabled(index.row() > 0);
	}
	else {
		// show blank editor page
		_pages->setCurrentIndex(0);
		_row = -1;

		// disable all buttons
		_deleteButton->setDisabled(true);
		_nextButton->setDisabled(true);
		_previousButton->setDisabled(true);
	}
}

void obstacleEditor::nextButtonPressed(void) {
	// set new index
	QModelIndex index = _model->index(_row + 1, 0);
	this->setCurrentIndex(index);

	// signal other views that index has changed
	emit indexChanged(index);
}

void obstacleEditor::prevButtonPressed(void) {
	// set new index
	QModelIndex index = _model->index(_row - 1, 0);
	this->setCurrentIndex(index);

	// signal other views that index has changed
	emit indexChanged(index);
}

void obstacleEditor::deleteCurrentIndex(void) {
	// save current index
	int row = _row;

	// remove current obstacle from model
	_model->removeRows(row, 1);

	// if it is invalid, then set the last row in the model
	if (row >= _model->rowCount())
		row = _model->rowCount() - 1;

	// set new index
	this->setCurrentIndex(_model->index(row, rsObstacleModel::ID));

	// signal a change in current obstacle
	emit indexChanged(_model->index(row, rsObstacleModel::ID));
}

void obstacleEditor::setUnits(bool si) {
	// do nothing if units don't change
	if (_units == si) return;

	// save units
	_units = si;

	// reload current editor
	this->setCurrentIndex(_model->index(_row, rsObstacleModel::FORM));
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
 *	\param		model data model from obstacleEditor model.
 */
boxEditor::boxEditor(obstacleModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

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
	QWidget::connect(pXBox, SIGNAL(valueChanged(double)), this, SLOT(submitPX(double)));
	pXBox->setToolTip("Set the X position of the box");
	pXBox->setToolTipDuration(-1);

	// position y
	QLabel *pYLabel = new QLabel(tr("Pos Y:"));
	_pYUnits = new QLabel();
	QDoubleSpinBox *pYBox = new QDoubleSpinBox();
	pYBox->setObjectName("py");
	pYBox->setMinimum(-1000000);
	pYBox->setMaximum(1000000);
	pYBox->setSingleStep(0.5);
	pYLabel->setBuddy(pYBox);
	QWidget::connect(pYBox, SIGNAL(valueChanged(double)), this, SLOT(submitPY(double)));
	pYBox->setToolTip("Set the Y position of the box");
	pYBox->setToolTipDuration(-1);

	// position z
	QLabel *pZLabel = new QLabel(tr("Pos Z:"));
	_pZUnits = new QLabel();
	QDoubleSpinBox *pZBox = new QDoubleSpinBox();
	pZBox->setObjectName("pz");
	pZBox->setMinimum(-1000000);
	pZBox->setMaximum(1000000);
	pZBox->setSingleStep(0.5);
	pZLabel->setBuddy(pZBox);
	QWidget::connect(pZBox, SIGNAL(valueChanged(double)), this, SLOT(submitPZ(double)));
	pZBox->setToolTip("Set the Z position of the box");
	pZBox->setToolTipDuration(-1);

	// length x
	QLabel *lXLabel = new QLabel(tr("Length X:"));
	_lXUnits = new QLabel();
	QDoubleSpinBox *lXBox = new QDoubleSpinBox();
	lXBox->setObjectName("lx");
	lXBox->setMinimum(-100);
	lXBox->setMaximum(100);
	lXBox->setSingleStep(0.5);
	lXLabel->setBuddy(lXBox);
	QWidget::connect(lXBox, SIGNAL(valueChanged(double)), this, SLOT(submitL1(double)));
	lXBox->setToolTip("Set the X length of the box");
	lXBox->setToolTipDuration(-1);

	// length y
	QLabel *lYLabel = new QLabel(tr("Length Y:"));
	_lYUnits = new QLabel();
	QDoubleSpinBox *lYBox = new QDoubleSpinBox();
	lYBox->setObjectName("ly");
	lYBox->setMinimum(-100);
	lYBox->setMaximum(100);
	lYBox->setSingleStep(0.5);
	lYLabel->setBuddy(lYBox);
	QWidget::connect(lYBox, SIGNAL(valueChanged(double)), this, SLOT(submitL2(double)));
	lYBox->setToolTip("Set the Y length of the box");
	lYBox->setToolTipDuration(-1);

	// length z
	QLabel *lZLabel = new QLabel(tr("Length Z:"));
	_lZUnits = new QLabel();
	QDoubleSpinBox *lZBox = new QDoubleSpinBox();
	lZBox->setObjectName("lz");
	lZBox->setMinimum(-100);
	lZBox->setMaximum(100);
	lZBox->setSingleStep(0.5);
	lZLabel->setBuddy(lZBox);
	QWidget::connect(lZBox, SIGNAL(valueChanged(double)), this, SLOT(submitL3(double)));
	lZBox->setToolTip("Set the Z length of the box");
	lZBox->setToolTipDuration(-1);

	// mass
	QLabel *massLabel = new QLabel(tr("Mass:"));
	_massUnits = new QLabel();
	QDoubleSpinBox *massBox = new QDoubleSpinBox();
	massBox->setObjectName("mass");
	massBox->setMinimum(0);
	massBox->setMaximum(100000);
	massBox->setSingleStep(0.5);
	massLabel->setBuddy(massBox);
	QWidget::connect(massBox, SIGNAL(valueChanged(double)), this, SLOT(submitMass(double)));
	massBox->setToolTip("Set the mass of the box");
	massBox->setToolTipDuration(-1);

	// color
	_colorPicker = new bodyColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the box");
	_colorPicker->setToolTipDuration(-1);

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
	QHBoxLayout *hbox5 = new QHBoxLayout();
	hbox5->addWidget(massLabel, 2, Qt::AlignRight);
	hbox5->addWidget(massBox, 5);
	hbox5->addWidget(_massUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox5);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(_colorPicker);
	layout->addLayout(hbox6);
	layout->addStretch(2);
	this->setLayout(layout);
}

void boxEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::P_X), value);
}

void boxEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::P_Y), value);
}

void boxEditor::submitPZ(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::P_Z), value);
}

void boxEditor::submitL1(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::L_1), value);
}

void boxEditor::submitL2(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::L_2), value);
}

void boxEditor::submitL3(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::L_3), value);
}

void boxEditor::submitMass(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::MASS), value);
}

void boxEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsObstacleModel::COLOR), color);
}

/*!	\brief Slot to re-enable all inputs.
 *
 *	\param		row Row of the model to load.
 */
void boxEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsObstacleModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsObstacleModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("pz"))->setValue(_model->data(_model->index(row, rsObstacleModel::P_Z), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("lx"))->setValue(_model->data(_model->index(row, rsObstacleModel::L_1), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("ly"))->setValue(_model->data(_model->index(row, rsObstacleModel::L_2), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("lz"))->setValue(_model->data(_model->index(row, rsObstacleModel::L_3), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("mass"))->setValue(_model->data(_model->index(row, rsObstacleModel::MASS), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsObstacleModel::COLOR), Qt::EditRole).toString());
	(this->findChild<bodyColorPicker *>("color"))->setColor(color);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void boxEditor::setUnits(bool si) {
	QString text;
	if (si) {
		text = tr("cm");
		_massUnits->setText(tr("kg"));
	}
	else {
		text = tr("in");
		_massUnits->setText(tr("lb"));
	}
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
 *	\param		model data model from obstacleEditor model.
 */
cylinderEditor::cylinderEditor(obstacleModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

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
	QWidget::connect(pXBox, SIGNAL(valueChanged(double)), this, SLOT(submitPX(double)));
	pXBox->setToolTip("Set the X position of the cylinder");
	pXBox->setToolTipDuration(-1);

	// position y
	QLabel *pYLabel = new QLabel(tr("Pos Y:"));
	_pYUnits = new QLabel();
	QDoubleSpinBox *pYBox = new QDoubleSpinBox();
	pYBox->setObjectName("py");
	pYBox->setMinimum(-1000000);
	pYBox->setMaximum(1000000);
	pYBox->setSingleStep(0.5);
	pYLabel->setBuddy(pYBox);
	QWidget::connect(pYBox, SIGNAL(valueChanged(double)), this, SLOT(submitPY(double)));
	pYBox->setToolTip("Set the Y position of the cylinder");
	pYBox->setToolTipDuration(-1);

	// position z
	QLabel *pZLabel = new QLabel(tr("Pos Z:"));
	_pZUnits = new QLabel();
	QDoubleSpinBox *pZBox = new QDoubleSpinBox();
	pZBox->setObjectName("pz");
	pZBox->setMinimum(-1000000);
	pZBox->setMaximum(1000000);
	pZBox->setSingleStep(0.5);
	pZLabel->setBuddy(pZBox);
	QWidget::connect(pZBox, SIGNAL(valueChanged(double)), this, SLOT(submitPZ(double)));
	pZBox->setToolTip("Set the Z position of the cylinder");
	pZBox->setToolTipDuration(-1);

	// radius
	QLabel *lXLabel = new QLabel(tr("Radius:"));
	_lXUnits = new QLabel();
	QDoubleSpinBox *lXBox = new QDoubleSpinBox();
	lXBox->setObjectName("radius");
	lXBox->setMinimum(0);
	lXBox->setMaximum(100);
	lXBox->setSingleStep(0.5);
	lXLabel->setBuddy(lXBox);
	QWidget::connect(lXBox, SIGNAL(valueChanged(double)), this, SLOT(submitL1(double)));
	lXBox->setToolTip("Set the radius of the cylinder");
	lXBox->setToolTipDuration(-1);

	// length
	QLabel *lYLabel = new QLabel(tr("Length:"));
	_lYUnits = new QLabel();
	QDoubleSpinBox *lYBox = new QDoubleSpinBox();
	lYBox->setObjectName("length");
	lYBox->setMinimum(0);
	lYBox->setMaximum(100);
	lYBox->setSingleStep(0.5);
	lYLabel->setBuddy(lYBox);
	QWidget::connect(lYBox, SIGNAL(valueChanged(double)), this, SLOT(submitL2(double)));
	lYBox->setToolTip("Set the length of the cylinder");
	lYBox->setToolTipDuration(-1);

	// mass
	QLabel *massLabel = new QLabel(tr("Mass:"));
	_massUnits = new QLabel();
	QDoubleSpinBox *massBox = new QDoubleSpinBox();
	massBox->setObjectName("mass");
	massBox->setMinimum(0);
	massBox->setMaximum(100000);
	massBox->setSingleStep(0.5);
	massLabel->setBuddy(massBox);
	QWidget::connect(massBox, SIGNAL(valueChanged(double)), this, SLOT(submitMass(double)));
	massBox->setToolTip("Set the mass of the cylinder");
	massBox->setToolTipDuration(-1);

	// axis
	QLabel *axisLabel = new QLabel(tr("Axis: "));
	QStringList axisItems;
	axisItems << tr("X") << tr("Y") << tr("Z");
	QStringListModel *axisModel = new QStringListModel(axisItems, this);
	QComboBox *axisBox = new QComboBox();
	axisBox->setObjectName("axis");
	axisBox->setModel(axisModel);
	axisLabel->setBuddy(axisBox);
	QWidget::connect(axisBox, SIGNAL(currentIndexChanged(int)), this, SLOT(submitAxis(int)));
	axisBox->setToolTip("Pick the major axis of the cylinder");
	axisBox->setToolTipDuration(-1);

	// color
	_colorPicker = new bodyColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the cylinder");
	_colorPicker->setToolTipDuration(-1);

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
	QHBoxLayout *hbox5 = new QHBoxLayout();
	hbox5->addWidget(massLabel, 2, Qt::AlignRight);
	hbox5->addWidget(massBox, 5);
	hbox5->addWidget(_massUnits, 1, Qt::AlignLeft);
	hbox5->addWidget(axisLabel, 2, Qt::AlignRight);
	hbox5->addWidget(axisBox, 5);
	hbox5->addStretch(1);
	layout->addLayout(hbox5);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(_colorPicker);
	layout->addLayout(hbox6);
	layout->addStretch(2);
	this->setLayout(layout);
}

void cylinderEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::P_X), value);
}

void cylinderEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::P_Y), value);
}

void cylinderEditor::submitPZ(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::P_Z), value);
}

void cylinderEditor::submitL1(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::L_1), value);
}

void cylinderEditor::submitL2(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::L_2), value);
}

void cylinderEditor::submitAxis(int value) {
	_model->setData(_model->index(_row, rsObstacleModel::AXIS), value);
}

void cylinderEditor::submitMass(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::MASS), value);
}

void cylinderEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsObstacleModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void cylinderEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsObstacleModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsObstacleModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("pz"))->setValue(_model->data(_model->index(row, rsObstacleModel::P_Z), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("radius"))->setValue(_model->data(_model->index(row, rsObstacleModel::L_1), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("length"))->setValue(_model->data(_model->index(row, rsObstacleModel::L_2), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("mass"))->setValue(_model->data(_model->index(row, rsObstacleModel::MASS), Qt::EditRole).toDouble());
	(this->findChild<QComboBox *>("axis"))->setCurrentIndex(_model->data(_model->index(row, rsObstacleModel::AXIS), Qt::EditRole).toInt());
	QColor color(_model->data(_model->index(row, rsObstacleModel::COLOR), Qt::EditRole).toString());
	(this->findChild<bodyColorPicker *>("color"))->setColor(color);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void cylinderEditor::setUnits(bool si) {
	QString text;
	if (si) {
		text = tr("cm");
		_massUnits->setText(tr("kg"));
	}
	else {
		text = tr("in");
		_massUnits->setText(tr("lb"));
	}
	_pXUnits->setText(text);
	_pYUnits->setText(text);
	_pZUnits->setText(text);
	_lXUnits->setText(text);
	_lYUnits->setText(text);
}

/*!
 *
 *
 *	Dot Editor
 *
 *
 */

/*!	\brief Dot Drawing Editor.
 *
 *	Build individual dot editor with relevant pieces of information.
 *
 *	\param		model data model from obstacleEditor model.
 */
dotEditor::dotEditor(obstacleModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Dot Editor</span>"));

	// position x
	QLabel *pXLabel = new QLabel(tr("Pos X:"));
	_pXUnits = new QLabel();
	QDoubleSpinBox *pXBox = new QDoubleSpinBox();
	pXBox->setObjectName("px");
	pXBox->setMinimum(-1000000);
	pXBox->setMaximum(1000000);
	pXBox->setSingleStep(0.5);
	pXLabel->setBuddy(pXBox);
	QWidget::connect(pXBox, SIGNAL(valueChanged(double)), this, SLOT(submitPX(double)));
	pXBox->setToolTip("Set the X position of the dot");
	pXBox->setToolTipDuration(-1);

	// position y
	QLabel *pYLabel = new QLabel(tr("Pos Y:"));
	_pYUnits = new QLabel();
	QDoubleSpinBox *pYBox = new QDoubleSpinBox();
	pYBox->setObjectName("py");
	pYBox->setMinimum(-1000000);
	pYBox->setMaximum(1000000);
	pYBox->setSingleStep(0.5);
	pYLabel->setBuddy(pYBox);
	QWidget::connect(pYBox, SIGNAL(valueChanged(double)), this, SLOT(submitPY(double)));
	pYBox->setToolTip("Set the Y position of the dot");
	pYBox->setToolTipDuration(-1);

	// position z
	QLabel *pZLabel = new QLabel(tr("Pos Z:"));
	_pZUnits = new QLabel();
	QDoubleSpinBox *pZBox = new QDoubleSpinBox();
	pZBox->setObjectName("pz");
	pZBox->setMinimum(-1000000);
	pZBox->setMaximum(1000000);
	pZBox->setSingleStep(0.5);
	pZLabel->setBuddy(pZBox);
	QWidget::connect(pZBox, SIGNAL(valueChanged(double)), this, SLOT(submitPZ(double)));
	pZBox->setToolTip("Set the Z position of the dot");
	pZBox->setToolTipDuration(-1);

	// size
	QLabel *sizeLabel = new QLabel(tr("Size:"));
	QDoubleSpinBox *sizeBox = new QDoubleSpinBox();
	sizeBox->setObjectName("size");
	sizeBox->setMinimum(0);
	sizeBox->setMaximum(100);
	sizeBox->setSingleStep(1);
	sizeLabel->setBuddy(sizeBox);
	QWidget::connect(sizeBox, SIGNAL(valueChanged(double)), this, SLOT(submitSize(double)));
	sizeBox->setToolTip("Set the size of the dot");
	sizeBox->setToolTipDuration(-1);

	// color
	_colorPicker = new bodyColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the dot");
	_colorPicker->setToolTipDuration(-1);

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
	QHBoxLayout *hbox5 = new QHBoxLayout();
	hbox5->addWidget(sizeLabel, 2, Qt::AlignRight);
	hbox5->addWidget(sizeBox, 5);
	layout->addLayout(hbox5);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(_colorPicker);
	layout->addLayout(hbox6);
	layout->addStretch(2);
	this->setLayout(layout);
}

void dotEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::P_X), value);
}

void dotEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::P_Y), value);
}

void dotEditor::submitPZ(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::P_Z), value);
}

void dotEditor::submitSize(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::SIZE), value);
}

void dotEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsObstacleModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void dotEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsObstacleModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsObstacleModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("pz"))->setValue(_model->data(_model->index(row, rsObstacleModel::P_Z), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("size"))->setValue(_model->data(_model->index(row, rsObstacleModel::SIZE), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsObstacleModel::COLOR), Qt::EditRole).toString());
	(this->findChild<bodyColorPicker *>("color"))->setColor(color);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void dotEditor::setUnits(bool si) {
	QString text;
	if (si) text = tr("cm");
	else text = tr("in");
	_pXUnits->setText(text);
	_pYUnits->setText(text);
	_pZUnits->setText(text);
}

/*!
*
*
*	Empty Editor
*
*
*/
emptyEditor::emptyEditor(QWidget *parent) : QWidget(parent) {
	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Obstacles and Drawings Editor</span>"));

	// set note to user
	QLabel *note1 = new QLabel(tr("<span style=\"font-size:9pt;\">There are no obstacles or drawings in the Scene.</span>"));
	QLabel *note2 = new QLabel(tr("<span style=\"font-size:9pt;\">Therefore nothing to edit here.</span>"));

	// display info
	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(title, 1, Qt::AlignHCenter);
	layout->addLayout(hbox);
	layout->addStretch(1);
	QHBoxLayout *hbox1 = new QHBoxLayout();
	hbox1->addWidget(note1, 3, Qt::AlignCenter);
	layout->addLayout(hbox1);
	QHBoxLayout *hbox2 = new QHBoxLayout();
	hbox2->addWidget(note2, 3, Qt::AlignCenter);
	layout->addLayout(hbox2);
	layout->addStretch(1);
	this->setLayout(layout);
}

/*!
 *
 *
 *	Line Editor
 *
 *
 */

/*!	\brief Line Drawing Editor.
 *
 *	Build individual line editor with relevant pieces of information.
 *
 *	\param		model data model from obstacleEditor model.
 */
lineEditor::lineEditor(obstacleModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Line Editor</span>"));

	// position x1
	QLabel *pXLabel = new QLabel(tr("Start X:"));
	_pXUnits = new QLabel();
	QDoubleSpinBox *pXBox = new QDoubleSpinBox();
	pXBox->setObjectName("px1");
	pXBox->setMinimum(-1000000);
	pXBox->setMaximum(1000000);
	pXBox->setSingleStep(0.5);
	pXLabel->setBuddy(pXBox);
	QWidget::connect(pXBox, SIGNAL(valueChanged(double)), this, SLOT(submitPX(double)));
	pXBox->setToolTip("Set the starting X position of the line");
	pXBox->setToolTipDuration(-1);

	// position y1
	QLabel *pYLabel = new QLabel(tr("Start Y:"));
	_pYUnits = new QLabel();
	QDoubleSpinBox *pYBox = new QDoubleSpinBox();
	pYBox->setObjectName("py1");
	pYBox->setMinimum(-1000000);
	pYBox->setMaximum(1000000);
	pYBox->setSingleStep(0.5);
	pYLabel->setBuddy(pYBox);
	QWidget::connect(pYBox, SIGNAL(valueChanged(double)), this, SLOT(submitPY(double)));
	pXBox->setToolTip("Set the starting Y position of the line");
	pXBox->setToolTipDuration(-1);

	// position z1
	QLabel *pZLabel = new QLabel(tr("Start Z:"));
	_pZUnits = new QLabel();
	QDoubleSpinBox *pZBox = new QDoubleSpinBox();
	pZBox->setObjectName("pz1");
	pZBox->setMinimum(-1000000);
	pZBox->setMaximum(1000000);
	pZBox->setSingleStep(0.5);
	pZLabel->setBuddy(pZBox);
	QWidget::connect(pZBox, SIGNAL(valueChanged(double)), this, SLOT(submitPZ(double)));
	pXBox->setToolTip("Set the starting Z position of the line");
	pXBox->setToolTipDuration(-1);

	// position x2
	QLabel *lXLabel = new QLabel(tr("End X:"));
	_lXUnits = new QLabel();
	QDoubleSpinBox *lXBox = new QDoubleSpinBox();
	lXBox->setObjectName("px2");
	lXBox->setMinimum(-1000000);
	lXBox->setMaximum(1000000);
	lXBox->setSingleStep(0.5);
	lXLabel->setBuddy(lXBox);
	QWidget::connect(lXBox, SIGNAL(valueChanged(double)), this, SLOT(submitL1(double)));
	pXBox->setToolTip("Set the ending X position of the line");
	pXBox->setToolTipDuration(-1);

	// position y2
	QLabel *lYLabel = new QLabel(tr("End Y:"));
	_lYUnits = new QLabel();
	QDoubleSpinBox *lYBox = new QDoubleSpinBox();
	lYBox->setObjectName("py2");
	lYBox->setMinimum(-1000000);
	lYBox->setMaximum(1000000);
	lYBox->setSingleStep(0.5);
	lYLabel->setBuddy(lYBox);
	QWidget::connect(lYBox, SIGNAL(valueChanged(double)), this, SLOT(submitL2(double)));
	pXBox->setToolTip("Set the ending Y position of the line");
	pXBox->setToolTipDuration(-1);

	// position z2
	QLabel *lZLabel = new QLabel(tr("End Z:"));
	_lZUnits = new QLabel();
	QDoubleSpinBox *lZBox = new QDoubleSpinBox();
	lZBox->setObjectName("pz2");
	lZBox->setMinimum(-1000000);
	lZBox->setMaximum(1000000);
	lZBox->setSingleStep(0.5);
	lZLabel->setBuddy(lZBox);
	QWidget::connect(lZBox, SIGNAL(valueChanged(double)), this, SLOT(submitL3(double)));
	pXBox->setToolTip("Set the ending Z position of the line");
	pXBox->setToolTipDuration(-1);

	// width
	QLabel *widthLabel = new QLabel(tr("Width:"));
	QDoubleSpinBox *widthBox = new QDoubleSpinBox();
	widthBox->setObjectName("width");
	widthBox->setMinimum(0);
	widthBox->setMaximum(100);
	widthBox->setSingleStep(1);
	widthLabel->setBuddy(widthBox);
	QWidget::connect(widthBox, SIGNAL(valueChanged(double)), this, SLOT(submitSize(double)));
	widthBox->setToolTip("Set the width of the line");
	widthBox->setToolTipDuration(-1);

	// color
	_colorPicker = new bodyColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the line");
	_colorPicker->setToolTipDuration(-1);

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
	QHBoxLayout *hbox5 = new QHBoxLayout();
	hbox5->addWidget(widthLabel, 2, Qt::AlignRight);
	hbox5->addWidget(widthBox, 5);
	layout->addLayout(hbox5);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(_colorPicker);
	layout->addLayout(hbox6);
	layout->addStretch(2);
	this->setLayout(layout);
}

void lineEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::P_X), value);
}

void lineEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::P_Y), value);
}

void lineEditor::submitPZ(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::P_Z), value);
}

void lineEditor::submitL1(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::L_1), value);
}

void lineEditor::submitL2(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::L_2), value);
}

void lineEditor::submitL3(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::L_3), value);
}

void lineEditor::submitSize(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::SIZE), value);
}

void lineEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsObstacleModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void lineEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px1"))->setValue(_model->data(_model->index(row, rsObstacleModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py1"))->setValue(_model->data(_model->index(row, rsObstacleModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("pz1"))->setValue(_model->data(_model->index(row, rsObstacleModel::P_Z), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("px2"))->setValue(_model->data(_model->index(row, rsObstacleModel::L_1), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py2"))->setValue(_model->data(_model->index(row, rsObstacleModel::L_2), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("pz2"))->setValue(_model->data(_model->index(row, rsObstacleModel::L_3), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("width"))->setValue(_model->data(_model->index(row, rsObstacleModel::SIZE), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsObstacleModel::COLOR), Qt::EditRole).toString());
	(this->findChild<bodyColorPicker *>("color"))->setColor(color);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void lineEditor::setUnits(bool si) {
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
 *	Sphere Editor
 *
 *
 */

/*!	\brief Sphere Obstacle Editor.
 *
 *	Build individual sphere editor with relevant pieces of information.
 *
 *	\param		model data model from obstacleEditor model.
 */
sphereEditor::sphereEditor(obstacleModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

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
	QWidget::connect(pXBox, SIGNAL(valueChanged(double)), this, SLOT(submitPX(double)));
	pXBox->setToolTip("Set the X position of the sphere");
	pXBox->setToolTipDuration(-1);

	// position y
	QLabel *pYLabel = new QLabel(tr("Pos Y:"));
	_pYUnits = new QLabel();
	QDoubleSpinBox *pYBox = new QDoubleSpinBox();
	pYBox->setObjectName("py");
	pYBox->setMinimum(-1000000);
	pYBox->setMaximum(1000000);
	pYBox->setSingleStep(0.5);
	pYLabel->setBuddy(pYBox);
	QWidget::connect(pYBox, SIGNAL(valueChanged(double)), this, SLOT(submitPY(double)));
	pYBox->setToolTip("Set the Y position of the sphere");
	pYBox->setToolTipDuration(-1);

	// position z
	QLabel *pZLabel = new QLabel(tr("Pos Z:"));
	_pZUnits = new QLabel();
	QDoubleSpinBox *pZBox = new QDoubleSpinBox();
	pZBox->setObjectName("pz");
	pZBox->setMinimum(-1000000);
	pZBox->setMaximum(1000000);
	pZBox->setSingleStep(0.5);
	pZLabel->setBuddy(pZBox);
	QWidget::connect(pZBox, SIGNAL(valueChanged(double)), this, SLOT(submitPZ(double)));
	pZBox->setToolTip("Set the Z position of the sphere");
	pZBox->setToolTipDuration(-1);

	// radius
	QLabel *lXLabel = new QLabel(tr("Radius:"));
	_lXUnits = new QLabel();
	QDoubleSpinBox *lXBox = new QDoubleSpinBox();
	lXBox->setObjectName("radius");
	lXBox->setMinimum(0);
	lXBox->setMaximum(100);
	lXBox->setSingleStep(0.5);
	lXLabel->setBuddy(lXBox);
	QWidget::connect(lXBox, SIGNAL(valueChanged(double)), this, SLOT(submitL1(double)));
	lXBox->setToolTip("Set the radius of the sphere");
	lXBox->setToolTipDuration(-1);

	// mass
	QLabel *massLabel = new QLabel(tr("Mass:"));
	_massUnits = new QLabel();
	QDoubleSpinBox *massBox = new QDoubleSpinBox();
	massBox->setObjectName("mass");
	massBox->setMinimum(0);
	massBox->setMaximum(100000);
	massBox->setSingleStep(0.5);
	massLabel->setBuddy(massBox);
	QWidget::connect(massBox, SIGNAL(valueChanged(double)), this, SLOT(submitMass(double)));
	massBox->setToolTip("Set the mass of the sphere");
	massBox->setToolTipDuration(-1);

	// color
	_colorPicker = new bodyColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the sphere");
	_colorPicker->setToolTipDuration(-1);

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
	QHBoxLayout *hbox5 = new QHBoxLayout();
	hbox5->addWidget(massLabel, 2, Qt::AlignRight);
	hbox5->addWidget(massBox, 5);
	hbox5->addWidget(_massUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox5);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(_colorPicker);
	layout->addLayout(hbox6);
	layout->addStretch(2);
	this->setLayout(layout);
}

void sphereEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::P_X), value);
}

void sphereEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::P_Y), value);
}

void sphereEditor::submitPZ(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::P_Z), value);
}

void sphereEditor::submitL1(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::L_1), value);
}

void sphereEditor::submitMass(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::MASS), value);
}

void sphereEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsObstacleModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void sphereEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsObstacleModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsObstacleModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("pz"))->setValue(_model->data(_model->index(row, rsObstacleModel::P_Z), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("radius"))->setValue(_model->data(_model->index(row, rsObstacleModel::L_1), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("mass"))->setValue(_model->data(_model->index(row, rsObstacleModel::MASS), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsObstacleModel::COLOR), Qt::EditRole).toString());
	(this->findChild<bodyColorPicker *>("color"))->setColor(color);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void sphereEditor::setUnits(bool si) {
	QString text;
	if (si) {
		text = tr("cm");
		_massUnits->setText(tr("kg"));
	}
	else {
		text = tr("in");
		_massUnits->setText(tr("lb"));
	}
	_pXUnits->setText(text);
	_pYUnits->setText(text);
	_pZUnits->setText(text);
	_lXUnits->setText(text);
}

/*!
 *
 *
 *	Text Editor
 *
 *
 */

/*!	\brief Text Drawing Editor.
 *
 *	Build individual text editor with relevant pieces of information.
 *
 *	\param		model data model from obstacleEditor model.
 */
textEditor::textEditor(obstacleModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Text Editor</span>"));

	// label
	QLabel *nameLabel = new QLabel(tr("Text:"));
	QLineEdit *nameEdit = new QLineEdit;
	nameEdit->setObjectName("name");
	nameLabel->setBuddy(nameEdit);
	QWidget::connect(nameEdit, SIGNAL(textEdited(QString)), this, SLOT(submitName(QString)));
	nameEdit->setToolTip("Set the text to display");
	nameEdit->setToolTipDuration(-1);

	// position x
	QLabel *pXLabel = new QLabel(tr("Pos X:"));
	_pXUnits = new QLabel();
	QDoubleSpinBox *pXBox = new QDoubleSpinBox();
	pXBox->setObjectName("px");
	pXBox->setMinimum(-1000000);
	pXBox->setMaximum(1000000);
	pXBox->setSingleStep(0.5);
	pXLabel->setBuddy(pXBox);
	QWidget::connect(pXBox, SIGNAL(valueChanged(double)), this, SLOT(submitPX(double)));
	pXBox->setToolTip("Set the X position of the text");
	pXBox->setToolTipDuration(-1);

	// position y
	QLabel *pYLabel = new QLabel(tr("Pos Y:"));
	_pYUnits = new QLabel();
	QDoubleSpinBox *pYBox = new QDoubleSpinBox();
	pYBox->setObjectName("py");
	pYBox->setMinimum(-1000000);
	pYBox->setMaximum(1000000);
	pYBox->setSingleStep(0.5);
	pYLabel->setBuddy(pYBox);
	QWidget::connect(pYBox, SIGNAL(valueChanged(double)), this, SLOT(submitPY(double)));
	pYBox->setToolTip("Set the Y position of the text");
	pYBox->setToolTipDuration(-1);

	// position z
	QLabel *pZLabel = new QLabel(tr("Pos Z:"));
	_pZUnits = new QLabel();
	QDoubleSpinBox *pZBox = new QDoubleSpinBox();
	pZBox->setObjectName("pz");
	pZBox->setMinimum(-1000000);
	pZBox->setMaximum(1000000);
	pZBox->setSingleStep(0.5);
	pZLabel->setBuddy(pZBox);
	QWidget::connect(pZBox, SIGNAL(valueChanged(double)), this, SLOT(submitPZ(double)));
	pZBox->setToolTip("Set the Z position of the text");
	pZBox->setToolTipDuration(-1);

	// size
	QLabel *sizeLabel = new QLabel(tr("Size:"));
	QDoubleSpinBox *sizeBox = new QDoubleSpinBox();
	sizeBox->setObjectName("size");
	sizeBox->setMinimum(0);
	sizeBox->setMaximum(100);
	sizeBox->setSingleStep(1);
	sizeLabel->setBuddy(sizeBox);
	QWidget::connect(sizeBox, SIGNAL(valueChanged(double)), this, SLOT(submitSize(double)));
	sizeBox->setToolTip("Set the size of the text");
	sizeBox->setToolTipDuration(-1);

	// color
	_colorPicker = new bodyColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the box");
	_colorPicker->setToolTipDuration(-1);

	// lay out grid
	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *hbox0 = new QHBoxLayout();
	hbox0->addWidget(title, 5, Qt::AlignHCenter);
	layout->addLayout(hbox0);
	layout->addStretch(1);
	QHBoxLayout *hbox1 = new QHBoxLayout();
	hbox1->addWidget(nameLabel, 2, Qt::AlignRight);
	hbox1->addWidget(nameEdit, 5);
	hbox1->addStretch(1);
	layout->addLayout(hbox1);
	QHBoxLayout *hbox2 = new QHBoxLayout();
	hbox2->addWidget(pXLabel, 2, Qt::AlignRight);
	hbox2->addWidget(pXBox, 5);
	hbox2->addWidget(_pXUnits, 1, Qt::AlignLeft);
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
	QHBoxLayout *hbox5 = new QHBoxLayout();
	hbox5->addWidget(sizeLabel, 2, Qt::AlignRight);
	hbox5->addWidget(sizeBox, 5);
	layout->addLayout(hbox5);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(_colorPicker);
	layout->addLayout(hbox6);
	layout->addStretch(2);
	this->setLayout(layout);
}

void textEditor::submitName(QString value) {
	_model->setData(_model->index(_row, rsObstacleModel::TEXT), value);
}

void textEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::P_X), value);
}

void textEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::P_Y), value);
}

void textEditor::submitPZ(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::P_Z), value);
}

void textEditor::submitSize(double value) {
	_model->setData(_model->index(_row, rsObstacleModel::SIZE), value);
}

void textEditor::submitColor(QColor value) {
	_model->setData(_model->index(_row, rsObstacleModel::COLOR), value);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void textEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QLineEdit *>("name"))->setText(_model->data(_model->index(row, rsObstacleModel::TEXT), Qt::EditRole).toString());
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsObstacleModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsObstacleModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("pz"))->setValue(_model->data(_model->index(row, rsObstacleModel::P_Z), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("size"))->setValue(_model->data(_model->index(row, rsObstacleModel::SIZE), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsObstacleModel::COLOR), Qt::EditRole).toString());
	(this->findChild<bodyColorPicker *>("color"))->setColor(color);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void textEditor::setUnits(bool si) {
	QString text;
	if (si) text = tr("cm");
	else text = tr("in");
	_pXUnits->setText(text);
	_pYUnits->setText(text);
	_pZUnits->setText(text);
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
	QString s = "background-color: ";
	_button->setStyleSheet(s + _color.name());
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
	QString s = "background-color: ";
	_button->setStyleSheet(s + _color.name());
	emit colorChanged(color);
}

void bodyColorPicker::onButtonClicked(void) {
	const QColor color = QColorDialog::getColor(_color, this);
	if (!color.isValid())
		return;

	this->setColor(color);
}

