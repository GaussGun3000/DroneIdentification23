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
    analyzer = std::make_unique<LogAnalyzer>();
}

void MainWindow::startButtonClicked()
{

    auto result = analyzer->analizeLogs();
    if (result->status)
    {
        ui.correctFrames->setText(QString("%1/%2").arg(result->correctFrames).arg(result->totalFrames));
        ui.MAC->setText(result->MAC);
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
        this->analyzer->setInputFile(fileName);
        ui.startButton->setEnabled(true);
        ui.statusBar->showMessage("Input file was set successfully, ready to start");
        qInfo() << QString("%1 %2").arg("Input file was set as ", fileName);
    }
    else
        ui.statusBar->showMessage("Unable to set input file: received empty file name");
}

void MainWindow::outputFileButton()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open .txt file (will be truncated)"), QDir::currentPath(), tr("txt files (*.txt)"));
    if (!fileName.isEmpty())
    {
        this->analyzer->setOutputFile(fileName);
        ui.startButton->setEnabled(true);
        ui.statusBar->showMessage("Output file was set successfully");
        qInfo() << QString("%1 %2").arg("Output file was set as ", fileName);
    }
    else
        ui.statusBar->showMessage("Unable to set output file: received empty file name");
}

MainWindow::~MainWindow()
{

}
