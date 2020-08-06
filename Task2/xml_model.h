#ifndef XML_MODEL_H
#define XML_MODEL_H

#include <QVariant>
#include <QDomNode>

class XmlModel
{
public:
    XmlModel(const QDomNode &_node);
    ~XmlModel();

    void appendChild(XmlModel *child);
    XmlModel *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    XmlModel *parentItem();
    void setParentItem(XmlModel *parent);
    bool setData(int column, QString value);
    bool removeChilds(int row, int amount);

    void addElement(QDomNode node);
    void prependChild(XmlModel *child);

    bool isElement();

private:
    QVector<XmlModel*> _childItems;
    QVector<QVariant> _itemData;
    XmlModel *_parentItem = nullptr;
    QDomNode _node;
};
#endif // XML_MODEL_H
