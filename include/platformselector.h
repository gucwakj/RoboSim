#ifndef PLATFORMSELECTOR_H_
#define PLATFORMSELECTOR_H_

#include <QRadioButton>
#include <QString>
#include <QStringList>
#include <QWidget>

class platformSelector : public QWidget {
		Q_OBJECT
	public:
		platformSelector(QWidget* = 0);
		~platformSelector(void);

	public slots:
		void hardware(bool);
		void simulated(bool);

	private:
		std::string getChHomePath(void);
		QString printRoboSimPath(void);

	private:
		QRadioButton *_hardware;
		QRadioButton *_simulated;
		QStringList _chrcBuffer;
		QString _chrcPath;
		QString _chhome;
};

#endif // PLATFORMSELECTOR_H_

