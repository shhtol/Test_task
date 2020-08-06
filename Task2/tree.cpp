#include "tree.h"
#include <QDebug>

XmlTree::XmlTree(const QDomDocument& doc, QObject *parent): QAbstractItemModel(parent) {
    _root = new XmlModel(doc);
    _doc = doc;
    showTree(doc.lastChild(), _root);
}

void XmlTree::showTree(QDomNode root, XmlModel *rootItem) {
    while(!root.isNull()) {
        XmlModel *newRootItem = showNode(root, rootItem);
        showTree(root.firstChild(), newRootItem);
        root = root.nextSibling();
    }
}

XmlModel *XmlTree::showNode(QDomNode node, XmlModel *root) {
    if(node.isNull() || node.isComment()) { return root; }
    XmlModel* newRow = new XmlModel(node);
    if (node.hasAttributes()) {
        auto attrs = node.attributes();
        for(int i = 0; i < attrs.size(); i++) {
            newRow->appendChild(new XmlModel(attrs.item(i)));
        }
    }
    root->appendChild(newRow);
    return newRow;
}

XmlTree::~XmlTree() {
    delete _root;
}

QModelIndex XmlTree::index(int row, int column, const QModelIndex &parent) const {
    if(!hasIndex(row, column, parent)) {return QModelIndex();}
    XmlModel *parentItem = parent.isValid() ? static_cast<XmlModel*>(parent.internalPointer()) : _root;
    XmlModel *child = parentItem->child(row);
    if(child) {
        return createIndex(row, column, child);
    } else {
        return QModelIndex();
    }
}

QModelIndex XmlTree::parent(const QModelIndex &child) const {
    if(!child.isValid()) { return QModelIndex(); }
    XmlModel *childItem = static_cast<XmlModel*>(child.internalPointer());
    XmlModel *parent = childItem->parentItem();
    if(parent == _root) { return QModelIndex(); }
    return createIndex(parent->row(), 0, parent);
}

int XmlTree:: rowCount(const QModelIndex &parent) const {
    if (parent.column() > 0) { return 0; }

    XmlModel *parentItem = parent.isValid() ? static_cast<XmlModel*>(parent.internalPointer()) :  _root;
    return parentItem->childCount();
}

int XmlTree::columnCount(const QModelIndex &parent) const {
    if(parent.isValid()) {
        return static_cast<XmlModel*>(parent.internalPointer())->columnCount();
    }
    return _root->columnCount();
}

QVariant XmlTree::data(const QModelIndex &index, int role) const {
    if(!index.isValid()) { return QVariant(); }
    switch (role) {
    case Qt::EditRole:    [[fallthrough]]; //break
    case Qt::DisplayRole: return static_cast<XmlModel*>(index.internalPointer())->data(index.column());
    default: return QVariant();
    }
}

Qt::ItemFlags XmlTree::flags(const QModelIndex &index) const {
    if(!index.isValid()) { return Qt::NoItemFlags; }
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}



bool XmlTree::removeRows(int row, int count, const QModelIndex &parent) {
    beginRemoveRows(parent, row, row + count - 1);
    if(parent.isValid()) {
        static_cast<XmlModel*>(parent.internalPointer())->removeChilds(row, count);
    } else {
        _root->removeChilds(row, count);
    }
    endRemoveRows();
    return true;
}

void XmlTree::addElement(const QModelIndex &parent) {
    if(!parent.isValid()) return;
    beginInsertRows(parent, 0, 0);
    static_cast<XmlModel*>(parent.internalPointer())->addElement(_doc.createElement("element"));
    endInsertRows();
}

void XmlTree::addText(const QModelIndex &parent) {
    if(!parent.isValid()) return;
    beginInsertRows(parent, 0, 0);
    static_cast<XmlModel*>(parent.internalPointer())->addElement(_doc.createTextNode("text"));
    endInsertRows();
}

QDomDocument XmlTree::getDocument() {
    return _doc;
}
