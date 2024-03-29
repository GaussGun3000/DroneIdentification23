#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include "LogAnalyzer.h"
#include <memory>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void startButtonClicked();
    void inputFileButton();
    void outputFileButton();

private:
    Ui::MainWindowClass ui;
    std::unique_ptr<LogAnalyzer> analyzer;
};
