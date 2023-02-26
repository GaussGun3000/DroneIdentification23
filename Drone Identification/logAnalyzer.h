#pragma once
#include <qstring.h>
class LogAnalyzer
{
public:
	bool analizeLogs(QString logFileName);
private:
	bool checksumCRC(void* frame);
	bool isBeaconFrame(void* frame);
};


