#ifndef COPIER_H
#define COPIER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QPair>

class Copier : public QObject
{
	Q_OBJECT

public:
	Copier(const QList<QPair<QString, QString>>& files, const QString& format, const QString& destinationDir);

private slots:
	void run();

signals:
	void started();
	void finished();
	void progress(int value);
	void progress(const QString& path);
	void copied(const QString& name);
	void error(const QString& name);

private:
	QList<QPair<QString, QString>> mFiles;
	QString mDestinationDir;
	QString mFormat;
};

#endif