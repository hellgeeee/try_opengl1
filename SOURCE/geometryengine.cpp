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

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QtMath>
#include "geometryengine.h"

GeometryEngine::GeometryEngine()
    : index_buf(QOpenGLBuffer::IndexBuffer), texture(0)
{
     scale_proportion = 1.0f;
     radius = 1.0f;
}

GeometryEngine::GeometryEngine(QVector3D topleft)
    : index_buf(QOpenGLBuffer::IndexBuffer), texture(0)
{
    scale_proportion = 1.0f;
    radius = 1.0f;

    setGeometry();

    global_transform.setToIdentity();
    scale_proportion = 1.0f;
    translation = QVector3D(-6.0f, 0.0f, 0.0f);
    rotation  = QQuaternion(cos(M_PI/2), 0.0, 0.0, 1.0);
    //rotation = QQuaternion::fromAxisAndAngle(QVector3D(0.0, 0.0, 1.0).normalized(), qCos(1));
    //rotation *= QQuaternion::fromAxisAndAngle(QVector3D(0.0, 1.0, 0.0).normalized(), qSin(30));

    init();
    translate(topleft);
}

GeometryEngine::~GeometryEngine()
{
    if(vertex_buf.isCreated())
        vertex_buf.destroy();
    if(index_buf.isCreated())
        index_buf.destroy();
    if(texture != 0)
        if(texture->isCreated())
            texture->destroy();
}

void GeometryEngine::init()
{
    if(vertex_buf.isCreated())
        vertex_buf.destroy();
    if(index_buf.isCreated())
        index_buf.destroy();
    if(texture != 0) {
        if(texture->isCreated()){
            delete texture;
            texture = 0;
        }
    }

    // Generate 2 VBOs
    // Transfer vertex data to VBO 0
    vertex_buf.create();
    vertex_buf.bind();
    vertex_buf.allocate(vertices.constData(), vertices.size() * sizeof(VertexData));
    vertex_buf.release();

    // Transfer index data to VBO 1
    index_buf.create();
    index_buf.bind();
    index_buf.allocate(indices.constData(), indices.size() * sizeof(GLushort));
    index_buf.release();

//    m_texture = new QOpenGLTexture(texture_img.mirrored());
    // Load cube.png image
    texture = new QOpenGLTexture(QImage(":/cube.png").mirrored());

    // Set nearest filtering mode for texture minification
    texture->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    texture->setWrapMode(QOpenGLTexture::Repeat);

}

void GeometryEngine::draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions)
{

    if(!vertex_buf.isCreated() || !index_buf.isCreated())
        return;

    vertex_buf.bind();
    index_buf.bind();
    texture->bind(0);

    QMatrix4x4 matrix_tmp;
    matrix_tmp.setToIdentity();
    matrix_tmp.translate(translation);
    matrix_tmp.rotate(rotation);
    matrix_tmp.scale(scale_proportion);
    matrix_tmp = global_transform * matrix_tmp;

    program->setUniformValue("texture", 0);
    program->setUniformValue("model_matrix", matrix_tmp);

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

    // Draw cube geometry using indices from VBO 1
    int a =  index_buf.size();
    functions->glDrawElements(GL_TRIANGLES, index_buf.size(), GL_UNSIGNED_INT, 0);

    vertex_buf.release();
    index_buf.release();
    texture->release();
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

void GeometryEngine::setGeometry()
{
    // For cube we would need only 8 vertices but we have to
    // duplicate vertex for each face because texture coordinate
    // is different.
    vertices = {
        // Vertex data for face 0
        {QVector3D(-radius, -radius,  radius), QVector2D(0.0f, 0.0f)},  // v0
        {QVector3D( radius, -radius,  radius), QVector2D(0.0f, 1.0f)}, // v1
        {QVector3D(-radius,  radius,  radius), QVector2D(1.0f, 0.0f)},  // v2
        {QVector3D( radius,  radius,  radius), QVector2D(1.0f, 1.0f)}, // v3

        // Vertex data for face 1
        {QVector3D( radius, -radius, -radius), QVector2D(0.0f, 0.0f)}, // v4
        {QVector3D( radius, -radius,  radius), QVector2D(0.0f, 1.0f)}, // v5
        {QVector3D( radius,  radius, -radius), QVector2D(1.0f, 0.0f)}, // v6
        {QVector3D( radius,  radius,  radius), QVector2D(1.0f, 1.0f)},  // v7

        // Vertex data for face 2
        {QVector3D( radius, -radius, -radius), QVector2D(0.0f, 0.0f)}, // v8
        {QVector3D(-radius, -radius, -radius), QVector2D(0.0f, 1.0f)},  // v9
        {QVector3D( radius,  radius, -radius), QVector2D(1.0f, 0.0f)}, // v10
        {QVector3D(-radius,  radius, -radius), QVector2D(1.0f, 1.0f)},  // v11

        // Vertex data for face 3
        {QVector3D(-radius, -radius, -radius), QVector2D(0.0f, 0.0f)}, // v12
        {QVector3D(-radius, -radius,  radius), QVector2D(0.0f, 1.0f)},  // v13
        {QVector3D(-radius,  radius, -radius), QVector2D(1.0f, 0.0f)}, // v14
        {QVector3D(-radius,  radius,  radius), QVector2D(1.0f, 1.0f)},  // v15

        // Vertex data for face 4
        {QVector3D(-radius, -radius, -radius), QVector2D(0.0f, 0.0f)}, // v16
        {QVector3D( radius, -radius, -radius), QVector2D(0.0f, 1.0f)}, // v17
        {QVector3D(-radius, -radius,  radius), QVector2D(1.0f, 0.0f)}, // v18
        {QVector3D( radius, -radius,  radius), QVector2D(1.0f, 1.0f)}, // v19

        // Vertex data for face 5
        {QVector3D(-radius,  radius,  radius), QVector2D(0.0f, 0.0f)}, // v20
        {QVector3D( radius,  radius,  radius), QVector2D(1.0f, 0.0f)}, // v21
        {QVector3D(-radius,  radius, -radius), QVector2D(0.0f, 1.0f)}, // v22
        {QVector3D( radius,  radius, -radius), QVector2D(1.0f, 1.0f)}  // v23
    };

    // Indices for drawing cube faces using triangle strips.
    // Triangle strips can be connected by duplicating indices
    // between the strips. If connecting strips have opposite
    // vertex order then last index of the first strip and first
    // index of the second strip needs to be duplicated. If
    // connecting strips have same vertex order then only last
    // index of the first strip needs to be duplicated.
    indices = {
       //  0,  1,  2,
      //  2,  1, 3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
//        0+4,  1+4,  2+4,
        2+4,  1+4, 3+4//, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
       // 0 + 8,  1 + 8,  2 + 8,  2 + 8,  1 + 8, 3 + 8, // Face 2 - triangle strip ( v8,  v9, v10, v11)
       // 0 + 12,  1 + 12,  2 + 12,  2 + 12,  1 + 12, 3 + 12, // Face 3 - triangle strip (v12, v13, v14, v15)
        //0 + 16,  1 + 16,  2 + 16,  2 + 16,  1 + 16, 3 + 16, // Face 4 - triangle strip (v16, v17, v18, v19)
        //0 + 20,  1 + 20,  2 + 20,  2 + 20,  1 + 20, 3 + 20      // Face 5 - triangle strip (v20, v21, v22, v23)
    };

}
