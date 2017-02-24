#include <algorithm>
#include <iostream>
#include <cmath>

#include "objectmodel.h"

using namespace rsObjectModel;

objectModel::objectModel(QObject *parent) : QAbstractTableModel(parent) {
	// set US units
	_units = false;
}

objectModel::~objectModel(void) {
}

bool objectModel::addObject(int form, int role) {
	if (role == Qt::EditRole) {
		// add row
		int row = _list.size();
		this->insertRows(row, 1);

		// new object data
		_list[row][ID] = QVariant((row) ? this->data(createIndex(row-1, ID), Qt::EditRole).toInt() + 1 : 0).toString();
		_list[row][FORM] = QVariant(form).toString();
		if (_units)
			_list[row][P_X] = QVariant((row) ? this->data(createIndex(row-1, P_X)).toDouble() + 0.2 : 0).toString();	// offset by 20 cm
		else
			_list[row][P_X] = QVariant((row) ? this->data(createIndex(row-1, P_X)).toDouble() + 0.1524 : 0).toString();	// offset by 6 in
		_list[row][P_Y] = QVariant(0).toString();
		_list[row][P_Z] = QVariant(0).toString();
		_list[row][R_PSI] = QVariant(0).toString();
		switch (form) {
			case rs::HackySack:
				_list[row][P_Z] = QVariant(0.0275).toString();
				break;
			case rs::PullupBar:
				_list[row][P_Z] = QVariant(0.056585).toString();
				break;
			case rs::WoodBlock:
				_list[row][P_Z] = QVariant(0.0127).toString();
				break;
		}
		_list[row][L_1] = QVariant(0.0254).toString();	// 1 inch
		_list[row][L_2] = QVariant(0.0254).toString();	// 1 inch
		_list[row][L_3] = QVariant(0.0254).toString();	// 1 inch
		switch (form) {
			case rs::CompetitionBorder:
				_list[row][L_1] = QVariant(2.4384).toString();	// 8 feet
				_list[row][L_2] = QVariant(1.2192).toString();	// 4 feet
				_list[row][L_3] = QVariant(0.0191).toString();	// 0.75 inch
				break;
			case rs::HackySack:
				_list[row][L_1] = QVariant(0.055).toString();
				_list[row][L_2] = QVariant(0.055).toString();
				_list[row][L_3] = QVariant(0.055).toString();
				break;
		}
		// match object default color to icon
		switch (form) {
			case rs::Box:
			case rs::Cylinder:
			case rs::Sphere:
				_list[row][COLOR] = QString("#0000ff");	// blue
				break;
			case rs::Arc:
			case rs::ArcSector:
			case rs::ArcSegment:
			case rs::Arrow:
			case rs::Circle:
			case rs::Dot:
			case rs::Ellipse:
			case rs::Line:
			case rs::Polygon:
			case rs::Quad:
			case rs::Rectangle:
			case rs::Text:
			case rs::Triangle:
				_list[row][COLOR] = QString("#ff0000");	// red
				break;
			case rs::CompetitionBorder:
			case rs::PullupBar:
				_list[row][COLOR] = QString("#ffffff");	// white
				break;
		}
		_list[row][MASS] = QVariant(0.453593).toString();					// 1 lb in kg
		_list[row][rsObjectModel::SIZE] = QVariant(5).toString();			// f*$k microsoft
		if (form == rs::Text) {
			_list[row][rsObjectModel::SIZE] = QVariant(35).toString();		// f*$k microsoft
		}
		_list[row][AXIS] = QVariant(2).toString();
		_list[row][TEXT] = QString("HI");

		// set defaults for all drawings
		switch (form) {
			case rs::Arc:
			case rs::ArcSector:
			case rs::ArcSegment:
				_list[row][P_Z] = QVariant(rs::IN2M(6)).toString();
				_list[row][R_PHI] = QVariant(0).toString();
				_list[row][R_THETA] = QVariant(135).toString();
				break;
			case rs::Arrow:
				_list[row][L_1] = QVariant(rs::IN2M(6)).toString();
				_list[row][L_2] = QVariant(rs::IN2M(6)).toString();
				break;
			case rs::Circle:
				_list[row][P_Z] = QVariant(rs::IN2M(6)).toString();
				break;
			case rs::Ellipse:
				_list[row][L_1] = QVariant(rs::IN2M(12)).toString();
				_list[row][L_2] = QVariant(rs::IN2M(6)).toString();
				_list[row][L_3] = QVariant(0).toString();
				break;
			case rs::Line:
				_list[row][P_X] = QVariant(0).toString();
				_list[row][P_Y] = QVariant(0).toString();
				_list[row][P_Z] = QVariant(0).toString();
				_list[row][L_1] = QVariant(rs::IN2M(6)).toString();
				_list[row][L_2] = QVariant(rs::IN2M(6)).toString();
				_list[row][L_3] = QVariant(0).toString();
				break;
			case rs::Polygon:
				_list[row][P_Z] = QVariant(rs::IN2M(6)).toString();
				_list[row][L_1] = QVariant(rs::IN2M(6)).toString();
				break;
			case rs::Quad:
				_list[row][P_X] = QVariant(0).toString();
				_list[row][P_Y] = QVariant(rs::IN2M(6)).toString();
				_list[row][P_Z] = QVariant(rs::IN2M(6)).toString();
				_list[row][L_1] = QVariant(0).toString();
				_list[row][L_2] = QVariant(0).toString();
				_list[row][L_3] = QVariant(rs::IN2M(12)).toString();
				_list[row][R_PHI] = QVariant(0).toString();
				_list[row][R_THETA] = QVariant(6).toString();
				break;
			case rs::Rectangle:
				_list[row][L_1] = QVariant(rs::IN2M(12)).toString();
				_list[row][L_2] = QVariant(rs::IN2M(6)).toString();
				break;
			case rs::Triangle:
				_list[row][P_X] = QVariant(0).toString();
				_list[row][P_Y] = QVariant(rs::IN2M(6)).toString();
				_list[row][P_Z] = QVariant(rs::IN2M(6)).toString();
				_list[row][L_1] = QVariant(0).toString();
				_list[row][L_2] = QVariant(0).toString();
				_list[row][L_3] = QVariant(rs::IN2M(6)).toString();
				break;
		}

		emit dataChanged(createIndex(row, 0), createIndex(row, NUM_COLUMNS));
		return true;
	}
	return false;
}

bool objectModel::newMarker(int id, int form, const rs::Pos &p1, const rs::Pos &p2, const rs::Pos &pt, const rs::Vec &c, int size, std::string name, int role) {
	if (role == Qt::EditRole) {
		// add row
		int row = _list.size();
		this->insertRows(row, 1);

		// new marker data
		_list[row][ID] = QVariant(id).toString();
		_list[row][FORM] = QVariant(form).toString();
		_list[row][P_X] = QVariant(p1[0]).toString();
		_list[row][P_Y] = QVariant(p1[1]).toString();
		_list[row][P_Z] = QVariant(p1[2]).toString();
		if (form == rs::Quad) {
			_list[row][R_PHI] = QVariant(rs::R2D(pt[0])).toString();
			_list[row][R_THETA] = QVariant(rs::R2D(pt[1])).toString();
			_list[row][R_PSI] = QVariant(rs::R2D(pt[2])).toString();
		}
		else {
			_list[row][R_PHI] = QVariant(rs::R2D(p2[0])).toString();
			_list[row][R_THETA] = QVariant(rs::R2D(p2[1])).toString();
			_list[row][R_PSI] = QVariant(rs::R2D(p2[2])).toString();
		}
		_list[row][L_1] = QVariant(p2[0]).toString();
		_list[row][L_2] = QVariant(p2[1]).toString();
		_list[row][L_3] = QVariant(p2[2]).toString();
		QColor qtc(c[0] * 255, c[1] * 255, c[2] * 255, c[3] * 255);
		_list[row][COLOR] = QString(qtc.name());
		QColor qtf(c[0] * 255, c[1] * 255, c[2] * 255, 0);
		_list[row][FILL] = QString(qtf.name());
		_list[row][rsObjectModel::SIZE] = QVariant(size).toString();	// f*$k microsoft
		_list[row][AXIS] = QVariant(2).toString();
		_list[row][TEXT] = QString(name.c_str());
		emit dataChanged(createIndex(row, 0), createIndex(row, NUM_COLUMNS));
		return true;
	}
	return false;
}

bool objectModel::newObstacle(int id, int form, const rs::Pos &p, const rs::Quat &q, const rs::Vec &c, const rs::Vec &l, double mass, int role) {
	if (role == Qt::EditRole) {
		// add row
		int row = _list.size();
		this->insertRows(row, 1);

		// new object data
		_list[row][ID] = QVariant(id).toString();
		_list[row][FORM] = QVariant(form).toString();
		_list[row][P_X] = QVariant(p[0]).toString();
		_list[row][P_Y] = QVariant(p[1]).toString();
		_list[row][P_Z] = QVariant(p[2]).toString();
		_list[row][R_PSI] = QVariant(asin(q[2]) * 180 * 2 / rs::Pi).toString();
		_list[row][L_1] = QVariant(l[0]).toString();
		_list[row][L_2] = QVariant(l[1]).toString();
		_list[row][L_3] = QVariant(l[2]).toString();
		QColor qtc(c[0] * 255, c[1] * 255, c[2] * 255, c[3] * 255);
		_list[row][COLOR] = QString(qtc.name());
		_list[row][MASS] = QVariant(mass).toString();
		_list[row][AXIS] = QVariant(2).toString();
		_list[row][TEXT] = QString();
		emit dataChanged(createIndex(row, 0), createIndex(row, NUM_COLUMNS));
		return true;
	}
	return false;
}

QVariant objectModel::findByID(int id) {
	for (int i = 0; i < this->rowCount(); i++) {
		if (id == _list[i][rsObjectModel::ID].toInt())
			return i;
	}
	return QVariant();
}

void objectModel::setUnits(bool si) {
	_units = si;
}

void objectModel::printModel(void) {
	qDebug() << "model:";
	for (int i = 0; i < _list.size(); i++) {
		for (int j = 0; j < NUM_COLUMNS; j++) {
			std::cerr << (_list[i][j]).toStdString() << " ";
		}
		std::cerr << std::endl;
	}
}

int objectModel::columnCount(const QModelIndex&) const {
	return NUM_COLUMNS;
}

int objectModel::rowCount(const QModelIndex&) const {
	return _list.size();
}

QVariant objectModel::data(const QModelIndex &index, int role) const {
	// check if index is valid
	if (!index.isValid())
		return QVariant();

	// return data
	if (role == Qt::DisplayRole) {
		if (index.column() == rsObjectModel::ID) {
			switch (_list[index.row()][rsObjectModel::FORM].toInt()) {
				case rs::Arc:
					return QString(tr("Arc"));
				case rs::ArcSector:
					return QString(tr("Arc Sector"));
				case rs::ArcSegment:
					return QString(tr("Arc Segment"));
				case rs::Arrow:
					return QString(tr("Arrow"));
				case rs::Box:
					return QString(tr("Box"));
				case rs::Circle:
					return QString(tr("Circle"));
				case rs::CompetitionBorder:
					return QString(tr("Competition Border"));
				case rs::Cylinder:
					return QString(tr("Cylinder"));
				case rs::Dot:
					return QString(tr("Point"));
				case rs::Ellipse:
					return QString(tr("Ellipse"));
				case rs::HackySack:
					return QString(tr("Hacky Sack"));
				case rs::Line:
					return QString(tr("Line"));
				case rs::Polygon:
					return QString(tr("Polygon"));
				case rs::PullupBar:
					return QString(tr("Pullup Bar"));
				case rs::Quad:
					return QString(tr("Quad"));
				case rs::Rectangle:
					return QString(tr("Rectangle"));
				case rs::Sphere:
					return QString(tr("Sphere"));
				case rs::Text:
					return QString(tr("Text"));
				case rs::Triangle:
					return QString(tr("Triangle"));
				case rs::WoodBlock:
					return QString(tr("Wood Block"));
			}
		}
		else
			return _list[index.row()][index.column()];
	}
	else if (role == Qt::EditRole) {
		if (index.column() == P_X || index.column() == P_Y || index.column() == P_Z) {
			return this->convertLength(_list[index.row()][index.column()].toDouble(), false);
		}
		else if (index.column() == L_1 || index.column() == L_2 || index.column() == L_3) {
			return this->convertLength(_list[index.row()][index.column()].toDouble(), false);
		}
		else if (index.column() == MASS) {
			return this->convertMass(_list[index.row()][index.column()].toDouble(), false);
		}
		return _list[index.row()][index.column()];
	}
	else if (role == Qt::DecorationRole) {
		QPixmap image;
		switch (_list[index.row()][rsObjectModel::FORM].toInt()) {
			case rs::Arc:
				image.load("icons/arc32.png");
				break;
			case rs::ArcSector:
				image.load("icons/arcsector32.png");
				break;
			case rs::ArcSegment:
				image.load("icons/arcsegment32.png");
				break;
			case rs::Arrow:
				image.load("icons/arrow32.png");
				break;
			case rs::Box:
				image.load("icons/box32.png");
				break;
			case rs::Circle:
				image.load("icons/circle32.png");
				break;
			case rs::CompetitionBorder:
				image.load("icons/competitionborder32.png");
				break;
			case rs::Cylinder:
				image.load("icons/cylinder32.png");
				break;
			case rs::Dot:
				image.load("icons/point32.png");
				break;
			case rs::Ellipse:
				image.load("icons/ellipse32.png");
				break;
			case rs::HackySack:
				image.load("icons/hackysack32.png");
				break;
			case rs::Line:
				image.load("icons/line32.png");
				break;
			case rs::Polygon:
				image.load("icons/polygon32.png");
				break;
			case rs::PullupBar:
				image.load("icons/pullup32.png");
				break;
			case rs::Quad:
				image.load("icons/quad32.png");
				break;
			case rs::Rectangle:
				image.load("icons/rectangle32.png");
				break;
			case rs::Sphere:
				image.load("icons/sphere32.png");
				break;
			case rs::Text:
				image.load("icons/text32.png");
				break;
			case rs::Triangle:
				image.load("icons/triangle32.png");
				break;
			case rs::WoodBlock:
				image.load("icons/woodblock32.png");
				break;
		}
		return image;
	}

	// default return
	return QVariant();
}

QVariant objectModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return QString(tr("Column %1")).arg(section);
	else
		return QString(tr("Row %1")).arg(section);
}

void objectModel::sort(int column, Qt::SortOrder order) {
	switch (column) {
		case ID: {
			for (int i = 0; i < _list.size() - 1; i++) {
				if (_list[i][ID].toInt() > _list[i + 1][ID].toInt()) {
					QStringList temp = _list.takeAt(i);
					_list.insert(i+1, temp);
				}
			}
			 break;
		}
		default:
			break;
	}
	return;
}

Qt::ItemFlags objectModel::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;

	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDropEnabled;
}

Qt::DropActions objectModel::supportedDropActions(void) const {
	return Qt::CopyAction;
}

bool objectModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
	if (action == Qt::IgnoreAction)
		return true;

	QByteArray encoded = data->data("application/x-qabstractitemmodeldatalist");
	QDataStream stream(&encoded, QIODevice::ReadOnly);

	int r, c;
	QMap<int,  QVariant> map;
	stream >> r >> c >> map;

	if (!map[0].toString().compare("Arc"))
		this->addObject(rs::Arc);
	else if (!map[0].toString().compare("Arc Sector"))
		this->addObject(rs::ArcSector);
	else if (!map[0].toString().compare("Arc Segment"))
		this->addObject(rs::ArcSegment);
	else if (!map[0].toString().compare("Arrow"))
		this->addObject(rs::Arrow);
	else if (!map[0].toString().compare("Box"))
		this->addObject(rs::Box);
	else if (!map[0].toString().compare("Circle"))
		this->addObject(rs::Circle);
	else if (!map[0].toString().compare("Competition Border"))
		this->addObject(rs::CompetitionBorder);
	else if (!map[0].toString().compare("Cylinder"))
		this->addObject(rs::Cylinder);
	else if (!map[0].toString().compare("Point"))
		this->addObject(rs::Dot);
	else if (!map[0].toString().compare("Ellipse"))
		this->addObject(rs::Ellipse);
	else if (!map[0].toString().compare("Hacky Sack"))
		this->addObject(rs::HackySack);
	else if (!map[0].toString().compare("Line"))
		this->addObject(rs::Line);
	else if (!map[0].toString().compare("Polygon"))
		this->addObject(rs::Polygon);
	else if (!map[0].toString().compare("Pullup Bar"))
		this->addObject(rs::PullupBar);
	else if (!map[0].toString().compare("Quad"))
		this->addObject(rs::Quad);
	else if (!map[0].toString().compare("Rectangle"))
		this->addObject(rs::Rectangle);
	else if (!map[0].toString().compare("Sphere"))
		this->addObject(rs::Sphere);
	else if (!map[0].toString().compare("Text"))
		this->addObject(rs::Text);
	else if (!map[0].toString().compare("Triangle"))
		this->addObject(rs::Triangle);
	else if (!map[0].toString().compare("Wood Block"))
		this->addObject(rs::WoodBlock);

	return true;
}

bool objectModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	if (index.isValid() && role == Qt::EditRole) {
		// store old value
		QString oldValue = _list[index.row()][index.column()];
		// if numerical values convert to meters
		if (index.column() == P_X || index.column() == P_Y || index.column() == P_Z ||
			index.column() == L_1 || index.column() == L_2 || index.column() == L_3) {
			QVariant newValue = this->convertLength(value.toDouble(), true);
			_list[index.row()][index.column()] = newValue.toString();
		}
		else if (index.column() == MASS) {
			QVariant newValue = this->convertMass(value.toDouble(), true);
			_list[index.row()][MASS] = newValue.toString();
		}
		else {
			_list[index.row()][index.column()] = value.toString();
		}
		// only emit signal if data has actually changed
		if (oldValue.compare(_list[index.row()][index.column()])) {
			emit dataChanged(index, index);
		}
		return true;
	}
	return false;
}

bool objectModel::insertRows(int row, int count, const QModelIndex &parent) {
	// signal that rows are being added
	beginInsertRows(parent, row, row + count - 1);

	// add new item to the list
	for (int i = 0; i < count; i++) {
		QStringList items;
		for (int j = 0; j < NUM_COLUMNS; j++)
			items.append("0");
		_list.insert(row, items);
	}

	// signal that rows have been added
	endInsertRows();

	// success
	return true;
}

bool objectModel::removeRows(int row, int count, const QModelIndex &parent) {
	// signal that rows are being deleted
	beginRemoveRows(parent, row, row + count - 1);

	// delete items from the list
	for (int i = 0; i < count; i++)
		_list.removeAt(row);

	// signal that rows have been deleted
	endRemoveRows();

	// success
	return true;
}

QVariant objectModel::convertLength(double value, bool store) const {
	QVariant tmp;

	// convert [cm/in] -> [m]
	if (store)
		tmp = ((_units) ? value/100 : value/39.37);
	else
		tmp = ((_units) ? value*100 : value*39.37);

	return tmp;
}

QVariant objectModel::convertMass(double value, bool store) const {
	QVariant tmp;

	// convert [kg/lb] -> [kg]
	if (store)
		tmp = ((_units) ? value : value * 0.453593);
	else
		tmp = ((_units) ? value : value / 0.453593);

	return tmp;
}

void objectModel::clear(void) {
	for (int i = _list.size(); i >= 0; i--) {
		this->removeRows(i, 1);
	}
}

