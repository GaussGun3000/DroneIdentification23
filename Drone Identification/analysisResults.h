#pragma once
#include <QString>

struct AnalysisReults
{
	QByteArray MAC;
	quint32 totalFrames = 0;
	quint32 correctFrames = 0;
	quint32 totalBeacons = 0;
};