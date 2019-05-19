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

#include "mainwidget.h"

#include <QMouseEvent>

#include <math.h>

MainWidget::MainWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    objects(0),
    texture(0),
    camera_offset_z(0)
{
}

MainWidget::~MainWidget()
{
    // Make sure the context is current when deleting the texture
    // and the buffers.
    makeCurrent();
    delete texture;
    for(int i = 0; i < objects.size(); i++)
        delete objects[i];
    doneCurrent();
}

//! [0]
void MainWidget::mousePressEvent(QMouseEvent *e)
{
    // Save mouse press position
    mousePressPosition = QVector2D(e->localPos());
}

void MainWidget::mouseMoveEvent(QMouseEvent *e)
{

}

void MainWidget::mouseReleaseEvent(QMouseEvent *e)
{
    // Mouse release position - mouse press position
//    QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;

//    // Rotation axis is perpendicular to the mouse position difference
//    // vector
//    QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

//    // Accelerate angular speed relative to the length of the mouse sweep
//    qreal acc = diff.length() / 100.0;

//    // Calculate new rotation axis as weighted sum
//    rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

//    // Increase angular speed
//    angularSpeed += acc;
}

void MainWidget::wheelEvent(QWheelEvent *e)
{
    if(e->delta() > 0){
        camera_offset_z += 0.25f;
    } else {
        camera_offset_z -= 0.25f;
    }
}
//! [0]

//! [1]
void MainWidget::timerEvent(QTimerEvent *)
{
    for(int i = 0; i < objects.size(); i++){
//        objects[i]->rotate(QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, qSin(angle_object)));
    }
}
//! [1]

void MainWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(.1, .3, 0, 1);

    initShaders();
    initTextures();

//! [2]
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);
//! [2]

    for(int i = 0; i < 27; i++)
        objects.append(new GeometryEngine());

    // Use QBasicTimer because its faster than QTimer
    timer.start(12, this);
}

//! [3]
void MainWidget::initShaders()
{
    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    if (!program.bind())
        close();
}
//! [3]

//! [4]
void MainWidget::initTextures()
{
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

void MainWidget::initCube(float radius)
{
    // For cube we would need only 8 vertices but we have to
    // duplicate vertex for each face because texture coordinate
    // is different.
    VertexData vertices[] = {
        // Vertex data for face 0
        {QVector3D(-radius, -radius,  radius), QVector2D(0.0f, 0.0f)},  // v0
        {QVector3D( radius, -radius,  radius), QVector2D(0.0f, 1.0f)}, // v1
        {QVector3D(-radius,  radius,  radius), QVector2D(1.0f, 0.0f)},  // v2
        {QVector3D( radius,  radius,  radius), QVector2D(1.0f, 1.0f)}, // v3

        // Vertex data for face 1
        {QVector3D( radius, -radius,  radius), QVector2D(0.0f, 0.0f)}, // v4
        {QVector3D( radius, -radius, -radius), QVector2D(0.0f, 1.0f)}, // v5
        {QVector3D( radius,  radius,  radius), QVector2D(1.0f, 0.0f)},  // v6
        {QVector3D( radius,  radius, -radius), QVector2D(1.0f, 1.0f)}, // v7

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
    GLushort indices[] = {
         0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
         4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
         8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
        12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
        16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
        20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
    };



}
//! [4]

//! [5]
void MainWidget::resizeGL(int w, int h)
{
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}
//! [5]

void MainWidget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    texture->bind();

//! [6]
    // Calculate model view transformation
    QMatrix4x4 matrix;
    matrix.translate(0.0, 0.0, -5.0);
    matrix.rotate(rotation);

    // Set modelview-projection matrix
    program.setUniformValue("mvp_matrix", projection * matrix);
//! [6]

    // Use texture unit 0 which contains cube.png
    program.setUniformValue("texture", 0);

    // Draw cube geometry
    for(int i = 0; i < objects.size(); i++)
        objects[i]->draw(&program, context()->functions());
}
