#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <xml_model.h>
#include <tree.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QUrl>
#include <fstream>
#include <string>

#define WINDOW_TITLE "Test Editor"
#define XML_FILE_FILTER  "xml files(*.xml)"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_actionSave_triggered();

private:
    Ui::MainWindow *ui;
    QUrl _fileUrl;
    void setupTreeView();
};

#endif // MAINWINDOW_H
