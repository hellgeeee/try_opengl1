QT       += core gui widgets

TARGET = try_opengl_2
TEMPLATE = app

SOURCES += main.cpp

SOURCES += \
    mainwidget.cpp \
    geometryengine.cpp

HEADERS += \
    mainwidget.h \
    geometryengine.h

RESOURCES += \
    shaders.qrc \
    textures.qrc

