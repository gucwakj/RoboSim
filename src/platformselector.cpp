#include "platformselector.h"

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGroupBox>
#include <QProcess>
#include <QVBoxLayout>

platformSelector::platformSelector(QWidget *parent) : QWidget(parent) {
	// make graphical elements
	QGroupBox *groupbox = new QGroupBox(tr("Platform"), this);
	groupbox->setAlignment(Qt::AlignCenter);
	_hardware = new QRadioButton(tr("Hardware Robots"));
	_simulated = new QRadioButton(tr("Virtual Robots"));

	// simulated on by default
	_simulated->setChecked(true);

	// lay out grid
	QVBoxLayout *vbox = new QVBoxLayout(groupbox);
	vbox->addWidget(_simulated);
	vbox->addWidget(_hardware);
	groupbox->setLayout(vbox);
	QVBoxLayout *vbox1 = new QVBoxLayout(this);
	vbox1->addWidget(groupbox);
	this->setLayout(vbox1);

	// connect buttons to slots
	QWidget::connect(_hardware, SIGNAL(toggled(bool)), this, SLOT(hardware(bool)));
	QWidget::connect(_simulated, SIGNAL(toggled(bool)), this, SLOT(simulated(bool)));

	// get CHHOME directory
#ifdef Q_OS_WIN
    DWORD size;
    HKEY key;
#if defined(_WIN64)
    RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Wow6432Node\\SoftIntegration"), 0, KEY_QUERY_VALUE, &key);
#else
    RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\SoftIntegration"), 0, KEY_QUERY_VALUE, &key);
#endif
    char path[1024];
    RegQueryValueEx(key, TEXT("CHHOME"), NULL, NULL, (LPBYTE)path, &size);
    path[size] = '\0';
    if (path[0] == '\0') {
		_chhome.append("C:/Ch");
    }
    else {
		_chhome.append(path);
    }
#else
	_chhome.append("/usr/local/ch");
#endif

	// get chrc filepath
	_chrcPath = QDir::homePath();
#ifdef Q_OS_WIN
	_chrcPath.append("/_chrc");
#else
	_chrcPath.append("/.chrc");
#endif
	QFileInfo checkFile(_chrcPath);
	if (!checkFile.exists()) {
		QString source(_chhome);
#ifdef Q_OS_WIN
		source.append("\\config\\chrc");
#else
		source.append("/config/chrc");
#endif
		QFile::copy(source, _chrcPath);
	}

	// open chrc
	QFile file(_chrcPath);
	if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
		return;

	// read chrc into buffer
	QTextStream in(&file);
	while (!in.atEnd()) {
		QString line = in.readLine();
		if (!line.compare("// RoboSim Begin")) {
			line = in.readLine();
			line = in.readLine();
			continue;
		}
		_chrcBuffer << line;
	}
	file.close();

	// add ending newline
	if (_chrcBuffer.last().compare(""))
		_chrcBuffer.push_back("");

	// write virtual robots to chrc
	this->simulated(true);
}

platformSelector::~platformSelector(void) {
	// set system up for hardware robots
	this->hardware(true);
}

QString platformSelector::printRoboSimPath(void) {
	QString path("// RoboSim Begin\n_ipath = stradd(\"");
	path.append(_chhome);
	path.append("/package/chrobosim/include;\", _ipath);\n// RoboSim End\n\n");
	return path;
}

void platformSelector::hardware(bool on) {
	if (on) {
		QFile file(_chrcPath);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			return;

		QTextStream out(&file);
		for (int i = 0; i < _chrcBuffer.size(); i++)
			out << _chrcBuffer[i] << "\n";
		file.close();
	}
}

void platformSelector::simulated(bool on) {
	if (on) {
		QFile file(_chrcPath);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			return;

		QTextStream out(&file);
		for (int i = 0; i < _chrcBuffer.size(); i++)
			out << _chrcBuffer[i] << "\n";
		out << this->printRoboSimPath();
		file.close();
	}
}

