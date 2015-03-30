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
	//_pages->addWidget(new customEditor(_mapper));
	//_pages->addWidget(new preconfigEditor(_mapper));

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

	// set up units for item labels
	this->setUnits(true);

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
		// load appropriate page
		int id = _model->data(_model->index(index.row(), rsObstacleModel::FORM), Qt::EditRole).toInt();
		_pages->setCurrentIndex(id);
		switch (id) {
			case rs::BOX:
				dynamic_cast<boxEditor*>(_pages->currentWidget())->nullIndex(false);
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
		if (dynamic_cast<boxEditor*>(_pages->currentWidget()))
			dynamic_cast<boxEditor*>(_pages->currentWidget())->nullIndex(true);
		/*else if (dynamic_cast<cylinderEditor*>(_pages->currentWidget()))
			dynamic_cast<cylinderEditor*>(_pages->currentWidget())->nullIndex(true);
		else if (dynamic_cast<dotEditor*>(_pages->currentWidget()))
			dynamic_cast<dotEditor*>(_pages->currentWidget())->nullIndex(true);
		else if (dynamic_cast<lineEditor*>(_pages->currentWidget()))
			dynamic_cast<lineEditor*>(_pages->currentWidget())->nullIndex(true);
		else if (dynamic_cast<sphereEditor*>(_pages->currentWidget()))
			dynamic_cast<sphereEditor*>(_pages->currentWidget())->nullIndex(true);
		else if (dynamic_cast<textEditor*>(_pages->currentWidget()))
			dynamic_cast<textEditor*>(_pages->currentWidget())->nullIndex(true);*/

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
	/*if (dynamic_cast<Editor*>(_pages->currentWidget()))
		dynamic_cast<individualEditor*>(_pages->currentWidget())->setUnits(si);
	else if (dynamic_cast<preconfigEditor*>(_pages->currentWidget()))
		dynamic_cast<preconfigEditor*>(_pages->currentWidget())->setUnits(si);
	else if (dynamic_cast<customEditor*>(_pages->currentWidget()))
		dynamic_cast<customEditor*>(_pages->currentWidget())->setUnits(si);*/
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
	mapper->addMapping(pXBox, rsObstacleModel::P_X);
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
	mapper->addMapping(pYBox, rsObstacleModel::P_Y);
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
	mapper->addMapping(pZBox, rsObstacleModel::P_Z);
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
	mapper->addMapping(lXBox, rsObstacleModel::L_1);
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
	mapper->addMapping(lYBox, rsObstacleModel::L_2);
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
	mapper->addMapping(lZBox, rsObstacleModel::L_3);
	QWidget::connect(lZBox, SIGNAL(valueChanged(double)), _mapper, SLOT(submit()));

	// color
	_colorEditor = new colorEditor();
	_colorEditor->setObjectName("color");
	mapper->addMapping(_colorEditor, rsObstacleModel::COLOR, "color");
	QWidget::connect(_colorEditor, SIGNAL(colorChanged(QColor)), _mapper, SLOT(submit()));

	// set up units for item labels
	this->setUnits(false);

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
	hbox6->addWidget(_colorEditor);
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
	QColor color = (this->findChild<QPushButton *>("colorbutton"))->palette().color(QPalette::Button);
	if (nullify)
		color.setAlpha(50);
	else
		color.setAlpha(255);
	(this->findChild<QPushButton *>("colorbutton"))->setPalette(color);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void boxEditor::setUnits(bool si) {
	QString text;
	if (si)
		text = tr("cm");
	else
		text = tr("in");
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
 *	CustomEditor
 *
 *
 */

/*!	\brief Custom Wheeled obstacle Editor.
 *
 *	Build individual obstacle editor with custom wheel size.
 *
 *	\param		mapper data mapper from obstacleEditor model.
 */
/*customEditor::customEditor(QDataWidgetMapper *mapper, QWidget *parent) : QWidget(parent) {
	// save mapper
	_mapper = mapper;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">obstacle Editor</span>"));

	// form list
	QLabel *formLabel = new QLabel(tr("Form: "));
	QStringList formItems;
	formItems << tr("Linkbot I") << tr("Linkbot L") << tr("Mindstorms") << tr("Mobot");
	QStringListModel *formModel = new QStringListModel(formItems, this);
	QComboBox *formBox = new QComboBox();
	formBox->setObjectName("form");
	formBox->setModel(formModel);
	formLabel->setBuddy(formBox);
	mapper->addMapping(formBox, rsModel::FORM);
	QWidget::connect(formBox, SIGNAL(currentIndexChanged(int)), _mapper, SLOT(submit()));

	// name
	QLabel *nameLabel = new QLabel(tr("Name:"));
	QLineEdit *nameEdit = new QLineEdit;
	nameEdit->setObjectName("name");
	nameLabel->setBuddy(nameEdit);
	mapper->addMapping(nameEdit, rsModel::NAME);
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
	mapper->addMapping(pXBox, rsModel::P_X);
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
	mapper->addMapping(pYBox, rsModel::P_Y);
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
	mapper->addMapping(_rZBox, rsModel::R_PSI);
	QWidget::connect(_rZBox, SIGNAL(valueChanged(double)), this, SLOT(rotate(double)));

	// wheels list
	QLabel *wheelLabel = new QLabel(tr("Wheels:"));
	_wheelUnits = new QLabel(tr("cm"));
	_wheelBox = new QComboBox();
	_wheelBox->setObjectName("wheels");
	wheelLabel->setBuddy(_wheelBox);
	mapper->addMapping(_wheelBox, rsModel::WHEEL);
	QWidget::connect(_wheelBox, SIGNAL(currentIndexChanged(int)), _mapper, SLOT(submit()));

	// radius of wheel
	QLabel *radiusLabel = new QLabel(tr("Radius:"));
	_radiusUnits = new QLabel(tr("cm"));
	QLineEdit *radiusEdit = new QLineEdit;
	radiusEdit->setObjectName("radius");
	radiusLabel->setBuddy(radiusEdit);
	mapper->addMapping(radiusEdit, rsModel::RADIUS);
	QWidget::connect(radiusEdit, SIGNAL(editingFinished()), _mapper, SLOT(submit()));

	// color
	_colorEditor = new colorEditor();
	_colorEditor->setObjectName("color");
	mapper->addMapping(_colorEditor, rsModel::COLOR, "color");
	QWidget::connect(_colorEditor, SIGNAL(colorChanged(QColor)), _mapper, SLOT(submit()));

	// set up units for item labels
	this->setUnits(true);

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
	hbox7->addWidget(_colorEditor);
	layout->addLayout(hbox7);
	layout->addStretch(2);
	this->setLayout(layout);
}*/

/*!	\brief Slot to keep rotations between
 *		   -360 and 360 degrees.
 *
 *	\param		value Current value of the spinbox.
 */
/*void customEditor::rotate(double value) {
	_rZBox->setValue(value - static_cast<int>(value/360)*360);
	_mapper->submit();
}*/

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
/*void customEditor::nullIndex(bool nullify) {
	(this->findChild<QComboBox *>("form"))->setDisabled(nullify);
	(this->findChild<QLineEdit *>("name"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("px"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("py"))->setDisabled(nullify);
	(this->findChild<QDoubleSpinBox *>("rz"))->setDisabled(nullify);
	(this->findChild<QComboBox *>("wheels"))->setDisabled(nullify);
	(this->findChild<QLineEdit *>("radius"))->setDisabled(nullify);
	(this->findChild<QPushButton *>("colorbutton"))->setDisabled(nullify);
	QColor color = (this->findChild<QPushButton *>("colorbutton"))->palette().color(QPalette::Button);
	if (nullify)
		color.setAlpha(50);
	else
		color.setAlpha(255);
	(this->findChild<QPushButton *>("colorbutton"))->setPalette(color);
}*/

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
/*void customEditor::setUnits(bool si) {
	QString text;
	if (si)
		text = tr("cm");
	else
		text = tr("in");
	_pXUnits->setText(text);
	_pYUnits->setText(text);
	_wheelUnits->setText(text);

	// set wheel list to new values
	QStringList wheelItems;
	if (si)
		wheelItems << "None" << "4.13" << "4.45" << "5.08" << "Custom";
	else
		wheelItems << "None" << "1.625" << "1.75" << "2.00" << "Custom";
	QStringListModel *wheelModel = new QStringListModel(wheelItems, this);
	_wheelBox->setModel(wheelModel);
}*/

/*!
 *
 *
 *	colorEditor
 *
 *
 */
/*colorEditor::colorEditor(QWidget *parent) : QWidget(parent) {
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

QColor colorEditor::color(void) const {
    return _color;
}

void colorEditor::setColor(const QColor color) {
	if (color == _color)
		return;

	_color = color;
	_button->setPalette(QPalette(_color));
}

void colorEditor::onButtonClicked(void) {
	const QColor color = QColorDialog::getColor(_color, this);
	if (!color.isValid())
		return;

	this->setColor(color);
	emit colorChanged(color);
}*/

/*!
 *
 *
 *	obstacleEditorDelegate
 *
 *
 */
obstacleEditorDelegate::obstacleEditorDelegate(QObject *parent) : QItemDelegate(parent) {
}

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
qDebug() << "ob: " << model->data(index);
qDebug() << "ob: " << editor->property("value");
	QItemDelegate::setModelData(editor, model, index);
}

