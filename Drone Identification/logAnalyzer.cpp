#include "LogAnalyzer.h"
#include "qfile.h"
#include "qdebug.h"
#include "qtextstream.h"
#include "qcryptographichash"
#include <memory>
#include <iostream>
#include <bitset>
#include "fcs.h"

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
                    QByteArray frameBody = frame->mid(headerOffset, frame->size() - fcsLen);
                    auto ssid = extractSSID(frame); //parseInformationElements(frameBody);
                    if (ssid)
                    {
                        beacons++;
                    }
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

const bool LogAnalyzer::checksumCRC(QString& line)
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

    uint8_t* dataPtr = reinterpret_cast<uint8_t*>(frameContent.data());

    quint32 checksum = kors::wifi::calcFcs(dataPtr, frameContent.size()); //calculateCRC32(frameContent);
    return checksum == fcs;
}

const std::unique_ptr<QByteArray> LogAnalyzer::extractFrameData(QString& line)
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


const bool LogAnalyzer::isBeaconFrame(std::unique_ptr<QByteArray>& frame)
{
    QByteArray controlHeader = frame->left(2);
    quint8 ch_firstByte = controlHeader.at(0);
    quint8 ch_secondByte = controlHeader.at(1);
    std::bitset<sizeof(quint8) * 8> bits(ch_firstByte);
    std::string binaryString = bits.to_string();
    std::bitset<sizeof(quint8) * 8> bits2(ch_secondByte);
    std::string binaryString2 = bits2.to_string();
    quint8 frameType = (ch_firstByte & 0b00001100) >> 2;   // Bit order is backwards (right ot left)
    quint8 frameSubtype = (ch_firstByte & 0b11110000) >> 4;
    bool toDs = (ch_secondByte & 0x01) == 0x01;
    bool fromDs = (ch_secondByte & 0x02) == 0x02;

    return (frameType == 0x00) && (frameSubtype == 0x08) && !toDs && !fromDs;  
}

std::unique_ptr<QByteArray> LogAnalyzer::extractSSID(std::unique_ptr<QByteArray>& frame)
{
     quint8 id = frame->at(headerOffset);
     quint8 length = frame->at(headerOffset + 1);
     QByteArray ssidBytes;
     if (id == 0)
         ssidBytes = frame -> mid(headerOffset + 2, length);
     return std::make_unique<QByteArray>(ssidBytes);
}

//const std::unique_ptr<QByteArray> LogAnalyzer::parseInformationElements(QByteArray& frameBody)
//{
//    uint32_t i = 0;
//    uint32_t length = frameBody.size();
//    while (i < length)
//    {
//        uint8_t id = frameBody.at(i);
//        uint8_t len = frameBody.at(i + 1);
//        if (i + len + 2 > length)
//        {
//            qWarning() << "Information element exceeds frame body length!";
//            return nullptr;
//        }
//        if (id == 0)
//        {
//            QByteArray ssid = frameBody.mid(i + 2, len);
//            return std::make_unique<QByteArray>(ssid);
//        }
//        i += len + 2;
//    }
//    return nullptr;
//}


