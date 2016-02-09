#pragma once

#include <QImage>
#include <QColor>
#include <QString>
#include <QRectF>
#include <QList>
#include <QPointF>

class QSGTexture;
class QSGNode;

class MinecraftGUIFont
{
private:
    static const QChar defaultMappings[];
    QSGTexture *texture = nullptr;
    int textureWidth, textureHeight;
    int textureCharWidths[256];
    int textureCharWidth, textureCharHeight;

    void updateTextureCharSizes(const QImage &img);

public:
    MinecraftGUIFont();

    void setTexture(const QImage &img, QSGTexture *texture) {
        this->texture = texture;
        this->textureWidth = img.width();
        this->textureHeight = img.height();
        updateTextureCharSizes(img);
    }
    bool hasTexture() {
        return (texture != nullptr);
    }

    int getDefaultMappingId(QChar c);

    QSGNode *build(QPointF pos, const QString &text, QColor color, float scale);
    void build(QPointF pos, QList<QRectF> &rects, QList<QRectF> &uvs, const QString &text, float scale);

    int calculateWidth(const QString &text);

    int getCharHeight() {
        return textureCharHeight;
    }

};
