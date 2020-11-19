#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include <QMap>
#include <QPair>
#include <QString>
#include <QDateTime>

class QStandardItemModel;

class MainWindow : public QMainWindow, Ui::MainWindowClass
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);

private slots:
    void scan();
	void copy();

	void scannerStarted();
	void scannerFinished();
	void scannerNotice(const QString& message);
	void scannerDetection(const QString& name, const QString& path, const QDateTime& dateTime);

	void copierStarted();
	void copierFinished();
	void copySuccess(const QString& name);
	void copyFailed(const QString& name);
	void copyProgress(const QString& path);
	
	void selectScanDir();
	void selectCopyDir();
	void add();
	void addFromFile();
	void addAll();
	void remove();
	void removeAll();
	void regExpComboBoxChanged(const QString& text);
	void regExpLineEditChanged(const QString& text);

private:
	QString processName(const QString& name);
	void addNameCopy(const QString& name);
	QMap<QString, QPair<QString, QDateTime>> mNameDateTimesMap;
	QMap<QString, int> mResultRowMap;
	QMap<QString, bool> mCopyNameMap;
	QStandardItemModel* mResultModel;
	QStandardItemModel* mCopyModel;
	bool mUserFilter = false;
	QString mRegExp25;
	QString mRegExp50;
	QString mRegExp100;
	QString mRegExp250;
	QString mRegExp500;
	QString mRegExp1M;
};

#endif