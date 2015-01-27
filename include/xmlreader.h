#ifndef XMLREADER_H
#define XMLREADER_H

#include <iostream>

#include <QFile>
#include <QMessageBox>
#include <QTableWidget>
#include <QXmlStreamReader>

class xmlReader {
	public:
		xmlReader(QTableWidget *table);
		bool read(const QString &fileName);
		int getVersion(void);
	private:
		void read_config_element(void);
		void read_ground_element(void);
		void read_graphics_element(void);
		void read_sim_element(void);
		void skip_unknown_element(void);

		void readBookindexElement();
		void readEntryElement(QTableWidgetItem *parent);
		void readPageElement(QTableWidgetItem *parent);

		QTableWidget *_table;
		QXmlStreamReader _reader;
		int _version;
};

#endif // XMLREADER_H
