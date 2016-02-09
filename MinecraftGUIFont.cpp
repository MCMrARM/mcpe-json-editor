#include "MinecraftGUIFont.h"
#include "QSGColorTextureMaterial.h"
#include <QSGGeometry>
#include <QSGGeometryNode>
#include "QMinecraftUtils.h"

const QChar MinecraftGUIFont::defaultMappings[] = {
    0xc0, 0xc1, 0xc2, 0xc8, 0xca, 0xcb, 0xcd, 0xd3, 0xd4, 0xd5, 0xda, 0xdf, 0xe3, 0xf5, 0x11f, 0x131,
    0x131, 0x152, 0x153, 0x15e, 0x15f, 0x174, 0x175, 0x17e, 0x207, 0, 0, 0, 0, 0, 0, 0,
    ' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
    '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
    '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', 0,
    0xc7, 0xfc, 0xe9, 0xe2, 0xe4, 0xe0, 0xe5, 0xe7, 0xea, 0xeb, 0xe8, 0xef, 0xee, 0xec, 0xc4, 0xc5,
    0xc9, 0xe6, 0xc6, 0xf4, 0xf6, 0xf2, 0xfb, 0xf9, 0xff, 0xd6, 0xdc, 0xf8, 0xa3, 0xd8, 0xd7, 0x192,
    0xe1, 0xed, 0xf3, 0xfa, 0xf1, 0xd1, 0xaa, 0xba, 0xbf, 0xae, 0xac, 0xbd, 0xbc, 0xa1, 0xab, 0xbb,
    0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556, 0x2555, 0x2563, 0x2551, 0x2557, 0x255d, 0x255c, 0x255b, 0x2510,
    0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x255e, 0x255f, 0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x2567,
    0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256b, 0x256a, 0x2518, 0x250c, 0x2588, 0x2584, 0x258c, 0x2590, 0x2580,
    0x3b1, 0x3b2, 0x393, 0x3c0, 0x3a3, 0x3c3, 0x3bc, 0x3c4, 0x3a6, 0x398, 0x3a9, 0x3b4, 0x221e, 0x2205, 0x2208, 0x2229,
    0x2261, 0xb1, 0x2265, 0x2264, 0x2320, 0x2321, 0xf7, 0x2248, 0xb0, 0x2219, 0xb7, 0x221a, 0x207f, 0xb2, 0x25a0, 0
};

MinecraftGUIFont::MinecraftGUIFont()
{

}

void MinecraftGUIFont::updateTextureCharSizes(const QImage &img) {
    textureCharWidth = img.width() / 16;
    textureCharHeight = img.height() / 16;
    int x = 0;
    for (int i = 0; i < 256; i++) {
        int baseX = (i % 16) * textureCharWidth;
        int baseY = (i / 16) * textureCharHeight;
        int w = textureCharWidth - 1;
        int maxY = baseY + textureCharHeight;
        for ( ; w >= 0; w--) {
            int x = baseX + w;
            bool foundPixel = false;
            for (int y = baseY; y < maxY; y++) {
                if (i == 84)
                    qDebug() << x << y << qAlpha(img.pixel(x, y));
                if (qAlpha(img.pixel(x, y)) > 0) {
                    foundPixel = true;
                    break;
                }
            }
            if (foundPixel)
                break;
        }
        textureCharWidths[i] = w + 1;
    }
}

int MinecraftGUIFont::getDefaultMappingId(QChar c) {
    for (int i = 0; i < 256; i++) {
        if (defaultMappings[i] == c)
            return i;
    }
    return -1;
}

QSGNode *MinecraftGUIFont::build(QPointF pos, const QString &text, QColor color, float scale) {
    QSGColorTextureMaterial *material = new QSGColorTextureMaterial();
    material->setColor(color);
    material->setTexture(texture);

    QList<QRectF> rects, uvs;
    build(pos, rects, uvs, text, scale);

    QSGGeometry *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), rects.count() * 4);
    QMinecraftUtils::setTexturedRectsGeometry(geometry, rects, uvs);

    QSGGeometryNode *node = new QSGGeometryNode();
    node->setMaterial(material);
    node->setGeometry(geometry);
    node->setFlag(QSGNode::OwnsMaterial);
    node->setFlag(QSGNode::OwnsGeometry);
    return node;
}

void MinecraftGUIFont::build(QPointF pos, QList<QRectF> &rects, QList<QRectF> &uvs, const QString &text, float scale) {
    int bw = textureCharWidth;
    int h = textureCharHeight;
    for (QChar c : text) {
        int i = getDefaultMappingId(c);
        if (i == -1)
            continue;

        int texX = (i % 16) * bw;
        int texY = (i / 16) * h;

        int w = textureCharWidths[i];

        rects.push_back(QRectF(pos.x(), pos.y(), w * scale, h * scale));
        uvs.push_back(QRectF((float) texX/textureWidth, (float) texY/textureHeight, (float) w/textureWidth, (float) h/textureHeight));

        pos.setX(pos.x() + w * scale + 1);
    }
}

int MinecraftGUIFont::calculateWidth(const QString &text) {
    int w = 0;
    for (QChar c : text) {
        int i = getDefaultMappingId(c);
        if (i == -1)
            continue;
        w += textureCharWidths[i] + 1;
    }
    return (w > 0 ? (w - 1) : 0);
}
