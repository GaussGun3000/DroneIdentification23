#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <qfile.h>
#include <qtextstream.h>
#include <qdebug.h>
#include <qdatetime.h>

void loggingMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    QString txt;
    QString timestamp = QDateTime::currentDateTime().toString("dd-MM hh:mm:ss");
    QString msg = QString("[%1] %2").arg(timestamp, message);
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
        break;
    case QtInfoMsg:
        txt = QString("Info: %1 (%2)").arg(msg).arg(context.file);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
        break;
    }
    QFile outFile("log.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qInstallMessageHandler(loggingMessageHandler);
    MainWindow w;
    qInfo() << "Started application";
    w.show();
    return a.exec();
}
