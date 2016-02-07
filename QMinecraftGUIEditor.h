#pragma once

#include <QQuickItem>
#include <QString>

class MinecraftJSONParser;
class MCGUIComponent;

class QMinecraftGUIEditor : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(qreal screenWidth READ screenWidth WRITE setScreenWidth NOTIFY screenWidthChanged)
    Q_PROPERTY(qreal screenHeight READ screenHeight WRITE setScreenHeight NOTIFY screenHeightChanged)
    Q_PROPERTY(QString editComponent READ editComponentString WRITE setEditComponent NOTIFY editComponentChanged)

    MinecraftJSONParser *mParser;
    MCGUIComponent *mEditComponent;
    qreal mScreenWidth, mScreenHeight;
    bool mRebuildComponent = true;

public:
    QMinecraftGUIEditor(QQuickItem *parent = 0);

    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);

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

    void setScreenWidth(qreal val);
    void setScreenHeight(qreal val);

signals:
    void screenWidthChanged(qreal val);
    void screenHeightChanged(qreal val);
    void editComponentChanged(QString const &val);

};
