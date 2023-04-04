#include <QtCore/QCoreApplication>
#include <QtWidgets/QApplication>
#include <qbytearray.h>
#include <memory>
#include <qstring.h>
#include <iostream>
#include "../Drone Identification/LogAnalyzer.h"
#include "../Drone Identification/analysisResults.h"

int main(int argc, char* argv[])
{
    const QString inputFile = R"(E:\Software\Visual Studio 2019\Projects\Drone Identification\Drone Identification\empty.log)";
    const QString expectedMAC = "38:e2:6e:1a:69:ac";
    const qint32 expectedBeacons = 55;

    LogAnalyzer analyzer;

    analyzer.setInputFile(inputFile);
    auto result = analyzer.analizeLogs();

    if (result->MAC == expectedMAC && result->totalBeacons == expectedBeacons)
        std::cout << "Success\n";
    else
        std::cout << "Fail: MAC or total Beacons did not match expected value\n";

    return 0;

}
