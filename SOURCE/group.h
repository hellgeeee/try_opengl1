#ifndef GROUP_H
#define GROUP_H

#include "transformational_object.h"
#include "geometryengine.h"
#include <QVector>

class Group : public Transformational_object
{
public:
    Group();
    ~Group();

    void draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions);
    void translate(const QVector3D &translateVector);
    void rotate(const QQuaternion &r );
    void scale(const float & s);
    void setGlobalTransform(const QMatrix4x4 & g);

    void addObject(Transformational_object *object);

private:
    // positions
    QMatrix4x4 global_transform;
    QVector3D translation;
    QQuaternion rotation;
    float scale_proportion;

    QVector<Transformational_object *> objects;
};

#endif // GROUP_H
