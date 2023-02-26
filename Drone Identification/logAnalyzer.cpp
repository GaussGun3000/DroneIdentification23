#include "LogAnalyzer.h"
#include "qfile.h"


bool LogAnalyzer::analizeLogs(QString logFileName)
{
    QFile file(logFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }

    return false;
}

bool LogAnalyzer::checksumCRC(void* frame)
{
    return false;
}

bool LogAnalyzer::isBeaconFrame(void* frame)
{
    return 0;
}
