#include "objecteditor.h"

/*!
 *
 *
 *	objectEditor
 *
 *
 */

objectEditor::objectEditor(objectModel *model, QWidget *parent) : QWidget(parent) {
	// store object model
	_model = model;

	// set size properties
	QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Preferred);
	sp.setHorizontalStretch(1);
	this->setSizePolicy(sp);

	// set up editor pages
	_pages = new QStackedWidget;
	_pages->addWidget(new emptyEditor());
	_pages->addWidget(new arcEditor(_model));
	_pages->addWidget(new arcSectorEditor(_model));
	_pages->addWidget(new arcSegmentEditor(_model));
	_pages->addWidget(new arrowEditor(_model));
	_pages->addWidget(new boxEditor(_model));
	_pages->addWidget(new circleEditor(_model));
	_pages->addWidget(new competitionborderEditor(_model));
	_pages->addWidget(new cylinderEditor(_model));
	_pages->addWidget(new dotEditor(_model));
	_pages->addWidget(new ellipseEditor(_model));
	_pages->addWidget(new hackysackEditor(_model));
	_pages->addWidget(new lineEditor(_model));
	_pages->addWidget(new polygonEditor(_model));
	_pages->addWidget(new pullupbarEditor(_model));
	_pages->addWidget(new quadEditor(_model));
	_pages->addWidget(new rectangleEditor(_model));
	_pages->addWidget(new sphereEditor(_model));
	_pages->addWidget(new textEditor(_model));
	_pages->addWidget(new triangleEditor(_model));
	_pages->addWidget(new woodblockEditor(_model));

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

void objectEditor::dataChanged(QModelIndex/*topLeft*/, QModelIndex bottomRight) {
	if (bottomRight.row() != _row)
		this->setCurrentIndex(bottomRight);
}

void objectEditor::setCurrentIndex(const QModelIndex &index) {
	if (index.isValid()) {
		// set new curent model row
		_row = index.row();

		// load appropriate page
		int form = _model->data(_model->index(index.row(), rsObjectModel::FORM), Qt::EditRole).toInt();
		switch (form) {
			case rs::Arc:
				_pages->setCurrentIndex(rs::Arc + 1);
				dynamic_cast<arcEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<arcEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::ArcSector:
				_pages->setCurrentIndex(rs::ArcSector + 1);
				dynamic_cast<arcSectorEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<arcSectorEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::ArcSegment:
				_pages->setCurrentIndex(rs::ArcSegment + 1);
				dynamic_cast<arcSegmentEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<arcSegmentEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::Arrow:
				_pages->setCurrentIndex(rs::Arrow + 1);
				dynamic_cast<arrowEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<arrowEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::Box:
				_pages->setCurrentIndex(rs::Box + 1);
				dynamic_cast<boxEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<boxEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::Circle:
				_pages->setCurrentIndex(rs::Circle + 1);
				dynamic_cast<circleEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<circleEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::CompetitionBorder:
				_pages->setCurrentIndex(rs::CompetitionBorder + 1);
				dynamic_cast<competitionborderEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<competitionborderEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::Cylinder:
				_pages->setCurrentIndex(rs::Cylinder + 1);
				dynamic_cast<cylinderEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<cylinderEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::Dot:
				_pages->setCurrentIndex(rs::Dot + 1);
				dynamic_cast<dotEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<dotEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::Ellipse:
				_pages->setCurrentIndex(rs::Ellipse + 1);
				dynamic_cast<ellipseEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<ellipseEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::HackySack:
				_pages->setCurrentIndex(rs::HackySack + 1);
				dynamic_cast<hackysackEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<hackysackEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::Line:
				_pages->setCurrentIndex(rs::Line + 1);
				dynamic_cast<lineEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<lineEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::Polygon:
				_pages->setCurrentIndex(rs::Polygon + 1);
				dynamic_cast<polygonEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<polygonEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::PullupBar:
				_pages->setCurrentIndex(rs::PullupBar + 1);
				dynamic_cast<pullupbarEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<pullupbarEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::Quad:
				_pages->setCurrentIndex(rs::Quad + 1);
				dynamic_cast<quadEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<quadEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::Rectangle:
				_pages->setCurrentIndex(rs::Rectangle + 1);
				dynamic_cast<rectangleEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<rectangleEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::Sphere:
				_pages->setCurrentIndex(rs::Sphere + 1);
				dynamic_cast<sphereEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<sphereEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::Text:
				_pages->setCurrentIndex(rs::Text + 1);
				dynamic_cast<textEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<textEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::Triangle:
				_pages->setCurrentIndex(rs::Triangle + 1);
				dynamic_cast<triangleEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<triangleEditor *>(_pages->currentWidget())->setIndex(_row);
				break;
			case rs::WoodBlock:
				_pages->setCurrentIndex(rs::WoodBlock + 1);
				dynamic_cast<woodblockEditor *>(_pages->currentWidget())->setUnits(_units);
				dynamic_cast<woodblockEditor *>(_pages->currentWidget())->setIndex(_row);
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

void objectEditor::nextButtonPressed(void) {
	// set new index
	QModelIndex index = _model->index(_row + 1, 0);
	this->setCurrentIndex(index);

	// signal other views that index has changed
	emit indexChanged(index);
}

void objectEditor::prevButtonPressed(void) {
	// set new index
	QModelIndex index = _model->index(_row - 1, 0);
	this->setCurrentIndex(index);

	// signal other views that index has changed
	emit indexChanged(index);
}

void objectEditor::deleteCurrentIndex(void) {
	// save current index
	int row = _row;

	// remove current object from model
	_model->removeRows(row, 1);

	// if it is invalid, then set the last row in the model
	if (row >= _model->rowCount())
		row = _model->rowCount() - 1;

	// set new index
	this->setCurrentIndex(_model->index(row, rsObjectModel::ID));

	// signal a change in current object
	emit indexChanged(_model->index(row, rsObjectModel::ID));
}

void objectEditor::setUnits(bool si) {
	// do nothing if units don't change
	if (_units == si) return;

	// save units
	_units = si;

	// reload current editor
	this->setCurrentIndex(_model->index(_row, rsObjectModel::FORM));
}

/*!
 *
 *
 *	Arc Editor
 *
 *
 */

/*!	\brief Arc Obstacle Editor.
 *
 *	Build individual arc editor with relevant pieces of information.
 *
 *	\param		model data model from objectEditor model.
 */
arcEditor::arcEditor(objectModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Arc Editor</span>"));

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
	pXBox->setToolTip("Set the X position of the arc");
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
	pYBox->setToolTip("Set the Y position of the arc");
	pYBox->setToolTipDuration(-1);

	// position z
	QLabel *pZLabel = new QLabel(tr("Radius:"));
	_pZUnits = new QLabel();
	QDoubleSpinBox *pZBox = new QDoubleSpinBox();
	pZBox->setObjectName("radius");
	pZBox->setMinimum(-1000000);
	pZBox->setMaximum(1000000);
	pZBox->setSingleStep(0.5);
	pZLabel->setBuddy(pZBox);
	QWidget::connect(pZBox, SIGNAL(valueChanged(double)), this, SLOT(submitPZ(double)));
	pZBox->setToolTip("Set the radius of the arc");
	pZBox->setToolTipDuration(-1);

	// rotation psi
	QLabel *l1Label = new QLabel(tr("Start:"));
	QLabel *l1Units = new QLabel(QString::fromUtf8("°"));
	QDoubleSpinBox *l1Box = new QDoubleSpinBox();
	l1Box->setObjectName("start");
	l1Box->setMinimum(-360);
	l1Box->setMaximum(360);
	l1Box->setSingleStep(0.5);
	l1Label->setBuddy(l1Box);
	QWidget::connect(l1Box, SIGNAL(valueChanged(double)), this, SLOT(submitRX(double)));
	l1Box->setToolTip("Set the starting angle of the arc");
	l1Box->setToolTipDuration(-1);

	// rotation psi
	QLabel *l2Label = new QLabel(tr("End:"));
	QLabel *l2Units = new QLabel(QString::fromUtf8("°"));
	QDoubleSpinBox *l2Box = new QDoubleSpinBox();
	l2Box->setObjectName("end");
	l2Box->setMinimum(-360);
	l2Box->setMaximum(360);
	l2Box->setSingleStep(0.5);
	l2Label->setBuddy(l2Box);
	QWidget::connect(l2Box, SIGNAL(valueChanged(double)), this, SLOT(submitRY(double)));
	l2Box->setToolTip("Set the ending angle of the arc");
	l2Box->setToolTipDuration(-1);

	// width
	QLabel *widthLabel = new QLabel(tr("Line Width:"));
	QDoubleSpinBox *widthBox = new QDoubleSpinBox();
	widthBox->setObjectName("size");
	widthBox->setMinimum(0);
	widthBox->setMaximum(100);
	widthBox->setSingleStep(1);
	widthLabel->setBuddy(widthBox);
	QWidget::connect(widthBox, SIGNAL(valueChanged(double)), this, SLOT(submitSize(double)));
	widthBox->setToolTip("Set the line width of the arc");
	widthBox->setToolTipDuration(-1);

	// color
	_colorPicker = new bodyColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the arc");
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
	hbox2->addWidget(pYLabel, 2, Qt::AlignRight);
	hbox2->addWidget(pYBox, 5);
	hbox2->addWidget(_pYUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox2);
	QHBoxLayout *hbox3 = new QHBoxLayout();
	hbox3->addWidget(l1Label, 2, Qt::AlignRight);
	hbox3->addWidget(l1Box, 5);
	hbox3->addWidget(l1Units, 1, Qt::AlignLeft);
	hbox3->addWidget(l2Label, 2, Qt::AlignRight);
	hbox3->addWidget(l2Box, 5);
	hbox3->addWidget(l2Units, 1, Qt::AlignLeft);
	layout->addLayout(hbox3);
	QHBoxLayout *hbox4 = new QHBoxLayout();
	hbox4->addWidget(pZLabel, 2, Qt::AlignRight);
	hbox4->addWidget(pZBox, 5);
	hbox4->addWidget(_pZUnits, 1, Qt::AlignLeft);
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

void arcEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_X), value);
}

void arcEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Y), value);
}

void arcEditor::submitPZ(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Z), value);
}

void arcEditor::submitRX(double value) {
	_model->setData(_model->index(_row, rsObjectModel::R_PHI), value);
}

void arcEditor::submitRY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::R_THETA), value);
}

void arcEditor::submitSize(double value) {
	_model->setData(_model->index(_row, rsObjectModel::SIZE), value);
}

void arcEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsObjectModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void arcEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsObjectModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("radius"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Z), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("start"))->setValue(_model->data(_model->index(row, rsObjectModel::R_PHI), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("end"))->setValue(_model->data(_model->index(row, rsObjectModel::R_THETA), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("size"))->setValue(_model->data(_model->index(row, rsObjectModel::SIZE), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsObjectModel::COLOR), Qt::EditRole).toString());
	(this->findChild<bodyColorPicker *>("color"))->setColor(color);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void arcEditor::setUnits(bool si) {
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
 *	Arc Sector Editor
 *
 *
 */

/*!	\brief Arc Obstacle Editor.
 *
 *	Build individual arc editor with relevant pieces of information.
 *
 *	\param		model data model from objectEditor model.
 */
arcSectorEditor::arcSectorEditor(objectModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Arc Sector Editor</span>"));

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
	pXBox->setToolTip("Set the X position of the arc sector");
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
	pYBox->setToolTip("Set the Y position of the arc sector");
	pYBox->setToolTipDuration(-1);

	// position z
	QLabel *pZLabel = new QLabel(tr("Radius:"));
	_pZUnits = new QLabel();
	QDoubleSpinBox *pZBox = new QDoubleSpinBox();
	pZBox->setObjectName("radius");
	pZBox->setMinimum(-1000000);
	pZBox->setMaximum(1000000);
	pZBox->setSingleStep(0.5);
	pZLabel->setBuddy(pZBox);
	QWidget::connect(pZBox, SIGNAL(valueChanged(double)), this, SLOT(submitPZ(double)));
	pZBox->setToolTip("Set the radius of the arc sector");
	pZBox->setToolTipDuration(-1);

	// rotation psi
	QLabel *l1Label = new QLabel(tr("Start:"));
	QLabel *l1Units = new QLabel(QString::fromUtf8("°"));
	QDoubleSpinBox *l1Box = new QDoubleSpinBox();
	l1Box->setObjectName("start");
	l1Box->setMinimum(-360);
	l1Box->setMaximum(360);
	l1Box->setSingleStep(0.5);
	l1Label->setBuddy(l1Box);
	QWidget::connect(l1Box, SIGNAL(valueChanged(double)), this, SLOT(submitRX(double)));
	l1Box->setToolTip("Set the starting angle of the arc sector");
	l1Box->setToolTipDuration(-1);

	// rotation psi
	QLabel *l2Label = new QLabel(tr("End:"));
	QLabel *l2Units = new QLabel(QString::fromUtf8("°"));
	QDoubleSpinBox *l2Box = new QDoubleSpinBox();
	l2Box->setObjectName("end");
	l2Box->setMinimum(-360);
	l2Box->setMaximum(360);
	l2Box->setSingleStep(0.5);
	l2Label->setBuddy(l2Box);
	QWidget::connect(l2Box, SIGNAL(valueChanged(double)), this, SLOT(submitRY(double)));
	l2Box->setToolTip("Set the ending angle of the arc sector");
	l2Box->setToolTipDuration(-1);

	// width
	QLabel *widthLabel = new QLabel(tr("Line Width:"));
	QDoubleSpinBox *widthBox = new QDoubleSpinBox();
	widthBox->setObjectName("size");
	widthBox->setMinimum(0);
	widthBox->setMaximum(100);
	widthBox->setSingleStep(1);
	widthLabel->setBuddy(widthBox);
	QWidget::connect(widthBox, SIGNAL(valueChanged(double)), this, SLOT(submitSize(double)));
	widthBox->setToolTip("Set the line width of the arc sector");
	widthBox->setToolTipDuration(-1);

	// color
	_colorPicker = new bodyColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the arc sector");
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
	hbox2->addWidget(pYLabel, 2, Qt::AlignRight);
	hbox2->addWidget(pYBox, 5);
	hbox2->addWidget(_pYUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox2);
	QHBoxLayout *hbox3 = new QHBoxLayout();
	hbox3->addWidget(l1Label, 2, Qt::AlignRight);
	hbox3->addWidget(l1Box, 5);
	hbox3->addWidget(l1Units, 1, Qt::AlignLeft);
	hbox3->addWidget(l2Label, 2, Qt::AlignRight);
	hbox3->addWidget(l2Box, 5);
	hbox3->addWidget(l2Units, 1, Qt::AlignLeft);
	layout->addLayout(hbox3);
	QHBoxLayout *hbox4 = new QHBoxLayout();
	hbox4->addWidget(pZLabel, 2, Qt::AlignRight);
	hbox4->addWidget(pZBox, 5);
	hbox4->addWidget(_pZUnits, 1, Qt::AlignLeft);
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

void arcSectorEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_X), value);
}

void arcSectorEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Y), value);
}

void arcSectorEditor::submitPZ(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Z), value);
}

void arcSectorEditor::submitRX(double value) {
	_model->setData(_model->index(_row, rsObjectModel::R_PHI), value);
}

void arcSectorEditor::submitRY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::R_THETA), value);
}

void arcSectorEditor::submitSize(double value) {
	_model->setData(_model->index(_row, rsObjectModel::SIZE), value);
}

void arcSectorEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsObjectModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void arcSectorEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsObjectModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("radius"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Z), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("start"))->setValue(_model->data(_model->index(row, rsObjectModel::R_PHI), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("end"))->setValue(_model->data(_model->index(row, rsObjectModel::R_THETA), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("size"))->setValue(_model->data(_model->index(row, rsObjectModel::SIZE), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsObjectModel::COLOR), Qt::EditRole).toString());
	(this->findChild<bodyColorPicker *>("color"))->setColor(color);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void arcSectorEditor::setUnits(bool si) {
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
 *	Arc Secgment Editor
 *
 *
 */

/*!	\brief Arc Segment Obstacle Editor.
 *
 *	Build individual arc segment editor with relevant pieces of information.
 *
 *	\param		model data model from objectEditor model.
 */
arcSegmentEditor::arcSegmentEditor(objectModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Arc Segment Editor</span>"));

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
	pXBox->setToolTip("Set the X position of the arc segment");
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
	pYBox->setToolTip("Set the Y position of the arc segment");
	pYBox->setToolTipDuration(-1);

	// position z
	QLabel *pZLabel = new QLabel(tr("Radius:"));
	_pZUnits = new QLabel();
	QDoubleSpinBox *pZBox = new QDoubleSpinBox();
	pZBox->setObjectName("radius");
	pZBox->setMinimum(-1000000);
	pZBox->setMaximum(1000000);
	pZBox->setSingleStep(0.5);
	pZLabel->setBuddy(pZBox);
	QWidget::connect(pZBox, SIGNAL(valueChanged(double)), this, SLOT(submitPZ(double)));
	pZBox->setToolTip("Set the radius of the arc segment");
	pZBox->setToolTipDuration(-1);

	// rotation psi
	QLabel *l1Label = new QLabel(tr("Start:"));
	QLabel *l1Units = new QLabel(QString::fromUtf8("°"));
	QDoubleSpinBox *l1Box = new QDoubleSpinBox();
	l1Box->setObjectName("start");
	l1Box->setMinimum(-360);
	l1Box->setMaximum(360);
	l1Box->setSingleStep(0.5);
	l1Label->setBuddy(l1Box);
	QWidget::connect(l1Box, SIGNAL(valueChanged(double)), this, SLOT(submitRX(double)));
	l1Box->setToolTip("Set the starting angle of the arc segment");
	l1Box->setToolTipDuration(-1);

	// rotation psi
	QLabel *l2Label = new QLabel(tr("End:"));
	QLabel *l2Units = new QLabel(QString::fromUtf8("°"));
	QDoubleSpinBox *l2Box = new QDoubleSpinBox();
	l2Box->setObjectName("end");
	l2Box->setMinimum(-360);
	l2Box->setMaximum(360);
	l2Box->setSingleStep(0.5);
	l2Label->setBuddy(l2Box);
	QWidget::connect(l2Box, SIGNAL(valueChanged(double)), this, SLOT(submitRY(double)));
	l2Box->setToolTip("Set the ending angle of the arc segment");
	l2Box->setToolTipDuration(-1);

	// width
	QLabel *widthLabel = new QLabel(tr("Line Width:"));
	QDoubleSpinBox *widthBox = new QDoubleSpinBox();
	widthBox->setObjectName("size");
	widthBox->setMinimum(0);
	widthBox->setMaximum(100);
	widthBox->setSingleStep(1);
	widthLabel->setBuddy(widthBox);
	QWidget::connect(widthBox, SIGNAL(valueChanged(double)), this, SLOT(submitSize(double)));
	widthBox->setToolTip("Set the line width of the arc segment");
	widthBox->setToolTipDuration(-1);

	// color
	_colorPicker = new bodyColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the arc segment");
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
	hbox2->addWidget(pYLabel, 2, Qt::AlignRight);
	hbox2->addWidget(pYBox, 5);
	hbox2->addWidget(_pYUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox2);
	QHBoxLayout *hbox3 = new QHBoxLayout();
	hbox3->addWidget(l1Label, 2, Qt::AlignRight);
	hbox3->addWidget(l1Box, 5);
	hbox3->addWidget(l1Units, 1, Qt::AlignLeft);
	hbox3->addWidget(l2Label, 2, Qt::AlignRight);
	hbox3->addWidget(l2Box, 5);
	hbox3->addWidget(l2Units, 1, Qt::AlignLeft);
	layout->addLayout(hbox3);
	QHBoxLayout *hbox4 = new QHBoxLayout();
	hbox4->addWidget(pZLabel, 2, Qt::AlignRight);
	hbox4->addWidget(pZBox, 5);
	hbox4->addWidget(_pZUnits, 1, Qt::AlignLeft);
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

void arcSegmentEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_X), value);
}

void arcSegmentEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Y), value);
}

void arcSegmentEditor::submitPZ(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Z), value);
}

void arcSegmentEditor::submitRX(double value) {
	_model->setData(_model->index(_row, rsObjectModel::R_PHI), value);
}

void arcSegmentEditor::submitRY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::R_THETA), value);
}

void arcSegmentEditor::submitSize(double value) {
	_model->setData(_model->index(_row, rsObjectModel::SIZE), value);
}

void arcSegmentEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsObjectModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void arcSegmentEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsObjectModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("radius"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Z), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("start"))->setValue(_model->data(_model->index(row, rsObjectModel::R_PHI), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("end"))->setValue(_model->data(_model->index(row, rsObjectModel::R_THETA), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("size"))->setValue(_model->data(_model->index(row, rsObjectModel::SIZE), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsObjectModel::COLOR), Qt::EditRole).toString());
	(this->findChild<bodyColorPicker *>("color"))->setColor(color);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void arcSegmentEditor::setUnits(bool si) {
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
 *	Arrow Editor
 *
 *
 */

/*!	\brief Arrow Drawing Editor.
 *
 *	Build individual arrow editor with relevant pieces of information.
 *
 *	\param		model data model from objectEditor model.
 */
arrowEditor::arrowEditor(objectModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Arrow Editor</span>"));

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
	pXBox->setToolTip("Set the starting X position of the arrow");
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
	pXBox->setToolTip("Set the starting Y position of the arrow");
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
	pXBox->setToolTip("Set the ending X position of the arrow");
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
	pXBox->setToolTip("Set the ending Y position of the arrow");
	pXBox->setToolTipDuration(-1);

	// width
	QLabel *widthLabel = new QLabel(tr("Line Width:"));
	QDoubleSpinBox *widthBox = new QDoubleSpinBox();
	widthBox->setObjectName("size");
	widthBox->setMinimum(0);
	widthBox->setMaximum(100);
	widthBox->setSingleStep(1);
	widthLabel->setBuddy(widthBox);
	QWidget::connect(widthBox, SIGNAL(valueChanged(double)), this, SLOT(submitSize(double)));
	widthBox->setToolTip("Set the line width of the arrow");
	widthBox->setToolTipDuration(-1);

	// color
	_colorPicker = new bodyColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the arrow");
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

void arrowEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_X), value);
}

void arrowEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Y), value);
}

void arrowEditor::submitL1(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_1), value);
}

void arrowEditor::submitL2(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_2), value);
}

void arrowEditor::submitSize(double value) {
	_model->setData(_model->index(_row, rsObjectModel::SIZE), value);
}

void arrowEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsObjectModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void arrowEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px1"))->setValue(_model->data(_model->index(row, rsObjectModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py1"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("px2"))->setValue(_model->data(_model->index(row, rsObjectModel::L_1), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py2"))->setValue(_model->data(_model->index(row, rsObjectModel::L_2), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("size"))->setValue(_model->data(_model->index(row, rsObjectModel::SIZE), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsObjectModel::COLOR), Qt::EditRole).toString());
	(this->findChild<bodyColorPicker *>("color"))->setColor(color);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void arrowEditor::setUnits(bool si) {
	QString text;
	if (si) text = tr("cm");
	else text = tr("in");
	_pXUnits->setText(text);
	_pYUnits->setText(text);
	_lXUnits->setText(text);
	_lYUnits->setText(text);
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
 *	\param		model data model from objectEditor model.
 */
boxEditor::boxEditor(objectModel *model, QWidget *parent) : QWidget(parent) {
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
	_model->setData(_model->index(_row, rsObjectModel::P_X), value);
}

void boxEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Y), value);
}

void boxEditor::submitPZ(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Z), value);
}

void boxEditor::submitL1(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_1), value);
}

void boxEditor::submitL2(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_2), value);
}

void boxEditor::submitL3(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_3), value);
}

void boxEditor::submitMass(double value) {
	_model->setData(_model->index(_row, rsObjectModel::MASS), value);
}

void boxEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsObjectModel::COLOR), color);
}

/*!	\brief Slot to re-enable all inputs.
 *
 *	\param		row Row of the model to load.
 */
void boxEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsObjectModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("pz"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Z), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("lx"))->setValue(_model->data(_model->index(row, rsObjectModel::L_1), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("ly"))->setValue(_model->data(_model->index(row, rsObjectModel::L_2), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("lz"))->setValue(_model->data(_model->index(row, rsObjectModel::L_3), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("mass"))->setValue(_model->data(_model->index(row, rsObjectModel::MASS), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsObjectModel::COLOR), Qt::EditRole).toString());
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
 *	Circle Editor
 *
 *
 */

/*!	\brief Circle Obstacle Editor.
 *
 *	Build individual circle editor with relevant pieces of information.
 *
 *	\param		model data model from objectEditor model.
 */
circleEditor::circleEditor(objectModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Circle Editor</span>"));

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
	pXBox->setToolTip("Set the X position of the circle");
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
	pYBox->setToolTip("Set the Y position of the circle");
	pYBox->setToolTipDuration(-1);

	// position z
	QLabel *pZLabel = new QLabel(tr("Radius:"));
	_pZUnits = new QLabel();
	QDoubleSpinBox *pZBox = new QDoubleSpinBox();
	pZBox->setObjectName("radius");
	pZBox->setMinimum(-1000000);
	pZBox->setMaximum(1000000);
	pZBox->setSingleStep(0.5);
	pZLabel->setBuddy(pZBox);
	QWidget::connect(pZBox, SIGNAL(valueChanged(double)), this, SLOT(submitPZ(double)));
	pZBox->setToolTip("Set the radius of the circle");
	pZBox->setToolTipDuration(-1);

	// width
	QLabel *widthLabel = new QLabel(tr("Line Width:"));
	QDoubleSpinBox *widthBox = new QDoubleSpinBox();
	widthBox->setObjectName("size");
	widthBox->setMinimum(0);
	widthBox->setMaximum(100);
	widthBox->setSingleStep(1);
	widthLabel->setBuddy(widthBox);
	QWidget::connect(widthBox, SIGNAL(valueChanged(double)), this, SLOT(submitSize(double)));
	widthBox->setToolTip("Set the line width of the circle");
	widthBox->setToolTipDuration(-1);

	// color
	_colorPicker = new bodyColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the circle");
	_colorPicker->setToolTipDuration(-1);

	// fill
	bodyColorPicker *fill = new bodyColorPicker("Fill Color");
	fill->setObjectName("fill");
	QWidget::connect(fill, SIGNAL(colorChanged(QColor)), this, SLOT(submitFill(QColor)));
	fill->setToolTip("Choose the fill color of the circle");
	fill->setToolTipDuration(-1);

	// lay out grid
	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *hbox0 = new QHBoxLayout();
	hbox0->addWidget(title, 5, Qt::AlignHCenter);
	layout->addLayout(hbox0);
	layout->addStretch(1);
	QHBoxLayout *hbox1 = new QHBoxLayout();
	hbox1->addWidget(pXLabel, 2, Qt::AlignRight);
	hbox1->addWidget(pXBox, 5);
	hbox1->addWidget(_pXUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox1);
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
	hbox5->addWidget(widthLabel, 2, Qt::AlignRight);
	hbox5->addWidget(widthBox, 5);
	layout->addLayout(hbox5);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(_colorPicker);
	layout->addLayout(hbox6);
	QHBoxLayout *hbox7 = new QHBoxLayout();
	hbox7->addWidget(fill);
	layout->addLayout(hbox7);
	layout->addStretch(2);
	this->setLayout(layout);
}

void circleEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_X), value);
}

void circleEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Y), value);
}

void circleEditor::submitPZ(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Z), value);
}

void circleEditor::submitSize(double value) {
	_model->setData(_model->index(_row, rsObjectModel::SIZE), value);
}

void circleEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsObjectModel::COLOR), color);
}

void circleEditor::submitFill(QColor color) {
	_model->setData(_model->index(_row, rsObjectModel::FILL), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void circleEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsObjectModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("radius"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Z), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("size"))->setValue(_model->data(_model->index(row, rsObjectModel::SIZE), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsObjectModel::COLOR), Qt::EditRole).toString());
	(this->findChild<bodyColorPicker *>("color"))->setColor(color);
	QColor fill(_model->data(_model->index(row, rsObjectModel::FILL), Qt::EditRole).toString());
	(this->findChild<bodyColorPicker *>("fill"))->setColor(fill);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void circleEditor::setUnits(bool si) {
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
 *	Competition Border Editor
 *
 *
 */

/*!	\brief Competition Border Drawing Editor.
 *
 *	Build individual competition border editor with relevant pieces of information.
 *
 *	\param		model data model from objectEditor model.
 */
competitionborderEditor::competitionborderEditor(objectModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Competition Border Editor</span>"));

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
	pXBox->setToolTip("Set the X position of the competition border");
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
	pYBox->setToolTip("Set the Y position of the competition border");
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
	_rZBox->setToolTip("Set the rotation of the competition border");
	_rZBox->setToolTipDuration(-1);

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
	lXBox->setToolTip("Set the X length of the competition border");
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
	lYBox->setToolTip("Set the Y length of the competition border");
	lYBox->setToolTipDuration(-1);

	// length z
	QLabel *lZLabel = new QLabel(tr("Radius:"));
	_lZUnits = new QLabel();
	QDoubleSpinBox *lZBox = new QDoubleSpinBox();
	lZBox->setObjectName("lz");
	lZBox->setMinimum(-100);
	lZBox->setMaximum(100);
	lZBox->setSingleStep(0.5);
	lZLabel->setBuddy(lZBox);
	QWidget::connect(lZBox, SIGNAL(valueChanged(double)), this, SLOT(submitL3(double)));
	lZBox->setToolTip("Set the radius of the competition border cylinders");
	lZBox->setToolTipDuration(-1);

	// color
	_colorPicker = new bodyColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the competition border");
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
	hbox4->addWidget(rZLabel, 2, Qt::AlignRight);
	hbox4->addWidget(_rZBox, 5);
	hbox4->addWidget(rZUnits, 1, Qt::AlignLeft);
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

void competitionborderEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_X), value);
}

void competitionborderEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Y), value);
}

void competitionborderEditor::submitRZ(double value) {
	_rZBox->setValue(value - static_cast<int>(value / 360) * 360);
	_model->setData(_model->index(_row, rsObjectModel::R_PSI), value);
}

void competitionborderEditor::submitL1(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_1), value);
}

void competitionborderEditor::submitL2(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_2), value);
}

void competitionborderEditor::submitL3(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_3), value);
}

void competitionborderEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsObjectModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void competitionborderEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsObjectModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("rz"))->setValue(_model->data(_model->index(row, rsObjectModel::R_PSI), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("lx"))->setValue(_model->data(_model->index(row, rsObjectModel::L_1), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("ly"))->setValue(_model->data(_model->index(row, rsObjectModel::L_2), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("lz"))->setValue(_model->data(_model->index(row, rsObjectModel::L_3), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsObjectModel::COLOR), Qt::EditRole).toString());
	(this->findChild<bodyColorPicker *>("color"))->setColor(color);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void competitionborderEditor::setUnits(bool si) {
	QString text;
	if (si) text = tr("cm");
	else text = tr("in");
	_pXUnits->setText(text);
	_pYUnits->setText(text);
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
 *	\param		model data model from objectEditor model.
 */
cylinderEditor::cylinderEditor(objectModel *model, QWidget *parent) : QWidget(parent) {
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
	hbox4->addWidget(axisLabel, 2, Qt::AlignRight);
	hbox4->addWidget(axisBox, 5);
	hbox4->addStretch(1);
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

void cylinderEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_X), value);
}

void cylinderEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Y), value);
}

void cylinderEditor::submitPZ(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Z), value);
}

void cylinderEditor::submitL1(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_1), value);
}

void cylinderEditor::submitL2(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_2), value);
}

void cylinderEditor::submitAxis(int value) {
	_model->setData(_model->index(_row, rsObjectModel::AXIS), value);
}

void cylinderEditor::submitMass(double value) {
	_model->setData(_model->index(_row, rsObjectModel::MASS), value);
}

void cylinderEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsObjectModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void cylinderEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsObjectModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("pz"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Z), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("radius"))->setValue(_model->data(_model->index(row, rsObjectModel::L_1), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("length"))->setValue(_model->data(_model->index(row, rsObjectModel::L_2), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("mass"))->setValue(_model->data(_model->index(row, rsObjectModel::MASS), Qt::EditRole).toDouble());
	(this->findChild<QComboBox *>("axis"))->setCurrentIndex(_model->data(_model->index(row, rsObjectModel::AXIS), Qt::EditRole).toInt());
	QColor color(_model->data(_model->index(row, rsObjectModel::COLOR), Qt::EditRole).toString());
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
 *	\param		model data model from objectEditor model.
 */
dotEditor::dotEditor(objectModel *model, QWidget *parent) : QWidget(parent) {
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
	hbox5->addStretch(1);
	layout->addLayout(hbox5);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(_colorPicker);
	layout->addLayout(hbox6);
	layout->addStretch(2);
	this->setLayout(layout);
}

void dotEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_X), value);
}

void dotEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Y), value);
}

void dotEditor::submitPZ(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Z), value);
}

void dotEditor::submitSize(double value) {
	_model->setData(_model->index(_row, rsObjectModel::SIZE), value);
}

void dotEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsObjectModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void dotEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsObjectModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("pz"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Z), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("size"))->setValue(_model->data(_model->index(row, rsObjectModel::SIZE), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsObjectModel::COLOR), Qt::EditRole).toString());
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
 *	Ellipse Editor
 *
 *
 */

/*!	\brief Ellipse Obstacle Editor.
 *
 *	Build individual ellipse editor with relevant pieces of information.
 *
 *	\param		model data model from objectEditor model.
 */
ellipseEditor::ellipseEditor(objectModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Ellipse Editor</span>"));

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
	pXBox->setToolTip("Set the X position of the ellipse");
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
	pYBox->setToolTip("Set the Y position of the ellipse");
	pYBox->setToolTipDuration(-1);

	// width
	QLabel *lXLabel = new QLabel(tr("Width:"));
	_lXUnits = new QLabel();
	QDoubleSpinBox *lXBox = new QDoubleSpinBox();
	lXBox->setObjectName("width");
	lXBox->setMinimum(-1000000);
	lXBox->setMaximum(1000000);
	lXBox->setSingleStep(0.5);
	lXLabel->setBuddy(lXBox);
	QWidget::connect(lXBox, SIGNAL(valueChanged(double)), this, SLOT(submitL1(double)));
	lXBox->setToolTip("Set the width of the ellipse");
	lXBox->setToolTipDuration(-1);

	// height
	QLabel *lYLabel = new QLabel(tr("Height:"));
	_lYUnits = new QLabel();
	QDoubleSpinBox *lYBox = new QDoubleSpinBox();
	lYBox->setObjectName("height");
	lYBox->setMinimum(-1000000);
	lYBox->setMaximum(1000000);
	lYBox->setSingleStep(0.5);
	lYLabel->setBuddy(lYBox);
	QWidget::connect(lYBox, SIGNAL(valueChanged(double)), this, SLOT(submitL2(double)));
	lYBox->setToolTip("Set the height of the ellipse");
	lYBox->setToolTipDuration(-1);

	// rotation psi
	QLabel *rZLabel = new QLabel(tr("Angle:"));
	QLabel *rZUnits = new QLabel(QString::fromUtf8("°"));
	_rZBox = new QDoubleSpinBox();
	_rZBox->setObjectName("angle");
	_rZBox->setMinimum(-360);
	_rZBox->setMaximum(360);
	_rZBox->setSingleStep(0.5);
	rZLabel->setBuddy(_rZBox);
	QWidget::connect(_rZBox, SIGNAL(valueChanged(double)), this, SLOT(submitRZ(double)));
	_rZBox->setToolTip("Set the rotation of the ellipse");
	_rZBox->setToolTipDuration(-1);

	// width
	QLabel *widthLabel = new QLabel(tr("Line Width:"));
	QDoubleSpinBox *widthBox = new QDoubleSpinBox();
	widthBox->setObjectName("size");
	widthBox->setMinimum(0);
	widthBox->setMaximum(100);
	widthBox->setSingleStep(1);
	widthLabel->setBuddy(widthBox);
	QWidget::connect(widthBox, SIGNAL(valueChanged(double)), this, SLOT(submitSize(double)));
	widthBox->setToolTip("Set the line width of the ellipse");
	widthBox->setToolTipDuration(-1);

	// color
	_colorPicker = new bodyColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the ellipse");
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
	hbox4->addWidget(rZLabel, 2, Qt::AlignRight);
	hbox4->addWidget(_rZBox, 5);
	hbox4->addWidget(rZUnits, 1, Qt::AlignLeft);
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

void ellipseEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_X), value);
}

void ellipseEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Y), value);
}

void ellipseEditor::submitL1(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_1), value);
}

void ellipseEditor::submitL2(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_2), value);
}

void ellipseEditor::submitRZ(double value) {
	_rZBox->setValue(value - static_cast<int>(value / 360) * 360);
	_model->setData(_model->index(_row, rsObjectModel::L_3), value);
}

void ellipseEditor::submitSize(double value) {
	_model->setData(_model->index(_row, rsObjectModel::SIZE), value);
}

void ellipseEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsObjectModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void ellipseEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsObjectModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("width"))->setValue(_model->data(_model->index(row, rsObjectModel::L_1), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("height"))->setValue(_model->data(_model->index(row, rsObjectModel::L_2), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("angle"))->setValue(_model->data(_model->index(row, rsObjectModel::L_3), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("size"))->setValue(_model->data(_model->index(row, rsObjectModel::SIZE), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsObjectModel::COLOR), Qt::EditRole).toString());
	(this->findChild<bodyColorPicker *>("color"))->setColor(color);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void ellipseEditor::setUnits(bool si) {
	QString text;
	if (si) text = tr("cm");
	else text = tr("in");
	_pXUnits->setText(text);
	_pYUnits->setText(text);
	_lXUnits->setText(text);
	_lYUnits->setText(text);
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
	QLabel *note1 = new QLabel(tr("<span style=\"font-size:9pt;\">There are no objects or drawings in the Scene.</span>"));
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
 *	Hacky Sack Editor
 *
 *
 */

/*!	\brief Hacky Sack Drawing Editor.
 *
 *	Build individual hackysack editor with relevant pieces of information.
 *
 *	\param		model data model from objectEditor model.
 */
hackysackEditor::hackysackEditor(objectModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Hacky Sack Editor</span>"));

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
	pXBox->setToolTip("Set the X position of the hacky sack");
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
	pYBox->setToolTip("Set the Y position of the hacky sack");
	pYBox->setToolTipDuration(-1);

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
	layout->addStretch(2);
	this->setLayout(layout);
}

void hackysackEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_X), value);
}

void hackysackEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Y), value);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void hackysackEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsObjectModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Y), Qt::EditRole).toDouble());
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void hackysackEditor::setUnits(bool si) {
	QString text;
	if (si) text = tr("cm");
	else text = tr("in");
	_pXUnits->setText(text);
	_pYUnits->setText(text);
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
 *	\param		model data model from objectEditor model.
 */
lineEditor::lineEditor(objectModel *model, QWidget *parent) : QWidget(parent) {
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
	QLabel *widthLabel = new QLabel(tr("Line Width:"));
	QDoubleSpinBox *widthBox = new QDoubleSpinBox();
	widthBox->setObjectName("size");
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
	hbox5->addStretch(1);
	layout->addLayout(hbox5);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(_colorPicker);
	layout->addLayout(hbox6);
	layout->addStretch(2);
	this->setLayout(layout);
}

void lineEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_X), value);
}

void lineEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Y), value);
}

void lineEditor::submitPZ(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Z), value);
}

void lineEditor::submitL1(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_1), value);
}

void lineEditor::submitL2(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_2), value);
}

void lineEditor::submitL3(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_3), value);
}

void lineEditor::submitSize(double value) {
	_model->setData(_model->index(_row, rsObjectModel::SIZE), value);
}

void lineEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsObjectModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void lineEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px1"))->setValue(_model->data(_model->index(row, rsObjectModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py1"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("pz1"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Z), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("px2"))->setValue(_model->data(_model->index(row, rsObjectModel::L_1), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py2"))->setValue(_model->data(_model->index(row, rsObjectModel::L_2), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("pz2"))->setValue(_model->data(_model->index(row, rsObjectModel::L_3), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("size"))->setValue(_model->data(_model->index(row, rsObjectModel::SIZE), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsObjectModel::COLOR), Qt::EditRole).toString());
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
 *	Polygon Editor
 *
 *
 */

/*!	\brief Polygon Obstacle Editor.
 *
 *	Build individual polygon editor with relevant pieces of information.
 *
 *	\param		model data model from objectEditor model.
 */
polygonEditor::polygonEditor(objectModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Polygon Editor</span>"));

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
	pXBox->setToolTip("Set the X position of the circle");
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
	pYBox->setToolTip("Set the Y position of the circle");
	pYBox->setToolTipDuration(-1);

	// position z
	QLabel *pZLabel = new QLabel(tr("Length:"));
	_pZUnits = new QLabel();
	QDoubleSpinBox *pZBox = new QDoubleSpinBox();
	pZBox->setObjectName("length");
	pZBox->setMinimum(-1000000);
	pZBox->setMaximum(1000000);
	pZBox->setSingleStep(0.5);
	pZLabel->setBuddy(pZBox);
	QWidget::connect(pZBox, SIGNAL(valueChanged(double)), this, SLOT(submitPZ(double)));
	pZBox->setToolTip("Set the length of the polygon sides");
	pZBox->setToolTipDuration(-1);

	// position x2
	QLabel *lXLabel = new QLabel(tr("Number:"));
	_lXUnits = new QLabel();
	QDoubleSpinBox *lXBox = new QDoubleSpinBox();
	lXBox->setObjectName("n");
	lXBox->setMinimum(-1000000);
	lXBox->setMaximum(1000000);
	lXBox->setSingleStep(0.5);
	lXLabel->setBuddy(lXBox);
	QWidget::connect(lXBox, SIGNAL(valueChanged(double)), this, SLOT(submitL1(double)));
	lXBox->setToolTip("Set the number of polygon sides");
	lXBox->setToolTipDuration(-1);

	// width
	QLabel *widthLabel = new QLabel(tr("Line Width:"));
	QDoubleSpinBox *widthBox = new QDoubleSpinBox();
	widthBox->setObjectName("size");
	widthBox->setMinimum(0);
	widthBox->setMaximum(100);
	widthBox->setSingleStep(1);
	widthLabel->setBuddy(widthBox);
	QWidget::connect(widthBox, SIGNAL(valueChanged(double)), this, SLOT(submitSize(double)));
	widthBox->setToolTip("Set the line width of the polygon");
	widthBox->setToolTipDuration(-1);

	// color
	_colorPicker = new bodyColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the polygon");
	_colorPicker->setToolTipDuration(-1);

	// lay out grid
	QVBoxLayout *layout = new QVBoxLayout(this);
	QHBoxLayout *hbox0 = new QHBoxLayout();
	hbox0->addWidget(title, 5, Qt::AlignHCenter);
	layout->addLayout(hbox0);
	layout->addStretch(1);
	QHBoxLayout *hbox1 = new QHBoxLayout();
	hbox1->addWidget(pXLabel, 2, Qt::AlignRight);
	hbox1->addWidget(pXBox, 5);
	hbox1->addWidget(_pXUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox1);
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
	hbox5->addWidget(lXLabel, 2, Qt::AlignRight);
	hbox5->addWidget(lXBox, 5);
	hbox5->addWidget(_lXUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox5);
	QHBoxLayout *hbox7 = new QHBoxLayout();
	hbox7->addWidget(widthLabel, 2, Qt::AlignRight);
	hbox7->addWidget(widthBox, 5);
	layout->addLayout(hbox7);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(_colorPicker);
	layout->addLayout(hbox6);
	layout->addStretch(2);
	this->setLayout(layout);
}

void polygonEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_X), value);
}

void polygonEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Y), value);
}

void polygonEditor::submitPZ(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Z), value);
}

void polygonEditor::submitL1(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_1), value);
}

void polygonEditor::submitSize(double value) {
	_model->setData(_model->index(_row, rsObjectModel::SIZE), value);
}

void polygonEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsObjectModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void polygonEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsObjectModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("length"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Z), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("n"))->setValue(_model->data(_model->index(row, rsObjectModel::L_1), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("size"))->setValue(_model->data(_model->index(row, rsObjectModel::SIZE), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsObjectModel::COLOR), Qt::EditRole).toString());
	(this->findChild<bodyColorPicker *>("color"))->setColor(color);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void polygonEditor::setUnits(bool si) {
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
 *	Pullup Bar Editor
 *
 *
 */

/*!	\brief Pullup Bar Drawing Editor.
 *
 *	Build individual pullup bar editor with relevant pieces of information.
 *
 *	\param		model data model from objectEditor model.
 */
pullupbarEditor::pullupbarEditor(objectModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Pullup Bar Editor</span>"));

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
	pXBox->setToolTip("Set the X position of the hacky sack");
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
	pYBox->setToolTip("Set the Y position of the hacky sack");
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
	hbox4->addWidget(rZLabel, 2, Qt::AlignRight);
	hbox4->addWidget(_rZBox, 5);
	hbox4->addWidget(rZUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox4);
	layout->addStretch(2);
	this->setLayout(layout);
}

void pullupbarEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_X), value);
}

void pullupbarEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Y), value);
}

void pullupbarEditor::submitRZ(double value) {
	_rZBox->setValue(value - static_cast<int>(value / 360) * 360);
	_model->setData(_model->index(_row, rsObjectModel::R_PSI), value);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void pullupbarEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsObjectModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("rz"))->setValue(_model->data(_model->index(row, rsObjectModel::R_PSI), Qt::EditRole).toDouble());
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void pullupbarEditor::setUnits(bool si) {
	QString text;
	if (si) text = tr("cm");
	else text = tr("in");
	_pXUnits->setText(text);
	_pYUnits->setText(text);
}

/*!
 *
 *
 *	Quad Editor
 *
 *
 */

/*!	\brief Quad Drawing Editor.
 *
 *	Build individual quad editor with relevant pieces of information.
 *
 *	\param		model data model from objectEditor model.
 */
quadEditor::quadEditor(objectModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Quad Editor</span>"));

	// position x1
	QLabel *pXLabel = new QLabel(tr("X:"));
	_pXUnits = new QLabel();
	QDoubleSpinBox *pXBox = new QDoubleSpinBox();
	pXBox->setObjectName("px1");
	pXBox->setMinimum(-1000000);
	pXBox->setMaximum(1000000);
	pXBox->setSingleStep(0.5);
	pXLabel->setBuddy(pXBox);
	QWidget::connect(pXBox, SIGNAL(valueChanged(double)), this, SLOT(submitPX(double)));
	pXBox->setToolTip("Set the starting X position of the quad");
	pXBox->setToolTipDuration(-1);

	// position y1
	QLabel *pYLabel = new QLabel(tr("X:"));
	_pYUnits = new QLabel();
	QDoubleSpinBox *pYBox = new QDoubleSpinBox();
	pYBox->setObjectName("py1");
	pYBox->setMinimum(-1000000);
	pYBox->setMaximum(1000000);
	pYBox->setSingleStep(0.5);
	pYLabel->setBuddy(pYBox);
	QWidget::connect(pYBox, SIGNAL(valueChanged(double)), this, SLOT(submitPY(double)));
	pXBox->setToolTip("Set the starting Y position of the quad");
	pXBox->setToolTipDuration(-1);

	// position z1
	QLabel *pZLabel = new QLabel(tr("X:"));
	_pZUnits = new QLabel();
	QDoubleSpinBox *pZBox = new QDoubleSpinBox();
	pZBox->setObjectName("pz1");
	pZBox->setMinimum(-1000000);
	pZBox->setMaximum(1000000);
	pZBox->setSingleStep(0.5);
	pZLabel->setBuddy(pZBox);
	QWidget::connect(pZBox, SIGNAL(valueChanged(double)), this, SLOT(submitPZ(double)));
	pXBox->setToolTip("Set the starting Z position of the quad");
	pXBox->setToolTipDuration(-1);

	// position x2
	QLabel *lXLabel = new QLabel(tr("Y:"));
	_lXUnits = new QLabel();
	QDoubleSpinBox *lXBox = new QDoubleSpinBox();
	lXBox->setObjectName("px2");
	lXBox->setMinimum(-1000000);
	lXBox->setMaximum(1000000);
	lXBox->setSingleStep(0.5);
	lXLabel->setBuddy(lXBox);
	QWidget::connect(lXBox, SIGNAL(valueChanged(double)), this, SLOT(submitL1(double)));
	pXBox->setToolTip("Set the ending X position of the quad");
	pXBox->setToolTipDuration(-1);

	// position y2
	QLabel *lYLabel = new QLabel(tr("Y:"));
	_lYUnits = new QLabel();
	QDoubleSpinBox *lYBox = new QDoubleSpinBox();
	lYBox->setObjectName("py2");
	lYBox->setMinimum(-1000000);
	lYBox->setMaximum(1000000);
	lYBox->setSingleStep(0.5);
	lYLabel->setBuddy(lYBox);
	QWidget::connect(lYBox, SIGNAL(valueChanged(double)), this, SLOT(submitL2(double)));
	pXBox->setToolTip("Set the ending Y position of the quad");
	pXBox->setToolTipDuration(-1);

	// position z2
	QLabel *lZLabel = new QLabel(tr("Y:"));
	_lZUnits = new QLabel();
	QDoubleSpinBox *lZBox = new QDoubleSpinBox();
	lZBox->setObjectName("pz2");
	lZBox->setMinimum(-1000000);
	lZBox->setMaximum(1000000);
	lZBox->setSingleStep(0.5);
	lZLabel->setBuddy(lZBox);
	QWidget::connect(lZBox, SIGNAL(valueChanged(double)), this, SLOT(submitL3(double)));
	pXBox->setToolTip("Set the ending Z position of the quad");
	pXBox->setToolTipDuration(-1);

	// position y1
	QLabel *rXLabel = new QLabel(tr("X:"));
	_rXUnits = new QLabel();
	QDoubleSpinBox *rXBox = new QDoubleSpinBox();
	rXBox->setObjectName("rx");
	rXBox->setMinimum(-1000000);
	rXBox->setMaximum(1000000);
	rXBox->setSingleStep(0.5);
	rXLabel->setBuddy(rXBox);
	QWidget::connect(rXBox, SIGNAL(valueChanged(double)), this, SLOT(submitRX(double)));
	rXBox->setToolTip("Set the starting Y position of the quad");
	rXBox->setToolTipDuration(-1);

	// position z1
	QLabel *rYLabel = new QLabel(tr("Y:"));
	_rYUnits = new QLabel();
	QDoubleSpinBox *rYBox = new QDoubleSpinBox();
	rYBox->setObjectName("ry");
	rYBox->setMinimum(-1000000);
	rYBox->setMaximum(1000000);
	rYBox->setSingleStep(0.5);
	rYLabel->setBuddy(rYBox);
	QWidget::connect(rYBox, SIGNAL(valueChanged(double)), this, SLOT(submitRY(double)));
	rYBox->setToolTip("Set the starting Z position of the quad");
	rYBox->setToolTipDuration(-1);

	// width
	QLabel *widthLabel = new QLabel(tr("Line Width:"));
	QDoubleSpinBox *widthBox = new QDoubleSpinBox();
	widthBox->setObjectName("size");
	widthBox->setMinimum(0);
	widthBox->setMaximum(100);
	widthBox->setSingleStep(1);
	widthLabel->setBuddy(widthBox);
	QWidget::connect(widthBox, SIGNAL(valueChanged(double)), this, SLOT(submitSize(double)));
	widthBox->setToolTip("Set the line width of the quad");
	widthBox->setToolTipDuration(-1);

	// color
	_colorPicker = new bodyColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the quad");
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
	QHBoxLayout *hbox7 = new QHBoxLayout();
	hbox7->addWidget(rXLabel, 2, Qt::AlignRight);
	hbox7->addWidget(rXBox, 5);
	hbox7->addWidget(_rXUnits, 1, Qt::AlignLeft);
	hbox7->addWidget(rYLabel, 2, Qt::AlignRight);
	hbox7->addWidget(rYBox, 5);
	hbox7->addWidget(_rYUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox7);
	QHBoxLayout *hbox5 = new QHBoxLayout();
	hbox5->addWidget(widthLabel, 2, Qt::AlignRight);
	hbox5->addWidget(widthBox, 5);
	hbox5->addStretch(1);
	layout->addLayout(hbox5);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(_colorPicker);
	layout->addLayout(hbox6);
	layout->addStretch(2);
	this->setLayout(layout);
}

void quadEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_X), value);
}

void quadEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Y), value);
}

void quadEditor::submitPZ(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Z), value);
}

void quadEditor::submitRX(double value) {
	_model->setData(_model->index(_row, rsObjectModel::R_PHI), value);
}

void quadEditor::submitRY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::R_THETA), value);
}

void quadEditor::submitL1(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_1), value);
}

void quadEditor::submitL2(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_2), value);
}

void quadEditor::submitL3(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_3), value);
}

void quadEditor::submitSize(double value) {
	_model->setData(_model->index(_row, rsObjectModel::SIZE), value);
}

void quadEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsObjectModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void quadEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px1"))->setValue(_model->data(_model->index(row, rsObjectModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py1"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("pz1"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Z), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("rx"))->setValue(_model->data(_model->index(row, rsObjectModel::R_PHI), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("ry"))->setValue(_model->data(_model->index(row, rsObjectModel::R_THETA), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("px2"))->setValue(_model->data(_model->index(row, rsObjectModel::L_1), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py2"))->setValue(_model->data(_model->index(row, rsObjectModel::L_2), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("pz2"))->setValue(_model->data(_model->index(row, rsObjectModel::L_3), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("size"))->setValue(_model->data(_model->index(row, rsObjectModel::SIZE), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsObjectModel::COLOR), Qt::EditRole).toString());
	(this->findChild<bodyColorPicker *>("color"))->setColor(color);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void quadEditor::setUnits(bool si) {
	QString text;
	if (si) text = tr("cm");
	else text = tr("in");
	_pXUnits->setText(text);
	_pYUnits->setText(text);
	_pZUnits->setText(text);
	_rXUnits->setText(text);
	_rYUnits->setText(text);
	_lXUnits->setText(text);
	_lYUnits->setText(text);
	_lZUnits->setText(text);
}

/*!
 *
 *
 *	Rectangle Editor
 *
 *
 */

/*!	\brief Rectangle Drawing Editor.
 *
 *	Build individual rectangle editor with relevant pieces of information.
 *
 *	\param		model data model from objectEditor model.
 */
rectangleEditor::rectangleEditor(objectModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Rectangle Editor</span>"));

	// position x1
	QLabel *pXLabel = new QLabel(tr("X:"));
	_pXUnits = new QLabel();
	QDoubleSpinBox *pXBox = new QDoubleSpinBox();
	pXBox->setObjectName("px");
	pXBox->setMinimum(-1000000);
	pXBox->setMaximum(1000000);
	pXBox->setSingleStep(0.5);
	pXLabel->setBuddy(pXBox);
	QWidget::connect(pXBox, SIGNAL(valueChanged(double)), this, SLOT(submitPX(double)));
	pXBox->setToolTip("Set the X position of the rectangle");
	pXBox->setToolTipDuration(-1);

	// position y1
	QLabel *pYLabel = new QLabel(tr("Y:"));
	_pYUnits = new QLabel();
	QDoubleSpinBox *pYBox = new QDoubleSpinBox();
	pYBox->setObjectName("py");
	pYBox->setMinimum(-1000000);
	pYBox->setMaximum(1000000);
	pYBox->setSingleStep(0.5);
	pYLabel->setBuddy(pYBox);
	QWidget::connect(pYBox, SIGNAL(valueChanged(double)), this, SLOT(submitPY(double)));
	pXBox->setToolTip("Set the Y position of the rectangle");
	pXBox->setToolTipDuration(-1);

	// width
	QLabel *lXLabel = new QLabel(tr("Width:"));
	_lXUnits = new QLabel();
	QDoubleSpinBox *lXBox = new QDoubleSpinBox();
	lXBox->setObjectName("width");
	lXBox->setMinimum(-1000000);
	lXBox->setMaximum(1000000);
	lXBox->setSingleStep(0.5);
	lXLabel->setBuddy(lXBox);
	QWidget::connect(lXBox, SIGNAL(valueChanged(double)), this, SLOT(submitL1(double)));
	lXBox->setToolTip("Set the width of the rectangle");
	lXBox->setToolTipDuration(-1);

	// height
	QLabel *lYLabel = new QLabel(tr("Height:"));
	_lYUnits = new QLabel();
	QDoubleSpinBox *lYBox = new QDoubleSpinBox();
	lYBox->setObjectName("height");
	lYBox->setMinimum(-1000000);
	lYBox->setMaximum(1000000);
	lYBox->setSingleStep(0.5);
	lYLabel->setBuddy(lYBox);
	QWidget::connect(lYBox, SIGNAL(valueChanged(double)), this, SLOT(submitL2(double)));
	lYBox->setToolTip("Set the height of the rectangle");
	lYBox->setToolTipDuration(-1);

	// size
	QLabel *widthLabel = new QLabel(tr("Line Width:"));
	QDoubleSpinBox *widthBox = new QDoubleSpinBox();
	widthBox->setObjectName("size");
	widthBox->setMinimum(0);
	widthBox->setMaximum(100);
	widthBox->setSingleStep(1);
	widthLabel->setBuddy(widthBox);
	QWidget::connect(widthBox, SIGNAL(valueChanged(double)), this, SLOT(submitSize(double)));
	widthBox->setToolTip("Set the line width of the rectangle");
	widthBox->setToolTipDuration(-1);

	// color
	_colorPicker = new bodyColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the rectangle");
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
	QHBoxLayout *hbox5 = new QHBoxLayout();
	hbox5->addWidget(widthLabel, 2, Qt::AlignRight);
	hbox5->addWidget(widthBox, 5);
	hbox5->addStretch(1);
	layout->addLayout(hbox5);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(_colorPicker);
	layout->addLayout(hbox6);
	layout->addStretch(2);
	this->setLayout(layout);
}

void rectangleEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_X), value);
}

void rectangleEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Y), value);
}

void rectangleEditor::submitL1(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_1), value);
}

void rectangleEditor::submitL2(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_2), value);
}

void rectangleEditor::submitSize(double value) {
	_model->setData(_model->index(_row, rsObjectModel::SIZE), value);
}

void rectangleEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsObjectModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void rectangleEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsObjectModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("width"))->setValue(_model->data(_model->index(row, rsObjectModel::L_1), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("height"))->setValue(_model->data(_model->index(row, rsObjectModel::L_2), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("size"))->setValue(_model->data(_model->index(row, rsObjectModel::SIZE), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsObjectModel::COLOR), Qt::EditRole).toString());
	(this->findChild<bodyColorPicker *>("color"))->setColor(color);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void rectangleEditor::setUnits(bool si) {
	QString text;
	if (si) text = tr("cm");
	else text = tr("in");
	_pXUnits->setText(text);
	_pYUnits->setText(text);
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
 *	\param		model data model from objectEditor model.
 */
sphereEditor::sphereEditor(objectModel *model, QWidget *parent) : QWidget(parent) {
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
	QHBoxLayout *hbox1 = new QHBoxLayout();
	hbox1->addWidget(pXLabel, 2, Qt::AlignRight);
	hbox1->addWidget(pXBox, 5);
	hbox1->addWidget(_pXUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox1);
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
	QHBoxLayout *hbox2 = new QHBoxLayout();
	hbox2->addWidget(lXLabel, 2, Qt::AlignRight);
	hbox2->addWidget(lXBox, 5);
	hbox2->addWidget(_lXUnits, 1, Qt::AlignLeft);
	layout->addLayout(hbox2);
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
	_model->setData(_model->index(_row, rsObjectModel::P_X), value);
}

void sphereEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Y), value);
}

void sphereEditor::submitPZ(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Z), value);
}

void sphereEditor::submitL1(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_1), value);
}

void sphereEditor::submitMass(double value) {
	_model->setData(_model->index(_row, rsObjectModel::MASS), value);
}

void sphereEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsObjectModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void sphereEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsObjectModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("pz"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Z), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("radius"))->setValue(_model->data(_model->index(row, rsObjectModel::L_1), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("mass"))->setValue(_model->data(_model->index(row, rsObjectModel::MASS), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsObjectModel::COLOR), Qt::EditRole).toString());
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
 *	\param		model data model from objectEditor model.
 */
textEditor::textEditor(objectModel *model, QWidget *parent) : QWidget(parent) {
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
	sizeBox->setMaximum(10000);
	sizeBox->setSingleStep(1);
	sizeLabel->setBuddy(sizeBox);
	QWidget::connect(sizeBox, SIGNAL(valueChanged(double)), this, SLOT(submitSize(double)));
	sizeBox->setToolTip("Set the size of the text");
	sizeBox->setToolTipDuration(-1);

	// color
	_colorPicker = new bodyColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the text");
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
	hbox5->addStretch(1);
	layout->addLayout(hbox5);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(_colorPicker);
	layout->addLayout(hbox6);
	layout->addStretch(2);
	this->setLayout(layout);
}

void textEditor::submitName(QString value) {
	_model->setData(_model->index(_row, rsObjectModel::TEXT), value);
}

void textEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_X), value);
}

void textEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Y), value);
}

void textEditor::submitPZ(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Z), value);
}

void textEditor::submitSize(double value) {
	_model->setData(_model->index(_row, rsObjectModel::SIZE), value);
}

void textEditor::submitColor(QColor value) {
	_model->setData(_model->index(_row, rsObjectModel::COLOR), value);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void textEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QLineEdit *>("name"))->setText(_model->data(_model->index(row, rsObjectModel::TEXT), Qt::EditRole).toString());
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsObjectModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("pz"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Z), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("size"))->setValue(_model->data(_model->index(row, rsObjectModel::SIZE), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsObjectModel::COLOR), Qt::EditRole).toString());
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
 *	Triangle Editor
 *
 *
 */

/*!	\brief Triangle Drawing Editor.
 *
 *	Build individual triangle editor with relevant pieces of information.
 *
 *	\param		model data model from objectEditor model.
 */
triangleEditor::triangleEditor(objectModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Triangle Editor</span>"));

	// position x1
	QLabel *pXLabel = new QLabel(tr("X1:"));
	_pXUnits = new QLabel();
	QDoubleSpinBox *pXBox = new QDoubleSpinBox();
	pXBox->setObjectName("x1");
	pXBox->setMinimum(-1000000);
	pXBox->setMaximum(1000000);
	pXBox->setSingleStep(0.5);
	pXLabel->setBuddy(pXBox);
	QWidget::connect(pXBox, SIGNAL(valueChanged(double)), this, SLOT(submitPX(double)));
	pXBox->setToolTip("Set the X1 position of the triangle");
	pXBox->setToolTipDuration(-1);

	// position y1
	QLabel *pYLabel = new QLabel(tr("X2:"));
	_pYUnits = new QLabel();
	QDoubleSpinBox *pYBox = new QDoubleSpinBox();
	pYBox->setObjectName("x2");
	pYBox->setMinimum(-1000000);
	pYBox->setMaximum(1000000);
	pYBox->setSingleStep(0.5);
	pYLabel->setBuddy(pYBox);
	QWidget::connect(pYBox, SIGNAL(valueChanged(double)), this, SLOT(submitPY(double)));
	pYBox->setToolTip("Set the X2 position of the triangle");
	pYBox->setToolTipDuration(-1);

	// position z1
	QLabel *pZLabel = new QLabel(tr("X3:"));
	_pZUnits = new QLabel();
	QDoubleSpinBox *pZBox = new QDoubleSpinBox();
	pZBox->setObjectName("x3");
	pZBox->setMinimum(-1000000);
	pZBox->setMaximum(1000000);
	pZBox->setSingleStep(0.5);
	pZLabel->setBuddy(pZBox);
	QWidget::connect(pZBox, SIGNAL(valueChanged(double)), this, SLOT(submitPZ(double)));
	pZBox->setToolTip("Set the X3 position of the triangle");
	pZBox->setToolTipDuration(-1);

	// position x2
	QLabel *lXLabel = new QLabel(tr("Y1:"));
	_lXUnits = new QLabel();
	QDoubleSpinBox *lXBox = new QDoubleSpinBox();
	lXBox->setObjectName("y1");
	lXBox->setMinimum(-1000000);
	lXBox->setMaximum(1000000);
	lXBox->setSingleStep(0.5);
	lXLabel->setBuddy(lXBox);
	QWidget::connect(lXBox, SIGNAL(valueChanged(double)), this, SLOT(submitL1(double)));
	lXBox->setToolTip("Set the Y1 position of the triangle");
	lXBox->setToolTipDuration(-1);

	// position y2
	QLabel *lYLabel = new QLabel(tr("Y2:"));
	_lYUnits = new QLabel();
	QDoubleSpinBox *lYBox = new QDoubleSpinBox();
	lYBox->setObjectName("y2");
	lYBox->setMinimum(-1000000);
	lYBox->setMaximum(1000000);
	lYBox->setSingleStep(0.5);
	lYLabel->setBuddy(lYBox);
	QWidget::connect(lYBox, SIGNAL(valueChanged(double)), this, SLOT(submitL2(double)));
	lYBox->setToolTip("Set the Y2 position of the triangle");
	lYBox->setToolTipDuration(-1);

	// position z2
	QLabel *lZLabel = new QLabel(tr("Y3:"));
	_lZUnits = new QLabel();
	QDoubleSpinBox *lZBox = new QDoubleSpinBox();
	lZBox->setObjectName("y3");
	lZBox->setMinimum(-1000000);
	lZBox->setMaximum(1000000);
	lZBox->setSingleStep(0.5);
	lZLabel->setBuddy(lZBox);
	QWidget::connect(lZBox, SIGNAL(valueChanged(double)), this, SLOT(submitL3(double)));
	lZBox->setToolTip("Set the Y3 position of the triangle");
	lZBox->setToolTipDuration(-1);

	// size
	QLabel *widthLabel = new QLabel(tr("Line Width:"));
	QDoubleSpinBox *widthBox = new QDoubleSpinBox();
	widthBox->setObjectName("size");
	widthBox->setMinimum(0);
	widthBox->setMaximum(100);
	widthBox->setSingleStep(1);
	widthLabel->setBuddy(widthBox);
	QWidget::connect(widthBox, SIGNAL(valueChanged(double)), this, SLOT(submitSize(double)));
	widthBox->setToolTip("Set the line width of the triangle");
	widthBox->setToolTipDuration(-1);

	// color
	_colorPicker = new bodyColorPicker();
	_colorPicker->setObjectName("color");
	QWidget::connect(_colorPicker, SIGNAL(colorChanged(QColor)), this, SLOT(submitColor(QColor)));
	_colorPicker->setToolTip("Choose the color of the triangle");
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
	hbox5->addStretch(1);
	layout->addLayout(hbox5);
	QHBoxLayout *hbox6 = new QHBoxLayout();
	hbox6->addWidget(_colorPicker);
	layout->addLayout(hbox6);
	layout->addStretch(2);
	this->setLayout(layout);
}

void triangleEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_X), value);
}

void triangleEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Y), value);
}

void triangleEditor::submitPZ(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Z), value);
}

void triangleEditor::submitL1(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_1), value);
}

void triangleEditor::submitL2(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_2), value);
}

void triangleEditor::submitL3(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_3), value);
}

void triangleEditor::submitSize(double value) {
	_model->setData(_model->index(_row, rsObjectModel::SIZE), value);
}

void triangleEditor::submitColor(QColor color) {
	_model->setData(_model->index(_row, rsObjectModel::COLOR), color);
}

/*!	\brief Slot to nullify all inputs.
 *
 *	\param		nullify To nullify inputs or not.
 */
void triangleEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("x1"))->setValue(_model->data(_model->index(row, rsObjectModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("x2"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("x3"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Z), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("y1"))->setValue(_model->data(_model->index(row, rsObjectModel::L_1), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("y2"))->setValue(_model->data(_model->index(row, rsObjectModel::L_2), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("y3"))->setValue(_model->data(_model->index(row, rsObjectModel::L_3), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("size"))->setValue(_model->data(_model->index(row, rsObjectModel::SIZE), Qt::EditRole).toDouble());
	QColor color(_model->data(_model->index(row, rsObjectModel::COLOR), Qt::EditRole).toString());
	(this->findChild<bodyColorPicker *>("color"))->setColor(color);
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void triangleEditor::setUnits(bool si) {
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
 *	Wood Block Editor
 *
 *
 */

/*!	\brief Box Obstacle Editor.
 *
 *	Build individual box editor with relevant pieces of information.
 *
 *	\param		model data model from objectEditor model.
 */
woodblockEditor::woodblockEditor(objectModel *model, QWidget *parent) : QWidget(parent) {
	// save model
	_model = model;

	// set title
	QLabel *title = new QLabel(tr("<span style=\" font-size: 10pt; font-weight:bold;\">Wood Block Editor</span>"));

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
	pXBox->setToolTip("Set the X position of the block");
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
	pYBox->setToolTip("Set the Y position of the block");
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
	pZBox->setToolTip("Set the Z position of the block");
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
	lXBox->setToolTip("Set the X length of the block");
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
	lYBox->setToolTip("Set the Y length of the block");
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
	lZBox->setToolTip("Set the Z length of the block");
	lZBox->setToolTipDuration(-1);

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
	layout->addStretch(2);
	this->setLayout(layout);
}

void woodblockEditor::submitPX(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_X), value);
}

void woodblockEditor::submitPY(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Y), value);
}

void woodblockEditor::submitPZ(double value) {
	_model->setData(_model->index(_row, rsObjectModel::P_Z), value);
}

void woodblockEditor::submitL1(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_1), value);
}

void woodblockEditor::submitL2(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_2), value);
}

void woodblockEditor::submitL3(double value) {
	_model->setData(_model->index(_row, rsObjectModel::L_3), value);
}

/*!	\brief Slot to re-enable all inputs.
 *
 *	\param		row Row of the model to load.
 */
void woodblockEditor::setIndex(int row) {
	_row = row;
	(this->findChild<QDoubleSpinBox *>("px"))->setValue(_model->data(_model->index(row, rsObjectModel::P_X), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("py"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Y), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("pz"))->setValue(_model->data(_model->index(row, rsObjectModel::P_Z), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("lx"))->setValue(_model->data(_model->index(row, rsObjectModel::L_1), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("ly"))->setValue(_model->data(_model->index(row, rsObjectModel::L_2), Qt::EditRole).toDouble());
	(this->findChild<QDoubleSpinBox *>("lz"))->setValue(_model->data(_model->index(row, rsObjectModel::L_3), Qt::EditRole).toDouble());
}

/*!	\brief Slot to set units labels.
 *
 *	\param		si Units are SI (true) or US (false).
 */
void woodblockEditor::setUnits(bool si) {
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
 *	bodyColorPicker
 *
 *
 */
bodyColorPicker::bodyColorPicker(QString string, QWidget *parent) : QWidget(parent) {
	QHBoxLayout *hbox = new QHBoxLayout(this);

	QLabel *label = new QLabel(string.append(":"));
	hbox->addWidget(label, 2, Qt::AlignRight);

	_color = QColor(0, 255, 0);

	_button = new QPushButton(this);
	_button->setObjectName("colorbutton");
	_button->setStyleSheet(QString("border-style: outset;border-width: 2px;border-color: black;background-color: rgba(%1,%2,%3,%4)").arg(_color.red()).arg(_color.green()).arg(_color.blue()).arg(_color.alpha()));
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
	_button->setStyleSheet(QString("border-style: outset;border-width: 2px;border-color: black;background-color: rgba(%1,%2,%3,%4)").arg(_color.red()).arg(_color.green()).arg(_color.blue()).arg(_color.alpha()));
	emit colorChanged(color);
}

void bodyColorPicker::onButtonClicked(void) {
	const QColor color = QColorDialog::getColor(_color, this, QString(), QColorDialog::ShowAlphaChannel);
	if (!color.isValid())
		return;

	this->setColor(color);
}

