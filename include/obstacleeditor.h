#ifndef OBSTACLEEDITOR_H_
#define OBSTACLEEDITOR_H_

#include <iostream>

#include <QColorDialog>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMetaProperty>
#include <QPushButton>
#include <QStackedWidget>
#include <QStringListModel>
#include <QWidget>

#include "obstaclemodel.h"

class bodyColorPicker : public QWidget {
		Q_OBJECT
		Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

	public:
		explicit bodyColorPicker(QWidget* = 0);
		virtual ~bodyColorPicker(void) {};

		QColor color(void) const;

	signals:
		void colorChanged(const QColor);

	public slots:
		void setColor(const QColor);

	private slots:
		void onButtonClicked(void);

	private:
		QColor _color;
		QPushButton *_button;
};

class obstacleEditor : public QWidget {
		Q_OBJECT
	public:
		obstacleEditor(obstacleModel*, QWidget* = 0);

	signals:
		void indexChanged(const QModelIndex&);

	public slots:
		void dataChanged(QModelIndex, QModelIndex);
		void setCurrentIndex(const QModelIndex&);

	protected slots:
		void nextButtonPressed(void);
		void prevButtonPressed(void);
		void deleteCurrentIndex(void);
		void setUnits(bool);

	private:
		obstacleModel *_model;
		QStackedWidget *_pages;
		QPushButton *_deleteButton;
		QPushButton *_nextButton;
		QPushButton *_previousButton;
		bool _units;
		int _row;
};

class boxEditor : public QWidget {
		Q_OBJECT
	public:
		boxEditor(obstacleModel*, QWidget* = 0);
		void setIndex(int);
		void setUnits(bool);

	protected slots:
		void submitPX(double);
		void submitPY(double);
		void submitPZ(double);
		void submitL1(double);
		void submitL2(double);
		void submitL3(double);
		void submitMass(double);
		void submitColor(QColor);

	private:
		obstacleModel *_model;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_pZUnits;
		QLabel *_lXUnits;
		QLabel *_lYUnits;
		QLabel *_lZUnits;
		QLabel *_massUnits;
		bodyColorPicker *_colorPicker;
		int _row;
};

class cylinderEditor : public QWidget {
		Q_OBJECT
	public:
		cylinderEditor(obstacleModel*, QWidget* = 0);
		void setIndex(int);
		void setUnits(bool);

	protected slots:
		void submitPX(double);
		void submitPY(double);
		void submitPZ(double);
		void submitL1(double);
		void submitL2(double);
		void submitAxis(int);
		void submitMass(double);
		void submitColor(QColor);

	private:
		obstacleModel *_model;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_pZUnits;
		QLabel *_lXUnits;
		QLabel *_lYUnits;
		QLabel *_massUnits;
		bodyColorPicker *_colorPicker;
		int _row;
};

class dotEditor : public QWidget {
		Q_OBJECT
	public:
		dotEditor(obstacleModel*, QWidget* = 0);
		void setIndex(int);
		void setUnits(bool);

	protected slots:
		void submitPX(double);
		void submitPY(double);
		void submitPZ(double);
		void submitSize(double);
		void submitColor(QColor);

	private:
		obstacleModel *_model;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_pZUnits;
		bodyColorPicker *_colorPicker;
		int _row;
};

class emptyEditor : public QWidget {
		Q_OBJECT
	public:
		emptyEditor(QWidget* = 0);
};

class lineEditor : public QWidget {
		Q_OBJECT
	public:
		lineEditor(obstacleModel*, QWidget* = 0);
		void setIndex(int);
		void setUnits(bool);

	protected slots:
		void submitPX(double);
		void submitPY(double);
		void submitPZ(double);
		void submitL1(double);
		void submitL2(double);
		void submitL3(double);
		void submitSize(double);
		void submitColor(QColor);

	private:
		obstacleModel *_model;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_pZUnits;
		QLabel *_lXUnits;
		QLabel *_lYUnits;
		QLabel *_lZUnits;
		bodyColorPicker *_colorPicker;
		int _row;
};

class sphereEditor : public QWidget {
		Q_OBJECT
	public:
		sphereEditor(obstacleModel*, QWidget* = 0);
		void setIndex(int);
		void setUnits(bool);

	protected slots:
		void submitPX(double);
		void submitPY(double);
		void submitPZ(double);
		void submitL1(double);
		void submitMass(double);
		void submitColor(QColor);

	private:
		obstacleModel *_model;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_pZUnits;
		QLabel *_lXUnits;
		QLabel *_massUnits;
		bodyColorPicker *_colorPicker;
		int _row;
};

class textEditor : public QWidget {
		Q_OBJECT
	public:
		textEditor(obstacleModel*, QWidget* = 0);
		void setIndex(int);
		void setUnits(bool);

	protected slots:
		void submitName(QString);
		void submitPX(double);
		void submitPY(double);
		void submitPZ(double);
		void submitSize(double);
		void submitColor(QColor);

	private:
		obstacleModel *_model;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_pZUnits;
		bodyColorPicker *_colorPicker;
		int _row;
};

#endif // OBSTACLEEDITOR_H_

