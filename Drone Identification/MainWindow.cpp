#include "MainWindow.h"
#include "LogAnalyzer.h"
#include <memory>
#include "qfiledialog.h"


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
    if (analyzer->analizeLogs())
    {
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
        ui.startButton->setEnabled(true);
        ui.statusBar->showMessage("Input file was set successfully, ready to start");
    }
}

void MainWindow::outputFileButton()
{

}

MainWindow::~MainWindow()
{


}
