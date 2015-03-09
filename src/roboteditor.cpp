#include "roboteditor.h"

robotEditor::robotEditor(robotModel *model, QWidget *parent) : QWidget(parent) {
	// store robot model
	_model = model;

	// set size properties
	this->setMaximumWidth(256);
	this->setMinimumWidth(256);

	// set up mapper
	_mapper = new QDataWidgetMapper(this);
	_mapper->setModel(_model);
	_mapper->setItemDelegate(new robotEditorDelegate(this));
	_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

	// form list
	QLabel *formLabel = new QLabel(tr("Form: "));
	QStringList formItems;
	formItems << tr("Linkbot I") << tr("Linkbot L") << tr("Mindstorms") << tr("Mobot");
	QStringListModel *formModel = new QStringListModel(formItems, this);
	QComboBox *formBox = new QComboBox();
	formBox->setModel(formModel);
	formLabel->setBuddy(formBox);
	_mapper->addMapping(formBox, rsModel::FORM);

	// position x
	QLabel *pXLabel = new QLabel(tr("Pos X:"));
	_pXUnits = new QLabel();
	QDoubleSpinBox *pXBox = new QDoubleSpinBox();
	pXBox->setMinimum(-1000000);
	pXBox->setMaximum(1000000);
	pXBox->setSingleStep(0.5);
	pXLabel->setBuddy(pXBox);
	_mapper->addMapping(pXBox, rsModel::P_X);
	QWidget::connect(pXBox, SIGNAL(valueChanged(double)), _mapper, SLOT(submit()));

	// position y
	QLabel *pYLabel = new QLabel(tr("Pos Y:"));
	_pYUnits = new QLabel();
	QDoubleSpinBox *pYBox = new QDoubleSpinBox();
	pYBox->setMinimum(-1000000);
	pYBox->setMaximum(1000000);
	pYBox->setSingleStep(0.5);
	pYLabel->setBuddy(pYBox);
	_mapper->addMapping(pYBox, rsModel::P_Y);
	QWidget::connect(pYBox, SIGNAL(valueChanged(double)), _mapper, SLOT(submit()));

	// rotation psi
	QLabel *rZLabel = new QLabel(tr("Angle:"));
	QLabel *rZUnits = new QLabel(QString::fromUtf8("°"));
	_rZBox = new QDoubleSpinBox();
	_rZBox->setMinimum(-360);
	_rZBox->setMaximum(360);
	_rZBox->setSingleStep(0.5);
	rZLabel->setBuddy(_rZBox);
	_mapper->addMapping(_rZBox, rsModel::R_PSI);
	QWidget::connect(_rZBox, SIGNAL(valueChanged(double)), this, SLOT(rotate(double)));

	// wheels list
	QLabel *wheelLabel = new QLabel(tr("Wheels:"));
	_wheelUnits = new QLabel(tr("cm"));
	_wheelBox = new QComboBox();
	wheelLabel->setBuddy(_wheelBox);
	_mapper->addMapping(_wheelBox, rsModel::WHEEL);

	// set up buttons
	_nextButton = new QPushButton(tr("Next"));
	_nextButton->setEnabled(false);
	_previousButton = new QPushButton(tr("Previous"));
	_previousButton->setEnabled(false);

	// create signal connections
	QWidget::connect(_previousButton, SIGNAL(clicked()), _mapper, SLOT(toPrevious()));
	QWidget::connect(_previousButton, SIGNAL(clicked()), this, SLOT(buttonPressed()));
	QWidget::connect(_nextButton, SIGNAL(clicked()), _mapper, SLOT(toNext()));
	QWidget::connect(_nextButton, SIGNAL(clicked()), this, SLOT(buttonPressed()));

	// set up units for item labels
	this->setUnits(true);

	// lay out grid
	QVBoxLayout *vbox = new QVBoxLayout();
	QGroupBox *group = new QGroupBox(tr("Robot Editor"));
	QVBoxLayout *layout = new QVBoxLayout(group);
	layout->addStretch(1);
	QHBoxLayout *hbox1 = new QHBoxLayout();
	hbox1->addWidget(formLabel, 0, Qt::AlignRight);
	hbox1->addWidget(formBox);
	layout->addLayout(hbox1);
	QHBoxLayout *hbox2 = new QHBoxLayout();
	hbox2->addWidget(pXLabel, 0, Qt::AlignRight);
	hbox2->addWidget(pXBox, 1, Qt::AlignRight);
	hbox2->addWidget(_pXUnits, 0, Qt::AlignLeft);
	layout->addLayout(hbox2);
	QHBoxLayout *hbox3 = new QHBoxLayout();
	hbox3->addWidget(pYLabel, 0, Qt::AlignRight);
	hbox3->addWidget(pYBox, 1, Qt::AlignRight);
	hbox3->addWidget(_pYUnits, 0, Qt::AlignLeft);
	layout->addLayout(hbox3);
	QHBoxLayout *hbox4 = new QHBoxLayout();
	hbox4->addWidget(rZLabel, 0, Qt::AlignRight);
	hbox4->addWidget(_rZBox, 1, Qt::AlignRight);
	hbox4->addWidget(rZUnits, 0, Qt::AlignLeft);
	layout->addLayout(hbox4);
	QHBoxLayout *hbox5 = new QHBoxLayout();
	hbox5->addWidget(wheelLabel, 0, Qt::AlignRight);
	hbox5->addWidget(_wheelBox, 1, Qt::AlignRight);
	hbox5->addWidget(_wheelUnits, 0, Qt::AlignLeft);
	layout->addLayout(hbox5);
	layout->addStretch(1);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(_previousButton, 0, Qt::AlignCenter);
	hbox6->addWidget(_nextButton, 0, Qt::AlignCenter);
	layout->addLayout(hbox6);
	group->setLayout(layout);
	vbox->addWidget(group);
	this->setLayout(vbox);

	// go to first item
	_mapper->toFirst();
}

void robotEditor::dataChanged(QModelIndex/*topLeft*/, QModelIndex bottomRight) {
	this->setCurrentIndex(bottomRight);
}

void robotEditor::setCurrentIndex(const QModelIndex &index) {
	// set new index for mapper
	_mapper->setCurrentIndex(index.row());

	// update editor view
	_previousButton->setEnabled(index.row() > 0);
	_nextButton->setEnabled(index.row() < _mapper->model()->rowCount() - 1);
}

void robotEditor::buttonPressed(void) {
	// signal other views that index has changed
	emit indexChanged(_mapper->model()->index(_mapper->currentIndex(), 0));
}

/*!	\brief Slot to keep rotations between
 *		   -360 and 360 degrees.
 *
 *	\param		value Current value of the spinbox.
 */
void robotEditor::rotate(double value) {
	_rZBox->setValue(value - static_cast<int>(value/360)*360);
	_mapper->submit();
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void robotEditor::setUnits(bool si) {
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
}

robotEditorDelegate::robotEditorDelegate(QObject *parent) : QItemDelegate(parent) {
}

void robotEditorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
	if (!strcmp(editor->metaObject()->className(), "QComboBox")) {
		editor->setProperty("currentIndex", index.data());
		return;
	}
	QItemDelegate::setEditorData(editor, index);
}

void robotEditorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
	if (!strcmp(editor->metaObject()->className(), "QComboBox")) {
		QVariant value = editor->property("currentIndex");
		if (value.isValid()) {
			model->setData(index, value);
			return;
		}
	}
	QItemDelegate::setModelData(editor, model, index);
}
