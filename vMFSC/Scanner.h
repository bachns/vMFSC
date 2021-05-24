#ifndef SCANNER_H
#define SCANNER_H

#include <QObject>

class Scanner : public QObject
{
	Q_OBJECT

public:
	Scanner();
	QString mFolder;
	QString mRegExp;
	bool mFileFilter = true;
	int mParentDirLevel = 0;
	QString mFormat;
	bool mGdbFilter = false;

private slots:
	void run();

signals:
	void started();
	void finished();
	void notice(const QString& message);
	void detection(const QString& name, const QString& path, const QDateTime& dateTime);
};

#endif