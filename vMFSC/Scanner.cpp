#include "Scanner.h"
#include <QDirIterator>
#include <QRegExp>
#include <QDateTime>

Scanner::Scanner()
{
}

void Scanner::run()
{
	emit started();
	QRegExp re(mRegExp);
	QDirIterator it(mFolder, mFileFilter ? QDir::Files : QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
	while (it.hasNext())
	{
		QString path = it.next();
		emit notice(path);
		QFileInfo fileInfo(path);
		QString& name = fileInfo.fileName();
		QDateTime dateTime = fileInfo.lastModified();
		if (re.exactMatch(name))
		{
			int level = mParentDirLevel;
			while (level)
			{
				QDir dir(path);
				dir.cd("..");
				path = dir.path();
				level--;
			}
			emit detection(name, path, dateTime);
		}
	}
	emit finished();
}
