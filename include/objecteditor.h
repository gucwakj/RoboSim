#ifndef OBJECTEDITOR_H_
#define OBJECTEDITOR_H_

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

#include "objectmodel.h"

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

class objectEditor : public QWidget {
		Q_OBJECT
	public:
		objectEditor(objectModel*, QWidget* = 0);

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
		objectModel *_model;
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
		boxEditor(objectModel*, QWidget* = 0);
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
		objectModel *_model;
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

class circleEditor : public QWidget {
		Q_OBJECT
	public:
		circleEditor(objectModel*, QWidget* = 0);
		void setIndex(int);
		void setUnits(bool);

	protected slots:
		void submitPX(double);
		void submitPY(double);
		void submitPZ(double);
		void submitColor(QColor);

	private:
		objectModel *_model;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_pZUnits;
		bodyColorPicker *_colorPicker;
		int _row;
};

class competitionborderEditor : public QWidget {
		Q_OBJECT
	public:
		competitionborderEditor(objectModel*, QWidget* = 0);
		void setIndex(int);
		void setUnits(bool);

	protected slots:
		void submitPX(double);
		void submitPY(double);
		void submitRZ(double);
		void submitL1(double);
		void submitL2(double);
		void submitL3(double);
		void submitColor(QColor);

	private:
		objectModel *_model;
		QDoubleSpinBox *_rZBox;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_lXUnits;
		QLabel *_lYUnits;
		QLabel *_lZUnits;
		bodyColorPicker *_colorPicker;
		int _row;
};

class cylinderEditor : public QWidget {
		Q_OBJECT
	public:
		cylinderEditor(objectModel*, QWidget* = 0);
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
		objectModel *_model;
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
		dotEditor(objectModel*, QWidget* = 0);
		void setIndex(int);
		void setUnits(bool);

	protected slots:
		void submitPX(double);
		void submitPY(double);
		void submitPZ(double);
		void submitSize(double);
		void submitColor(QColor);

	private:
		objectModel *_model;
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

class hackysackEditor : public QWidget {
		Q_OBJECT
	public:
		hackysackEditor(objectModel*, QWidget* = 0);
		void setIndex(int);
		void setUnits(bool);

	protected slots:
		void submitPX(double);
		void submitPY(double);

	private:
		objectModel *_model;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		int _row;
};

class lineEditor : public QWidget {
		Q_OBJECT
	public:
		lineEditor(objectModel*, QWidget* = 0);
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
		objectModel *_model;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_pZUnits;
		QLabel *_lXUnits;
		QLabel *_lYUnits;
		QLabel *_lZUnits;
		bodyColorPicker *_colorPicker;
		int _row;
};

class polygonEditor : public QWidget {
		Q_OBJECT
	public:
		polygonEditor(objectModel*, QWidget* = 0);
		void setIndex(int);
		void setUnits(bool);

	protected slots:
		void submitPX(double);
		void submitPY(double);
		void submitPZ(double);
		void submitL1(double);
		void submitSize(double);
		void submitColor(QColor);

	private:
		objectModel *_model;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_pZUnits;
		QLabel *_lXUnits;
		bodyColorPicker *_colorPicker;
		int _row;
};

class pullupbarEditor : public QWidget {
		Q_OBJECT
	public:
		pullupbarEditor(objectModel*, QWidget* = 0);
		void setIndex(int);
		void setUnits(bool);

	protected slots:
		void submitPX(double);
		void submitPY(double);
		void submitRZ(double);

	private:
		objectModel *_model;
		QDoubleSpinBox *_rZBox;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		int _row;
};

class rectangleEditor : public QWidget {
		Q_OBJECT
	public:
		rectangleEditor(objectModel*, QWidget* = 0);
		void setIndex(int);
		void setUnits(bool);

	protected slots:
		void submitPX(double);
		void submitPY(double);
		void submitL1(double);
		void submitL2(double);
		void submitSize(double);
		void submitColor(QColor);

	private:
		objectModel *_model;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_lXUnits;
		QLabel *_lYUnits;
		bodyColorPicker *_colorPicker;
		int _row;
};

class sphereEditor : public QWidget {
		Q_OBJECT
	public:
		sphereEditor(objectModel*, QWidget* = 0);
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
		objectModel *_model;
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
		textEditor(objectModel*, QWidget* = 0);
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
		objectModel *_model;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_pZUnits;
		bodyColorPicker *_colorPicker;
		int _row;
};

class triangleEditor : public QWidget {
		Q_OBJECT
	public:
		triangleEditor(objectModel*, QWidget* = 0);
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
		objectModel *_model;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_pZUnits;
		QLabel *_lXUnits;
		QLabel *_lYUnits;
		QLabel *_lZUnits;
		bodyColorPicker *_colorPicker;
		int _row;
};

class woodblockEditor : public QWidget {
		Q_OBJECT
	public:
		woodblockEditor(objectModel*, QWidget* = 0);
		void setIndex(int);
		void setUnits(bool);

	protected slots:
		void submitPX(double);
		void submitPY(double);
		void submitPZ(double);
		void submitL1(double);
		void submitL2(double);
		void submitL3(double);

	private:
		objectModel *_model;
		QLabel *_pXUnits;
		QLabel *_pYUnits;
		QLabel *_pZUnits;
		QLabel *_lXUnits;
		QLabel *_lYUnits;
		QLabel *_lZUnits;
		int _row;
};

#endif // OBJECTEDITOR_H_

