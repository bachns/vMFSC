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
	QRegExp re(mRegExp, Qt::CaseInsensitive);
	QDirIterator it(mFolder, mFileFilter ? QDir::Files : QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
	while (it.hasNext())
	{
		QString path = it.next();
		if (mGdbFilter == false && path.indexOf(".gdb", 0, Qt::CaseInsensitive) >= 0)
			continue;

		emit notice(path);
		QFileInfo fileInfo(path);
		QString& name = fileInfo.fileName();
		QString& basename = fileInfo.baseName();
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
			emit detection(basename, path, dateTime);
		}
	}
	emit finished();
}
