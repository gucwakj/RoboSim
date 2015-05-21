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
	this->setFixedWidth(256);

	// set up mapper
	_mapper = new QDataWidgetMapper(this);
	_mapper->setModel(_model);
	_mapper->setItemDelegate(new robotEditorDelegate(this));
	_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

	// set up editor pages
	_pages = new QStackedWidget;
	_pages->addWidget(new linkbotEditor(_mapper));
	_pages->addWidget(new linkbotLEditor(_mapper));
	_pages->addWidget(new customEditor(_mapper));
	_pages->addWidget(new preconfigEditor(_mapper));
	_pages->addWidget(new mindstormsEditor(_mapper));

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

void robotEditor::dataChanged(QModelIndex/*topLeft*/, QModelIndex bottomRight) {
	int form = _model->data(_model->index(bottomRight.row(), rsRobotModel::FORM)).toInt();
	if (bottomRight.row() != _row || _form != form)
		this->setCurrentIndex(bottomRight);
}

void robotEditor::setCurrentIndex(const QModelIndex &index) {
	if (index.isValid()) {
		// disable current mappings
		//_mapper->clearMapping();

		// set new curent model row
		_row = index.row();

		// load appropriate page
		int form = _model->data(_model->index(index.row(), rsRobotModel::FORM)).toInt();
		_form = form;
		if (form == rs::EV3 || form == rs::NXT) {
			_pages->setCurrentIndex(4);	// mindstorms
			this->setUnits(_units);
			dynamic_cast<mindstormsEditor *>(_pages->currentWidget())->nullIndex(false);
		}
		else {
			if (_model->data(_model->index(index.row(), rsRobotModel::PRECONFIG), Qt::EditRole).toInt()) {
				_pages->setCurrentIndex(3);	// preconfig
				this->setUnits(_units);
				dynamic_cast<preconfigEditor *>(_pages->currentWidget())->nullIndex(false);
			}
			else {
				if (form == rs::LINKBOTL) {
					_pages->setCurrentIndex(1);	// Linkbot-L
					this->setUnits(_units);
					dynamic_cast<linkbotLEditor *>(_pages->currentWidget())->nullIndex(false);
				}
				else if (_model->data(_model->index(index.row(), rsRobotModel::WHEELLEFT), Qt::EditRole).toInt() == 4) {
					_pages->setCurrentIndex(2);	// custom wheeled Linkbot-I
					this->setUnits(_units);
					dynamic_cast<customEditor *>(_pages->currentWidget())->nullIndex(false);
				}
				else {
					_pages->setCurrentIndex(0);	// Linkbot-I
					this->setUnits(_units);
					dynamic_cast<linkbotEditor *>(_pages->currentWidget())->nullIndex(false);
				}
			}
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
		if (dynamic_cast<linkbotEditor *>(_pages->currentWidget()))
			dynamic_cast<linkbotEditor *>(_pages->currentWidget())->nullIndex(true);
		else if (dynamic_cast<linkbotLEditor *>(_pages->currentWidget()))
			dynamic_cast<linkbotLEditor *>(_pages->currentWidget())->nullIndex(true);
		else if (dynamic_cast<mindstormsEditor *>(_pages->currentWidget()))
			dynamic_cast<mindstormsEditor *>(_pages->currentWidget())->nullIndex(true);
		else if (dynamic_cast<preconfigEditor *>(_pages->currentWidget()))
			dynamic_cast<preconfigEditor *>(_pages->currentWidget())->nullIndex(true);
		else if (dynamic_cast<customEditor *>(_pages->currentWidget()))
			dynamic_cast<customEditor *>(_pages->currentWidget())->nullIndex(true);

		// disable all buttons
		_deleteButton->setDisabled(true);
		_nextButton->setDisabled(true);
		_previousButton->setDisabled(true);
	}
}

void robotEditor::buttonPressed(void) {
	// get new index
	QModelIndex index = _mapper->model()->index(_mapper->currentIndex(), 0);

	// set editor to new values
	this->setCurrentIndex(index);

	// signal other views that index has changed
	emit indexChanged(index);
}

void robotEditor::deleteCurrentIndex(void) {
	// save current index
	int index = _mapper->currentIndex();

	// remove current robot from model
	_mapper->model()->removeRows(_mapper->currentIndex(), 1);

	// new index is same row as last one
	_mapper->setCurrentIndex(index);

	// if it is invalid, then set the last row in the model
	if (_mapper->currentIndex() == -1) {
		this->setCurrentIndex(_mapper->model()->index(_mapper->model()->rowCount()-1, rsRobotModel::ID));
	}

	// signal a change in current robot
	emit indexChanged(_mapper->model()->index(_mapper->currentIndex(), rsRobotModel::ID));
}

void robotEditor::setUnits(bool si) {
	// save units
	_units = si;

	// set units labels for editors
	if (dynamic_cast<linkbotEditor *>(_pages->currentWidget()))
		dynamic_cast<linkbotEditor *>(_pages->currentWidget())->setUnits(si);
	else if (dynamic_cast<linkbotLEditor *>(_pages->currentWidget()))
		dynamic_cast<linkbotLEditor *>(_pages->currentWidget())->setUnits(si);
	else if (dynamic_cast<mindstormsEditor *>(_pages->currentWidget()))
		dynamic_cast<mindstormsEditor *>(_pages->currentWidget())->setUnits(si);
	else if (dynamic_cast<preconfigEditor *>(_pages->currentWidget()))
		dynamic_cast<preconfigEditor *>(_pages->currentWidget())->setUnits(si);
	else if (dynamic_cast<customEditor *>(_pages->currentWidget()))
		dynamic_cast<customEditor *>(_pages->currentWidget())->setUnits(si);
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
 *	\param		mapper data mapper from robotEditor model.
 */
linkbotEditor::linkbotEditor(QDataWidgetMapper *mapper, QWidget *parent) : QWidget(parent) {
	// save mapper
	_mapper = mapper;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Robot Editor</span>"));

	// form list
	QLabel *formLabel = new QLabel(tr("Form: "));
	QStringList formItems;
	formItems << "Linkbot I" << "Linkbot L" << "Mindstorms EV3" << "Mindstorms NXT";
	QStringListModel *formModel = new QStringListModel(formItems, this);
	QComboBox *formBox = new QComboBox();
	formBox->setObjectName("form");
	formBox->setModel(formModel);
	formLabel->setBuddy(formBox);
	QWidget::connect(formBox, SIGNAL(currentIndexChanged(int)), _mapper, SLOT(submit()));

	// name
	QLabel *nameLabel = new QLabel(tr("Name:"));
	QLineEdit *nameEdit = new QLineEdit;
	nameEdit->setObjectName("name");
	nameLabel->setBuddy(nameEdit);
	QWidget::connect(nameEdit, SIGNAL(editingFinished()), _mapper, SLOT(submit()));

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

	// rotation psi
	QLabel *rZLabel = new QLabel(tr("Angle:"));
	QLabel *rZUnits = new QLabel(QString::fromUtf8("°"));
	_rZBox = new QDoubleSpinBox();
	_rZBox->setObjectName("rz");
	_rZBox->setMinimum(-360);
	_rZBox->setMaximum(360);
	_rZBox->setSingleStep(0.5);
	rZLabel->setBuddy(_rZBox);
	QWidget::connect(_rZBox, SIGNAL(valueChanged(double)), this, SLOT(rotate(double)));

	// left wheel list
	QLabel *wheelLLabel = new QLabel(tr("Left Wheel:"));
	QStringList wheelLItems;
	wheelLItems << "None" << "4.13" << "4.45" << "5.08" << tr("Custom");
	QStringListModel *wheelLModel = new QStringListModel(wheelLItems, this);
	_wheelLUnits = new QLabel(tr("cm"));
	_wheelLBox = new QComboBox();
	_wheelLBox->setObjectName("wheelLeft");
	_wheelLBox->setModel(wheelLModel);
	wheelLLabel->setBuddy(_wheelLBox);
	QWidget::connect(_wheelLBox, SIGNAL(currentIndexChanged(int)), _mapper, SLOT(submit()));

	// right wheel list
	QLabel *wheelRLabel = new QLabel(tr("Right Wheel:"));
	_wheelRUnits = new QLabel(tr("cm"));
	_wheelRBox = new QComboBox();
	_wheelRBox->setObjectName("wheelRight");
	wheelRLabel->setBuddy(_wheelRBox);
	QWidget::connect(_wheelRBox, SIGNAL(currentIndexChanged(int)), _mapper, SLOT(submit()));

	// color
	_colorPicker = new ledColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), _mapper, SLOT(submit()));

	// lay out grid
	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *hbox0 = new QHBoxLayout();
	hbox0->addWidget(title, 5, Qt::AlignHCenter);
	layout->addLayout(hbox0);
	layout->addStretch(1);
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
	layout->addStretch(2);
	this->setLayout(layout);
}

/*!	\brief Slot to keep rotations between
 *		   -360 and 360 degrees.
 *
 *	\param		value Current value of the spinbox.
 */
void linkbotEditor::rotate(double value) {
	_rZBox->setValue(value - static_cast<int>(value/360)*360);
	_mapper->submit();
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void linkbotEditor::nullIndex(bool nullify) {
	// nullify (or not) input boxes
	(this->findChild<QComboBox *>("form"))->setDisabled(nullify);
	(this->findChild<QLineEdit *>("name"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("px"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("py"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("rz"))->setDisabled(nullify);
	(this->findChild<QComboBox *>("wheelLeft"))->setDisabled(nullify);
	(this->findChild<QComboBox *>("wheelRight"))->setDisabled(nullify);
	(this->findChild<QPushButton *>("colorbutton"))->setDisabled(nullify);

	// dim color button
	QColor color = (this->findChild<QPushButton *>("colorbutton"))->palette().color(QPalette::Button);
	if (nullify) color.setAlpha(50);
	else color.setAlpha(255);
	(this->findChild<QPushButton *>("colorbutton"))->setPalette(color);

	// re-enable mapping
	if (!nullify) {
		_mapper->addMapping(this->findChild<QComboBox *>("form"), rsRobotModel::FORM);
		_mapper->addMapping(this->findChild<QLineEdit *>("name"), rsRobotModel::NAME);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("px"), rsRobotModel::P_X);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("py"), rsRobotModel::P_Y);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("rz"), rsRobotModel::R_PSI);
		_mapper->addMapping(this->findChild<QComboBox *>("wheelLeft"), rsRobotModel::WHEELLEFT);
		_mapper->addMapping(this->findChild<QComboBox *>("wheelRight"), rsRobotModel::WHEELRIGHT);
		_mapper->addMapping(this->findChild<ledColorPicker *>("color"), rsRobotModel::COLOR, "color");
	}
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void linkbotEditor::setUnits(bool si) {
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
		wheelLItems << "None" << "4.13" << "4.45" << "5.08" << tr("Custom");
		wheelRItems << "None" << "4.13" << "4.45" << "5.08" << tr("Custom");
	}
	else {
		wheelLItems << "None" << "1.625" << "1.75" << "2.00" << tr("Custom");
		wheelRItems << "None" << "1.625" << "1.75" << "2.00" << tr("Custom");
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
 *	\param		mapper data mapper from robotEditor model.
 */
linkbotLEditor::linkbotLEditor(QDataWidgetMapper *mapper, QWidget *parent) : QWidget(parent) {
	// save mapper
	_mapper = mapper;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Robot Editor</span>"));

	// form list
	QLabel *formLabel = new QLabel(tr("Form: "));
	QStringList formItems;
	formItems << "Linkbot I" << "Linkbot L" << "Mindstorms EV3" << "Mindstorms NXT";
	QStringListModel *formModel = new QStringListModel(formItems, this);
	QComboBox *formBox = new QComboBox();
	formBox->setObjectName("form");
	formBox->setModel(formModel);
	formLabel->setBuddy(formBox);
	QWidget::connect(formBox, SIGNAL(currentIndexChanged(int)), _mapper, SLOT(submit()));

	// name
	QLabel *nameLabel = new QLabel(tr("Name:"));
	QLineEdit *nameEdit = new QLineEdit;
	nameEdit->setObjectName("name");
	nameLabel->setBuddy(nameEdit);
	QWidget::connect(nameEdit, SIGNAL(editingFinished()), _mapper, SLOT(submit()));

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

	// rotation psi
	QLabel *rZLabel = new QLabel(tr("Angle:"));
	QLabel *rZUnits = new QLabel(QString::fromUtf8("°"));
	_rZBox = new QDoubleSpinBox();
	_rZBox->setObjectName("rz");
	_rZBox->setMinimum(-360);
	_rZBox->setMaximum(360);
	_rZBox->setSingleStep(0.5);
	rZLabel->setBuddy(_rZBox);
	QWidget::connect(_rZBox, SIGNAL(valueChanged(double)), this, SLOT(rotate(double)));

	// color
	_colorPicker = new ledColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), _mapper, SLOT(submit()));

	// lay out grid
	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *hbox0 = new QHBoxLayout();
	hbox0->addWidget(title, 5, Qt::AlignHCenter);
	layout->addLayout(hbox0);
	layout->addStretch(1);
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
	layout->addStretch(2);
	this->setLayout(layout);
}

/*!	\brief Slot to keep rotations between
 *		   -360 and 360 degrees.
 *
 *	\param		value Current value of the spinbox.
 */
void linkbotLEditor::rotate(double value) {
	_rZBox->setValue(value - static_cast<int>(value/360)*360);
	_mapper->submit();
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void linkbotLEditor::nullIndex(bool nullify) {
	// nullify (or not) input boxes
	(this->findChild<QComboBox *>("form"))->setDisabled(nullify);
	(this->findChild<QLineEdit *>("name"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("px"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("py"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("rz"))->setDisabled(nullify);
	(this->findChild<QPushButton *>("colorbutton"))->setDisabled(nullify);

	// dim color button
	QColor color = (this->findChild<QPushButton *>("colorbutton"))->palette().color(QPalette::Button);
	if (nullify) color.setAlpha(50);
	else color.setAlpha(255);
	(this->findChild<QPushButton *>("colorbutton"))->setPalette(color);

	// re-enable mapping
	if (!nullify) {
		_mapper->addMapping(this->findChild<QComboBox *>("form"), rsRobotModel::FORM);
		_mapper->addMapping(this->findChild<QLineEdit *>("name"), rsRobotModel::NAME);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("px"), rsRobotModel::P_X);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("py"), rsRobotModel::P_Y);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("rz"), rsRobotModel::R_PSI);
		_mapper->addMapping(this->findChild<ledColorPicker *>("color"), rsRobotModel::COLOR, "color");
	}
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
 *	\param		mapper data mapper from robotEditor model.
 */
mindstormsEditor::mindstormsEditor(QDataWidgetMapper *mapper, QWidget *parent) : QWidget(parent) {
	// save mapper
	_mapper = mapper;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Robot Editor</span>"));

	// form list
	QLabel *formLabel = new QLabel(tr("Form: "));
	QStringList formItems;
	formItems << "Linkbot I" << "Linkbot L" << "Mindstorms EV3" << "Mindstorms NXT";
	QStringListModel *formModel = new QStringListModel(formItems, this);
	QComboBox *formBox = new QComboBox();
	formBox->setObjectName("form");
	formBox->setModel(formModel);
	formLabel->setBuddy(formBox);
	QWidget::connect(formBox, SIGNAL(currentIndexChanged(int)), _mapper, SLOT(submit()));

	// name
	QLabel *nameLabel = new QLabel(tr("Name:"));
	QLineEdit *nameEdit = new QLineEdit;
	nameEdit->setObjectName("name");
	nameLabel->setBuddy(nameEdit);
	QWidget::connect(nameEdit, SIGNAL(editingFinished()), _mapper, SLOT(submit()));

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

	// rotation psi
	QLabel *rZLabel = new QLabel(tr("Angle:"));
	QLabel *rZUnits = new QLabel(QString::fromUtf8("°"));
	_rZBox = new QDoubleSpinBox();
	_rZBox->setObjectName("rz");
	_rZBox->setMinimum(-360);
	_rZBox->setMaximum(360);
	_rZBox->setSingleStep(0.5);
	rZLabel->setBuddy(_rZBox);
	QWidget::connect(_rZBox, SIGNAL(valueChanged(double)), this, SLOT(rotate(double)));

	// left wheel list
	QLabel *wheelLLabel = new QLabel(tr("Left Wheel:"));
	QStringList wheelLItems;
	wheelLItems << "None" << "4.13" << "4.45" << "5.08";
	QStringListModel *wheelLModel = new QStringListModel(wheelLItems, this);
	_wheelLUnits = new QLabel(tr("cm"));
	_wheelLBox = new QComboBox();
	_wheelLBox->setObjectName("wheelLeft");
	_wheelLBox->setModel(wheelLModel);
	wheelLLabel->setBuddy(_wheelLBox);
	QWidget::connect(_wheelLBox, SIGNAL(currentIndexChanged(int)), _mapper, SLOT(submit()));

	// right wheel list
	QLabel *wheelRLabel = new QLabel(tr("Right Wheel:"));
	_wheelRUnits = new QLabel(tr("cm"));
	_wheelRBox = new QComboBox();
	_wheelRBox->setObjectName("wheelRight");
	wheelRLabel->setBuddy(_wheelRBox);
	QWidget::connect(_wheelRBox, SIGNAL(currentIndexChanged(int)), _mapper, SLOT(submit()));

	// color
	_colorPicker = new ledColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), _mapper, SLOT(submit()));

	// lay out grid
	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *hbox0 = new QHBoxLayout();
	hbox0->addWidget(title, 5, Qt::AlignHCenter);
	layout->addLayout(hbox0);
	layout->addStretch(1);
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
	layout->addStretch(2);
	this->setLayout(layout);
}

/*!	\brief Slot to keep rotations between
 *		   -360 and 360 degrees.
 *
 *	\param		value Current value of the spinbox.
 */
void mindstormsEditor::rotate(double value) {
	_rZBox->setValue(value - static_cast<int>(value/360)*360);
	_mapper->submit();
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void mindstormsEditor::nullIndex(bool nullify) {
	// nullify (or not) input boxes
	(this->findChild<QComboBox *>("form"))->setDisabled(nullify);
	(this->findChild<QLineEdit *>("name"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("px"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("py"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("rz"))->setDisabled(nullify);
	(this->findChild<QComboBox *>("wheelLeft"))->setDisabled(nullify);
	(this->findChild<QComboBox *>("wheelRight"))->setDisabled(nullify);
	(this->findChild<QPushButton *>("colorbutton"))->setDisabled(nullify);

	// dim color button
	QColor color = (this->findChild<QPushButton *>("colorbutton"))->palette().color(QPalette::Button);
	if (nullify) color.setAlpha(50);
	else color.setAlpha(255);
	(this->findChild<QPushButton *>("colorbutton"))->setPalette(color);

	// re-enable mapping
	if (!nullify) {
		_mapper->addMapping(this->findChild<QComboBox *>("form"), rsRobotModel::FORM);
		_mapper->addMapping(this->findChild<QLineEdit *>("name"), rsRobotModel::NAME);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("px"), rsRobotModel::P_X);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("py"), rsRobotModel::P_Y);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("rz"), rsRobotModel::R_PSI);
		_mapper->addMapping(this->findChild<QComboBox *>("wheelLeft"), rsRobotModel::WHEELLEFT);
		_mapper->addMapping(this->findChild<QComboBox *>("wheelRight"), rsRobotModel::WHEELRIGHT);
		_mapper->addMapping(this->findChild<ledColorPicker *>("color"), rsRobotModel::COLOR, "color");
	}
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
		wheelLItems << "None" << "4.13" << "4.45";
		wheelRItems << "None" << "4.13" << "4.45";
	}
	else {
		wheelLItems << "None" << "1.625" << "1.75";
		wheelRItems << "None" << "1.625" << "1.75";
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
 *	\param		mapper data mapper from robotEditor model.
 */
customEditor::customEditor(QDataWidgetMapper *mapper, QWidget *parent) : QWidget(parent) {
	// save mapper
	_mapper = mapper;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Robot Editor</span>"));

	// form list
	QLabel *formLabel = new QLabel(tr("Form: "));
	QStringList formItems;
	formItems << "Linkbot I" << "Linkbot L" << "Mindstorms EV3" << "Mindstorms NXT";
	QStringListModel *formModel = new QStringListModel(formItems, this);
	QComboBox *formBox = new QComboBox();
	formBox->setObjectName("form");
	formBox->setModel(formModel);
	formLabel->setBuddy(formBox);
	QWidget::connect(formBox, SIGNAL(currentIndexChanged(int)), _mapper, SLOT(submit()));

	// name
	QLabel *nameLabel = new QLabel(tr("Name:"));
	QLineEdit *nameEdit = new QLineEdit;
	nameEdit->setObjectName("name");
	nameLabel->setBuddy(nameEdit);
	QWidget::connect(nameEdit, SIGNAL(editingFinished()), _mapper, SLOT(submit()));

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

	// rotation psi
	QLabel *rZLabel = new QLabel(tr("Angle:"));
	QLabel *rZUnits = new QLabel(QString::fromUtf8("°"));
	_rZBox = new QDoubleSpinBox();
	_rZBox->setObjectName("rz");
	_rZBox->setMinimum(-360);
	_rZBox->setMaximum(360);
	_rZBox->setSingleStep(0.5);
	rZLabel->setBuddy(_rZBox);
	QWidget::connect(_rZBox, SIGNAL(valueChanged(double)), this, SLOT(rotate(double)));

	// wheels list
	QLabel *wheelLabel = new QLabel(tr("Wheels:"));
	_wheelUnits = new QLabel(tr("cm"));
	_wheelBox = new QComboBox();
	_wheelBox->setObjectName("wheels");
	wheelLabel->setBuddy(_wheelBox);
	QWidget::connect(_wheelBox, SIGNAL(currentIndexChanged(int)), _mapper, SLOT(submit()));

	// radius of wheel
	QLabel *radiusLabel = new QLabel(tr("Radius:"));
	_radiusUnits = new QLabel(tr("cm"));
	QLineEdit *radiusEdit = new QLineEdit;
	radiusEdit->setObjectName("radius");
	radiusLabel->setBuddy(radiusEdit);
	QWidget::connect(radiusEdit, SIGNAL(editingFinished()), _mapper, SLOT(submit()));

	// color
	_colorPicker = new ledColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), _mapper, SLOT(submit()));

	// lay out grid
	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *hbox0 = new QHBoxLayout();
	hbox0->addWidget(title, 5, Qt::AlignHCenter);
	layout->addLayout(hbox0);
	layout->addStretch(1);
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
	layout->addStretch(2);
	this->setLayout(layout);
}

/*!	\brief Slot to keep rotations between
 *		   -360 and 360 degrees.
 *
 *	\param		value Current value of the spinbox.
 */
void customEditor::rotate(double value) {
	_rZBox->setValue(value - static_cast<int>(value/360)*360);
	_mapper->submit();
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void customEditor::nullIndex(bool nullify) {
	// nullify (or not) input boxes
	(this->findChild<QComboBox *>("form"))->setDisabled(nullify);
	(this->findChild<QLineEdit *>("name"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("px"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("py"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("rz"))->setDisabled(nullify);
	(this->findChild<QComboBox *>("wheels"))->setDisabled(nullify);
	(this->findChild<QLineEdit *>("radius"))->setDisabled(nullify);
	(this->findChild<QPushButton *>("colorbutton"))->setDisabled(nullify);

	// dim color button
	QColor color = (this->findChild<QPushButton *>("colorbutton"))->palette().color(QPalette::Button);
	if (nullify) color.setAlpha(50);
	else color.setAlpha(255);
	(this->findChild<QPushButton *>("colorbutton"))->setPalette(color);

	// re-enable mapping
	if (!nullify) {
		_mapper->addMapping(this->findChild<QComboBox *>("form"), rsRobotModel::FORM);
		_mapper->addMapping(this->findChild<QLineEdit *>("name"), rsRobotModel::NAME);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("px"), rsRobotModel::P_X);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("py"), rsRobotModel::P_Y);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("rz"), rsRobotModel::R_PSI);
		_mapper->addMapping(this->findChild<QComboBox *>("wheels"), rsRobotModel::WHEELLEFT);
		_mapper->addMapping(this->findChild<QLineEdit *>("radius"), rsRobotModel::RADIUS);
		_mapper->addMapping(this->findChild<ledColorPicker *>("color"), rsRobotModel::COLOR, "color");
	}
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
 *	\param		mapper data mapper from robotEditor model.
 */
preconfigEditor::preconfigEditor(QDataWidgetMapper *mapper, QWidget *parent) : QWidget(parent) {
	// save mapper
	_mapper = mapper;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">PreConfig Editor</span>"));

	// name
	QLabel *nameLabel = new QLabel(tr("Name:"));
	QLineEdit *nameEdit = new QLineEdit;
	nameEdit->setObjectName("name");
	nameLabel->setBuddy(nameEdit);
	QWidget::connect(nameEdit, SIGNAL(editingFinished()), _mapper, SLOT(submit()));

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

	// rotation psi
	QLabel *rZLabel = new QLabel(tr("Angle:"));
	QLabel *rZUnits = new QLabel(QString::fromUtf8("°"));
	_rZBox = new QDoubleSpinBox();
	_rZBox->setObjectName("rz");
	_rZBox->setMinimum(-360);
	_rZBox->setMaximum(360);
	_rZBox->setSingleStep(0.5);
	rZLabel->setBuddy(_rZBox);
	QWidget::connect(_rZBox, SIGNAL(valueChanged(double)), this, SLOT(rotate(double)));

	// color
	_colorPicker = new ledColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), _mapper, SLOT(submit()));

	// lay out grid
	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *hbox0 = new QHBoxLayout();
	hbox0->addWidget(title, 5, Qt::AlignHCenter);
	layout->addLayout(hbox0);
	layout->addStretch(1);
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
	layout->addStretch(2);
	this->setLayout(layout);
}

/*!	\brief Slot to keep rotations between
 *		   -360 and 360 degrees.
 *
 *	\param		value Current value of the spinbox.
 */
void preconfigEditor::rotate(double value) {
	_rZBox->setValue(value - static_cast<int>(value/360)*360);
	_mapper->submit();
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void preconfigEditor::nullIndex(bool nullify) {
	// nullify (or not) input boxes
	(this->findChild<QLineEdit *>("name"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("px"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("py"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("rz"))->setDisabled(nullify);
	(this->findChild<QPushButton *>("colorbutton"))->setDisabled(nullify);

	// dim color button
	QColor color = (this->findChild<QPushButton *>("colorbutton"))->palette().color(QPalette::Button);
	if (nullify) color.setAlpha(50);
	else color.setAlpha(255);
	(this->findChild<QPushButton *>("colorbutton"))->setPalette(color);

	// re-enable mapping
	if (!nullify) {
		_mapper->addMapping(this->findChild<QLineEdit *>("name"), rsRobotModel::NAME);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("px"), rsRobotModel::P_X);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("py"), rsRobotModel::P_Y);
		_mapper->addMapping(this->findChild<QDoubleSpinBox *>("rz"), rsRobotModel::R_PSI);
		_mapper->addMapping(this->findChild<ledColorPicker *>("color"), rsRobotModel::COLOR, "color");
	}
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
	_button->setPalette(QPalette(_color));
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
	_button->setPalette(QPalette(_color));
	emit colorChanged(_color);
}

void ledColorPicker::onButtonClicked(void) {
	const QColor color = QColorDialog::getColor(_color, this);
	if (!color.isValid())
		return;

	this->setColor(color);
}

/*!
 *
 *
 *	robotEditorDelegate
 *
 *
 */

robotEditorDelegate::robotEditorDelegate(QObject *parent) : QItemDelegate(parent) { }

void robotEditorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
	if (!editor->objectName().compare("form")) {
		QVariant value = editor->property("currentIndex");
		if (value.isValid()) {
			if (index.data() == rs::LINKBOTI)
				editor->setProperty("currentIndex", LINKBOTI);
			else if (index.data() == rs::LINKBOTL)
				editor->setProperty("currentIndex", LINKBOTL);
			else if (index.data() == rs::EV3)
				editor->setProperty("currentIndex", EV3);
			else if (index.data() == rs::NXT)
				editor->setProperty("currentIndex", NXT);
			return;
		}
	}
	else if (!strcmp(editor->metaObject()->className(), "QComboBox")) {
		editor->setProperty("currentIndex", index.data());
		return;
	}
	QItemDelegate::setEditorData(editor, index);
}

void robotEditorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
	if (!editor->objectName().compare("form")) {
		QVariant value = editor->property("currentIndex");
		if (value.isValid()) {
			if (value == LINKBOTI)
				model->setData(index, QVariant(rs::LINKBOTI));
			else if (value == LINKBOTL)
				model->setData(index, QVariant(rs::LINKBOTL));
			else if (value == EV3)
				model->setData(index, QVariant(rs::EV3));
			else if (value == NXT)
				model->setData(index, QVariant(rs::NXT));
		}
		return;
	}
	else if (!strcmp(editor->metaObject()->className(), "QComboBox")) {
		QVariant value = editor->property("currentIndex");
		if (value.isValid()) {
			model->setData(index, value);
		}
		return;
	}
	QItemDelegate::setModelData(editor, model, index);
}

