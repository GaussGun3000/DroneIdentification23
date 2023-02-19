#include "MainWindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.verticalLayout->setAlignment(ui.startButton, Qt::AlignCenter);
}

MainWindow::~MainWindow()
{
    

}
