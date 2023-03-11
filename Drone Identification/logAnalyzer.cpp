#include "LogAnalyzer.h"
#include "qfile.h"
#include "qdebug.h"
#include "qtextstream.h"
#include "qcryptographichash"
#include <memory>
#include <tins/tins.h>


quint32 LogAnalyzer::crcTable[256] = {};

void LogAnalyzer::generateCrcTable() 
{
    const quint32 IEEE80211_POLY = 0b100000100110000010001110110110111;
    for (int i = 0; i < 256; i++)
    {
        uint32_t c = i;
        for (int j = 0; j < 8; j++)
        {
            c = (c & 1) ? (IEEE80211_POLY ^ (c >> 1)) : (c >> 1);
        }
        this->crcTable[i] = c;
    }
}

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
        quint32 verifiedFrames = 0;
        this->generateCrcTable();
        while (!fileStream -> atEnd())
        {
            QString line = fileStream -> readLine();
            if (checksumCRC(line))
                verifiedFrames += 1;
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
    std::unique_ptr<QString> frameHex = extractFrameData(line);
    if (frameHex -> isEmpty())
    {
        qWarning() << "Could not extract frame hex from frame data!";
        return false;
    }
    QString fcs = frameHex -> right(fcsLen);  
    QString frameContent = frameHex -> left(frameHex -> size() - fcsLen);
    quint32 checksum = calculateCRC32(frameContent);

    return fcs.toUInt(nullptr, 16) == checksum;
}

std::unique_ptr<QString> LogAnalyzer::extractFrameData(QString& line)
{
    // TODO: insert return statement here
    QStringList frameData = line.split(",");
    if (frameData.size() != 5)
    {
        qWarning() << "Received a line of invalid format: " + line;
        return nullptr;
    }
    QString bits = frameData[4];
    QString hexStr = bits.mid(bits.indexOf("=") + 1);
    return std::make_unique<QString>(hexStr);
}

quint32 LogAnalyzer::calculateCRC32(const QString& frameContent)
{
    quint32 crc = 0xFFFFFFFF;

    QByteArray byteArray = QByteArray::fromHex(frameContent.toUtf8());

    for (const char byte : byteArray)
        crc = (crc >> 8) ^ crcTable[(crc ^ byte) & 0xFF];

 

    return crc ^ 0xFFFFFFFF;
}


bool LogAnalyzer::isBeaconFrame(void* frame)
{
    return 0;
}


