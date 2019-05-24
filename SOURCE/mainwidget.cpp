#include "mainwidget.h"

#include <QMouseEvent>

MainWidget::MainWidget(QWidget *parent) :
    QOpenGLWidget(parent)
{
    camera_offset_z = -20.0f;
    rotation = QQuaternion(1, 0, 0, 0);
    rotation *= QQuaternion::fromAxisAndAngle(QVector3D(0.0, 1.0, 0.0), 30);

    angle_object = 0.0;
}

MainWidget::~MainWidget()
{

//delete
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
   // mouse_position = QVector2D(e->localPos());
    update();
}

void MainWidget::mouseMoveEvent(QMouseEvent *e)
{
//    QVector2D diff_tmp = mouse_position - QVector2D(e->localPos());
////    float pass = diff_tmp.length();
////   // objects[0]->rotate( QQuaternion(pass/100, QVector3D( diff_tmp.y(), diff_tmp.x(), 0).normalized()));
////    rotation *=  QQuaternion(cos(pass/1000), QVector3D( diff_tmp.y(), diff_tmp.x(), 0).normalized());

//    for(int i = 0; i < objects.size(); i++){
//        objects[i]->rotate(QQuaternion::fromAxisAndAngle(1.0, 0.0, 0.0, qSin(0.1) ));
//        objects[i]->rotate(QQuaternion::fromAxisAndAngle(0.0, 1.0, 0.0, qCos(0.1) ));
//        objects[i]->rotate(QQuaternion::fromAxisAndAngle(0.0, 0.0, 1.0, qSin(0.5) ));
//    }


//    update();
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

void MainWidget::timerEvent(QTimerEvent *)
{
    for(int i = 0; i < objects.size(); i++){
        objects[i]->rotate( QQuaternion::fromAxisAndAngle(QVector3D(1.0, 0.0, 0.0), i*3.0) );
    }
    rotation *= QQuaternion::fromAxisAndAngle(QVector3D(0.0, 0.0, 1.0), 0.5);
    angle_object += M_PI / 180;
    update();
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

void MainWidget::initObjects()
{
    for(int i = 0; i < 3; i++)
        objects.append(new GeometryEngine(QVector3D(i * 3, 0, 0)));
}

void MainWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(.1, .15, 0, 1);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    //glEnable(GL_CULL_FACE);

    initShaders();

    // add objects
    initObjects();
    for(int i = 0; i < 3; i++){
       ;// objects[i]->translate(QVector3D(i*5, 0, 0));
    }

    timer.start(10, this);
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
    program.setUniformValue("light_position", QVector4D(0.0f, 0.0f, 0.0f, 1.0f));
    program.setUniformValue("light_power", 1.0f);

    // Use texture unit 0 which contains cube.png
    program.setUniformValue("texture", 0);

    // Draw cube geometry
    for(int i = 0; i < objects.size(); i++){
        objects[i]->draw(&program, context()->functions());
    }
}
