#pragma once
#include <qstring.h>
#include <qtextstream.h>
#include <qfile.h>
#include <memory>

class LogAnalyzer
{
public:
	bool analizeLogs(QString logFileName);
private:
	bool checksumCRC(QString& line);
	bool isBeaconFrame(void* frame);
	QByteArray& extractFrameData(QString& line);
	std::unique_ptr<QTextStream> openFile(const QString& fileName, std::unique_ptr<QFile>& fileptr);
};


