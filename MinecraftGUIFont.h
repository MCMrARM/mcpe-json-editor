#pragma once

#include <QImage>

class QSGTextureMaterial;

class MinecraftGUIFont
{
private:
    static const char defaultMappings[];
    QSGTextureMaterial *textureMaterial = nullptr;
    int textureWidth, textureHeight;
    int textureCharWidths[256];
    int textureCharHeight;

    void updateTextureCharSizes(const QImage &img);

public:
    MinecraftGUIFont();

    void setTexture(const QImage &img, QSGTextureMaterial *textureMaterial) {
        this->textureMaterial = textureMaterial;
        this->textureWidth = img.width();
        this->textureHeight = img.height();
        updateTextureCharSizes(img);
    }
    bool hasTexture() {
        return (textureMaterial != nullptr);
    }



};
