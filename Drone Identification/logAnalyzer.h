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
	const bool checksumCRC(QString& line);
	const bool isBeaconFrame(std::unique_ptr<QByteArray>& frame);
	const std::unique_ptr<QByteArray> parseInformationElements(QByteArray& frameBody);
	std::unique_ptr<QByteArray> extractSSID(std::unique_ptr<QByteArray>& frame);
	const std::unique_ptr<QByteArray> extractFrameData(QString& line);
	std::unique_ptr<QTextStream> openFile(const QString& fileName, std::unique_ptr<QFile>& fileptr);
	quint32 calculateCRC32(const QByteArray& data);

	const int fcsLen = 4;
	const int headerOffset = 36;
	static quint32 crcTable[256];
};


