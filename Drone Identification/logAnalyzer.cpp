#include "LogAnalyzer.h"
#include "qfile.h"
#include "qdebug.h"
#include "qtextstream.h"
#include "qcryptographichash"
#include <memory>
#include <iostream>
#include <QRegularExpression>
#include <qmap.h>
#include <Qvector>
#include "crc.h"




std::unique_ptr<AnalysisReults> LogAnalyzer::analizeLogs(QString& logFileName, QString& outputFileName)
{
    try
    { 
        std::unique_ptr<QFile> fileptr = nullptr;
        std::unique_ptr<QTextStream> fileStream = openFile(logFileName, fileptr);
        if (fileStream -> status() != QTextStream::Ok )
        {
            qCritical() << "Input stream is corrupted!";
            fileptr->close();
            return nullptr;
        }
        qInfo() << "Started input file log analysis"; 
        auto droneMacs = std::make_unique<QMap<QByteArray, QByteArray>>();
        auto beacons = std::make_unique<QVector<QByteArray>>();
        auto res = std::make_unique<AnalysisReults>();
        crc crcChecker;
        while (!fileStream -> atEnd())
        {
            QString line = fileStream -> readLine();
            if (line.isEmpty())
                qWarning() << "Empty line received!";

            else
            {
                std::unique_ptr<QByteArray> frame = extractFrameData(line);
                res->totalFrames++;
                if (crcChecker.checksumCRC(frame))  // Refactor so it takes frameData QByteArray as arg
                {
                    res->correctFrames++;
                    std::unique_ptr<QByteArray> frame = extractFrameData(line);
                    processCorrectFrame(frame, droneMacs, beacons, res);
                }
            }
        }
        fileptr->close();
        writeDroneBeacons(outputFileName, beacons);
        return res;
    }
    catch (const std::exception& ex)
    {
        qCritical() << "Uncaught exception: " << ex.what();
    }
    return nullptr;
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


const bool LogAnalyzer::isBeaconFrame(std::unique_ptr<QByteArray>& frame)
{
    QByteArray controlHeader = frame->left(2);
    quint8 ch_firstByte = controlHeader.at(0);
    quint8 ch_secondByte = controlHeader.at(1);
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

const bool LogAnalyzer::mathesDronePattern(std::unique_ptr<QByteArray>& ssid)
{
    QRegularExpression regex("Drone4_[\\d\\w]{7}");
    QRegularExpressionMatch match = regex.match(QString(ssid->data()));

    return match.hasMatch();
}

const std::unique_ptr<QByteArray> LogAnalyzer::exctractMAC(std::unique_ptr<QByteArray>& frame)
{
    return std::unique_ptr<QByteArray>();
}

const void LogAnalyzer::processCorrectFrame(std::unique_ptr<QByteArray>& frame, 
    std::unique_ptr<QMap<QByteArray, QByteArray>>& droneMacs,
    std::unique_ptr<QVector<QByteArray>>& beacons,
    std::unique_ptr<AnalysisReults>& res)
{
    if (isBeaconFrame(frame))
    {
        auto ssid = extractSSID(frame);
        if (ssid && mathesDronePattern(ssid))
        {
            QByteArray mac = frame->mid(addr2Start, addrLen);
            res->totalBeacons++;
            res->MAC = mac;
            droneMacs->insert(ssid->data(), mac);
            beacons->append(*frame);
        }
    }
    return;
}

void LogAnalyzer::writeDroneBeacons(QString& fileName, std::unique_ptr<QVector<QByteArray>>& beacons)
{
    if (fileName.isEmpty()) fileName = "output.txt";
    QFile output(fileName);
    if (!output.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qCritical() << "Couldn't write to output file!";
        return;
    }
    QTextStream ts(&output);
    uint32_t i = 0;
    for (QByteArray frame : *beacons)
        ts << i++ << '\t' << frame.toHex() << endl;
        
    output.close();

}


