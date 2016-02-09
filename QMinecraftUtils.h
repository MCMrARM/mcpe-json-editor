#pragma once

#include <QList>
#include <QRectF>

class QSGGeometry;

class QMinecraftUtils
{
public:
    static void setTexturedRectsGeometry(QSGGeometry *geometry, const QList<QRectF> &rects, const QList<QRectF> &uvs);
};
