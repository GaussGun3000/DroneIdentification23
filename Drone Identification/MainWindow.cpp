#include "MainWindow.h"
#include "LogAnalyzer.h"
#include <memory>
#include "qfiledialog.h"
#include "qdebug.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.verticalLayout->setAlignment(ui.startButton, Qt::AlignCenter);
    ui.statusBar->showMessage("Choose input file before starting! Default output is <CWD>/output.txt");
}

void MainWindow::startButtonClicked()
{
    std::unique_ptr<LogAnalyzer> analyzer(new LogAnalyzer);
    auto result = analyzer->analizeLogs(this->logFileName, this->outputFileName);
    if (!result->MAC.isEmpty())
    {
        ui.correctFrames->setText(QString("%1/%2").arg(result->correctFrames).arg(result->totalFrames));
        QString mac = result->MAC.toHex();
        ui.MAC->setText(QString("%1:%2:%3:%4:%5:%6").arg(mac.mid(0, 2)).arg(mac.mid(2, 2)).arg(mac.mid(4, 2))
            .arg(mac.mid(6, 2)).arg(mac.mid(8, 2)).arg(mac.mid(10, 2)));
        ui.beaconFrames->setText(QString("%1").arg(result->totalBeacons));
        ui.statusBar->showMessage("Analisys was completed succesfully. Check the results section");
    }
    else
    {
        ui.statusBar->showMessage("Analisys failed. Check program logs");
    }
}

void MainWindow::inputFileButton()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open .log data file"), QDir::currentPath(), tr("log files (*.log)"));
    if (!fileName.isEmpty())
    {
        this->logFileName = fileName;
        ui.startButton->setEnabled(true);
        ui.statusBar->showMessage("Input file was set successfully, ready to start");
        qInfo() << QString("%1 %2").arg("Input file was set as ", fileName);
    }
}

void MainWindow::outputFileButton()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open .txt file (will be truncated)"), QDir::currentPath(), tr("txt files (*.txt)"));
    if (!fileName.isEmpty())
    {
        this->outputFileName = fileName;
        ui.startButton->setEnabled(true);
        ui.statusBar->showMessage("Output file was set successfully");
        qInfo() << QString("%1 %2").arg("Output file was set as ", fileName);
    }
}

MainWindow::~MainWindow()
{

}
