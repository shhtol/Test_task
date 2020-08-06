#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDebug>
#include <QPushButton>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupTreeView();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_actionOpen_triggered() {
    QUrl fileUrl = QFileDialog::getOpenFileUrl(this, "Open", QUrl(), XML_FILE_FILTER);
    if(!fileUrl.isValid()) return;
    _fileUrl = fileUrl;
    QFile file(_fileUrl.toLocalFile());
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Cannot open the file\n" + file.errorString());
        return;
    }
    setWindowTitle(WINDOW_TITLE " - " + _fileUrl.fileName());
    QDomDocument doc;
    doc.setContent(&file);
    file.close();
    XmlTree *model = new XmlTree(doc);
    if(ui->treeView->model()) {
        delete ui->treeView->model();
    }
    ui->treeView->setModel(model);
    ui->treeView->expandAll();
    ui->actionSave->setEnabled(true);
}

void MainWindow::on_actionSave_triggered() {
    auto model = static_cast< XmlTree*>(ui->treeView->model());
    auto doc = model->getDocument();
    if(doc.isNull()) return;

    QFile file(_fileUrl.toLocalFile());
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Error", "Cannot open the file\n" + file.errorString());
        return;
    }
    QTextStream stream(&file);
    doc.save(stream, 4);
}


void MainWindow::setupTreeView() {
    ui->treeView->header()->setSectionResizeMode(QHeaderView::Stretch);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, [this](const QPoint &pos){
        auto index = ui->treeView->indexAt(pos);
        auto item = static_cast<XmlModel*>(index.internalPointer());
        if(!item) return;

        QMenu *menu = new QMenu;
        menu->setAttribute(Qt::WA_DeleteOnClose);
        if(item->isElement()) {
            menu->addAction("Add element", [=]{
                auto model = static_cast< XmlTree*>(ui->treeView->model());
                model->addElement(index);
            });
            menu->addAction("Add information", [=]{
                auto model = static_cast< XmlTree*>(ui->treeView->model());
                model->addText(index);
            });
        }
        menu->addAction("Delete", [=]{
            ui->treeView->model()->removeRows(index.row(), 1, index.parent());
        });
        menu->popup(ui->treeView->mapToGlobal(pos));
    });
}


