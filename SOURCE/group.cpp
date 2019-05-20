#include "group.h"

Group::Group()
{
    scale_proportion = 1.0f;
}

void Group::draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions)
{
    QMatrix4x4 local_matrix;
    local_matrix.setToIdentity();
    local_matrix.translate(translation);
    local_matrix.rotate(rotation);
    local_matrix.scale(scale_proportion);
    local_matrix = global_transform * local_matrix;

    for (int i = 0; i < objects.size(); i++){
        objects[i]->setGlobalTransform(local_matrix);
        objects[i]->draw(program, functions);
    }
}

void Group::translate(const QVector3D &t)
{
    translation += t;
}

void Group::rotate(const QQuaternion &r)
{
    rotation = r * rotation;
}

void Group::scale(const float &s)
{
    scale_proportion *= s;
}

void Group::setGlobalTransform(const QMatrix4x4 &g)
{
    global_transform = g;
}

