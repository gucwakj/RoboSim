#include "robotmodel.h"

using namespace rsModel;

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

	// set SI units
	_units = true;

	// create initial robot for model
	this->addRobot(rs::LINKBOTI);
}

robotModel::~robotModel(void) {
}

bool robotModel::addRobot(int form, int role) {
	int row = _list.size();
	this->insertRows(row, 1);

	if (role == Qt::EditRole) {
		_list[row][ID] = QVariant((row) ? this->data(createIndex(row-1, ID), Qt::EditRole).toInt() + 1 : 0).toString();
		_list[row][FORM] = QVariant(form).toString();
		_list[row][NAME] = QString("");
		_list[row][P_X] = QVariant((row) ? this->data(createIndex(row-1, P_X)).toDouble() + 0.1524 : 0).toString();	// offset by 6 inches
		_list[row][COLOR] = QString("#00ff00");
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
		emit dataChanged(createIndex(row, 0), createIndex(row, NUM_COLUMNS));
		return true;
	}
	return false;
}

bool robotModel::setUnits(bool si) {
	_units = si;
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
	//qDebug() << QString("row %1, col %2, role %3").arg(index.row()).arg(index.column()).arg(role);

	// check if index is valid
	if (!index.isValid())
		return QVariant();

	// return data
	if (role == Qt::DisplayRole) {
		if (index.column() == rsModel::ID) {
			switch (_list[index.row()][rsModel::FORM].toInt()) {
				case rs::LINKBOTI: case rs::LINKBOTL: case rs::LINKBOTT: {
					int id = _list[index.row()][rsModel::ID].toInt();
					QString name = _list[index.row()][rsModel::NAME];
					switch (_list[index.row()][rsModel::PRECONFIG].toInt()) {
						case rsLinkbot::BOW:				return QString("Bow\nRobots %1 - %2").arg(id + 1).arg(id + _l_preconfig[rsLinkbot::BOW]); break;
						case rsLinkbot::EXPLORER:			return QString("Explorer\nRobots %1 - %2").arg(id + 1).arg(id + _l_preconfig[rsLinkbot::EXPLORER]); break;
						case rsLinkbot::FOURBOTDRIVE:		return QString("Four Bot Drive\nRobots %1 - %2").arg(id + 1).arg(id + _l_preconfig[rsLinkbot::FOURBOTDRIVE]); break;
						case rsLinkbot::FOURWHEELDRIVE:		return QString("Four Wheel Drive\nRobots %1 - %2").arg(id + 1).arg(id + _l_preconfig[rsLinkbot::FOURWHEELDRIVE]); break;
						case rsLinkbot::FOURWHEELEXPLORER:	return QString("Four Wheel Explorer\nRobots %1 - %2").arg(id + 1).arg(id + _l_preconfig[rsLinkbot::FOURWHEELEXPLORER]); break;
						case rsLinkbot::GROUPBOW:			return QString("Group Bow\nRobots %1 - %2").arg(id + 1).arg(id + _l_preconfig[rsLinkbot::GROUPBOW]); break;
						case rsLinkbot::INCHWORM:			return QString("Inchworm\nRobots %1 - %2").arg(id + 1).arg(id + _l_preconfig[rsLinkbot::INCHWORM]); break;
						case rsLinkbot::LIFT:				return QString("Lift\nRobots %1 - %2").arg(id + 1).arg(id + _l_preconfig[rsLinkbot::LIFT]); break;
						case rsLinkbot::OMNIDRIVE:			return QString("Omnidrive\nRobots %1 - %2").arg(id + 1).arg(id + _l_preconfig[rsLinkbot::OMNIDRIVE]); break;
						case rsLinkbot::SNAKE:				return QString("Snake\nRobots %1 - %2").arg(id + 1).arg(id + _l_preconfig[rsLinkbot::SNAKE]); break;
						case rsLinkbot::STAND:				return QString("StandRobots %1 - %2").arg(id + 1).arg(id + _l_preconfig[rsLinkbot::STAND]); break;
						default:							return QString("%1\nRobot %2").arg(name).arg(id + 1); break;
					}
				}
				case rs::MINDSTORMS:
					return QString("Robot %1").arg(_list[index.row()][index.column()].toInt() + 1);
					break;
				case rs::MOBOT:
					return QString("Robot %1").arg(_list[index.row()][index.column()].toInt() + 1);
					break;
				default:
					return QString("Robot %1").arg(_list[index.row()][index.column()].toInt() + 1);
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
		switch (_list[index.row()][rsModel::FORM].toInt()) {
			case rs::LINKBOTI: {
				switch (_list[index.row()][rsModel::PRECONFIG].toInt()) {
					case rsLinkbot::BOW:				image.load("monkey_off_32x32.png"); break;
					case rsLinkbot::EXPLORER:			image.load("monkey_on_32x32.png"); break;
					case rsLinkbot::FOURBOTDRIVE:		image.load("monkey_on_32x32.png"); break;
					case rsLinkbot::FOURWHEELDRIVE:		image.load("monkey_on_32x32.png"); break;
					case rsLinkbot::FOURWHEELEXPLORER:	image.load("monkey_on_32x32.png"); break;
					case rsLinkbot::GROUPBOW:			image.load("monkey_on_32x32.png"); break;
					case rsLinkbot::INCHWORM:			image.load("monkey_on_32x32.png"); break;
					case rsLinkbot::LIFT:				image.load("monkey_on_32x32.png"); break;
					case rsLinkbot::OMNIDRIVE:			image.load("monkey_on_32x32.png"); break;
					case rsLinkbot::SNAKE:				image.load("monkey_on_32x32.png"); break;
					case rsLinkbot::STAND:				image.load("monkey_on_32x32.png"); break;
					default: 							image.load("linkbotI.png"); break;
				}
			}
			case rs::LINKBOTL:
				image.load("linkbotI.jpg");
				break;
			case rs::LINKBOTT:
				image.load("linkbotL.jpg");
				break;
			case rs::MINDSTORMS:
				image.load("mobot.jpg");
				break;
			case rs::MOBOT:
				image.load("mobot.jpg");
				break;
			default:
				image.load("monkey_on_32x32.png");
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
		return QString("Column %1").arg(section);
	else
		return QString("Row %1").arg(section);
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
		this->addRobot(rs::LINKBOTI);
	else if (!map[0].toString().compare("Linkbot L"))
		this->addRobot(rs::LINKBOTL);
	else if (!map[0].toString().compare("Mindstorms EV3"))
		this->addRobot(rs::MINDSTORMS);
	else if (!map[0].toString().compare("Mindstorms NXT"))
		this->addRobot(rs::MINDSTORMS);
	else if (!map[0].toString().compare("Mobot"))
		this->addRobot(rs::MOBOT);
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
		tmp = ((_units) ? value/39.370 : value/100);
	else
		tmp = ((_units) ? value*39.370 : value*100);

	return tmp;
}

