/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtQuick module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "QSGColorTextureMaterial.h"

#include <QSGMaterialShader>
#include <QtGui/qopenglshaderprogram.h>
#include <QtGui/qopenglfunctions.h>

inline static bool isPowerOfTwo(int x) {
    // Assumption: x >= 1
    return x == (x & -x);
}

QSGMaterialType QSGColorTextureMaterialShader::type;

QSGMaterialType *QSGColorTextureMaterial::type() const {
    return &QSGColorTextureMaterialShader::type;
}

QSGMaterialShader *QSGColorTextureMaterial::createShader() const {
    return new QSGColorTextureMaterialShader;
}

int QSGColorTextureMaterial::compare(const QSGMaterial *o) const
{
    Q_ASSERT(o && type() == o->type());
    const QSGColorTextureMaterial *other = static_cast<const QSGColorTextureMaterial *>(o);
    if (int diff = m_texture->textureId() - other->texture()->textureId())
        return diff;
    if (int diff = int(m_filtering) - int(other->m_filtering))
        return diff;
    return m_color.rgba() - other->color().rgba();
}

QSGColorTextureMaterialShader::QSGColorTextureMaterialShader() {
    setShaderSourceFile(QOpenGLShader::Vertex, QStringLiteral(":/texture.vert"));
    setShaderSourceFile(QOpenGLShader::Fragment, QStringLiteral(":/texture.frag"));
}

void QSGColorTextureMaterialShader::updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect) {
    Q_ASSERT(oldEffect == 0 || newEffect->type() == oldEffect->type());

    QSGColorTextureMaterial *tx = static_cast<QSGColorTextureMaterial *>(newEffect);
    QSGColorTextureMaterial *oldTx = static_cast<QSGColorTextureMaterial *>(oldEffect);

    QSGTexture *t = tx->texture();

    t->setFiltering(tx->filtering());

    t->setHorizontalWrapMode(tx->horizontalWrapMode());
    t->setVerticalWrapMode(tx->verticalWrapMode());
    bool npotSupported = const_cast<QOpenGLContext *>(state.context())
        ->functions()->hasOpenGLFeature(QOpenGLFunctions::NPOTTextureRepeat);
    if (!npotSupported) {
        QSize size = t->textureSize();
        const bool isNpot = !isPowerOfTwo(size.width()) || !isPowerOfTwo(size.height());
        if (isNpot) {
            t->setHorizontalWrapMode(QSGTexture::ClampToEdge);
            t->setVerticalWrapMode(QSGTexture::ClampToEdge);
        }
    }

    t->setMipmapFiltering(tx->mipmapFiltering());

    if (oldTx == 0 || oldTx->texture()->textureId() != t->textureId())
        t->bind();
    else
        t->updateBindOptions();

    if (state.isMatrixDirty())
        program()->setUniformValue(m_matrix_id, state.combinedMatrix());

    const QColor &c = tx->color();
    if (oldTx == 0 || c != oldTx->color() || state.isOpacityDirty()) {
        float opacity = state.opacity() * c.alphaF();
        QVector4D v(c.redF() * opacity,
                    c.greenF() *  opacity,
                    c.blueF() * opacity,
                    opacity);
        qDebug() << c.redF() << c.greenF() << c.blueF() << c.alphaF();
        program()->setUniformValue(m_color_id, v);
    }
}

char const *const *QSGColorTextureMaterialShader::attributeNames() const {
    static char const *const attr[] = { "qt_VertexPosition", "qt_VertexTexCoord", 0 };
    return attr;
}

void QSGColorTextureMaterialShader::initialize() {
    m_matrix_id = program()->uniformLocation("qt_Matrix");
    m_color_id = program()->uniformLocation("color");
}
