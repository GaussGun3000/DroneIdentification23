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
	void generateCrcTable();
	bool checksumCRC(QString& line);
	bool isBeaconFrame(void* frame);
	std::unique_ptr<QString> extractFrameData(QString& line);
	std::unique_ptr<QTextStream> openFile(const QString& fileName, std::unique_ptr<QFile>& fileptr);
	quint32 calculateCRC32(const QString& data);

	const int fcsLen = 8;
	static quint32 crcTable[256];
};


