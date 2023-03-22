#pragma once
#include <qstring.h>
#include <qfile.h>
#include <memory>

class crc
{
public:
	crc();
	const bool checksumCRC(std::unique_ptr<QByteArray>& frame);
private:
	void generateCrcTable();
	quint32 calculateCRC32(const QByteArray& data);
	static quint32 crcTable[256];

	const int fcsLen = 4;
};

