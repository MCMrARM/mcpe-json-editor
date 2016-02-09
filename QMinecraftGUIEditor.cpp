#include "QMinecraftGUIEditor.h"
#include "MinecraftGUIComponent.h"
#include "MinecraftJSONParser.h"
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGSimpleRectNode>
#include <QSGSimpleTextureNode>
#include <QSGTextureMaterial>
#include <QQuickWindow>
#include <QDebug>
#include <assert.h>
#include "Vec4.h"

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
        container->setFlag(QSGNode::OwnedByParent);
        container->setRect(5.f, 5.f, screenWidth(), screenHeight());
        if (mEditComponent != nullptr) {
            qDebug() << "Building component" << mEditComponent->mcNamespace << mEditComponent->name;

            MCGUIContext context (mParser->resolvedComponents);
            context.screenSize = {mScreenWidth, mScreenHeight};

            context.variables["$win10_edition"] = QJsonValue(true);

            QMap<int, QList<QSGNode*>> nodes;
            buildNode(nodes, context, mEditComponent, {5.f, 5.f}, 0);
            for (QList<QSGNode*> &list : nodes) {
                for (QSGNode *node : list) {
                    container->appendChildNode(node);
                }
            }
        }
        n->appendChildNode(container);
    }

    return n;
}

QMinecraftGUIEditor::TextureInfo *QMinecraftGUIEditor::getTexture(QString tex) {
    qDebug() << "getTexture:" << tex;
    if (textures.contains(tex))
        return textures[tex];
    QImage img (QString("images/") + tex);
    assert(!img.isNull());
    QSGTexture *texObj = window()->createTextureFromImage(img);
    if (texObj == nullptr)
        return nullptr;
    TextureInfo* info = new TextureInfo();
    info->texture = texObj;
    info->width = img.width();
    info->height = img.height();
    textures[tex] = info;
    return info;
}

QRectF QMinecraftGUIEditor::TextureInfo::getUV(Vec4 r) {
    QRectF uv;
    uv.setX(r.x / width);
    uv.setY(r.y / height);
    uv.setWidth(r.x2 / width);
    uv.setHeight(r.y2 / height);
    return uv;
}

QSGNode *buildImageNode(QSGTextureMaterial *material, QRectF rect, QRectF uv) {
    if (rect.width() <= 0 || rect.height() <= 0)
        return nullptr;
    QSGGeometry *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4);
    QSGGeometry::updateTexturedRectGeometry(geometry, rect, uv);

    QSGGeometryNode *node = new QSGGeometryNode();
    node->setFlag(QSGNode::OwnedByParent);
    node->setMaterial(material);
    node->setGeometry(geometry);
    node->setFlag(QSGNode::OwnsMaterial);
    node->setFlag(QSGNode::OwnsGeometry);
    return node;
}

QSGNode *QMinecraftGUIEditor::buildNode(QMap<int, QList<QSGNode*>> &nodes, MCGUIContext &context, MCGUIComponent *component, Vec2 off, int layer) {
    qDebug() << "Draw:" << (int) component->type << component->mcNamespace << "." << component->name;
    if (component->base != nullptr)
        qDebug() << "@" << component->base->mcNamespace << "." << component->base->name;
    context.enter(component);
    QSGNode *ret = nullptr;
    Vec2 pos = component->getPos(&context);
    pos.x += off.x;
    pos.y += off.y;
    off = pos;
    Vec2 size = component->calculateSize(&context);
    qDebug() << "Pos:" << pos.x << pos.y << size.x << size.y;
    float alpha = 1.f;
    if (MCGUIIsOfBaseType(component, Control)) {
        MCGUIBaseControl *control = MCGUICastToType(component, MCGUIBaseControl);
        layer += control->layer.get(&context);
    }

    switch (component->type) {
    case MCGUIComponent::Type::BUTTON: {
        qDebug() << "Draw button";
        QSGSimpleRectNode *node = new QSGSimpleRectNode();
        node->setFlag(QSGNode::OwnedByParent);
        node->setColor(Qt::transparent);
        node->setRect(pos.x, pos.y, size.x, size.y);
        ret = node;
    }
        break;
    case MCGUIComponent::Type::IMAGE: {
        qDebug() << "Draw image";
        MCGUIComponentImage *image = (MCGUIComponentImage*) component;
        alpha = image->alpha.get(&context);
        TextureInfo *tex = getTexture(image->texture.get(&context));
        assert(tex != nullptr);

        QSGTextureMaterial *material = new QSGTextureMaterial();
        material->setTexture(tex->texture);
        Vec2 uv = image->uv.get(&context);
        Vec2 uvSize = image->uvSize.get(&context);
        Vec4 ns = image->ninesliceSize.get(&context);

        qDebug() << uv.x << uv.y << uvSize.x << uvSize.y << tex->width << tex->height;
        qDebug() << ns.x << ns.y << ns.x2 << ns.y2;

        QSGNode *node = buildImageNode(material, {pos.x + ns.x, pos.y + ns.y, size.x - ns.x - ns.x2, size.y - ns.y - ns.y2}, tex->getUV({uv.x + ns.x, uv.y + ns.y, uvSize.x - ns.x - ns.x2, uvSize.y - ns.y - ns.y2}));

        if (ns.x != 0.f || ns.y != 0.f || ns.x2 != 0.f || ns.y2 != 0.f) {
            QSGNode *nodeTopLeft = buildImageNode(material, {pos.x, pos.y, ns.x, ns.y}, tex->getUV({uv.x, uv.y, ns.x, ns.y}));
            QSGNode *nodeTopRight = buildImageNode(material, {pos.x + size.x - ns.x2, pos.y, ns.x2, ns.y}, tex->getUV({uv.x + uvSize.x - ns.x2, uv.y, ns.x2, ns.y}));
            QSGNode *nodeBottomLeft = buildImageNode(material, {pos.x, pos.y + size.y - ns.y2, ns.x, ns.y2}, tex->getUV({uv.x, uv.y + uvSize.y - ns.y2, ns.x, ns.y2}));
            QSGNode *nodeBottomRight = buildImageNode(material, {pos.x + size.x - ns.x2, pos.y + size.y - ns.y2, ns.x2, ns.y2}, tex->getUV({uv.x + uvSize.x - ns.x2, uv.y + uvSize.y - ns.y2, ns.x2, ns.y2}));

            QSGNode *nodeLeft = buildImageNode(material, {pos.x, pos.y + ns.y, ns.x, size.y - ns.y - ns.y2}, tex->getUV({uv.x, uv.y + ns.y, ns.x, uvSize.y - ns.y - ns.y2}));
            QSGNode *nodeRight = buildImageNode(material, {pos.x + size.x - ns.x2, pos.y + ns.y, ns.x2, size.y - ns.y - ns.y2}, tex->getUV({uv.x + uvSize.x - ns.x2, uv.y + ns.y, ns.x2, uvSize.y - ns.y - ns.y2}));
            QSGNode *nodeTop = buildImageNode(material, {pos.x + ns.x, pos.y, size.x - ns.x - ns.x2, ns.y}, tex->getUV({uv.x + ns.x, uv.y, uvSize.x - ns.x - ns.x2, ns.y}));
            QSGNode *nodeBottom = buildImageNode(material, {pos.x + ns.x, pos.y + size.y - ns.y2, size.x - ns.x - ns.x2, ns.y2}, tex->getUV({uv.x + ns.x, uv.y + uvSize.y - ns.y2, uvSize.x - ns.x - ns.x2, ns.y2}));
            if (nodeTopLeft != nullptr)
                node->appendChildNode(nodeTopLeft);
            if (nodeTopRight != nullptr)
                node->appendChildNode(nodeTopRight);
            if (nodeBottomLeft != nullptr)
                node->appendChildNode(nodeBottomLeft);
            if (nodeBottomRight != nullptr)
                node->appendChildNode(nodeBottomRight);
            if (nodeLeft != nullptr)
                node->appendChildNode(nodeLeft);
            if (nodeRight != nullptr)
                node->appendChildNode(nodeRight);
            if (nodeTop != nullptr)
                node->appendChildNode(nodeTop);
            if (nodeBottom != nullptr)
                node->appendChildNode(nodeBottom);
        }
        ret = node;
    }
        break;
    case MCGUIComponent::Type::PANEL: {
        qDebug() << "Draw panel";
        QSGSimpleRectNode *node = new QSGSimpleRectNode();
        node->setFlag(QSGNode::OwnedByParent);
        node->setColor(Qt::transparent);
        node->setRect(pos.x, pos.y, size.x, size.y);
        ret = node;
    }
        break;
    case MCGUIComponent::Type::SCREEN: {
        qDebug() << "Draw screen";
        QSGSimpleRectNode *node = new QSGSimpleRectNode();
        node->setFlag(QSGNode::OwnedByParent);
        node->setColor(Qt::green);
        node->setRect(pos.x, pos.y, size.x, size.y);
        ret = node;
    }
        break;
    }
    if (ret != nullptr) {
        if (alpha != 1.f) {
            //QSGNode *container = new QSGNode();
            QSGOpacityNode *node = new QSGOpacityNode();
            node->setFlag(QSGNode::OwnedByParent);
            node->setOpacity(alpha);
            node->appendChildNode(ret);
            //container->appendChildNode(node);
            ret = node;
        }

        if (mDrawDebugLines) {
            QSGGeometryNode *borderNode = new QSGGeometryNode();
            borderNode->setFlag(QSGNode::OwnedByParent);
            QSGGeometry *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 5);
            geometry->setLineWidth(1);
            geometry->setDrawingMode(GL_LINE_STRIP);
            borderNode->setGeometry(geometry);
            borderNode->setFlag(QSGNode::OwnsGeometry);
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

        nodes[layer].push_back(ret);

        for (auto& control : component->controls) {
            MCGUIComponent *child = control.get(&context);
            if (child != nullptr) {
                buildNode(nodes, context, child, off, layer);
            }
        }
    }
    context.exit();
    return ret;
}
