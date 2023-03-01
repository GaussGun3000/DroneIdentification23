#include "LogAnalyzer.h"
#include "qfile.h"
#include "qdebug.h"
#include "qtextstream.h"
#include <memory>


bool LogAnalyzer::analizeLogs(QString logFileName)
{
    try
    { 
        std::unique_ptr<QFile> fileptr = nullptr;
        std::unique_ptr<QTextStream> fileStream = openFile(logFileName, fileptr);
        if (fileStream -> status() != QTextStream::Ok )
        {
            qCritical() << "Input stream is corrupted!";
            fileptr->close();
            return false;
        }
        qInfo() << "Started input file log analysis"; 
        while (!fileStream -> atEnd())
        {
            QString line = fileStream -> readLine();
        }

        fileptr->close();
        return true;
    }
    catch (const std::exception& ex)
    {
        qCritical() << "Uncaught exception: " << ex.what();
    }
    return false;
}

std::unique_ptr<QTextStream> LogAnalyzer::openFile(const QString& fileName, std::unique_ptr<QFile>& fileptr)
{
    fileptr = std::make_unique<QFile>(fileName);
    if (!fileptr -> open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << "Couldn't read input file!";
        return nullptr;
    }
    return std::make_unique<QTextStream>(fileptr.get());
}

bool LogAnalyzer::checksumCRC(QString& line)
{
    if (line.isEmpty())
    {
        qWarning() << "Empty line received!";
        return false;
    }
    QByteArray frameHex = extractFrameData(line);

    return false;
}

bool LogAnalyzer::isBeaconFrame(void* frame)
{
    return 0;
}

QByteArray& LogAnalyzer::extractFrameData(QString& line)
{
    // TODO: insert return statement here
    QStringList frameData = line.split(",");
    if (frameData.size() != 5)
    {
        qWarning() << "Received a line of invalid format: " + line;
        QByteArray nullarray;
        return nullarray;
    }
    QString bits = frameData[4];
    QString hexStr = bits.mid(bits.indexOf("=") + 1);
    QByteArray hex = QByteArray::fromHex(hexStr.toUtf8());
    return hex;
}
