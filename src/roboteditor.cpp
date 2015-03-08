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

	// robot ID label
	//_mapper->addMapping(_title, rsModel::ID, "text");

	// form list
	QLabel *formLabel = new QLabel(tr("Form: "));
	QStringList formItems;
	formItems << tr("Cubus") << tr("Linkbot I") << tr("Linkbot L") << tr("Linkbot T") << tr("Mobot") << tr("NXT");
	QStringListModel *formModel = new QStringListModel(formItems, this);
	QComboBox *formBox = new QComboBox();
	formBox->setModel(formModel);
	formLabel->setBuddy(formBox);
	_mapper->addMapping(formBox, rsModel::FORM);

	// position x
	QLabel *pXLabel = new QLabel(tr("Pos X:"));
	QDoubleSpinBox *pXBox = new QDoubleSpinBox();
	//pXBox->setMinimum(-180);
	//pXBox->setMinimum(180);
	pXBox->setSingleStep(0.5);
	pXLabel->setBuddy(pXBox);
	_mapper->addMapping(pXBox, rsModel::P_X);
	QWidget::connect(pXBox, SIGNAL(valueChanged(double)), _mapper, SLOT(submit()));

	// position y
	QLabel *pYLabel = new QLabel(tr("Pos Y:"));
	QDoubleSpinBox *pYBox = new QDoubleSpinBox();
	//pXBox->setMinimum(-180);
	//pXBox->setMinimum(180);
	//pXBox->setSingleStep(0.5);
	pYLabel->setBuddy(pYBox);
	_mapper->addMapping(pYBox, rsModel::P_Y);
	QWidget::connect(pYBox, SIGNAL(valueChanged(double)), _mapper, SLOT(submit()));

	// rotation phi
	QLabel *rZLabel = new QLabel(tr("Angle:"));
	QDoubleSpinBox *rZBox = new QDoubleSpinBox();
	//pXBox->setMinimum(-180);
	//pXBox->setMinimum(180);
	//pXBox->setSingleStep(0.5);
	pYLabel->setBuddy(rZBox);
	_mapper->addMapping(rZBox, rsModel::R_PSI);
	QWidget::connect(rZBox, SIGNAL(valueChanged(double)), _mapper, SLOT(submit()));

	// wheels list
	QLabel *wheelLabel = new QLabel(tr("Wheels:"));
	QStringList wheelItems;
	wheelItems << "None" << "1.625" << "1.75" << "2.00" << "Custom";
	QStringListModel *wheelModel = new QStringListModel(wheelItems, this);
	QComboBox *wheelBox = new QComboBox();
	wheelBox->setModel(wheelModel);
	wheelLabel->setBuddy(wheelBox);
	_mapper->addMapping(wheelBox, rsModel::WHEEL);

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
	hbox2->addWidget(pXBox);
	layout->addLayout(hbox2);
	QHBoxLayout *hbox3 = new QHBoxLayout();
	hbox3->addWidget(pYLabel, 0, Qt::AlignRight);
	hbox3->addWidget(pYBox);
	layout->addLayout(hbox3);
	QHBoxLayout *hbox4 = new QHBoxLayout();
	hbox4->addWidget(rZLabel, 0, Qt::AlignRight);
	hbox4->addWidget(rZBox);
	layout->addLayout(hbox4);
	QHBoxLayout *hbox5 = new QHBoxLayout();
	hbox5->addWidget(wheelLabel, 0, Qt::AlignRight);
	hbox5->addWidget(wheelBox);
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
