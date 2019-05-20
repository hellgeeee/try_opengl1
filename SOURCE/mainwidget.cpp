#include "mainwidget.h"

#include <QMouseEvent>
#include <math.h>

MainWidget::MainWidget(QWidget *parent) :
    QOpenGLWidget(parent)
{
    camera_offset_z = -20.0f;
}

MainWidget::~MainWidget()
{

//delete
    // Make sure the context is current when deleting the texture
    // and the buffers.
    makeCurrent();
    delete m_texture;
    for(int i = 0; i < objects.size(); i++)
        delete objects[i];
    doneCurrent();
}

//! [0]
void MainWidget::mousePressEvent(QMouseEvent *e)
{
    // Save mouse press position
    mouse_position = QVector2D(e->localPos());
    update();
}

void MainWidget::mouseMoveEvent(QMouseEvent *e)
{
    QVector2D diff_tmp = mouse_position - QVector2D(e->localPos());
//    float pass = diff_tmp.length();
//   // objects[0]->rotate( QQuaternion(pass/100, QVector3D( diff_tmp.y(), diff_tmp.x(), 0).normalized()));
//    rotation *=  QQuaternion(cos(pass/1000), QVector3D( diff_tmp.y(), diff_tmp.x(), 0).normalized());

    for(int i = 0; i < objects.size(); i++){
        objects[i]->rotate(QQuaternion::fromAxisAndAngle(1.0, 0.0, 0.0, qSin(0.1) ));
        objects[i]->rotate(QQuaternion::fromAxisAndAngle(0.0, 1.0, 0.0, qCos(0.1) ));
        objects[i]->rotate(QQuaternion::fromAxisAndAngle(0.0, 0.0, 1.0, qSin(0.5) ));
    }


    update();
}

void MainWidget::mouseReleaseEvent(QMouseEvent *e)
{
    // Mouse release position - mouse press position dsf
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
    update();
}

void MainWidget::wheelEvent(QWheelEvent *e)
{
    if(e->delta() > 0){
        camera_offset_z += 0.25f;
    } else {
        camera_offset_z -= 0.25f;
    }
    update();
}
//! [0]

//! [1]
void MainWidget::timerEvent(QTimerEvent *)
{
    for(int i = 0; i < objects.size(); i++){
//        objects[i]->rotate(QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, qSin(angle_object)));
    }
}

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

void MainWidget::initTextures()
{
    // Load cube.png image
    m_texture = new QOpenGLTexture(QImage(":/cube.png").mirrored());

    // Set nearest filtering mode for texture minification
    m_texture->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    m_texture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    m_texture->setWrapMode(QOpenGLTexture::Repeat);

    m_texture->bind();
}

void MainWidget::initCube(float radius)
{
    // For cube we would need only 8 vertices but we have to
    // duplicate vertex for each face because texture coordinate
    // is different.
    const QVector<GeometryEngine::VertexData> vertices = {
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
    const QVector<GLuint> indices = {
         0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
         4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
         8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
        12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
        16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
        20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
    };

   objects.append(new GeometryEngine(vertices, indices, QImage(":/cube.png")));

}

void MainWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(.1, .1, 0, 1);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);

    initShaders();
   // initTextures();

    // add objects
    for(int i = 0; i < 3; i++){
        initCube(2.0f);
        objects[i]->translate(QVector3D(i*5, 0, 0));
    }


}

void MainWidget::resizeGL(int w, int h)
{
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 3.0, zFar = 100.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}


void MainWidget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // Calculate model view transformation
    QMatrix4x4 view_matrix;
    view_matrix.setToIdentity();
    view_matrix.translate(0.0, 0.0, camera_offset_z);
    view_matrix.rotate(rotation);

    program.bind();
    // Set modelview-projection matrix
    program.setUniformValue("projection_matrix", projection);
    program.setUniformValue("view_matrix", view_matrix);
    program.setUniformValue("light_position", QVector4D(0.0f, 0.0f, -10.0f, 1.0f));
    program.setUniformValue("light_power", 10.0f);
//! [6]

    // Use texture unit 0 which contains cube.png
    program.setUniformValue("texture", 0);

    // Draw cube geometry
    for(int i = 0; i < objects.size(); i++){
        objects[i]->draw(&program, context()->functions());
    }
}
