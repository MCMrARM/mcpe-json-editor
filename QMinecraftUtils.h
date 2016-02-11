#pragma once

#include <QList>
#include <QRectF>
#include <QSGNode>

class QSGGeometry;

class QMinecraftUtils
{
public:
    static void setTexturedRectsGeometry(QSGGeometry *geometry, const QList<QRectF> &rects, const QList<QRectF> &uvs);

};

class QSGHideableNode {

public:
    QSGNode realNode;

    QSGNode *node;

    QSGHideableNode(QSGNode *node);

    void show();
    void hide();

};
