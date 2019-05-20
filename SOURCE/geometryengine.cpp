/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "geometryengine.h"

#include <QVector2D>
#include <QVector3D>


//! [0]
GeometryEngine::GeometryEngine()
    : index_buf(QOpenGLBuffer::IndexBuffer), m_texture(0)
{
     scale_proportion = 1.0f;
     radius = 1.0f;
}

GeometryEngine::GeometryEngine(const QVector<VertexData> &vertex_data, const QVector<GLuint> &index_data, const QImage texture_img )
    : index_buf(QOpenGLBuffer::IndexBuffer), m_texture(0)
{
    global_transform.setToIdentity();
    scale_proportion = 1.0f;
    translation = QVector3D(-4.0f, 0.0f, 0.0f);
    //rotation  = QQuaternion(0.0, 0.0, 0.0, 1.0);
    rotation = QQuaternion::fromAxisAndAngle(QVector3D(0.0, 0.0, 1.0).normalized(), qCos(1));
    //rotation *= QQuaternion::fromAxisAndAngle(QVector3D(0.0, 1.0, 0.0).normalized(), qSin(30));

    init(vertex_data, index_data, texture_img);
}

GeometryEngine::~GeometryEngine()
{
    vertex_buf.destroy();
    index_buf.destroy();
}

void GeometryEngine::init(const QVector<VertexData> &vert_data, const QVector<GLuint> &index_data, const QImage texture_img)
{
    if(vertex_buf.isCreated())
        vertex_buf.destroy();
    if(index_buf.isCreated())
        index_buf.destroy();
    if(m_texture != 0) {
        if(m_texture->isCreated()){
            delete m_texture;
        }
    }

    // Generate 2 VBOs
    // Transfer vertex data to VBO 0
    vertex_buf.create();
    vertex_buf.bind();
    vertex_buf.allocate(vert_data.constData(), 24 * sizeof(VertexData));
    vertex_buf.release();

    // Transfer index data to VBO 1
    index_buf.create();
    index_buf.bind();
    index_buf.allocate(index_data.constData(), 34 * sizeof(GLushort));
    index_buf.release();
//    m_texture = new QOpenGLTexture(texture_img.mirrored());
    // Load cube.png image
    m_texture = new QOpenGLTexture(QImage(":/cube.png").mirrored());

    // Set nearest filtering mode for texture minification
    m_texture->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    m_texture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    m_texture->setWrapMode(QOpenGLTexture::Repeat);

}

void GeometryEngine::draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions)
{

    if(!vertex_buf.isCreated() || !index_buf.isCreated())
        return;

    m_texture->bind(0);
    // Tell OpenGL programmable pipeline how to locate vertex position data
    program->setUniformValue("texture", 0);

    // Offset for position

    QMatrix4x4 model_matrix_tmp;
    model_matrix_tmp.setToIdentity();
    model_matrix_tmp.translate(translation);
    model_matrix_tmp.rotate(rotation);
    model_matrix_tmp.scale(scale_proportion);
    model_matrix_tmp = global_transform * model_matrix_tmp;

    program->setUniformValue("model_matrix", model_matrix_tmp);

    vertex_buf.bind();

    quintptr offset = 0;
    int vertexLocation = program->attributeLocation("position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    // Offset for normal coordinate
    offset += sizeof(QVector2D);
    // Tell OpenGL programmable pipeline how to locate normal coordinate data
    int normalLocation = program->attributeLocation("normal");
    program->enableAttributeArray(normalLocation);
    program->setAttributeBuffer(normalLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    // Tell OpenGL which VBOs to use
    index_buf.bind();

    // Draw cube geometry using indices from VBO 1
    int a = 0;
    a -=  index_buf.size();
    functions->glDrawElements(GL_TRIANGLE_STRIP, index_buf.size(), GL_UNSIGNED_INT, 0);

    // Tell OpenGL which VBOs to use
    vertex_buf.release();
    index_buf.release();
    m_texture->release();
}

void GeometryEngine::translate(const QVector3D &t)
{
    translation += t;
}
void GeometryEngine::rotate(const QQuaternion &r ) {
    rotation *= r;
}

void GeometryEngine::scale(const float & s) {
    scale_proportion *= s;
}

void GeometryEngine::setGlobalTransform(const QMatrix4x4 & g) {
    global_transform = g;
}
