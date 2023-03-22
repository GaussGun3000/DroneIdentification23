#pragma once
#include <qstring.h>
#include <qtextstream.h>
#include <qfile.h>
#include <memory>
#include "qmap.h"

class LogAnalyzer
{
public:
	bool analizeLogs(QString logFileName);
private:
	std::unique_ptr<QTextStream> openFile(const QString& fileName, std::unique_ptr<QFile>& fileptr);
	const std::unique_ptr<QByteArray> extractFrameData(QString& line);
	const bool isBeaconFrame(std::unique_ptr<QByteArray>& frame);
	std::unique_ptr<QByteArray> extractSSID(std::unique_ptr<QByteArray>& frame);
	const bool mathesDronePattern(std::unique_ptr<QByteArray>& ssid);
	const std::unique_ptr<QByteArray> exctractMAC(std::unique_ptr<QByteArray>& frame);
	const void processCorrectFrame(std::unique_ptr<QByteArray>& frame, std::unique_ptr<QMap<QByteArray, QByteArray>>& droneMacs);

	const int headerOffset = 36;
	const int addr2Start = 10;
	const int addrLen = 6;
};


