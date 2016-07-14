#ifndef ROBOTEDITOR_H_
#define ROBOTEDITOR_H_

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

#include "robotmodel.h"

enum FormList {
	LINKBOTI,
	LINKBOTL,
	//EV3,
	NXT,
	NUM_FORMS
};

class ledColorPicker : public QWidget {
		Q_OBJECT
		Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

	public:
		explicit ledColorPicker(QWidget* = 0);
		virtual ~ledColorPicker(void) {};

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

class robotEditor : public QWidget {
		Q_OBJECT
	public:
		robotEditor(robotModel*, QWidget* = 0);

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
		robotModel *_model;
		QStackedWidget *_pages;
		QPushButton *_deleteButton;
		QPushButton *_nextButton;
		QPushButton *_previousButton;
		bool _units;
		int _form;
		int _row;
};

class blankEditor : public QWidget {
	Q_OBJECT
public:
	blankEditor(QWidget* = 0);
};

class linkbotIEditor : public QWidget {
		Q_OBJECT
	public:
		linkbotIEditor(robotModel*, QWidget* = 0);
		void setIndex(int);
		void setUnits(bool);

	protected slots:
		void submitForm(int);
		void submitName(QString);
		void submitPX(double);
		void submitPY(double);
		void submitRZ(double);
		void submitLeftWheel(int);
		void submitRightWheel(int);
		void submitCaster(int);
		void submitColor(QColor);

	private:
		robotModel *_model;
		QDoubleSpinBox *_rZBox;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_wheelLUnits;
		QLabel *_wheelRUnits;
		QComboBox *_wheelLBox;
		QComboBox *_wheelRBox;
		ledColorPicker *_colorPicker;
		int _row;
};

class linkbotLEditor : public QWidget {
		Q_OBJECT
	public:
		linkbotLEditor(robotModel*, QWidget* = 0);
		void setIndex(int);
		void setUnits(bool);

	protected slots:
		void submitForm(int);
		void submitName(QString);
		void submitPX(double);
		void submitPY(double);
		void submitRZ(double);
		void submitColor(QColor);

	private:
		robotModel *_model;
		QDoubleSpinBox *_rZBox;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		ledColorPicker *_colorPicker;
		int _row;
};

class mindstormsEditor : public QWidget {
		Q_OBJECT
	public:
		mindstormsEditor(robotModel*, QWidget* = 0);
		void setIndex(int);
		void setUnits(bool);

	protected slots:
		void submitForm(int);
		void submitName(QString);
		void submitPX(double);
		void submitPY(double);
		void submitRZ(double);
		void submitLeftWheel(int);
		void submitRightWheel(int);
		void submitColor(QColor);

	private:
		robotModel *_model;
		QDoubleSpinBox *_rZBox;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_wheelLUnits;
		QLabel *_wheelRUnits;
		QComboBox *_wheelLBox;
		QComboBox *_wheelRBox;
		ledColorPicker *_colorPicker;
		int _row;
};

class customEditor : public QWidget {
		Q_OBJECT
	public:
		customEditor(robotModel*, QWidget* = 0);
		void setIndex(int);
		void setUnits(bool);

	protected slots:
		void submitForm(int);
		void submitName(QString);
		void submitPX(double);
		void submitPY(double);
		void submitRZ(double);
		void submitLeftWheel(int);
		void submitRadius(QString);
		void submitColor(QColor);

	private:
		robotModel *_model;
		QDoubleSpinBox *_rZBox;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_wheelUnits;
		QComboBox *_wheelBox;
		QLabel *_radiusUnits;
		ledColorPicker *_colorPicker;
		int _row;
};

class preconfigEditor : public QWidget {
		Q_OBJECT
	public:
		preconfigEditor(robotModel*, QWidget* = 0);
		void setIndex(int);
		void setUnits(bool);

	protected slots:
		void submitName(QString);
		void submitPX(double);
		void submitPY(double);
		void submitRZ(double);
		void submitColor(QColor);

	private:
		robotModel *_model;
		QDoubleSpinBox *_rZBox;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		ledColorPicker *_colorPicker;
		int _row;
};

#endif // ROBOTEDITOR_H_
