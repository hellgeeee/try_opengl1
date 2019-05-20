#ifndef TRANSFORMATIONAL_OBJECT_H
#define TRANSFORMATIONAL_OBJECT_H

class  QQuaternion;
class QVector3D;
class QMatrix4x4;
class QOpenGLShaderProgram;
class QOpenGLFunctions;

class Transformational_object{
public:
    virtual void translate(const QVector3D & t) = 0;
    virtual void draw(QOpenGLShaderProgram * program, QOpenGLFunctions *functions) = 0;
    virtual void rotate(const QQuaternion &r ) = 0;
    virtual void scale(const float & s) = 0;
    virtual void setGlobalTransform(const QMatrix4x4 & g) = 0;
};

#endif // TRANSFORMATIONAL_OBJECT_H
