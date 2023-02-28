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
	bool checksumCRC(void* frame);
	bool isBeaconFrame(void* frame);
	std::unique_ptr<QTextStream> openFile(const QString& fileName, std::unique_ptr<QFile>& fileptr);
};


