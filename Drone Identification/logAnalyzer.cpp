#include "LogAnalyzer.h"
#include "qfile.h"
#include "qdebug.h"
#include "qtextstream.h"
#include "qcryptographichash"
#include <memory>
#include <tins/tins.h>
#include <iostream>

using namespace Tins;



quint32 LogAnalyzer::crcTable[256] = {};

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
    std::unique_ptr<QByteArray> frame = extractFrameData(line);
    if (frame->isEmpty())
    {
        qWarning() << "Could not extract frame hex from frame data!";
        return false;
    }

    uint32_t fcs = *(reinterpret_cast<const uint32_t*>(frame -> right(fcsLen).constData()));
    QByteArray frameContent = frame->left(frame->size() - fcsLen);

    quint32 checksum = calculateCRC32(frameContent);
    //uint8_t* frame_bytes = reinterpret_cast<uint8_t*>(frame -> data());
    //std::vector<uint8_t> frame_v(frame->begin(), frame->end());
    //PDU::serialization_type pdu_bytes(*frame_bytes);
    //RawPDU raw_pdu(pdu_bytes);
    //const Dot11* dot11 = raw_pdu.find_pdu<Dot11>();
    //if (!dot11) {
    //    qWarning() << "The packet (" << frame.get() << ") is not an IEEE802.11 frame";
    //    return false;
    //}
    //
    //std::string addr = dot11->addr1().to_string();
    
    /*
    QString fcs = frameHex -> right(fcsLen);  
    QString frameContent = frameHex -> left(frameHex -> size() - fcsLen);
    quint32 checksum = calculateCRC32(frameContent);

    return fcs.toUInt(nullptr, 16) == checksum;*/
    return checksum == fcs;
}

std::unique_ptr<QByteArray> LogAnalyzer::extractFrameData(QString& line)
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
    QByteArray byteArray = QByteArray::fromHex(hexStr.toUtf8());
    return std::make_unique<QByteArray>(byteArray);
}

quint32 LogAnalyzer::calculateCRC32(const QByteArray& frameContent)
{
    quint32 crc = 0xFFFFFFFF;

    for (const char byte : frameContent)
        crc = (crc >> 8) ^ crcTable[(crc ^ byte) & 0xFF];

 

    return crc ^ 0xFFFFFFFF;
}


bool LogAnalyzer::isBeaconFrame(void* frame)
{
    return 0;
}


