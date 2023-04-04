#include "LogAnalyzer.h"
#include "qfile.h"
#include "qdebug.h"
#include "qtextstream.h"
#include "qcryptographichash"
#include <memory>
#include <QRegularExpression>
#include <qmap.h>
#include <Qvector>
#include <qbytearray.h>
#include "crc.h"

std::unique_ptr<AnalysisReults> LogAnalyzer::analizeLogs()
{
    try
    { 
        std::unique_ptr<QFile> fileptr = nullptr;
        std::unique_ptr<QTextStream> fileStream = openFile(fileptr);
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
                if(frame) 
                    res->totalFrames++;
                if (crcChecker.checksumCRC(frame))
                {
                    res->correctFrames++;
                    processCorrectFrame(frame, droneMacs, beacons, res);
                }
            }
        }
        fileptr->close();
        writeDroneBeacons(beacons);
        formatOutputStruct(res);
        return res;
    }
    catch (const std::exception& ex)
    {
        qCritical() << "Uncaught exception: " << ex.what();
    }
    return nullptr;
}

void LogAnalyzer::setInputFile(const QString& inputFileName)
{
    this->inputFileName = inputFileName;
}

void LogAnalyzer::setOutputFile(const QString& outputFileName)
{
    this->outputFileName = outputFileName;
}

std::unique_ptr<QTextStream> LogAnalyzer::openFile(std::unique_ptr<QFile>& fileptr)
{
    fileptr = std::make_unique<QFile>(this->inputFileName);
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
    return std::make_unique<QByteArray>(std::move(byteArray));
}


const bool LogAnalyzer::isBeaconFrame(std::unique_ptr<QByteArray>& frame)
{
    QByteArray controlHeader = frame->left(2);
    quint8 ch_firstByte = controlHeader.at(0);
    quint8 ch_secondByte = controlHeader.at(1);
    quint8 frameType = (ch_firstByte & 0b00001100) >> 2;   // Bit order is backwards (right to left)
    quint8 frameSubtype = (ch_firstByte & 0b11110000) >> 4;
    bool toDs = (ch_secondByte & 0x01) == 0x01;
    bool fromDs = (ch_secondByte & 0x02) == 0x02;

    return (frameType == 0x00) && (frameSubtype == 0x08) && !toDs && !fromDs;  
}

const std::unique_ptr<QByteArray> LogAnalyzer::extractSSID(std::unique_ptr<QByteArray>& frame)
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
            if (res->rawMAC.isEmpty()) 
                res->rawMAC = mac;
            if (mac == res->rawMAC) 
                beacons->append(*frame);
            droneMacs->insert(ssid->data(), mac);
        }
    }
    return;
}

void LogAnalyzer::writeDroneBeacons(std::unique_ptr<QVector<QByteArray>>& beacons)
{
    if (outputFileName.isEmpty()) outputFileName = "output.txt";
    QFile output(outputFileName);
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

void LogAnalyzer::formatOutputStruct(std::unique_ptr<AnalysisReults>& res)
{
    if (!res->rawMAC.isEmpty())
    {
        QString mac = res->rawMAC.toHex();
        res->MAC = QString("%1:%2:%3:%4:%5:%6").arg(mac.mid(0, 2), mac.mid(2, 2), mac.mid(4, 2), mac.mid(6, 2), mac.mid(8, 2), mac.mid(10, 2));
        res->status = true;
    }
}



