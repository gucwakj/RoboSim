#include <iostream>
#include <cmath>

#include "robotmodel.h"

using namespace rsRobotModel;

robotModel::robotModel(QObject *parent) : QAbstractTableModel(parent) {
	// set up preconfig
	_l_preconfig[rsLinkbot::BOW] = 2;
	_l_preconfig[rsLinkbot::EXPLORER] = 5;
	_l_preconfig[rsLinkbot::FOURBOTDRIVE] = 4;
	_l_preconfig[rsLinkbot::FOURWHEELDRIVE] = 4;
	_l_preconfig[rsLinkbot::FOURWHEELEXPLORER] = 5;
	_l_preconfig[rsLinkbot::GROUPBOW] = 4;
	_l_preconfig[rsLinkbot::INCHWORM] = 2;
	_l_preconfig[rsLinkbot::LIFT] = 4;
	_l_preconfig[rsLinkbot::OMNIDRIVE] = 4;
	_l_preconfig[rsLinkbot::SNAKE] = 5;
	_l_preconfig[rsLinkbot::STAND] = 2;

	// set US units
	_units = false;
}

robotModel::~robotModel(void) {
}

bool robotModel::addRobot(int form, int left, int right, int role) {
	if (role == Qt::EditRole) {
		// add row
		int row = _list.size();
		this->insertRows(row, 1);

		// new robot data
		if (row && this->data(createIndex(row - 1, PRECONFIG), Qt::EditRole).toInt()) {
			_list[row][ID] = QVariant(this->data(createIndex(row - 1, ID), Qt::EditRole).toInt() + 1 + _l_preconfig[this->data(createIndex(row - 1, PRECONFIG), Qt::EditRole).toInt()]).toString();
		}
		else {
			_list[row][ID] = QVariant((row) ? this->data(createIndex(row - 1, ID), Qt::EditRole).toInt() + 1 : 0).toString();
		}
		_list[row][FORM] = QVariant(form).toString();
		_list[row][NAME] = QString("");
		_list[row][P_X] = QVariant((row) ? this->data(createIndex(row - 1, P_X)).toDouble() + 0.1524 : 0).toString();	// offset by 6 inches
		_list[row][P_Y] = QVariant(0).toString();
		_list[row][P_Z] = QVariant(0).toString();
		_list[row][R_PHI] = QVariant(0).toString();
		_list[row][R_THETA] = QVariant(0).toString();
		_list[row][R_PSI] = QVariant(0).toString();
		_list[row][COLOR] = QString("#00ff00");
		_list[row][RADIUS] = QVariant(0).toString();
		_list[row][WHEELLEFT] = QVariant(left).toString();
		_list[row][WHEELRIGHT] = QVariant(right).toString();
		_list[row][PRECONFIG] = QVariant(0).toString();
		emit dataChanged(createIndex(row, 0), createIndex(row, NUM_COLUMNS));
		return true;
	}
	return false;
}

bool robotModel::newRobot(int id, int form, const rs::Pos &p, const rs::Quat &q, const rs::Vec &a, const rs::Vec &c, const rs::Vec &wheels, std::string name, int role) {
	if (role == Qt::EditRole) {
		// add row
		int row = _list.size();
		this->insertRows(row, 1);

		// new robot data
		_list[row][ID] = QVariant(id).toString();
		_list[row][FORM] = QVariant(form).toString();
		_list[row][NAME] = QString(name.c_str());
		_list[row][P_X] = QVariant(p[0]).toString();
		_list[row][P_Y] = QVariant(p[1]).toString();
		_list[row][P_Z] = QVariant(p[2]).toString();
		_list[row][R_PHI] = QVariant(0).toString();
		_list[row][R_THETA] = QVariant(0).toString();
		_list[row][R_PSI] = QVariant(asin(q[2]) * 180 * 2 / rs::Pi).toString();
		QColor qtc(c[0]*255, c[1]*255, c[2]*255, c[3]*255);
		_list[row][COLOR] = QString(qtc.name());
		_list[row][RADIUS] = QVariant(0).toString();
		_list[row][WHEELLEFT] = QVariant(wheels[0]).toString();
		_list[row][WHEELRIGHT] = QVariant(wheels[1]).toString();
		_list[row][PRECONFIG] = QVariant(0).toString();
		emit dataChanged(createIndex(row, 0), createIndex(row, NUM_COLUMNS));
		return true;
	}
	return false;
}

bool robotModel::addPreconfig(int type, int role) {
	int row = _list.size();
	this->insertRows(row, 1);

	if (role == Qt::EditRole) {
		if (row && this->data(createIndex(row-1, PRECONFIG), Qt::EditRole).toInt()) {
			_list[row][ID] = QVariant(this->data(createIndex(row-1, ID), Qt::EditRole).toInt() + 1 + _l_preconfig[type]).toString();
		}
		else {
			_list[row][ID] = QVariant((row) ? this->data(createIndex(row-1, ID), Qt::EditRole).toInt() + 1 : 0).toString();
		}
		_list[row][FORM] = QVariant(rs::LINKBOTI).toString();
		_list[row][NAME] = QString("");
		_list[row][P_X] = QVariant((row) ? this->data(createIndex(row-1, P_X)).toDouble() + 0.1524 : 0).toString();	// offset by 6 inches
		_list[row][COLOR] = QString("#00ff00");
		_list[row][PRECONFIG] = QVariant(type).toString();
		switch (type) {
			case rsLinkbot::BOW:
				_list[row][R_PHI] = QVariant(90).toString();
				_list[row][R_THETA] = QVariant(0).toString();
				_list[row][R_PSI] = QVariant(0).toString();
				break;
			case rsLinkbot::EXPLORER:
				break;
			case rsLinkbot::FOURBOTDRIVE:
				break;
			case rsLinkbot::FOURWHEELDRIVE:
				break;
			case rsLinkbot::FOURWHEELEXPLORER:
				break;
			case rsLinkbot::GROUPBOW:
				break;
			case rsLinkbot::INCHWORM:
				break;
			case rsLinkbot::LIFT:
				break;
			case rsLinkbot::OMNIDRIVE:
				break;
			case rsLinkbot::SNAKE:
				break;
			case rsLinkbot::STAND:
				break;
		}
		emit dataChanged(createIndex(row, 0), createIndex(row, NUM_COLUMNS));
		return true;
	}
	return false;
}

void robotModel::setUnits(bool si) {
	_units = si;
}

QVariant robotModel::findByID(int id) {
	for (int i = 0; i < this->rowCount(); i++) {
		if (id == _list[i][rsRobotModel::ID].toInt())
			return i;
	}
	return QVariant();
}

void robotModel::printModel(void) {
	qDebug() << "model:";
	for (int i = 0; i < _list.size(); i++) {
		for (int j = 0; j < NUM_COLUMNS; j++) {
			std::cerr << (_list[i][j]).toStdString() << " ";
		}
		std::cerr << std::endl;
	}
}

int robotModel::columnCount(const QModelIndex&) const {
	return NUM_COLUMNS;
}

int robotModel::rowCount(const QModelIndex&) const {
	return _list.size();
}

QVariant robotModel::data(const QModelIndex &index, int role) const {
	// check if index is valid
	if (!index.isValid())
		return QVariant();

	// return data
	if (role == Qt::DisplayRole) {
		if (index.column() == rsRobotModel::ID) {
			switch (_list[index.row()][rsRobotModel::FORM].toInt()) {
				case rs::LINKBOTI: case rs::LINKBOTL: case rs::LINKBOTT: {
					int id = _list[index.row()][rsRobotModel::ID].toInt();
					QString name = _list[index.row()][rsRobotModel::NAME];
					switch (_list[index.row()][rsRobotModel::PRECONFIG].toInt()) {
						case rsLinkbot::BOW:				return QString(tr("Bow\nRobots %1 - %2")).arg(id + 1).arg(id + _l_preconfig[rsLinkbot::BOW]); break;
						case rsLinkbot::EXPLORER:			return QString(tr("Explorer\nRobots %1 - %2")).arg(id + 1).arg(id + _l_preconfig[rsLinkbot::EXPLORER]); break;
						case rsLinkbot::FOURBOTDRIVE:		return QString(tr("Four Bot Drive\nRobots %1 - %2")).arg(id + 1).arg(id + _l_preconfig[rsLinkbot::FOURBOTDRIVE]); break;
						case rsLinkbot::FOURWHEELDRIVE:		return QString(tr("Four Wheel Drive\nRobots %1 - %2")).arg(id + 1).arg(id + _l_preconfig[rsLinkbot::FOURWHEELDRIVE]); break;
						case rsLinkbot::FOURWHEELEXPLORER:	return QString(tr("Four Wheel Explorer\nRobots %1 - %2")).arg(id + 1).arg(id + _l_preconfig[rsLinkbot::FOURWHEELEXPLORER]); break;
						case rsLinkbot::GROUPBOW:			return QString(tr("Group Bow\nRobots %1 - %2")).arg(id + 1).arg(id + _l_preconfig[rsLinkbot::GROUPBOW]); break;
						case rsLinkbot::INCHWORM:			return QString(tr("Inchworm\nRobots %1 - %2")).arg(id + 1).arg(id + _l_preconfig[rsLinkbot::INCHWORM]); break;
						case rsLinkbot::LIFT:				return QString(tr("Lift\nRobots %1 - %2")).arg(id + 1).arg(id + _l_preconfig[rsLinkbot::LIFT]); break;
						case rsLinkbot::OMNIDRIVE:			return QString(tr("Omnidrive\nRobots %1 - %2")).arg(id + 1).arg(id + _l_preconfig[rsLinkbot::OMNIDRIVE]); break;
						case rsLinkbot::SNAKE:				return QString(tr("Snake\nRobots %1 - %2")).arg(id + 1).arg(id + _l_preconfig[rsLinkbot::SNAKE]); break;
						case rsLinkbot::STAND:				return QString(tr("StandRobots %1 - %2")).arg(id + 1).arg(id + _l_preconfig[rsLinkbot::STAND]); break;
						default:							return QString(tr("%1\nRobot %2")).arg(name).arg(id + 1); break;
					}
				}
				case rs::EV3: case rs::NXT:
					return QString(tr("Robot %1")).arg(_list[index.row()][index.column()].toInt() + 1);
					break;
				default:
					return QString(tr("Robot %1")).arg(_list[index.row()][index.column()].toInt() + 1);
					break;
			}
		}
		else
			return _list[index.row()][index.column()];
	}
	else if (role == Qt::EditRole) {
		if (index.column() == P_X || index.column() == P_Y || index.column() == P_Z) {
			return this->convert(_list[index.row()][index.column()].toDouble(), false);
		}
		return _list[index.row()][index.column()];
	}
	else if (role == Qt::DecorationRole) {
		QPixmap image;
		switch (_list[index.row()][rsRobotModel::FORM].toInt()) {
			case rs::LINKBOTI: {
				switch (_list[index.row()][rsRobotModel::PRECONFIG].toInt()) {
					case rsLinkbot::BOW:				image.load("icons/bow32.png"); break;
					case rsLinkbot::EXPLORER:			image.load("icons/explorer32.png"); break;
					case rsLinkbot::FOURBOTDRIVE:		image.load("icons/fourbotexplorer32.png"); break;
					case rsLinkbot::FOURWHEELDRIVE:		image.load("icons/fourwheeldrive32.png"); break;
					case rsLinkbot::FOURWHEELEXPLORER:	image.load("icons/fourwheelexplorer32.png"); break;
					case rsLinkbot::GROUPBOW:			image.load("icons/groupbow32.png"); break;
					case rsLinkbot::INCHWORM:			image.load("icons/inchworm32.png"); break;
					case rsLinkbot::LIFT:				image.load("icons/lift32.png"); break;
					case rsLinkbot::OMNIDRIVE:			image.load("icons/omnidrive32.png"); break;
					case rsLinkbot::SNAKE:				image.load("icons/snake32.png"); break;
					case rsLinkbot::STAND:				image.load("icons/stand32.png"); break;
					default: 							image.load("icons/linkbotI32.png"); break;
				}
				break;
			}
			case rs::LINKBOTL:
				image.load("icons/linkbotL32.png");
				break;
			case rs::EV3:
				image.load("icons/mindstormsEV332.png");
				break;
			case rs::NXT:
				image.load("icons/mindstormsNXT32.png");
				break;
			default:
				image.load("icons/monkey32.png");
				break;
		}
		return image;
	}
	else
		return QVariant();
}

QVariant robotModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return QString(tr("Column %1")).arg(section);
	else
		return QString(tr("Row %1")).arg(section);
}

Qt::ItemFlags robotModel::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;

	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDropEnabled;
}

Qt::DropActions robotModel::supportedDropActions(void) const {
	return Qt::CopyAction;
}

bool robotModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
	if (action == Qt::IgnoreAction)
		return true;

	QByteArray encoded = data->data("application/x-qabstractitemmodeldatalist");
	QDataStream stream(&encoded, QIODevice::ReadOnly);

	int r, c;
	QMap<int,  QVariant> map;
	stream >> r >> c >> map;
	if (!map[0].toString().compare("Linkbot I"))
		this->addRobot(rs::LINKBOTI, 2, 2);
	else if (!map[0].toString().compare("Linkbot L"))
		this->addRobot(rs::LINKBOTL);
	else if (!map[0].toString().compare("Mindstorms EV3"))
		this->addRobot(rs::EV3);
	else if (!map[0].toString().compare("Mindstorms NXT"))
		this->addRobot(rs::NXT);
	else if (!map[0].toString().compare("Bow"))
		this->addPreconfig(rsLinkbot::BOW);
	else if (!map[0].toString().compare("Explorer"))
		this->addPreconfig(rsLinkbot::EXPLORER);
	else if (!map[0].toString().compare("Four Bot Drive"))
		this->addPreconfig(rsLinkbot::FOURBOTDRIVE);
	else if (!map[0].toString().compare("Four Wheel Drive"))
		this->addPreconfig(rsLinkbot::FOURWHEELDRIVE);
	else if (!map[0].toString().compare("Four Wheel Explorer"))
		this->addPreconfig(rsLinkbot::FOURWHEELEXPLORER);
	else if (!map[0].toString().compare("Group Bow"))
		this->addPreconfig(rsLinkbot::GROUPBOW);
	else if (!map[0].toString().compare("Inchworm"))
		this->addPreconfig(rsLinkbot::INCHWORM);
	else if (!map[0].toString().compare("Lift"))
		this->addPreconfig(rsLinkbot::LIFT);
	else if (!map[0].toString().compare("Omnidrive"))
		this->addPreconfig(rsLinkbot::OMNIDRIVE);
	else if (!map[0].toString().compare("Snake"))
		this->addPreconfig(rsLinkbot::SNAKE);
	else if (!map[0].toString().compare("Stand"))
		this->addPreconfig(rsLinkbot::STAND);

	return true;
}

bool robotModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	if (index.isValid() && role == Qt::EditRole) {
		// store old value
		QString oldValue = _list[index.row()][index.column()];
		// if numerical values convert to meters
		if (index.column() == P_X || index.column() == P_Y || index.column() == P_Z) {
			QVariant newValue = this->convert(value.toDouble(), true);
			_list[index.row()][index.column()] = newValue.toString();
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

bool robotModel::insertRows(int row, int count, const QModelIndex &parent) {
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

bool robotModel::removeRows(int row, int count, const QModelIndex &parent) {
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

QVariant robotModel::convert(double value, bool store) const {
	QVariant tmp;

	if (store)
		tmp = ((_units) ? value/100 : value/39.37);
	else
		tmp = ((_units) ? value*100 : value*39.37);

	return tmp;
}

