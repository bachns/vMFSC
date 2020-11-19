#include "Scanner.h"
#include <QDirIterator>
#include <QRegExp>
#include <QDateTime>
#include <QDebug>

Scanner::Scanner()
{
}

void Scanner::run()
{
	emit started();

	QRegExp re(mRegExp);
	QDirIterator it(mFolder, QStringList() << mFormat, QDir::Files, QDirIterator::Subdirectories);
	while (it.hasNext())
	{
		const QString& filePath = it.next();
		emit notice(filePath);

		QFileInfo fileInfo(filePath);
		QString& name = fileInfo.baseName();
		QDateTime dateTime = fileInfo.lastModified();

		if (re.exactMatch(fileInfo.baseName()))
		{
			emit detection(name, filePath, dateTime);
		}
	}

	emit finished();
}
