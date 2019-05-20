QT       += core gui widgets

TARGET = try_opengl_2
TEMPLATE = app

SOURCES += main.cpp \
    group.cpp

SOURCES += \
    mainwidget.cpp \
    geometryengine.cpp

HEADERS += \
    group.h \
    mainwidget.h \
    geometryengine.h \
    transformational_object.h

RESOURCES += \
    shaders.qrc \
    textures.qrc

