QT       += core gui openglwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    A3D/camera.cpp \
    A3D/entity.cpp \
    A3D/material.cpp \
    A3D/materialcache.cpp \
    A3D/materialcacheogl.cpp \
    A3D/mesh.cpp \
    A3D/meshcache.cpp \
    A3D/meshcacheogl.cpp \
    A3D/renderer.cpp \
    A3D/rendererogl.cpp \
    A3D/scene.cpp \
    A3D/view.cpp \
    main.cpp

HEADERS += \
	A3D/camera.h \
	A3D/common.h \
	A3D/entity.h \
	A3D/material.h \
	A3D/materialcache.h \
	A3D/materialcacheogl.h \
	A3D/mesh.h \
	A3D/meshcache.h \
	A3D/meshcacheogl.h \
	A3D/renderer.h \
	A3D/rendererogl.h \
	A3D/scene.h \
	A3D/view.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
	A3D/A3D.qrc
