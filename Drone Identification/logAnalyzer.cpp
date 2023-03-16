#include "LogAnalyzer.h"
#include "qfile.h"
#include "qdebug.h"
#include "qtextstream.h"
#include "qcryptographichash"
#include <memory>
#include <iostream>


quint32 LogAnalyzer::crcTable[256] = {};

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
        quint32 beacons = 0;
        this->generateCrcTable();
        while (!fileStream -> atEnd())
        {
            QString line = fileStream -> readLine();
            std::unique_ptr<QByteArray> frame = extractFrameData(line);
            if (checksumCRC(line))  // Refactor so it takes frameData QByteArray as arg
            {
                verifiedFrames += 1;
                if (isBeaconFrame(frame))
                {
                    auto ssid = extractSSID(frame);
                    beacons++;
                }
            }
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
    std::unique_ptr<QByteArray> frame = extractFrameData(line);
    if (frame->isEmpty())
    {
        qWarning() << "Could not extract frame hex from frame data!";
        return false;
    }
    uint32_t fcs = *(reinterpret_cast<const uint32_t*>(frame -> right(fcsLen).constData()));
    QByteArray frameContent = frame->left(frame->size() - fcsLen);
    quint32 checksum = calculateCRC32(frameContent);
    return checksum == fcs;
}

std::unique_ptr<QByteArray> LogAnalyzer::extractFrameData(QString& line)
{
    QStringList frameData = line.split(",");
    if (frameData.size() != 5)
    {
        qWarning() << "Received a line of invalid format: " + line;
        return nullptr;
    }
    QString bits = frameData[4];
    QString hexStr = bits.mid(bits.indexOf("=") + 1);
    QByteArray byteArray = QByteArray::fromHex(hexStr.toUtf8());
    return std::make_unique<QByteArray>(byteArray);
}

void LogAnalyzer::generateCrcTable()
{
    const quint32 IEEE80211_POLY = 0xedb88320;
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

quint32 LogAnalyzer::calculateCRC32(const QByteArray& frameContent)
{
    quint32 crc = 0xFFFFFFFF;

    for (const char byte : frameContent)
        crc = (crc >> 8) ^ crcTable[(crc ^ byte) & 0xFF];

    return crc ^ 0xFFFFFFFF;
}


bool LogAnalyzer::isBeaconFrame(std::unique_ptr<QByteArray>& frame)
{
    QByteArray controlHeader = frame->left(2);
    quint16 ch_firstByte = controlHeader.at(0);
    quint8 ch_secondByte = controlHeader.at(1);
    quint8 frameType = (ch_firstByte & 0b00110000) >> 4;
    quint8 frameSubtype = (ch_firstByte & 0b00001111);
    bool toDs = (ch_secondByte & 0x80) == 0x80;
    bool fromDs = (ch_secondByte & 0x40) == 0x40;

    return (frameType == 0x00) && (frameSubtype == 0x08) && !toDs && !fromDs;
}

std::unique_ptr<QByteArray> LogAnalyzer::extractSSID(std::unique_ptr<QByteArray>& frame)
{
     quint8 id = frame->at(ssidOffset);
     quint8 length = frame->at(ssidOffset + 1);
     QByteArray ssidBytes;
     if (id == 0)
         ssidBytes = frame -> mid(ssidOffset + 2, length);
     return std::make_unique<QByteArray>(ssidBytes);
}



