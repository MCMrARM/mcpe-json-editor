#include "QMinecraftGUIEditor.h"
#include "MinecraftGUIComponent.h"
#include "MinecraftJSONParser.h"
#include <QSGSimpleRectNode>
#include <QDebug>

QMinecraftGUIEditor::QMinecraftGUIEditor(QQuickItem *parent) : QQuickItem(parent), mScreenWidth(100.f), mScreenHeight(100.f)
{
    setFlag(ItemHasContents, true);
}

void QMinecraftGUIEditor::setEditComponent(QString const &el) {
    setEditComponent(mParser->resolvedComponents[el]);
}

void QMinecraftGUIEditor::setEditComponent(MCGUIComponent *component) {
    mEditComponent = component;
    mRebuildComponent = true;
    update();
}

void QMinecraftGUIEditor::setScreenWidth(qreal val) {
    mScreenWidth = val;
    emit screenWidthChanged(val);
    mRebuildComponent = true;
    update();
}

void QMinecraftGUIEditor::setScreenHeight(qreal val) {
    mScreenHeight = val;
    emit screenHeightChanged(val);
    mRebuildComponent = true;
    update();
}

QString QMinecraftGUIEditor::editComponentString() {
    return (mEditComponent == nullptr ? "" : (mEditComponent->mcNamespace + "." + mEditComponent->name));
}

QSGNode *QMinecraftGUIEditor::updatePaintNode(QSGNode *node, UpdatePaintNodeData *)
{
    QSGSimpleRectNode *n = static_cast<QSGSimpleRectNode *>(node);
    if (!n) {
        n = new QSGSimpleRectNode();
        n->setColor(Qt::gray);
    }
    n->setRect(boundingRect());

    if (mRebuildComponent) {
        n->removeAllChildNodes();
        QSGSimpleRectNode *container = new QSGSimpleRectNode();
        container->setColor(Qt::black);
        container->setRect(5.f, 5.f, screenWidth(), screenHeight());
        if (mEditComponent != nullptr) {
            qDebug() << "Building component" << mEditComponent->mcNamespace << mEditComponent->name;
        }
        n->appendChildNode(container);
    }

    return n;
}
