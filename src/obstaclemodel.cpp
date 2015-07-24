#include <algorithm>

#include "obstaclemodel.h"

using namespace rsObstacleModel;

obstacleModel::obstacleModel(QObject *parent) : QAbstractTableModel(parent) {
	// set US units
	_units = false;
}

obstacleModel::~obstacleModel(void) {
}

bool obstacleModel::addObstacle(int form, int role) {
	if (role == Qt::EditRole) {
		// add row
		int row = _list.size();
		this->insertRows(row, 1);

		// new obstacle data
		_list[row][ID] = QVariant((row) ? this->data(createIndex(row-1, ID), Qt::EditRole).toInt() + 1 : 0).toString();
		_list[row][FORM] = QVariant(form).toString();
		_list[row][P_X] = QVariant((row) ? this->data(createIndex(row-1, P_X)).toDouble() + 0.1524 : 0).toString();	// offset by 6 inches
		_list[row][P_Y] = QVariant(0).toString();
		_list[row][P_Z] = QVariant(0).toString();
		_list[row][L_1] = QVariant(0.0254).toString();	// 1 inch
		_list[row][L_2] = QVariant(0.0254).toString();	// 1 inch
		_list[row][L_3] = QVariant(0.0254).toString();	// 1 inch
		_list[row][COLOR] = QString("#00ff00");	// green
		_list[row][MASS] = QVariant(453.593).toString();	// 1lb in grams
		_list[row][rsObstacleModel::SIZE] = QVariant(1).toString();		// f*$k microsoft
		_list[row][AXIS] = QVariant(2).toString();
		_list[row][TEXT] = QString();
		emit dataChanged(createIndex(row, 0), createIndex(row, NUM_COLUMNS));
		return true;
	}
	return false;
}

bool obstacleModel::newMarker(int id, int form, const rs::Pos &p1, const rs::Pos &p2, const rs::Vec &c, int size, std::string name, int role) {
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
		_list[row][L_1] = QVariant(p2[0]).toString();
		_list[row][L_2] = QVariant(p2[1]).toString();
		_list[row][L_3] = QVariant(p2[2]).toString();
		QColor qtc(c[0] * 255, c[1] * 255, c[2] * 255, c[3] * 255);
		_list[row][COLOR] = QString(qtc.name());
		_list[row][rsObstacleModel::SIZE] = QVariant(size).toString();	// f*$k microsoft
		_list[row][AXIS] = QVariant(2).toString();
		_list[row][TEXT] = QString(name.c_str());
		this->sort(ID);
		emit dataChanged(createIndex(0, 0), createIndex(_list.size(), NUM_COLUMNS));
		return true;
	}
	return false;
}

bool obstacleModel::newObstacle(int id, int form, const rs::Pos &p, const rs::Quat &q, const rs::Vec &c, const rs::Vec &l, double mass, int role) {
	if (role == Qt::EditRole) {
		// add row
		int row = _list.size();
		this->insertRows(row, 1);

		// new obstacle data
		_list[row][ID] = QVariant(id).toString();
		_list[row][FORM] = QVariant(form).toString();
		_list[row][P_X] = QVariant(p[0]).toString();
		_list[row][P_Y] = QVariant(p[1]).toString();
		_list[row][P_Z] = QVariant(p[2]).toString();
		_list[row][L_1] = QVariant(l[0]).toString();
		_list[row][L_2] = QVariant(l[1]).toString();
		_list[row][L_3] = QVariant(l[2]).toString();
		QColor qtc(c[0] * 255, c[1] * 255, c[2] * 255, c[3] * 255);
		_list[row][COLOR] = QString(qtc.name());
		_list[row][MASS] = QVariant(mass).toString();
		_list[row][AXIS] = QVariant(2).toString();
		_list[row][TEXT] = QString();
		this->sort(ID);
		emit dataChanged(createIndex(0, 0), createIndex(_list.size(), NUM_COLUMNS));
		return true;
	}
	return false;
}

QVariant obstacleModel::findByID(int id) {
	for (int i = 0; i < this->rowCount(); i++) {
		if (id == _list[i][rsObstacleModel::ID].toInt())
			return i;
	}
	return QVariant();
}

void obstacleModel::setUnits(bool si) {
	_units = si;
}

void obstacleModel::printModel(void) {
	qDebug() << "model:";
	for (int i = 0; i < _list.size(); i++) {
		for (int j = 0; j < NUM_COLUMNS; j++) {
			std::cerr << (_list[i][j]).toStdString() << " ";
		}
		std::cerr << std::endl;
	}
}

int obstacleModel::columnCount(const QModelIndex&) const {
	return NUM_COLUMNS;
}

int obstacleModel::rowCount(const QModelIndex&) const {
	return _list.size();
}

QVariant obstacleModel::data(const QModelIndex &index, int role) const {
	// check if index is valid
	if (!index.isValid())
		return QVariant();

	// return data
	if (role == Qt::DisplayRole) {
		if (index.column() == rsObstacleModel::ID) {
			switch (_list[index.row()][rsObstacleModel::FORM].toInt()) {
				case rs::BOX:
					return QString(tr("Box %1")).arg(_list[index.row()][index.column()].toInt() + 1);
				case rs::CYLINDER:
					return QString(tr("Cylinder %1")).arg(_list[index.row()][index.column()].toInt() + 1);
				case rs::DOT:
					return QString(tr("Point %1")).arg(_list[index.row()][index.column()].toInt() + 1);
				case rs::LINE:
					return QString(tr("Line %1")).arg(_list[index.row()][index.column()].toInt() + 1);
				case rs::SPHERE:
					return QString(tr("Sphere %1")).arg(_list[index.row()][index.column()].toInt() + 1);
				case rs::TEXT:
					return QString(tr("Text %1")).arg(_list[index.row()][index.column()].toInt() + 1);
			}
		}
		else
			return _list[index.row()][index.column()];
	}
	else if (role == Qt::EditRole) {
		if (index.column() == P_X || index.column() == P_Y || index.column() == P_Z) {
			return this->convert(_list[index.row()][index.column()].toDouble(), false);
		}
		else if (index.column() == L_1 || index.column() == L_2 || index.column() == L_3) {
			return this->convert(_list[index.row()][index.column()].toDouble(), false);
		}
		else if (index.column() == MASS) {
			return this->convertMass(_list[index.row()][index.column()].toDouble(), false);
		}
		return _list[index.row()][index.column()];
	}
	else if (role == Qt::DecorationRole) {
		QPixmap image;
		switch (_list[index.row()][rsObstacleModel::FORM].toInt()) {
			case rs::BOX:
				image.load("icons/box32.png");
				break;
			case rs::CYLINDER:
				image.load("icons/cylinder32.png");
				break;
			case rs::DOT:
				image.load("icons/point32.png");
				break;
			case rs::LINE:
				image.load("icons/line32.png");
				break;
			case rs::SPHERE:
				image.load("icons/sphere32.png");
				break;
			case rs::TEXT:
				image.load("icons/text32.png");
				break;
		}
		return image;
	}

	// default return
	return QVariant();
}

QVariant obstacleModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return QString(tr("Column %1")).arg(section);
	else
		return QString(tr("Row %1")).arg(section);
}

void obstacleModel::sort(int column, Qt::SortOrder order) {
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

Qt::ItemFlags obstacleModel::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;

	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDropEnabled;
}

Qt::DropActions obstacleModel::supportedDropActions(void) const {
	return Qt::CopyAction;
}

bool obstacleModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
	if (action == Qt::IgnoreAction)
		return true;

	QByteArray encoded = data->data("application/x-qabstractitemmodeldatalist");
	QDataStream stream(&encoded, QIODevice::ReadOnly);

	int r, c;
	QMap<int,  QVariant> map;
	stream >> r >> c >> map;

	if (!map[0].toString().compare("Box"))
		this->addObstacle(rs::BOX);
	else if (!map[0].toString().compare("Cylinder"))
		this->addObstacle(rs::CYLINDER);
	else if (!map[0].toString().compare("Point"))
		this->addObstacle(rs::DOT);
	else if (!map[0].toString().compare("Line"))
		this->addObstacle(rs::LINE);
	else if (!map[0].toString().compare("Sphere"))
		this->addObstacle(rs::SPHERE);
	else if (!map[0].toString().compare("Text"))
		this->addObstacle(rs::TEXT);

	return true;
}

bool obstacleModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	if (index.isValid() && role == Qt::EditRole) {
		// store old value
		QString oldValue = _list[index.row()][index.column()];
		// if numerical values convert to meters
		if (index.column() == P_X || index.column() == P_Y || index.column() == P_Z ||
			index.column() == L_1 || index.column() == L_2 || index.column() == L_3) {
			QVariant newValue = this->convert(value.toDouble(), true);
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

bool obstacleModel::insertRows(int row, int count, const QModelIndex &parent) {
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

bool obstacleModel::removeRows(int row, int count, const QModelIndex &parent) {
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

QVariant obstacleModel::convert(double value, bool store) const {
	QVariant tmp;

	// convert [cm/in] -> [m]
	if (store)
		tmp = ((_units) ? value/100 : value/39.37);
	else
		tmp = ((_units) ? value*100 : value*39.37);

	return tmp;
}

QVariant obstacleModel::convertMass(double value, bool store) const {
	QVariant tmp;

	// convert [kg/lb] -> [g]
	if (store)
		tmp = ((_units) ? value / 1000 : value * 453.593);
	else
		tmp = ((_units) ? value * 1000 : value / 453.593);

	return tmp;
}

