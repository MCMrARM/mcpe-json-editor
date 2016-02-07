#include "QMinecraftGUIEditor.h"
#include "MinecraftGUIComponent.h"
#include "MinecraftJSONParser.h"
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGSimpleRectNode>
#include <QDebug>

QMinecraftGUIEditor::QMinecraftGUIEditor(QQuickItem *parent) : QQuickItem(parent), mScreenWidth(100.f), mScreenHeight(100.f) {
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

QSGNode *QMinecraftGUIEditor::updatePaintNode(QSGNode *node, UpdatePaintNodeData *) {
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

            MCGUIContext context (mParser->resolvedComponents);
            context.screenSize = {mScreenWidth, mScreenHeight};

            QSGNode *node = buildNode(context, mEditComponent, {5.f, 5.f});
            if (node != nullptr)
                container->appendChildNode(node);
        }
        n->appendChildNode(container);
    }

    return n;
}

QSGNode *QMinecraftGUIEditor::buildNode(MCGUIContext &context, MCGUIComponent *component, Vec2 off) {
    qDebug() << "Draw:" << (int) component->type << component->name;
    context.enter(component);
    QSGNode *ret = nullptr;
    Vec2 pos = component->getPos(&context);
    pos.x += off.x;
    pos.y += off.y;
    off = pos;
    Vec2 size = component->calculateSize(&context);
    qDebug() << "Pos:" << pos.x << pos.y << size.x << size.y;
    switch (component->type) {
    case MCGUIComponent::Type::IMAGE: {
        qDebug() << "Draw image";
        QSGSimpleRectNode *node = new QSGSimpleRectNode();
        node->setColor(Qt::red);
        node->setRect(pos.x, pos.y, size.x, size.y);
        ret = node;
    }
        break;
    case MCGUIComponent::Type::PANEL: {
        qDebug() << "Draw panel";
        QSGSimpleRectNode *node = new QSGSimpleRectNode();
        node->setColor(Qt::transparent);
        node->setRect(pos.x, pos.y, size.x, size.y);
        ret = node;
    }
        break;
    case MCGUIComponent::Type::SCREEN: {
        qDebug() << "Draw screen";
        QSGSimpleRectNode *node = new QSGSimpleRectNode();
        node->setColor(Qt::black);
        node->setRect(pos.x, pos.y, size.x, size.y);
        ret = node;
    }
        break;
    }
    if (ret != nullptr) {
        if (mDrawDebugLines) {
            QSGGeometryNode *borderNode = new QSGGeometryNode();
            QSGGeometry *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 5);
            geometry->setLineWidth(1);
            geometry->setDrawingMode(GL_LINE_STRIP);
            borderNode->setGeometry(geometry);
            QSGFlatColorMaterial *material = new QSGFlatColorMaterial();
            material->setColor(QColor(255, 0, 0));
            borderNode->setMaterial(material);
            borderNode->setFlag(QSGNode::OwnsMaterial);
            borderNode->markDirty(QSGNode::DirtyGeometry);
            QSGGeometry::Point2D *vertices = geometry->vertexDataAsPoint2D();
            vertices[0].set(pos.x, pos.y);
            vertices[1].set(pos.x + size.x, pos.y);
            vertices[2].set(pos.x + size.x, pos.y + size.y);
            vertices[3].set(pos.x, pos.y + size.y);
            vertices[4].set(pos.x, pos.y);
            ret->appendChildNode(borderNode);
        }

        for (auto& control : component->controls) {
            MCGUIComponent *child = control.get(&context);
            if (child != nullptr) {
                QSGNode *node = buildNode(context, child, off);
                if (node != nullptr)
                    ret->appendChildNode(node);
            }
        }
    }
    context.exit();
    return ret;
}
