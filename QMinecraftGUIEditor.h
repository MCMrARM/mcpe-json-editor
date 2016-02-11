#pragma once

#include <QQuickItem>
#include <QString>
#include <QMap>
#include <QRectF>
#include "Vec2.h"
#include "Vec4.h"
#include "MinecraftGUIFont.h"

class MinecraftJSONParser;
class MCGUIComponent;
class MCGUIContext;
class QSGTexture;
class QSGHideableNode;

class QMinecraftGUIEditor : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(qreal screenWidth READ screenWidth WRITE setScreenWidth NOTIFY screenWidthChanged)
    Q_PROPERTY(qreal screenHeight READ screenHeight WRITE setScreenHeight NOTIFY screenHeightChanged)
    Q_PROPERTY(qreal dpSize READ dpSize WRITE setMinecraftPixelSize NOTIFY dpSizeChanged)
    Q_PROPERTY(QString editComponent READ editComponentString WRITE setEditComponent NOTIFY editComponentChanged)

    MinecraftJSONParser *mParser;
    MCGUIComponent *mEditComponent = nullptr;
    MinecraftGUIFont mFont;
    qreal mScreenWidth, mScreenHeight;
    bool mRebuildComponent = true;
    bool mDrawDebugLines = false;
    qreal mPixelSize = 2.f;

    struct TextureInfo {
        QSGTexture* texture;
        int width, height;

        QRectF getUV(Vec4 r);
    };

    QMap<QString, TextureInfo*> textures;
    TextureInfo *getTexture(QString tex);

    struct HideableNode {
        //
    };

    struct MouseArea {
        QRectF area;
        bool hovered = false;
        bool pressed = false;
        QSGHideableNode *defaultNode = nullptr;
        QSGHideableNode *hoverNode = nullptr;
        QSGHideableNode *pressedNode = nullptr;

        bool inside(qreal x, qreal y) {
            return (x >= area.x() && x <= area.x() + area.width() &&
                    y >= area.y() && y <= area.y() + area.height());
        }
    };

    QList<MouseArea> mouseAreas;
    MouseArea *pressedArea = nullptr;

public:
    QMinecraftGUIEditor(QQuickItem *parent = 0);

    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);

    QSGNode *buildNode(QMap<int, QList<QSGNode*>> &nodes, MCGUIContext &context, MCGUIComponent *component, Vec2 off, int layer);

    void setMinecraftJSONParser(MinecraftJSONParser *parser) {
        mParser = parser;
    }

    QString editComponentString();

    void setEditComponent(QString const &el);
    void setEditComponent(MCGUIComponent *component);

    qreal screenWidth() const {
        return mScreenWidth;
    }
    qreal screenHeight() const {
        return mScreenHeight;
    }
    qreal dpSize() const {
        return mPixelSize;
    }

    void setScreenWidth(qreal val);
    void setScreenHeight(qreal val);

    void setMinecraftPixelSize(qreal val);

    void hoverMoveEvent(QHoverEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void screenWidthChanged(qreal val);
    void screenHeightChanged(qreal val);
    void dpSizeChanged(qreal val);
    void editComponentChanged(QString const &val);

};
