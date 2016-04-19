#include "roboteditor.h"

/*!
 *
 *
 *	RobotEditor
 *
 *
 */

robotEditor::robotEditor(robotModel *model, QWidget *parent) : QWidget(parent) {
	// store robot model
	_model = model;

	// set size properties
	QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Preferred);
	sp.setHorizontalStretch(1);
	this->setSizePolicy(sp);

	// set up editor pages
	_pages = new QStackedWidget;
	_pages->addWidget(new blankEditor());
	_pages->addWidget(new linkbotIEditor(_model));
	_pages->addWidget(new linkbotLEditor(_model));
	_pages->addWidget(new customEditor(_model));
	_pages->addWidget(new preconfigEditor(_model));
	_pages->addWidget(new mindstormsEditor(_model));

	// set up buttons
	_deleteButton = new QPushButton(tr("Delete"));
	_deleteButton->setEnabled(false);
	_deleteButton->setFixedWidth(75);
	_deleteButton->setToolTip("Delete the robot");
	_deleteButton->setToolTipDuration(-1);
	_nextButton = new QPushButton(tr("Next"));
	_nextButton->setEnabled(false);
	_nextButton->setFixedWidth(75);
	_nextButton->setToolTip("Edit the next robot in the list");
	_nextButton->setToolTipDuration(-1);
	_previousButton = new QPushButton(tr("Previous"));
	_previousButton->setEnabled(false);
	_previousButton->setFixedWidth(75);
	_previousButton->setToolTip("Edit the previous robot in the list");
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
	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(_previousButton, 0, Qt::AlignCenter);
	hbox->addWidget(_deleteButton, 1, Qt::AlignCenter);
	hbox->addWidget(_nextButton, 0, Qt::AlignCenter);
	vbox->addLayout(hbox);
	this->setLayout(vbox);

	// set variables for tracking editor
	_form = -1;
	_row = -1;
}

void robotEditor::dataChanged(QModelIndex/*topLeft*/, QModelIndex bottomRight) {
	int form = _model->data(_model->index(bottomRight.row(), rsRobotModel::FORM)).toInt();
	if (bottomRight.row() != _row || form != _form) {
		_form = form;
		this->setCurrentIndex(bottomRight);
	}
}

void robotEditor::setCurrentIndex(const QModelIndex &index) {
	if (index.isValid()) {
		// set new curent model row
		_row = index.row();
		// get new robot form
		_form = _model->data(_model->index(index.row(), rsRobotModel::FORM)).toInt();

		// load appropriate page
		if (_form == rs::EV3 || _form == rs::NXT) {
			_pages->setCurrentIndex(5);	// mindstorms
			dynamic_cast<mindstormsEditor *>(_pages->currentWidget())->setIndex(_row);
			dynamic_cast<mindstormsEditor *>(_pages->currentWidget())->setUnits(_units);
		}
		else {
			if (_model->data(_model->index(index.row(), rsRobotModel::PRECONFIG), Qt::EditRole).toInt()) {
				_pages->setCurrentIndex(4);	// preconfig
				dynamic_cast<preconfigEditor *>(_pages->currentWidget())->setIndex(_row);
				dynamic_cast<preconfigEditor *>(_pages->currentWidget())->setUnits(_units);
			}
			else {
				if (_form == rs::LinkbotL) {
					_pages->setCurrentIndex(2);	// Linkbot-L
					dynamic_cast<linkbotLEditor *>(_pages->currentWidget())->setIndex(_row);
					dynamic_cast<linkbotLEditor *>(_pages->currentWidget())->setUnits(_units);
				}
				else if (_model->data(_model->index(index.row(), rsRobotModel::WHEELLEFT), Qt::EditRole).toInt() == 4) {
					_pages->setCurrentIndex(3);	// custom wheeled Linkbot-I
					dynamic_cast<customEditor *>(_pages->currentWidget())->setIndex(_row);
					dynamic_cast<customEditor *>(_pages->currentWidget())->setUnits(_units);
				}
				else {
					_pages->setCurrentIndex(1);	// Linkbot-I
					dynamic_cast<linkbotIEditor *>(_pages->currentWidget())->setIndex(_row);
					dynamic_cast<linkbotIEditor *>(_pages->currentWidget())->setUnits(_units);
				}
			}
		}

		// update button states
		_deleteButton->setEnabled(true);
		_nextButton->setEnabled(index.row() < _model->rowCount() - 1);
		_previousButton->setEnabled(index.row() > 0);
	}
	else {
		// show blank editor page
		_pages->setCurrentIndex(0);
		_form = -1;
		_row = -1;

		// disable all buttons
		_deleteButton->setDisabled(true);
		_nextButton->setDisabled(true);
		_previousButton->setDisabled(true);
	}
}

void robotEditor::nextButtonPressed(void) {
	// set new index
	QModelIndex index = _model->index(_row + 1, 0);
	this->setCurrentIndex(index);

	// signal other views that index has changed
	emit indexChanged(index);
}

void robotEditor::prevButtonPressed(void) {
	// set new index
	QModelIndex index = _model->index(_row - 1, 0);
	this->setCurrentIndex(index);

	// signal other views that index has changed
	emit indexChanged(index);
}

void robotEditor::deleteCurrentIndex(void) {
	// save current index
	int row = _row;

	// remove current obstacle from model
	_model->removeRows(row, 1);

	// if it is invalid, then set the last row in the model
	if (row >= _model->rowCount())
		row = _model->rowCount() - 1;

	// set new index
	this->setCurrentIndex(_model->index(row, rsRobotModel::ID));

	// signal a change in current obstacle
	emit indexChanged(_model->index(row, rsRobotModel::ID));
}

void robotEditor::setUnits(bool si) {
	// do nothing if units don't change
	if (_units == si) return;

	// save units
	_units = si;

	// reload current editor
	this->setCurrentIndex(_model->index(_row, rsRobotModel::FORM));
}

/*!
*
*
*	Empty Editor
*
*
*/
blankEditor::blankEditor(QWidget *parent) : QWidget(parent) {
	// set title
	QLabel *title = new QLabel(tr("<span style=\"font-size:10pt; font-weight:bold;\">Robot Editor</span>"));

	// set note to user
	QLabel *note1 = new QLabel(tr("<span style=\"font-size:9pt;\">There are no robots in the Scene.</span>"));
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
 *	linkbotEditor
 *
 *
 */

/*!	\brief linkbot Robot Editor.
 *
 *	Build linkbot robot editor with relevant pieces of information.
 *
 *	\param		model data model from robotEditor model.
 */
linkbotIEditor::linkbotIEditor(robotModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Linkbot-I Editor</span>"));

	// form list
	QLabel *formLabel = new QLabel(tr("Form:"));
	QStringList formItems;
	formItems << "Linkbot I" << "Linkbot L" << "Mindstorms EV3/NXT";
	QStringListModel *formModel = new QStringListModel(formItems, this);
	QComboBox *formBox = new QComboBox();
	formBox->setObjectName("form");
	formBox->setModel(formModel);
	formLabel->setBuddy(formBox);
	QWidget::connect(formBox, SIGNAL(currentIndexChanged(int)), this, SLOT(submitForm(int)));
	formBox->setToolTip("Pick the type of robot");
	formBox->setToolTipDuration(-1);

	// name
	QLabel *nameLabel = new QLabel(tr("Name:"));
	QLineEdit *nameEdit = new QLineEdit;
	nameEdit->setObjectName("name");
	nameLabel->setBuddy(nameEdit);
	QWidget::connect(nameEdit, SIGNAL(textEdited(QString)), this, SLOT(submitName(QString)));
	nameEdit->setToolTip("Set a name for the robot");
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
	pXBox->setToolTip("Set the X position of the robot");
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
	pYBox->setToolTip("Set the Y position of the robot");
	pYBox->setToolTipDuration(-1);

	// rotation psi
	QLabel *rZLabel = new QLabel(tr("Angle:"));
	QLabel *rZUnits = new QLabel(QString::fromUtf8("°"));
	_rZBox = new QDoubleSpinBox();
	_rZBox->setObjectName("rz");
	_rZBox->setMinimum(-360);
	_rZBox->setMaximum(360);
	_rZBox->setSingleStep(0.5);
	rZLabel->setBuddy(_rZBox);
	QWidget::connect(_rZBox, SIGNAL(valueChanged(double)), this, SLOT(submitRZ(double)));
	_rZBox->setToolTip("Set the rotation of the robot");
	_rZBox->setToolTipDuration(-1);

	// left wheel list
	QLabel *wheelLLabel = new QLabel(tr("Left Wheel:"));
	QStringList wheelLItems;
	//wheelLItems << "None" << "4.13" << "4.45" << "5.08" << tr("Custom");
	wheelLItems << "None" << "4.13" << "4.45" << "5.08";
	QStringListModel *wheelLModel = new QStringListModel(wheelLItems, this);
	_wheelLUnits = new QLabel(tr("cm"));
	_wheelLBox = new QComboBox();
	_wheelLBox->setObjectName("wheelLeft");
	_wheelLBox->setModel(wheelLModel);
	wheelLLabel->setBuddy(_wheelLBox);
	QWidget::connect(_wheelLBox, SIGNAL(currentIndexChanged(int)), this, SLOT(submitLeftWheel(int)));
	_wheelLBox->setToolTip("Pick the left wheel of the robot");
	_wheelLBox->setToolTipDuration(-1);

	// right wheel list
	QLabel *wheelRLabel = new QLabel(tr("Right Wheel:"));
	QStringList wheelRItems;
	//wheelRItems << "None" << "4.13" << "4.45" << "5.08" << tr("Custom");
	wheelRItems << "None" << "4.13" << "4.45" << "5.08";
	QStringListModel *wheelRModel = new QStringListModel(wheelRItems, this);
	_wheelRUnits = new QLabel(tr("cm"));
	_wheelRBox = new QComboBox();
	_wheelRBox->setObjectName("wheelRight");
	_wheelRBox->setModel(wheelRModel);
	wheelRLabel->setBuddy(_wheelRBox);
	QWidget::connect(_wheelRBox, SIGNAL(currentIndexChanged(int)), this, SLOT(submitRightWheel(int)));
	_wheelRBox->setToolTip("Pick the right wheel of the robot");
	_wheelRBox->setToolTipDuration(-1);

	// color
	_colorPicker = new ledColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the robot's LED");
	_colorPicker->setToolTipDuration(-1);

	// lay out grid
	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *hbox0 = new QHBoxLayout();
	hbox0->addWidget(title, 5, Qt::AlignHCenter);
	layout->addLayout(hbox0);
	QHBoxLayout *hbox1 = new QHBoxLayout();
	hbox1->addWidget(formLabel, 2, Qt::AlignRight);
	hbox1->addWidget(formBox, 5);
	hbox1->addStretch(1);
	layout->addLayout(hbox1);
	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(nameLabel, 2, Qt::AlignRight);
	hbox->addWidget(nameEdit, 5);
	hbox->addStretch(1);
	layout->addLayout(hbox);
	QHBoxLayout *hbox2 = new QHBoxLayout();
	hbox2->addWidget(pXLabel, 2, Qt::AlignRight);
	hbox2->addWidget(pXBox, 5);
	hbox2->addWidget(_pXUnits, 1, Qt::AlignLeft);
	hbox2->addWidget(pYLabel, 2, Qt::AlignRight);
	hbox2->addWidget(pYBox, 5);
	hbox2->addWidget(_pYUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox2);
	QHBoxLayout *hbox4 = new QHBoxLayout();
	hbox4->addWidget(rZLabel, 2, Qt::AlignRight);
	hbox4->addWidget(_rZBox, 5);
	hbox4->addWidget(rZUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox4);
	QHBoxLayout *hbox5 = new QHBoxLayout();
	hbox5->addWidget(wheelLLabel, 2, Qt::AlignRight);
	hbox5->addWidget(_wheelLBox, 5);
	hbox5->addWidget(_wheelLUnits, 1, Qt::AlignLeft);
	hbox5->addWidget(wheelRLabel, 2, Qt::AlignRight);
	hbox5->addWidget(_wheelRBox, 5);
	hbox5->addWidget(_wheelRUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox5);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(_colorPicker);
	layout->addLayout(hbox6);
	layout->addStretch(1);
	this->setLayout(layout);
}

void linkbotIEditor::submitForm(int value) {
	int form = -1;
	if (value == LINKBOTI) form = rs::LinkbotI;
	else if (value == LINKBOTL) form = rs::LinkbotL;
	//else if (value == EV3) form = rs::EV3;
	else if (value == NXT) form = rs::NXT;
	_model->setData(_model->index(_row, rsRobotModel::FORM), form);
}

void linkbotIEditor::submitName(QString value) {
	int form = _model->data(_model->index(_row, rsRobotModel::FORM), Qt::EditRole).toInt();
	if (form == rs::LinkbotI)
		_model->setData(_model->index(_row, rsRobotModel::NAME), value);
}

void linkbotIEditor::submitPX(double value) {
	int form = _model->data(_model->index(_row, rsRobotModel::FORM), Qt::EditRole).toInt();
	if (form == rs::LinkbotI)
		_model->setData(_model->index(_row, rsRobotModel::P_X), value);
}

void linkbotIEditor::submitPY(double value) {
	int form = _model->data(_model->index(_row, rsRobotModel::FORM), Qt::EditRole).toInt();
	if (form == rs::LinkbotI)
		_model->setData(_model->index(_row, rsRobotModel::P_Y), value);
}

void linkbotIEditor::submitRZ(double value) {
	_rZBox->setValue(value - static_cast<int>(value / 360) * 360);
	int form = _model->data(_model->index(_row, rsRobotModel::FORM), Qt::EditRole).toInt();
	if (form == rs::LinkbotI)
		_model->setData(_model->index(_row, rsRobotModel::R_PSI), value);
}

void linkbotIEditor::submitLeftWheel(int value) {
	int form = _model->data(_model->index(_row, rsRobotModel::FORM), Qt::EditRole).toInt();
	if (form == rs::LinkbotI)
		_model->setData(_model->index(_row, rsRobotModel::WHEELLEFT), value);
}

void linkbotIEditor::submitRightWheel(int value) {
	int form = _model->data(_model->index(_row, rsRobotModel::FORM), Qt::EditRole).toInt();
	if (form == rs::LinkbotI)
		_model->setData(_model->index(_row, rsRobotModel::WHEELRIGHT), value);
}

void linkbotIEditor::submitColor(QColor color) {
	int form = _model->data(_model->index(_row, rsRobotModel::FORM), Qt::EditRole).toInt();
	if (form == rs::LinkbotI)
		_model->setData(_model->index(_row, rsRobotModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void linkbotIEditor::setIndex(int row) {
	_row = row;
	int form = _model->data(_model->index(row, rsRobotModel::FORM), Qt::EditRole).toInt();
	int ind = -1;
	if (form == rs::LinkbotI) ind = LINKBOTI;
	else if (form == rs::LinkbotL) ind = LINKBOTL;
	//else if (form == rs::EV3) ind = EV3;
	else if (form == rs::NXT) ind = NXT;
	(this->findChild<QComboBox *>("form"))->setCurrentIndex(ind);
	(this->findChild<QLineEdit *>("name"))->setText(_model->data(_model->index(row, rsRobotModel::NAME), Qt::EditRole).toString());
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsRobotModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsRobotModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("rz"))->setValue(_model->data(_model->index(row, rsRobotModel::R_PSI), Qt::EditRole).toDouble());
	(this->findChild<QComboBox *>("wheelLeft"))->setCurrentIndex(_model->data(_model->index(row, rsRobotModel::WHEELLEFT), Qt::EditRole).toInt());
	(this->findChild<QComboBox *>("wheelRight"))->setCurrentIndex(_model->data(_model->index(row, rsRobotModel::WHEELRIGHT), Qt::EditRole).toInt());
	QColor color(_model->data(_model->index(row, rsRobotModel::COLOR), Qt::EditRole).toString());
	(this->findChild<ledColorPicker *>("color"))->setColor(color);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void linkbotIEditor::setUnits(bool si) {
	// update unit labels
	QString text;
	if (si) text = tr("cm");
	else text = tr("in");
	_pXUnits->setText(text);
	_pYUnits->setText(text);
	_wheelLUnits->setText(text);
	_wheelRUnits->setText(text);

	// set wheel list to new values
	QStringList wheelLItems, wheelRItems;
	if (si) {
		//wheelLItems << "None" << "4.13" << "4.45" << "5.08" << tr("Custom");
		//wheelRItems << "None" << "4.13" << "4.45" << "5.08" << tr("Custom");
		wheelLItems << "None" << "4.13" << "4.45" << "5.08";
		wheelRItems << "None" << "4.13" << "4.45" << "5.08";
	}
	else {
		//wheelLItems << "None" << "1.625" << "1.75" << "2.00" << tr("Custom");
		//wheelRItems << "None" << "1.625" << "1.75" << "2.00" << tr("Custom");
		wheelLItems << "None" << "1.625" << "1.75" << "2.00";
		wheelRItems << "None" << "1.625" << "1.75" << "2.00";
	}
	QStringListModel *wheelLModel = new QStringListModel(wheelLItems, this);
	QStringListModel *wheelRModel = new QStringListModel(wheelRItems, this);
	int row = _wheelLBox->currentIndex();
	_wheelLBox->setModel(wheelLModel);
	_wheelLBox->setCurrentIndex(row);
	row = _wheelRBox->currentIndex();
	_wheelRBox->setModel(wheelRModel);
	_wheelRBox->setCurrentIndex(row);
}

/*!
 *
 *
 *	linkbotLEditor
 *
 *
 */

/*!	\brief linkbot Robot Editor.
 *
 *	Build linkbot robot editor with relevant pieces of information.
 *
 *	\param		model data model from robotEditor model.
 */
linkbotLEditor::linkbotLEditor(robotModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Linkbot-L Editor</span>"));

	// form list
	QLabel *formLabel = new QLabel(tr("Form:"));
	QStringList formItems;
	formItems << "Linkbot I" << "Linkbot L" << "Mindstorms EV3/NXT";
	QStringListModel *formModel = new QStringListModel(formItems, this);
	QComboBox *formBox = new QComboBox();
	formBox->setObjectName("form");
	formBox->setModel(formModel);
	formLabel->setBuddy(formBox);
	QWidget::connect(formBox, SIGNAL(currentIndexChanged(int)), this, SLOT(submitForm(int)));
	formBox->setToolTip("Pick the type of robot");
	formBox->setToolTipDuration(-1);

	// name
	QLabel *nameLabel = new QLabel(tr("Name:"));
	QLineEdit *nameEdit = new QLineEdit;
	nameEdit->setObjectName("name");
	nameLabel->setBuddy(nameEdit);
	QWidget::connect(nameEdit, SIGNAL(textChanged(QString)), this, SLOT(submitName(QString)));
	nameEdit->setToolTip("Set a name for the robot");
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
	pXBox->setToolTip("Set the X position of the robot");
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
	pYBox->setToolTip("Set the Y position of the robot");
	pYBox->setToolTipDuration(-1);

	// rotation psi
	QLabel *rZLabel = new QLabel(tr("Angle:"));
	QLabel *rZUnits = new QLabel(QString::fromUtf8("°"));
	_rZBox = new QDoubleSpinBox();
	_rZBox->setObjectName("rz");
	_rZBox->setMinimum(-360);
	_rZBox->setMaximum(360);
	_rZBox->setSingleStep(0.5);
	rZLabel->setBuddy(_rZBox);
	QWidget::connect(_rZBox, SIGNAL(valueChanged(double)), this, SLOT(submitRZ(double)));
	_rZBox->setToolTip("Set the rotation of the robot");
	_rZBox->setToolTipDuration(-1);

	// color
	_colorPicker = new ledColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the robot's LED");
	_colorPicker->setToolTipDuration(-1);

	// lay out grid
	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *hbox0 = new QHBoxLayout();
	hbox0->addWidget(title, 5, Qt::AlignHCenter);
	layout->addLayout(hbox0);
	QHBoxLayout *hbox1 = new QHBoxLayout();
	hbox1->addWidget(formLabel, 2, Qt::AlignRight);
	hbox1->addWidget(formBox, 5);
	hbox1->addStretch(1);
	layout->addLayout(hbox1);
	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(nameLabel, 2, Qt::AlignRight);
	hbox->addWidget(nameEdit, 5);
	hbox->addStretch(1);
	layout->addLayout(hbox);
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
	hbox4->addWidget(rZLabel, 2, Qt::AlignRight);
	hbox4->addWidget(_rZBox, 5);
	hbox4->addWidget(rZUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox4);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(_colorPicker);
	layout->addLayout(hbox6);
	layout->addStretch(1);
	this->setLayout(layout);
}

void linkbotLEditor::submitForm(int value) {
	int form = -1;
	if (value == LINKBOTI) form = rs::LinkbotI;
	else if (value == LINKBOTL) form = rs::LinkbotL;
	//else if (value == EV3) form = rs::EV3;
	else if (value == NXT) form = rs::NXT;
	_model->setData(_model->index(_row, rsRobotModel::FORM), form);
}

void linkbotLEditor::submitName(QString value) {
	int form = _model->data(_model->index(_row, rsRobotModel::FORM), Qt::EditRole).toInt();
	if (form == rs::LinkbotL)
		_model->setData(_model->index(_row, rsRobotModel::NAME), value);
}

void linkbotLEditor::submitPX(double value) {
	int form = _model->data(_model->index(_row, rsRobotModel::FORM), Qt::EditRole).toInt();
	if (form == rs::LinkbotL)
		_model->setData(_model->index(_row, rsRobotModel::P_X), value);
}

void linkbotLEditor::submitPY(double value) {
	int form = _model->data(_model->index(_row, rsRobotModel::FORM), Qt::EditRole).toInt();
	if (form == rs::LinkbotL)
		_model->setData(_model->index(_row, rsRobotModel::P_Y), value);
}

void linkbotLEditor::submitRZ(double value) {
	_rZBox->setValue(value - static_cast<int>(value / 360) * 360);
	int form = _model->data(_model->index(_row, rsRobotModel::FORM), Qt::EditRole).toInt();
	if (form == rs::LinkbotL)
		_model->setData(_model->index(_row, rsRobotModel::R_PSI), value);
}

void linkbotLEditor::submitColor(QColor color) {
	int form = _model->data(_model->index(_row, rsRobotModel::FORM), Qt::EditRole).toInt();
	if (form == rs::LinkbotL)
		_model->setData(_model->index(_row, rsRobotModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void linkbotLEditor::setIndex(int row) {
	_row = row;
	int form = _model->data(_model->index(row, rsRobotModel::FORM), Qt::EditRole).toInt();
	int ind = -1;
	if (form == rs::LinkbotI) ind = LINKBOTI;
	else if (form == rs::LinkbotL) ind = LINKBOTL;
	//else if (form == rs::EV3) ind = EV3;
	else if (form == rs::NXT) ind = NXT;
	(this->findChild<QComboBox *>("form"))->setCurrentIndex(ind);
	(this->findChild<QLineEdit *>("name"))->setText(_model->data(_model->index(row, rsRobotModel::NAME), Qt::EditRole).toString());
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsRobotModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsRobotModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("rz"))->setValue(_model->data(_model->index(row, rsRobotModel::R_PSI), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsRobotModel::COLOR), Qt::EditRole).toString());
	(this->findChild<ledColorPicker *>("color"))->setColor(color);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void linkbotLEditor::setUnits(bool si) {
	// update unit labels
	QString text;
	if (si) text = tr("cm");
	else text = tr("in");
	_pXUnits->setText(text);
	_pYUnits->setText(text);
}

/*!
 *
 *
 *	MindstormsEditor
 *
 *
 */

/*!	\brief Mindstorms Robot Editor.
 *
 *	Build mindstorms robot editor with relevant pieces of information.
 *
 *	\param		model data model from robotEditor model.
 */
mindstormsEditor::mindstormsEditor(robotModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Mindstorms Editor</span>"));

	// form list
	QLabel *formLabel = new QLabel(tr("Form:"));
	QStringList formItems;
	formItems << "Linkbot I" << "Linkbot L" << "Mindstorms EV3/NXT";
	QStringListModel *formModel = new QStringListModel(formItems, this);
	QComboBox *formBox = new QComboBox();
	formBox->setObjectName("form");
	formBox->setModel(formModel);
	formLabel->setBuddy(formBox);
	QWidget::connect(formBox, SIGNAL(currentIndexChanged(int)), this, SLOT(submitForm(int)));
	formBox->setToolTip("Pick the type of robot");
	formBox->setToolTipDuration(-1);

	// name
	QLabel *nameLabel = new QLabel(tr("Name:"));
	QLineEdit *nameEdit = new QLineEdit;
	nameEdit->setObjectName("name");
	nameLabel->setBuddy(nameEdit);
	QWidget::connect(nameEdit, SIGNAL(textChanged(QString)), this, SLOT(submitName(QString)));
	nameEdit->setToolTip("Set a name for the robot");
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
	pXBox->setToolTip("Set the X position of the robot");
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
	pYBox->setToolTip("Set the Y position of the robot");
	pYBox->setToolTipDuration(-1);

	// rotation psi
	QLabel *rZLabel = new QLabel(tr("Angle:"));
	QLabel *rZUnits = new QLabel(QString::fromUtf8("°"));
	_rZBox = new QDoubleSpinBox();
	_rZBox->setObjectName("rz");
	_rZBox->setMinimum(-360);
	_rZBox->setMaximum(360);
	_rZBox->setSingleStep(0.5);
	rZLabel->setBuddy(_rZBox);
	QWidget::connect(_rZBox, SIGNAL(valueChanged(double)), this, SLOT(submitRZ(double)));
	_rZBox->setToolTip("Set the rotation of the robot");
	_rZBox->setToolTipDuration(-1);

	// left wheel list
	QLabel *wheelLLabel = new QLabel(tr("Left Wheel:"));
	QStringList wheelLItems;
	wheelLItems << "2.80" << "4.10";
	QStringListModel *wheelLModel = new QStringListModel(wheelLItems, this);
	_wheelLUnits = new QLabel(tr("cm"));
	_wheelLBox = new QComboBox();
	_wheelLBox->setObjectName("wheelLeft");
	_wheelLBox->setModel(wheelLModel);
	wheelLLabel->setBuddy(_wheelLBox);
	QWidget::connect(_wheelLBox, SIGNAL(currentIndexChanged(int)), this, SLOT(submitLeftWheel(int)));
	_wheelLBox->setToolTip("Pick the left wheel of the robot");
	_wheelLBox->setToolTipDuration(-1);

	// right wheel list
	QLabel *wheelRLabel = new QLabel(tr("Right Wheel:"));
	QStringList wheelRItems;
	wheelRItems << "None" << "4.13" << "4.45" << "5.08";
	QStringListModel *wheelRModel = new QStringListModel(wheelRItems, this);
	_wheelRUnits = new QLabel(tr("cm"));
	_wheelRBox = new QComboBox();
	_wheelRBox->setObjectName("wheelRight");
	_wheelRBox->setModel(wheelRModel);
	wheelRLabel->setBuddy(_wheelRBox);
	QWidget::connect(_wheelRBox, SIGNAL(currentIndexChanged(int)), this, SLOT(submitRightWheel(int)));
	_wheelRBox->setToolTip("Pick the right wheel of the robot");
	_wheelRBox->setToolTipDuration(-1);

	// color
	_colorPicker = new ledColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the robot's LED");
	_colorPicker->setToolTipDuration(-1);

	// lay out grid
	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *hbox0 = new QHBoxLayout();
	hbox0->addWidget(title, 5, Qt::AlignHCenter);
	layout->addLayout(hbox0);
	QHBoxLayout *hbox1 = new QHBoxLayout();
	hbox1->addWidget(formLabel, 2, Qt::AlignRight);
	hbox1->addWidget(formBox, 5);
	hbox1->addStretch(1);
	layout->addLayout(hbox1);
	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(nameLabel, 2, Qt::AlignRight);
	hbox->addWidget(nameEdit, 5);
	hbox->addStretch(1);
	layout->addLayout(hbox);
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
	hbox4->addWidget(rZLabel, 2, Qt::AlignRight);
	hbox4->addWidget(_rZBox, 5);
	hbox4->addWidget(rZUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox4);
	QHBoxLayout *hbox5 = new QHBoxLayout();
	hbox5->addWidget(wheelLLabel, 2, Qt::AlignRight);
	hbox5->addWidget(_wheelLBox, 5);
	hbox5->addWidget(_wheelLUnits, 1, Qt::AlignLeft);
	hbox5->addWidget(wheelRLabel, 2, Qt::AlignRight);
	hbox5->addWidget(_wheelRBox, 5);
	hbox5->addWidget(_wheelRUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox5);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(_colorPicker);
	layout->addLayout(hbox6);
	layout->addStretch(1);
	this->setLayout(layout);
}

void mindstormsEditor::submitForm(int value) {
	int form = -1;
	if (value == LINKBOTI) form = rs::LinkbotI;
	else if (value == LINKBOTL) form = rs::LinkbotL;
	//else if (value == EV3) form = rs::EV3;
	else if (value == NXT) form = rs::NXT;
	int mform = _model->data(_model->index(_row, rsRobotModel::FORM), Qt::EditRole).toInt();
	if (rs::EV3 == mform || mform == rs::NXT)
		_model->setData(_model->index(_row, rsRobotModel::FORM), form);
}

void mindstormsEditor::submitName(QString value) {
	int form = _model->data(_model->index(_row, rsRobotModel::FORM), Qt::EditRole).toInt();
	if (rs::EV3 == form || form == rs::NXT)
		_model->setData(_model->index(_row, rsRobotModel::NAME), value);
}

void mindstormsEditor::submitPX(double value) {
	int form = _model->data(_model->index(_row, rsRobotModel::FORM), Qt::EditRole).toInt();
	if (rs::EV3 == form || form == rs::NXT)
		_model->setData(_model->index(_row, rsRobotModel::P_X), value);
}

void mindstormsEditor::submitPY(double value) {
	int form = _model->data(_model->index(_row, rsRobotModel::FORM), Qt::EditRole).toInt();
	if (rs::EV3 == form || form == rs::NXT)
		_model->setData(_model->index(_row, rsRobotModel::P_Y), value);
}

void mindstormsEditor::submitRZ(double value) {
	_rZBox->setValue(value - static_cast<int>(value / 360) * 360);
	int form = _model->data(_model->index(_row, rsRobotModel::FORM), Qt::EditRole).toInt();
	if (rs::EV3 == form || form == rs::NXT)
		_model->setData(_model->index(_row, rsRobotModel::R_PSI), value);
}

void mindstormsEditor::submitLeftWheel(int value) {
	int form = _model->data(_model->index(_row, rsRobotModel::FORM), Qt::EditRole).toInt();
	if (rs::EV3 == form || form == rs::NXT)
		_model->setData(_model->index(_row, rsRobotModel::WHEELLEFT), value);
}

void mindstormsEditor::submitRightWheel(int value) {
	int form = _model->data(_model->index(_row, rsRobotModel::FORM), Qt::EditRole).toInt();
	if (rs::EV3 == form || form == rs::NXT)
		_model->setData(_model->index(_row, rsRobotModel::WHEELRIGHT), value);
}

void mindstormsEditor::submitColor(QColor color) {
	int form = _model->data(_model->index(_row, rsRobotModel::FORM), Qt::EditRole).toInt();
	if (rs::EV3 == form || form == rs::NXT)
		_model->setData(_model->index(_row, rsRobotModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void mindstormsEditor::setIndex(int row) {
	_row = row;
	int form = _model->data(_model->index(row, rsRobotModel::FORM), Qt::EditRole).toInt();
	int ind = -1;
	if (form == rs::LinkbotI) ind = LINKBOTI;
	else if (form == rs::LinkbotL) ind = LINKBOTL;
	//else if (form == rs::EV3) ind = EV3;
	else if (form == rs::NXT) ind = NXT;
	(this->findChild<QComboBox *>("form"))->setCurrentIndex(ind);
	(this->findChild<QLineEdit *>("name"))->setText(_model->data(_model->index(row, rsRobotModel::NAME), Qt::EditRole).toString());
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsRobotModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsRobotModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("rz"))->setValue(_model->data(_model->index(row, rsRobotModel::R_PSI), Qt::EditRole).toDouble());
	(this->findChild<QComboBox *>("wheelLeft"))->setCurrentIndex(_model->data(_model->index(row, rsRobotModel::WHEELLEFT), Qt::EditRole).toInt());
	(this->findChild<QComboBox *>("wheelRight"))->setCurrentIndex(_model->data(_model->index(row, rsRobotModel::WHEELRIGHT), Qt::EditRole).toInt());
	QColor color(_model->data(_model->index(row, rsRobotModel::COLOR), Qt::EditRole).toString());
	(this->findChild<ledColorPicker *>("color"))->setColor(color);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void mindstormsEditor::setUnits(bool si) {
	// update unit labels
	QString text;
	if (si) text = tr("cm");
	else text = tr("in");
	_pXUnits->setText(text);
	_pYUnits->setText(text);
	_wheelLUnits->setText(text);
	_wheelRUnits->setText(text);

	// set wheel list to new values
	QStringList wheelLItems, wheelRItems;
	if (si) {
		wheelLItems << "2.80" << "4.10";
		wheelRItems << "2.80" << "4.10";
	}
	else {
		wheelLItems << "1.10" << "1.61";
		wheelRItems << "1.10" << "1.61";
	}
	QStringListModel *wheelLModel = new QStringListModel(wheelLItems, this);
	QStringListModel *wheelRModel = new QStringListModel(wheelRItems, this);
	int row = _wheelLBox->currentIndex();
	_wheelLBox->setModel(wheelLModel);
	_wheelLBox->setCurrentIndex(row);
	row = _wheelRBox->currentIndex();
	_wheelRBox->setModel(wheelRModel);
	_wheelRBox->setCurrentIndex(row);
}

/*!
 *
 *
 *	CustomEditor
 *
 *
 */

/*!	\brief Custom Wheeled Robot Editor.
 *
 *	Build linkbot robot editor with custom wheel size.
 *
 *	\param		model data model from robotEditor model.
 */
customEditor::customEditor(robotModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Robot Editor</span>"));

	// form list
	QLabel *formLabel = new QLabel(tr("Form:"));
	QStringList formItems;
	formItems << "Linkbot I" << "Linkbot L" << "Mindstorms EV3/NXT";
	QStringListModel *formModel = new QStringListModel(formItems, this);
	QComboBox *formBox = new QComboBox();
	formBox->setObjectName("form");
	formBox->setModel(formModel);
	formLabel->setBuddy(formBox);
	QWidget::connect(formBox, SIGNAL(currentIndexChanged(int)), this, SLOT(submitForm(int)));
	formBox->setToolTip("Pick the type of robot");
	formBox->setToolTipDuration(-1);

	// name
	QLabel *nameLabel = new QLabel(tr("Name:"));
	QLineEdit *nameEdit = new QLineEdit;
	nameEdit->setObjectName("name");
	nameLabel->setBuddy(nameEdit);
	QWidget::connect(nameEdit, SIGNAL(textChanged(QString)), this, SLOT(submitName(QString)));
	nameEdit->setToolTip("Set a name for the robot");
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
	pXBox->setToolTip("Set the X position of the robot");
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
	pYBox->setToolTip("Set the Y position of the robot");
	pYBox->setToolTipDuration(-1);

	// rotation psi
	QLabel *rZLabel = new QLabel(tr("Angle:"));
	QLabel *rZUnits = new QLabel(QString::fromUtf8("°"));
	_rZBox = new QDoubleSpinBox();
	_rZBox->setObjectName("rz");
	_rZBox->setMinimum(-360);
	_rZBox->setMaximum(360);
	_rZBox->setSingleStep(0.5);
	rZLabel->setBuddy(_rZBox);
	QWidget::connect(_rZBox, SIGNAL(valueChanged(double)), this, SLOT(submitRZ(double)));
	_rZBox->setToolTip("Set the rotation of the robot");
	_rZBox->setToolTipDuration(-1);

	// wheels list
	QLabel *wheelLabel = new QLabel(tr("Wheels:"));
	_wheelUnits = new QLabel(tr("cm"));
	_wheelBox = new QComboBox();
	_wheelBox->setObjectName("wheels");
	wheelLabel->setBuddy(_wheelBox);
	QWidget::connect(_wheelBox, SIGNAL(currentIndexChanged(int)), this, SLOT(submitLeftWheel(int)));
	_wheelBox->setToolTip("Pick the wheels of the robot");
	_wheelBox->setToolTipDuration(-1);

	// radius of wheel
	QLabel *radiusLabel = new QLabel(tr("Radius:"));
	_radiusUnits = new QLabel(tr("cm"));
	QLineEdit *radiusEdit = new QLineEdit;
	radiusEdit->setObjectName("radius");
	radiusLabel->setBuddy(radiusEdit);
	QWidget::connect(radiusEdit, SIGNAL(textChanged(QString)), this, SLOT(submitRadius(QString)));
	radiusEdit->setToolTip("Set the wheel radii of the robot");
	radiusEdit->setToolTipDuration(-1);

	// color
	_colorPicker = new ledColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the robot's LED");
	_colorPicker->setToolTipDuration(-1);

	// lay out grid
	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *hbox0 = new QHBoxLayout();
	hbox0->addWidget(title, 5, Qt::AlignHCenter);
	layout->addLayout(hbox0);
	QHBoxLayout *hbox1 = new QHBoxLayout();
	hbox1->addWidget(formLabel, 2, Qt::AlignRight);
	hbox1->addWidget(formBox, 5);
	hbox1->addStretch(1);
	layout->addLayout(hbox1);
	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(nameLabel, 2, Qt::AlignRight);
	hbox->addWidget(nameEdit, 5);
	hbox->addStretch(1);
	layout->addLayout(hbox);
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
	hbox4->addWidget(rZLabel, 2, Qt::AlignRight);
	hbox4->addWidget(_rZBox, 5);
	hbox4->addWidget(rZUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox4);
	QHBoxLayout *hbox5 = new QHBoxLayout();
	hbox5->addWidget(wheelLabel, 2, Qt::AlignRight);
	hbox5->addWidget(_wheelBox, 5);
	hbox5->addWidget(_wheelUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox5);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(radiusLabel, 2, Qt::AlignRight);
	hbox6->addWidget(radiusEdit, 5);
	hbox6->addWidget(_radiusUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox6);
	QHBoxLayout *hbox7 = new QHBoxLayout();
	hbox7->addWidget(_colorPicker);
	layout->addLayout(hbox7);
	layout->addStretch(1);
	this->setLayout(layout);
}

void customEditor::submitForm(int value) {
	int form = -1;
	if (value == LINKBOTI) form = rs::LinkbotI;
	else if (value == LINKBOTL) form = rs::LinkbotL;
	//else if (value == EV3) form = rs::EV3;
	else if (value == NXT) form = rs::NXT;
	_model->setData(_model->index(_row, rsRobotModel::FORM), form);
}

void customEditor::submitName(QString value) {
	_model->setData(_model->index(_row, rsRobotModel::NAME), value);
}

void customEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsRobotModel::P_X), value);
}

void customEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsRobotModel::P_Y), value);
}

void customEditor::submitRZ(double value) {
	_rZBox->setValue(value - static_cast<int>(value / 360) * 360);
	_model->setData(_model->index(_row, rsRobotModel::R_PSI), value);
}

void customEditor::submitLeftWheel(int value) {
	_model->setData(_model->index(_row, rsRobotModel::WHEELLEFT), value);
}

void customEditor::submitRadius(QString value) {
	_model->setData(_model->index(_row, rsRobotModel::P_Y), value);
}

void customEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsRobotModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void customEditor::setIndex(int row) {
	_row = row;
	int form = _model->data(_model->index(row, rsRobotModel::FORM), Qt::EditRole).toInt();
	int ind = -1;
	if (form == rs::LinkbotI) ind = LINKBOTI;
	else if (form == rs::LinkbotL) ind = LINKBOTL;
	//else if (form == rs::EV3) ind = EV3;
	else if (form == rs::NXT) ind = NXT;
	(this->findChild<QComboBox *>("form"))->setCurrentIndex(ind);
	(this->findChild<QLineEdit *>("name"))->setText(_model->data(_model->index(row, rsRobotModel::NAME), Qt::EditRole).toString());
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsRobotModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsRobotModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("rz"))->setValue(_model->data(_model->index(row, rsRobotModel::R_PSI), Qt::EditRole).toDouble());
	(this->findChild<QComboBox *>("wheels"))->setCurrentIndex(_model->data(_model->index(row, rsRobotModel::WHEELLEFT), Qt::EditRole).toInt());
	(this->findChild<QComboBox *>("radius"))->setCurrentIndex(_model->data(_model->index(row, rsRobotModel::RADIUS), Qt::EditRole).toInt());
	QColor color(_model->data(_model->index(row, rsRobotModel::COLOR), Qt::EditRole).toString());
	(this->findChild<ledColorPicker *>("color"))->setColor(color);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void customEditor::setUnits(bool si) {
	// update unit labels
	QString text;
	if (si) text = tr("cm");
	else text = tr("in");
	_pXUnits->setText(text);
	_pYUnits->setText(text);
	_wheelUnits->setText(text);
	_radiusUnits->setText(text);

	// set wheel list to new values
	QStringList wheelItems;
	if (si) wheelItems << "None" << "4.13" << "4.45" << "5.08" << tr("Custom");
	else wheelItems << "None" << "1.625" << "1.75" << "2.00" << tr("Custom");
	QStringListModel *wheelModel = new QStringListModel(wheelItems, this);
	_wheelBox->setModel(wheelModel);
}

/*!
 *
 *
 *	PreConfigEditor
 *
 *
 */

/*!	\brief Preconfig Robot Editor.
 *
 *	Build preconfigured robot editor with relevant pieces of information.
 *
 *	\param		model data model from robotEditor model.
 */
preconfigEditor::preconfigEditor(robotModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">PreConfig Editor</span>"));

	// name
	QLabel *nameLabel = new QLabel(tr("Name:"));
	QLineEdit *nameEdit = new QLineEdit;
	nameEdit->setObjectName("name");
	nameLabel->setBuddy(nameEdit);
	QWidget::connect(nameEdit, SIGNAL(textChanged(QString)), this, SLOT(submitName(QString)));
	nameEdit->setToolTip("Set a name for the robot");
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
	pXBox->setToolTip("Set the X position of the robot");
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
	pYBox->setToolTip("Set the Y position of the robot");
	pYBox->setToolTipDuration(-1);

	// rotation psi
	QLabel *rZLabel = new QLabel(tr("Angle:"));
	QLabel *rZUnits = new QLabel(QString::fromUtf8("°"));
	_rZBox = new QDoubleSpinBox();
	_rZBox->setObjectName("rz");
	_rZBox->setMinimum(-360);
	_rZBox->setMaximum(360);
	_rZBox->setSingleStep(0.5);
	rZLabel->setBuddy(_rZBox);
	QWidget::connect(_rZBox, SIGNAL(valueChanged(double)), this, SLOT(submitRZ(double)));
	_rZBox->setToolTip("Set the rotation of the robot");
	_rZBox->setToolTipDuration(-1);

	// color
	_colorPicker = new ledColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the robot's LED");
	_colorPicker->setToolTipDuration(-1);

	// lay out grid
	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *hbox0 = new QHBoxLayout();
	hbox0->addWidget(title, 5, Qt::AlignHCenter);
	layout->addLayout(hbox0);
	QHBoxLayout *hbox = new QHBoxLayout();
	hbox->addWidget(nameLabel, 2, Qt::AlignRight);
	hbox->addWidget(nameEdit, 5);
	hbox->addStretch(1);
	layout->addLayout(hbox);
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
	hbox4->addWidget(rZLabel, 2, Qt::AlignRight);
	hbox4->addWidget(_rZBox, 5);
	hbox4->addWidget(rZUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox4);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(_colorPicker);
	layout->addLayout(hbox6);
	layout->addStretch(1);
	this->setLayout(layout);
}

void preconfigEditor::submitName(QString value) {
	_model->setData(_model->index(_row, rsRobotModel::NAME), value);
}

void preconfigEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsRobotModel::P_X), value);
}

void preconfigEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsRobotModel::P_Y), value);
}

void preconfigEditor::submitRZ(double value) {
	_rZBox->setValue(value - static_cast<int>(value / 360) * 360);
	_model->setData(_model->index(_row, rsRobotModel::R_PSI), value);
}

void preconfigEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsRobotModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void preconfigEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QLineEdit *>("name"))->setText(_model->data(_model->index(row, rsRobotModel::NAME), Qt::EditRole).toString());
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsRobotModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsRobotModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("rz"))->setValue(_model->data(_model->index(row, rsRobotModel::R_PSI), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsRobotModel::COLOR), Qt::EditRole).toString());
	(this->findChild<ledColorPicker *>("color"))->setColor(color);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void preconfigEditor::setUnits(bool si) {
	// update unit labels
	QString text;
	if (si) text = tr("cm");
	else text = tr("in");
	_pXUnits->setText(text);
	_pYUnits->setText(text);
}

/*!
 *
 *
 *	ledColorPicker
 *
 *
 */

ledColorPicker::ledColorPicker(QWidget *parent) : QWidget(parent) {
	QHBoxLayout *hbox = new QHBoxLayout(this);

	QLabel *label = new QLabel(tr("LED Color:"));
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

QColor ledColorPicker::color(void) const {
	return _color;
}

void ledColorPicker::setColor(const QColor color) {
	if (color == _color)
		return;

	_color = color;
	QString s = "background-color: ";
	_button->setStyleSheet(s + _color.name());
	emit colorChanged(_color);
}

void ledColorPicker::onButtonClicked(void) {
	const QColor color = QColorDialog::getColor(_color, this);
	if (!color.isValid())
		return;

	this->setColor(color);
}

