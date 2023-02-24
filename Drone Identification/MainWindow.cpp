#include "MainWindow.h"
#include "LogAnalyzer.h"
#include <memory>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.verticalLayout->setAlignment(ui.startButton, Qt::AlignCenter);
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

MainWindow::~MainWindow()
{
    

}

void MainWindow::inputFileButton()
{
}

void MainWindow::outputFileButton()
{
}
