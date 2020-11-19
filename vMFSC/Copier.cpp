#include "Copier.h"

Copier::Copier(const QList<QPair<QString, QString>>& files, const QString& format, const QString& destinationDir)
	: mFiles(files), mFormat(format), mDestinationDir(destinationDir)
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
		QString dst = QString("%1/%2.%3").arg(mDestinationDir).arg(p.first).arg(mFormat);
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
	emit finished();
}