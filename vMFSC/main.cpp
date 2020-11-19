#include "MainWindow.h"
#include <QtWidgets/QApplication>

#define BUILD_CONSOLE 0

#if !(BUILD_CONSOLE)
#include <QScopedPointer>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>

QScopedPointer<QFile> g_logFile;
void messageHandler(QtMsgType type, const QMessageLogContext& context,
	const QString& msg)
{
	QTextStream output(g_logFile.data());
	output << QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss | ");

	switch (type) {
	case QtDebugMsg:
		output << "Debug: " << msg << endl;
		break;

	case QtInfoMsg:
		output << "Info: " << msg << endl;
		break;

	case QtWarningMsg:
		output << "Warning: " << msg << endl;
		break;

	case QtCriticalMsg:
		output << "Critical: " << msg << endl;
		break;

	case QtFatalMsg:
		output << "Fatal: " << msg << endl;

	default:
		break;
	}
	output.flush();
}
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if !(BUILD_CONSOLE)
	QDir dir;
	if (!dir.exists("logs")) dir.mkdir("logs");
	g_logFile.reset(new QFile("logs/mainlog.txt"));
	g_logFile.data()->open(QIODevice::WriteOnly | QIODevice::Text);
	qInstallMessageHandler(messageHandler);
#endif

    MainWindow w;
    w.showMaximized();
    return a.exec();
}
