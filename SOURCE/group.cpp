#include "group.h"

Group::Group()
{
    scale_proportion = 1.0f;
}

Group::~Group()
{

}

void Group::draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions)
{
    QMatrix4x4 tmp_matrix;
    tmp_matrix.setToIdentity();
    tmp_matrix.translate(translation);
    tmp_matrix.rotate(rotation);
    tmp_matrix.scale(scale_proportion);
    tmp_matrix = global_transform * tmp_matrix;

    for (int i = 0; i < objects.size(); i++){
        objects[i]->setGlobalTransform(tmp_matrix);
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

void Group::addObject(Transformational_object *object)
{
    objects.append(object);
}

