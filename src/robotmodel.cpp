#include <iostream>
#include <cmath>

#include "robotmodel.h"

using namespace rsRobotModel;

robotModel::robotModel(QObject *parent) : QAbstractTableModel(parent) {
	// set up preconfig
	_l_preconfig[rsLinkbot::Preconfigs::Bow] = 2;
	_l_preconfig[rsLinkbot::Preconfigs::Explorer] = 5;
	_l_preconfig[rsLinkbot::Preconfigs::FourBotDrive] = 4;
	_l_preconfig[rsLinkbot::Preconfigs::FourWheelDrive] = 4;
	_l_preconfig[rsLinkbot::Preconfigs::FourWheelExplorer] = 5;
	_l_preconfig[rsLinkbot::Preconfigs::GroupBow] = 4;
	_l_preconfig[rsLinkbot::Preconfigs::Inchworm] = 2;
	_l_preconfig[rsLinkbot::Preconfigs::Lift] = 4;
	_l_preconfig[rsLinkbot::Preconfigs::Omnidrive] = 4;
	_l_preconfig[rsLinkbot::Preconfigs::Snake] = 5;
	_l_preconfig[rsLinkbot::Preconfigs::Stand] = 2;

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
			_list[row][ID] = QVariant(this->data(createIndex(row - 1, ID), Qt::EditRole).toInt() + _l_preconfig[this->data(createIndex(row - 1, PRECONFIG), Qt::EditRole).toInt()]).toString();
		}
		else {
			_list[row][ID] = QVariant((row) ? this->data(createIndex(row - 1, ID), Qt::EditRole).toInt() + 1 : 0).toString();
		}
		_list[row][FORM] = QVariant(form).toString();
		_list[row][NAME] = QString("");
		double offset = 0;
		switch (form) {
			case rs::LinkbotI: case rs::LinkbotL: case rs::LinkbotT:
				offset = 0.1524;	// linkbot offset by 6 inches
				break;
			case rs::EV3: case rs::NXT:
				offset = 0.3048;	// mindstorms offset by 12 inches
				break;
		}
		_list[row][P_X] = QVariant((row) ? this->data(createIndex(row - 1, P_X)).toDouble() + offset : 0).toString();
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
			_list[row][ID] = QVariant(this->data(createIndex(row-1, ID), Qt::EditRole).toInt() + _l_preconfig[type]).toString();
		}
		else {
			_list[row][ID] = QVariant((row) ? this->data(createIndex(row-1, ID), Qt::EditRole).toInt() + 1 : 0).toString();
		}
		_list[row][FORM] = QVariant(rs::LinkbotI).toString();
		_list[row][NAME] = QString("");
		_list[row][P_X] = QVariant((row) ? this->data(createIndex(row-1, P_X)).toDouble() + 0.1524 : 0).toString();	// offset by 6 inches
		_list[row][COLOR] = QString("#00ff00");
		_list[row][PRECONFIG] = QVariant(type).toString();
		switch (type) {
			case rsLinkbot::Preconfigs::Bow:
				_list[row][P_Z] = QVariant(0.05275).toString();
				_list[row][R_PHI] = QVariant(90).toString();
				_list[row][R_THETA] = QVariant(0).toString();
				_list[row][R_PSI] = QVariant(0).toString();
				break;
			case rsLinkbot::Preconfigs::FourBotDrive:
				_list[row][P_Z] = QVariant(0.04445).toString();
				_list[row][R_PHI] = QVariant(-90).toString();
				_list[row][R_THETA] = QVariant(0).toString();
				_list[row][R_PSI] = QVariant(0).toString();
				break;
			case rsLinkbot::Preconfigs::FourWheelDrive:
				_list[row][P_Z] = QVariant(0.04445).toString();
				_list[row][R_PHI] = QVariant(0).toString();
				_list[row][R_THETA] = QVariant(0).toString();
				_list[row][R_PSI] = QVariant(0).toString();
				break;
			case rsLinkbot::Preconfigs::GroupBow:
				_list[row][P_Z] = QVariant(0.05275).toString();
				_list[row][R_PHI] = QVariant(90).toString();
				_list[row][R_THETA] = QVariant(0).toString();
				_list[row][R_PSI] = QVariant(0).toString();
				break;
			case rsLinkbot::Preconfigs::Inchworm:
				_list[row][P_Z] = QVariant(0).toString();
				_list[row][R_PHI] = QVariant(0).toString();
				_list[row][R_THETA] = QVariant(180).toString();
				_list[row][R_PSI] = QVariant(0).toString();
				break;
			case rsLinkbot::Preconfigs::Lift:
				_list[row][P_Z] = QVariant(0).toString();
				_list[row][R_PHI] = QVariant(0).toString();
				_list[row][R_THETA] = QVariant(0).toString();
				_list[row][R_PSI] = QVariant(0).toString();
				break;
			case rsLinkbot::Preconfigs::Stand:
				_list[row][P_Z] = QVariant(0).toString();
				_list[row][R_PHI] = QVariant(0).toString();
				_list[row][R_THETA] = QVariant(0).toString();
				_list[row][R_PSI] = QVariant(0).toString();
				break;
			default:
				break;
		}
		emit dataChanged(createIndex(row, 0), createIndex(row, NUM_COLUMNS));
		return true;
	}
	return false;
}

bool robotModel::newPreconfig(int id, int form, int shape, const rs::Pos &p, const rs::Quat &q, const rs::Vec &c, std::string name, int role) {
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
		_list[row][R_PHI] = QVariant(asin(q[0]) * 180 * 2 / rs::Pi).toString();
		_list[row][R_THETA] = QVariant(asin(q[1]) * 180 * 2 / rs::Pi).toString();
		_list[row][R_PSI] = QVariant(asin(q[2]) * 180 * 2 / rs::Pi).toString();
		QColor qtc(c[0]*255, c[1]*255, c[2]*255, c[3]*255);
		_list[row][COLOR] = QString(qtc.name());
		_list[row][RADIUS] = QVariant(0).toString();
		_list[row][PRECONFIG] = QVariant(shape).toString();
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
				case rs::LinkbotI: case rs::LinkbotL: case rs::LinkbotT: {
					int id = _list[index.row()][rsRobotModel::ID].toInt();
					QString name = _list[index.row()][rsRobotModel::NAME];
					switch (_list[index.row()][rsRobotModel::PRECONFIG].toInt()) {
						case rsLinkbot::Preconfigs::Bow:
							return QString(tr("Bow\nRobots %1 - %2")).arg(id + 1).arg(id + _l_preconfig[rsLinkbot::Preconfigs::Bow]);
							break;
						case rsLinkbot::Preconfigs::Explorer:
							return QString(tr("Explorer\nRobots %1 - %2")).arg(id + 1).arg(id + _l_preconfig[rsLinkbot::Preconfigs::Explorer]);
							break;
						case rsLinkbot::Preconfigs::FourBotDrive:
							return QString(tr("Four Bot Drive\nRobots %1 - %2")).arg(id + 1).arg(id + _l_preconfig[rsLinkbot::Preconfigs::FourBotDrive]);
							break;
						case rsLinkbot::Preconfigs::FourWheelDrive:
							return QString(tr("Four Wheel Drive\nRobots %1 - %2")).arg(id + 1).arg(id + _l_preconfig[rsLinkbot::Preconfigs::FourWheelDrive]);
							break;
						case rsLinkbot::Preconfigs::FourWheelExplorer:
							return QString(tr("Four Wheel Explorer\nRobots %1 - %2")).arg(id + 1).arg(id + _l_preconfig[rsLinkbot::Preconfigs::FourWheelExplorer]);
							break;
						case rsLinkbot::Preconfigs::GroupBow:
							return QString(tr("Group Bow\nRobots %1 - %2")).arg(id + 1).arg(id + _l_preconfig[rsLinkbot::Preconfigs::GroupBow]);
							break;
						case rsLinkbot::Preconfigs::Inchworm:
							return QString(tr("Inchworm\nRobots %1 - %2")).arg(id + 1).arg(id + _l_preconfig[rsLinkbot::Preconfigs::Inchworm]);
							break;
						case rsLinkbot::Preconfigs::Lift:
							return QString(tr("Lift\nRobots %1 - %2")).arg(id + 1).arg(id + _l_preconfig[rsLinkbot::Preconfigs::Lift]);
							break;
						case rsLinkbot::Preconfigs::Omnidrive:
							return QString(tr("Omnidrive\nRobots %1 - %2")).arg(id + 1).arg(id + _l_preconfig[rsLinkbot::Preconfigs::Omnidrive]);
							break;
						case rsLinkbot::Preconfigs::Snake:
							return QString(tr("Snake\nRobots %1 - %2")).arg(id + 1).arg(id + _l_preconfig[rsLinkbot::Preconfigs::Snake]);
							break;
						case rsLinkbot::Preconfigs::Stand:
							return QString(tr("Stand\nRobots %1 - %2")).arg(id + 1).arg(id + _l_preconfig[rsLinkbot::Preconfigs::Stand]);
							break;
						default:
							return QString(tr("%1\nRobot %2")).arg(name).arg(id + 1);
							break;
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
		switch (_list[index.row()][rsRobotModel::PRECONFIG].toInt()) {
			case rsLinkbot::Preconfigs::Bow:				image.load("icons/bow32.png"); break;
			case rsLinkbot::Preconfigs::Explorer:			image.load("icons/explorer32.png"); break;
			case rsLinkbot::Preconfigs::FourBotDrive:		image.load("icons/fourbotdrive32.png"); break;
			case rsLinkbot::Preconfigs::FourWheelDrive:		image.load("icons/fourwheeldrive32.png"); break;
			case rsLinkbot::Preconfigs::FourWheelExplorer:	image.load("icons/fourwheelexplorer32.png"); break;
			case rsLinkbot::Preconfigs::GroupBow:			image.load("icons/groupbow32.png"); break;
			case rsLinkbot::Preconfigs::Inchworm:			image.load("icons/inchworm32.png"); break;
			case rsLinkbot::Preconfigs::Lift:				image.load("icons/lift32.png"); break;
			case rsLinkbot::Preconfigs::Omnidrive:			image.load("icons/omnidrive32.png"); break;
			case rsLinkbot::Preconfigs::Snake:				image.load("icons/snake32.png"); break;
			case rsLinkbot::Preconfigs::Stand:				image.load("icons/stand32.png"); break;
			default:
				switch (_list[index.row()][rsRobotModel::FORM].toInt()) {
					case rs::LinkbotI:
						image.load("icons/linkbotI32.png");
						break;
					case rs::LinkbotL:
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
		}
		return image;
	}
	else
		return QVariant();
}

QVariant robotModel::headerData(int section, Qt::Orientation orientation, int role) const {
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
		this->addRobot(rs::LinkbotI, 2, 2);
	else if (!map[0].toString().compare("Linkbot L"))
		this->addRobot(rs::LinkbotL);
	else if (!map[0].toString().compare("Mindstorms EV3"))
		this->addRobot(rs::EV3);
	else if (!map[0].toString().compare("Mindstorms NXT"))
		this->addRobot(rs::NXT);
	else if (!map[0].toString().compare("Bow"))
		this->addPreconfig(rsLinkbot::Preconfigs::Bow);
	else if (!map[0].toString().compare("Explorer"))
		this->addPreconfig(rsLinkbot::Preconfigs::Explorer);
	else if (!map[0].toString().compare("Four Bot Drive"))
		this->addPreconfig(rsLinkbot::Preconfigs::FourBotDrive);
	else if (!map[0].toString().compare("Four Wheel Drive"))
		this->addPreconfig(rsLinkbot::Preconfigs::FourWheelDrive);
	else if (!map[0].toString().compare("Four Wheel Explorer"))
		this->addPreconfig(rsLinkbot::Preconfigs::FourWheelExplorer);
	else if (!map[0].toString().compare("Group Bow"))
		this->addPreconfig(rsLinkbot::Preconfigs::GroupBow);
	else if (!map[0].toString().compare("Inchworm"))
		this->addPreconfig(rsLinkbot::Preconfigs::Inchworm);
	else if (!map[0].toString().compare("Lift"))
		this->addPreconfig(rsLinkbot::Preconfigs::Lift);
	else if (!map[0].toString().compare("Omnidrive"))
		this->addPreconfig(rsLinkbot::Preconfigs::Omnidrive);
	else if (!map[0].toString().compare("Snake"))
		this->addPreconfig(rsLinkbot::Preconfigs::Snake);
	else if (!map[0].toString().compare("Stand"))
		this->addPreconfig(rsLinkbot::Preconfigs::Stand);

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

	// re-id all subsequent robots
	for (int i = row; i < _list.size(); i++)
		this->setData(this->index(i, ID), _list[i][ID].toInt() - 1);

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

void robotModel::clear(void) {
	for (int i = _list.size(); i >= 0; i--) {
		this->removeRows(i, 1);
	}
}

