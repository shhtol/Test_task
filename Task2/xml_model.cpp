#include "xml_model.h"
#include <QDebug>


XmlModel::XmlModel(const QDomNode &node) {
    this->_node = node;
}

XmlModel::~XmlModel() {
    qDeleteAll(_childItems);
}

void XmlModel::appendChild(XmlModel *child) {
    child->setParentItem(this);
    _childItems.append(child); 
}

void XmlModel::prependChild(XmlModel *child) {
    child->setParentItem(this);
    _childItems.prepend(child);
}

bool XmlModel::isElement() {
    return _node.isElement();
}

void XmlModel::addElement(QDomNode node) {
    XmlModel *item = new XmlModel(node);
    this->_node.appendChild(node);
    appendChild(item);
}

bool XmlModel::removeChilds(int row, int amount) {
    if(row < 0 || row > _childItems.size()) return false;
    if(amount < 0 || amount > _childItems.size()) return false;
    for(int i = row; i < row + amount; i++) {
        if(_childItems[i]->_node.isAttr()) {
            auto attr = _childItems[i]->_node.toAttr();
            _node.toElement().removeAttributeNode(attr);
        } else {
            _node.removeChild(_childItems[i]->_node);
        }
        delete _childItems[i];
    }
    _childItems.remove(row, amount);
    return true;
}

XmlModel *XmlModel::child(int row) {
    if(row < 0 || row >= _childItems.size()) return nullptr;
    return _childItems.at(row);
}

int XmlModel::childCount() const {
    return _childItems.size();
}

int XmlModel::columnCount() const {
    return 2;
}

QVariant XmlModel::data(int column) const {
    switch (column) {
    case 0:  return _node.nodeName();
    case 1:  return _node.nodeValue();
    default: return "NULL";
    }
}

int XmlModel::row() const{
    if(_parentItem) { return _parentItem->_childItems.indexOf(const_cast<XmlModel*>(this)); }
    return 0;
}

XmlModel *XmlModel::parentItem() {
    return _parentItem;
}

void XmlModel::setParentItem(XmlModel *parent) {
    _parentItem = parent;
}

bool XmlModel::setData(int column, QString value) {
    if(column == 0) {
        if(value[0].isDigit() || value.indexOf(' ') != -1) return false;
        if(_node.isElement()) {
            _node.toElement().setTagName(value);
        } else if(_node.isAttr()) {
            auto parent = _node.toAttr().ownerElement();
            if(parent.hasAttribute(value)) return false;
            auto doc = _node.ownerDocument();
            auto newAttr = doc.createAttribute(value);
            newAttr.setValue(_node.toAttr().value());
            parent.removeAttributeNode(_node.toAttr());
            parent.setAttributeNode(newAttr);
            _node = newAttr;
        }
    } else if(column == 1) {
        if(!_node.isElement()) { _node.setNodeValue(value); }
    }
    return true;
}
