QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += openglwidgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    $$PWD/A3D/entitycontroller.cpp \
    $$PWD/A3D/keyboardcameracontroller.cpp \
    $$PWD/A3D/linegroup.cpp \
    $$PWD/A3D/linegroupcache.cpp \
    $$PWD/A3D/linegroupcacheogl.cpp \
    $$PWD/A3D/meshgenerator.cpp \
    $$PWD/A3D/scenecontroller.cpp \
    $$PWD/A3D/surfacechartentity.cpp \
    $$PWD/A3D/textbillboardentity.cpp \
    $$PWD/A3D/viewcontroller.cpp \
    $$PWD/A3D/camera.cpp \
    $$PWD/A3D/common.cpp \
    $$PWD/A3D/cubemap.cpp \
    $$PWD/A3D/cubemapcache.cpp \
    $$PWD/A3D/cubemapcacheogl.cpp \
    $$PWD/A3D/entity.cpp \
    $$PWD/A3D/group.cpp \
    $$PWD/A3D/image.cpp \
    $$PWD/A3D/material.cpp \
    $$PWD/A3D/materialcache.cpp \
    $$PWD/A3D/materialcacheogl.cpp \
    $$PWD/A3D/materialproperties.cpp \
    $$PWD/A3D/materialpropertiescache.cpp \
    $$PWD/A3D/materialpropertiescacheogl.cpp \
    $$PWD/A3D/mesh.cpp \
    $$PWD/A3D/meshcache.cpp \
    $$PWD/A3D/meshcacheogl.cpp \
    $$PWD/A3D/model.cpp \
    $$PWD/A3D/renderer.cpp \
    $$PWD/A3D/rendererogl.cpp \
    $$PWD/A3D/resource.cpp \
    $$PWD/A3D/resourcemanager.cpp \
    $$PWD/A3D/resourcemanager_obj.cpp \
    $$PWD/A3D/scene.cpp \
    $$PWD/A3D/texture.cpp \
    $$PWD/A3D/texturecache.cpp \
    $$PWD/A3D/texturecacheogl.cpp \
    $$PWD/A3D/view.cpp

HEADERS += \
	$$PWD/A3D/entitycontroller.h \
	$$PWD/A3D/keyboardcameracontroller.h \
    $$PWD/A3D/linegroup.h \
    $$PWD/A3D/linegroupcache.h \
    $$PWD/A3D/linegroupcacheogl.h \
	$$PWD/A3D/scenecontroller.h \
    $$PWD/A3D/surfacechartentity.h \
    $$PWD/A3D/textbillboardentity.h \
	$$PWD/A3D/viewcontroller.h \
	$$PWD/Dependencies/stb/stb_image_write.h \
        $$PWD/A3D/camera.h \
        $$PWD/A3D/common.h \
        $$PWD/A3D/cubemap.h \
        $$PWD/A3D/cubemapcache.h \
        $$PWD/A3D/cubemapcacheogl.h \
        $$PWD/A3D/entity.h \
        $$PWD/A3D/group.h \
        $$PWD/A3D/image.h \
        $$PWD/A3D/material.h \
        $$PWD/A3D/materialcache.h \
        $$PWD/A3D/materialcacheogl.h \
        $$PWD/A3D/materialproperties.h \
        $$PWD/A3D/materialpropertiescache.h \
        $$PWD/A3D/materialpropertiescacheogl.h \
        $$PWD/A3D/mesh.h \
        $$PWD/A3D/meshcache.h \
        $$PWD/A3D/meshcacheogl.h \
        $$PWD/A3D/model.h \
        $$PWD/A3D/renderer.h \
        $$PWD/A3D/rendererogl.h \
        $$PWD/A3D/resource.h \
        $$PWD/A3D/resourcemanager.h \
        $$PWD/A3D/scene.h \
        $$PWD/A3D/texture.h \
        $$PWD/A3D/texturecache.h \
        $$PWD/A3D/texturecacheogl.h \
        $$PWD/A3D/view.h \
        $$PWD/Dependencies/stb/stb_image.h

DEFINES += STBI_NO_STDIO=1 STBI_NO_ZLIB=1 STBI_ONLY_HDR=1
INCLUDEPATH += Dependencies/stb/

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += A3D/A3D.qrc
