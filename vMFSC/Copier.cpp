#include <QFile>
#include <QDir>
#include "Copier.h"

Copier::Copier(const QList<QPair<QString, QString>>& files, const QString& destinationDir)
	: mFiles(files), mDestinationDir(destinationDir)
{
}

void Copier::run()
{
	emit started();
	for (int i = 0; i < mFiles.size(); ++i)
	{
		emit progress((i + 1) * 100 / mFiles.size());
		const QPair<QString, QString>& p = mFiles[i];
		const QString& src = p.second;
		emit progress(src);
		QString dst = QString("%1/%2").arg(mDestinationDir).arg(p.first);
		if (QFileInfo(dst).isFile())
		{
			QFile::remove(dst);
			if (QFile::copy(src, dst))
			{
				emit copied(p.first);
			}
			else
			{
				emit error(p.first);
			}
		}
		else
		{
			QDir dir(dst);
			dir.removeRecursively();
			copyDir(src, dst);
		}
	}
	emit finished();
}

void Copier::copyDir(const QString& sourceDir, const QString& destinationDir)
{
	QDir dir(sourceDir);
	if (!dir.exists())
		return;

	QStringList subDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (const QString& subDir : subDirs)
	{
		QString sourceSubDir = sourceDir + "/" + subDir;
		QString destinationSubDir = destinationDir + "/" + subDir;
		dir.mkpath(destinationSubDir);
		copyDir(sourceSubDir, destinationSubDir);
	}

	QStringList files = dir.entryList(QDir::Files);
	for (const QString& file : files)
	{
		QString source = sourceDir + "/" + file;
		QString destination = destinationDir + "/" + file;
		QFile::copy(source, destination);
		emit copied(destination);
	}
}