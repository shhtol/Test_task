#ifndef TREE_H
#define TREE_H
#include <QObject>
#include <QAbstractItemModel>
#include <QDomDocument>
#include "xml_model.h"
class XmlTree : public QAbstractItemModel
{
    Q_OBJECT
public:
    XmlTree(const QDomDocument& _doc, QObject* parent = nullptr);
    ~XmlTree() override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;
    void addElement(const QModelIndex &parent = QModelIndex());
    void addText(const QModelIndex &parent = QModelIndex());
    QDomDocument getDocument();

private:
    XmlModel *_root = nullptr;
    QDomDocument _doc;
    XmlModel *showNode(QDomNode node, XmlModel *root);
    void showTree(QDomNode root, XmlModel *rootItem);

};
#endif // TREE_H
