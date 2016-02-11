#include "QMinecraftUtils.h"

#include <QSGNode>
#include <QSGGeometry>
#include <assert.h>
#include <QDebug>

void QMinecraftUtils::setTexturedRectsGeometry(QSGGeometry *geometry, const QList<QRectF> &rects, const QList<QRectF> &uvs) {
    geometry->setDrawingMode(GL_TRIANGLES);
    assert(rects.count() == uvs.count());
    if (geometry->vertexCount() != rects.count() * 6) {
        geometry->allocate(rects.count() * 6);
    }
    auto itRects = rects.begin();
    auto itUvs = uvs.begin();
    QSGGeometry::TexturedPoint2D *v = geometry->vertexDataAsTexturedPoint2D();
    int off = 0;
    while (itRects != rects.end() && itUvs != uvs.end()) {
        QRectF rect = *itRects;
        QRectF uv = *itUvs;
        v[off].x = rect.left();
        v[off].y = rect.top();
        v[off].tx = uv.left();
        v[off].ty = uv.top();
        off++;
        v[off].x = rect.right();
        v[off].y = rect.top();
        v[off].tx = uv.right();
        v[off].ty = uv.top();
        off++;
        v[off].x = rect.left();
        v[off].y = rect.bottom();
        v[off].tx = uv.left();
        v[off].ty = uv.bottom();
        off++;
        v[off].x = rect.right();
        v[off].y = rect.top();
        v[off].tx = uv.right();
        v[off].ty = uv.top();
        off++;
        v[off].x = rect.left();
        v[off].y = rect.bottom();
        v[off].tx = uv.left();
        v[off].ty = uv.bottom();
        off++;
        v[off].x = rect.right();
        v[off].y = rect.bottom();
        v[off].tx = uv.right();
        v[off].ty = uv.bottom();
        off++;
        itRects++;
        itUvs++;
    }
}

QSGHideableNode::QSGHideableNode(QSGNode *node) : node(node) {
    show();
}

void QSGHideableNode::show() {
    if (node->parent() == nullptr)
        realNode.appendChildNode(node);
}

void QSGHideableNode::hide() {
    if (node->parent() == &realNode)
        realNode.removeChildNode(node);
}
