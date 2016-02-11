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
#include <QMinecraftUtils.h>
#include <assert.h>
#include "Vec4.h"
#include "QSGColorTextureMaterial.h"

QMinecraftGUIEditor::QMinecraftGUIEditor(QQuickItem *parent) : QQuickItem(parent), mScreenWidth(100.f), mScreenHeight(100.f) {
    setFlag(ItemHasContents, true);
    setFlag(ItemAcceptsInputMethod, true);
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::AllButtons);
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

void QMinecraftGUIEditor::setMinecraftPixelSize(qreal val) {
    mPixelSize = val;
    emit dpSizeChanged(val);
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

    if (!mFont.hasTexture()) {
        QImage img (QString("images/font/default8.png"));
        assert(!img.isNull());
        QSGTexture *texObj = window()->createTextureFromImage(img);
        mFont.setTexture(img, texObj);
    }

    if (mRebuildComponent) {
        mRebuildComponent = false;
        n->removeAllChildNodes();
        mouseAreas.clear();
        pressedArea = nullptr;
        QSGSimpleRectNode *container = new QSGSimpleRectNode();
        container->setColor(Qt::black);
        container->setFlag(QSGNode::OwnedByParent);
        container->setRect(5.f, 5.f, screenWidth() * mPixelSize, screenHeight() * mPixelSize);
        if (mEditComponent != nullptr) {
            qDebug() << "Building component" << mEditComponent->mcNamespace << mEditComponent->name;

            MCGUIContext context (mParser->resolvedComponents);
            context.screenSize = {mScreenWidth, mScreenHeight};

            context.variables["$win10_edition"] = QJsonValue(true);
            context.variables["$not_trial"] = QJsonValue(true);
            context.variables["$not_education_edition"] = QJsonValue(true);
            context.globalBindings["#button_a_description"] = "Select";
            context.globalBindings["#button_b_description"] = "Exit";

            QMap<int, QList<QSGNode*>> nodes;
            buildNode(nodes, context, mEditComponent, {5.f / mPixelSize, 5.f / mPixelSize}, 0);
            for (QList<QSGNode*> &list : nodes) {
                for (QSGNode *node : list) {
                    container->appendChildNode(node);
                }
            }
        }
        n->appendChildNode(container);
    }

    for (MouseArea &area : mouseAreas) {
        if (area.defaultNode != nullptr && (area.hovered || area.pressed))
            area.hoverNode->hide();
        if (area.hoverNode != nullptr && !area.hovered)
            area.hoverNode->hide();
        if (area.pressedNode != nullptr && !area.pressed)
            area.pressedNode->hide();
        if (area.pressed) {
            if (area.pressedNode != nullptr)
                area.pressedNode->show();
        } else if (area.hovered) {
            if (area.hoverNode != nullptr)
                area.hoverNode->show();
        } else {
            if (area.defaultNode != nullptr)
                area.defaultNode->show();
        }
    }

    return n;
}

void QMinecraftGUIEditor::hoverMoveEvent(QHoverEvent *event) {
    QQuickItem::hoverMoveEvent(event);
    QPointF pos = event->posF();
    qreal x = pos.x();
    qreal y = pos.y();
    for (MouseArea &area : mouseAreas) {
        area.hovered = area.inside(x, y);
    }
    update();
}

void QMinecraftGUIEditor::mousePressEvent(QMouseEvent *event) {
    if (pressedArea != nullptr)
        return;
    QPoint pos = event->pos();
    qreal x = (qreal) pos.x();
    qreal y = (qreal) pos.y();
    for (MouseArea &area : mouseAreas) {
        if (area.inside(x, y)) {
            area.pressed = true;
            pressedArea = &area;
            grabMouse();
            update();
            return;
        }
    }
}

void QMinecraftGUIEditor::mouseReleaseEvent(QMouseEvent *event) {
    if (pressedArea != nullptr) {
        pressedArea->pressed = false;
        pressedArea = nullptr;
        update();
        ungrabMouse();
    }
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
void appendImageRect(QList<QRectF> &rects, QList<QRectF> &uvs, QRectF rect, QRectF uv) {
    if (rect.width() <= 0 || rect.height() <= 0)
        return;
    rects.push_back(rect);
    uvs.push_back(uv);
}
QSGNode *buildImageNode(QSGTextureMaterial *material, const QList<QRectF> &rect, const QList<QRectF> &uv) {
    QSGGeometry *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), rect.count() * 4);
    QMinecraftUtils::setTexturedRectsGeometry(geometry, rect, uv);

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
    if (component->ignored.get(&context)) {
        context.exit();
        return nullptr;
    }
    QSGNode *ret = nullptr;
    Vec2 pos = component->getPos(&context);
    pos.x += off.x;
    pos.y += off.y;
    off = pos;
    pos = (pos * mPixelSize).floor();
    Vec2 size = component->calculateSize(&context);
    size = (size * mPixelSize).floor();
    qDebug() << "Pos:" << pos.x << pos.y << size.x << size.y;
    float alpha = 1.f;
    if (MCGUIIsOfBaseType(component, Control)) {
        MCGUIBaseControl *control = MCGUICastToType(component, MCGUIBaseControl);
        layer += control->layer.get(&context);
        if (!control->visible.get(&context)) {
            context.exit();
            return nullptr;
        }
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

        MCGUIColor color = image->color.get(&context);

        QSGTextureMaterial *material;
        if (color.r != 1.f || color.g != 1.f || color.b != 1.f || color.a != 1.f) {
            QSGColorTextureMaterial *colorMaterial = new QSGColorTextureMaterial();
            QColor qcolor;
            qcolor.setRgbF(color.r, color.g, color.b, color.a);
            colorMaterial->setColor(qcolor);
            material = colorMaterial;
        } else {
            material = new QSGTextureMaterial();
        }
        material->setTexture(tex->texture);
        Vec2 uv = image->uv.get(&context);
        Vec2 uvSize = image->uvSize.get(&context);
        Vec4 nsUv = image->ninesliceSize.get(&context);
        Vec4 ns = nsUv * mPixelSize;

        qDebug() << uv.x << uv.y << uvSize.x << uvSize.y << tex->width << tex->height;
        qDebug() << ns.x << ns.y << ns.x2 << ns.y2;

        QList<QRectF> rects, uvs;
        appendImageRect(rects, uvs, {pos.x + ns.x, pos.y + ns.y, size.x - ns.x - ns.x2, size.y - ns.y - ns.y2}, tex->getUV({uv.x + nsUv.x, uv.y + nsUv.y, uvSize.x - nsUv.x - nsUv.x2, uvSize.y - nsUv.y - nsUv.y2}));

        if (ns.x != 0.f || ns.y != 0.f || ns.x2 != 0.f || ns.y2 != 0.f) {
            appendImageRect(rects, uvs, {pos.x, pos.y, ns.x, ns.y}, tex->getUV({uv.x, uv.y, nsUv.x, nsUv.y}));
            appendImageRect(rects, uvs, {pos.x + size.x - ns.x2, pos.y, ns.x2, ns.y}, tex->getUV({uv.x + uvSize.x - nsUv.x2, uv.y, nsUv.x2, nsUv.y}));
            appendImageRect(rects, uvs, {pos.x, pos.y + size.y - ns.y2, ns.x, ns.y2}, tex->getUV({uv.x, uv.y + uvSize.y - nsUv.y2, nsUv.x, nsUv.y2}));
            appendImageRect(rects, uvs, {pos.x + size.x - ns.x2, pos.y + size.y - ns.y2, ns.x2, ns.y2}, tex->getUV({uv.x + uvSize.x - nsUv.x2, uv.y + uvSize.y - nsUv.y2, nsUv.x2, nsUv.y2}));

            appendImageRect(rects, uvs, {pos.x, pos.y + ns.y, ns.x, size.y - ns.y - ns.y2}, tex->getUV({uv.x, uv.y + nsUv.y, nsUv.x, uvSize.y - nsUv.y - nsUv.y2}));
            appendImageRect(rects, uvs, {pos.x + size.x - ns.x2, pos.y + ns.y, ns.x2, size.y - ns.y - ns.y2}, tex->getUV({uv.x + uvSize.x - nsUv.x2, uv.y + nsUv.y, nsUv.x2, uvSize.y - nsUv.y - nsUv.y2}));
            appendImageRect(rects, uvs, {pos.x + ns.x, pos.y, size.x - ns.x - ns.x2, ns.y}, tex->getUV({uv.x + nsUv.x, uv.y, uvSize.x - nsUv.x - nsUv.x2, nsUv.y}));
            appendImageRect(rects, uvs, {pos.x + ns.x, pos.y + size.y - ns.y2, size.x - ns.x - ns.x2, ns.y2}, tex->getUV({uv.x + nsUv.x, uv.y + uvSize.y - nsUv.y2, uvSize.x - nsUv.x - nsUv.x2, nsUv.y2}));
        }

        QSGNode *node = buildImageNode(material, rects, uvs);
        ret = node;
    }
        break;
    case MCGUIComponent::Type::LABEL: {
        qDebug() << "Draw label";
        MCGUIComponentLabel *label = (MCGUIComponentLabel*) component;
        MCGUIColor color = label->color.get(&context);
        QString text = label->text.get(&context);
        float fontSize = label->fontSize.get(&context) == MCGUIFontSize::SMALL ? 0.5f : 1.f;
        QColor qcolor;
        qcolor.setRgbF(color.r, color.g, color.b, color.a);
        Vec2 textOff = MCGUIGetAnchorPoint(label->calculateSize(&context), label->alignment.get(&context));
        textOff = textOff - MCGUIGetAnchorPoint({mFont.calculateWidth(text) * fontSize, mFont.getCharHeight() * fontSize}, label->alignment.get(&context));
        textOff = (textOff * mPixelSize).floor();
        QSGNode *node = mFont.build({pos.x + textOff.x, pos.y + textOff.y}, text, qcolor, mPixelSize * fontSize);
        node->setFlag(QSGNode::OwnedByParent);
        if (label->shadow.get(&context)) {
            QColor qcolor2;
            qcolor2.setRgbF(color.r / 4, color.g / 4, color.b / 4, color.a);
            QSGNode *node2 = mFont.build({pos.x + textOff.x + mPixelSize * fontSize, pos.y + textOff.y + mPixelSize * fontSize}, text, qcolor2, mPixelSize * fontSize);
            node2->setFlag(QSGNode::OwnedByParent);
            node2->appendChildNode(node);
            node = node2;
        }
        ret = node;
    }
        break;
    case MCGUIComponent::Type::PANEL:
    case MCGUIComponent::Type::INPUT_PANEL: {
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

        if (MCGUIIsOfBaseType(component, ButtonComponent)) {
            MCGUIBaseButtonComponent *btn = MCGUICastToType(component, MCGUIBaseButtonComponent);
            QString defaultControl = btn->defaultControl.get(&context).componentName;
            QString hoverControl = btn->hoverControl.get(&context).componentName;
            QString pressedControl = btn->pressedControl.get(&context).componentName;
            MouseArea area;
            area.area = QRectF(pos.x, pos.y, size.x, size.y);
            for (auto& control : component->controls) {
                MCGUIComponent *child = control.get(&context);
                if (child != nullptr) {
                    if (child->name == defaultControl || child->name == hoverControl || child->name == pressedControl) {
                        QMap<int, QList<QSGNode*>> subNodes;
                        buildNode(subNodes, context, child, off, layer);
                        QSGNode *container = new QSGNode();
                        for (QList<QSGNode*> &list : subNodes) {
                            for (QSGNode *node : list) {
                                container->appendChildNode(node);
                            }
                        }
                        if (child->name == defaultControl) {
                            area.defaultNode = new QSGHideableNode(container);
                            nodes[layer].push_back(&area.defaultNode->realNode);
                        } else if (child->name == hoverControl) {
                            area.hoverNode =  new QSGHideableNode(container);
                            nodes[layer].push_back(&area.hoverNode->realNode);
                        } else if (child->name == pressedControl) {
                            area.pressedNode =  new QSGHideableNode(container);
                            nodes[layer].push_back(&area.pressedNode->realNode);
                        }
                    } else {
                        buildNode(nodes, context, child, off, layer);
                    }
                }
            }
            mouseAreas.push_back(area);
        } else {
            for (auto& control : component->controls) {
                MCGUIComponent *child = control.get(&context);
                if (child != nullptr) {
                    buildNode(nodes, context, child, off, layer);
                }
            }
        }
    }
    context.exit();
    return ret;
}
