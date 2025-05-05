cmake_minimum_required(VERSION 3.19)
project(Tools_A3D)
option(A3D_LIBRARY_ONLY "Configures A3D as a library-only dependency (no main entrypoint)" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    OpenGLWidgets
)

# Sources
set(SOURCES
    main.cpp
    mathfunctions.cpp
    A3D/entitycontroller.cpp
    A3D/keyboardcameracontroller.cpp
    A3D/linegroup.cpp
    A3D/linegroupcache.cpp
    A3D/linegroupcacheogl.cpp
    A3D/scenecontroller.cpp
    A3D/surfacechartentity.cpp
    A3D/textbillboardentity.cpp
    A3D/viewcontroller.cpp
    A3D/camera.cpp
    A3D/common.cpp
    A3D/cubemap.cpp
    A3D/cubemapcache.cpp
    A3D/cubemapcacheogl.cpp
    A3D/entity.cpp
    A3D/group.cpp
    A3D/image.cpp
    A3D/material.cpp
    A3D/materialcache.cpp
    A3D/materialcacheogl.cpp
    A3D/materialproperties.cpp
    A3D/materialpropertiescache.cpp
    A3D/materialpropertiescacheogl.cpp
    A3D/mesh.cpp
    A3D/meshcache.cpp
    A3D/meshcacheogl.cpp
    A3D/model.cpp
    A3D/renderer.cpp
    A3D/rendererogl.cpp
    A3D/resource.cpp
    A3D/resourcemanager.cpp
    A3D/resourcemanager_obj.cpp
    A3D/scene.cpp
    A3D/texture.cpp
    A3D/texturecache.cpp
    A3D/texturecacheogl.cpp
    A3D/chart.cpp
    A3D/view.cpp
	keyeventmanager.cpp
)

set(INCPATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/stb
)

set(DEFINES
    STBI_NO_STDIO=1
    STBI_NO_ZLIB=1
    STBI_ONLY_HDR=1
)

# Headers
set(HEADERS
    mathfunctions.h
    A3D/entitycontroller.h
    A3D/keyboardcameracontroller.h
    A3D/linegroup.h
    A3D/linegroupcache.h
    A3D/linegroupcacheogl.h
    A3D/scenecontroller.h
    A3D/surfacechartentity.h
    A3D/textbillboardentity.h
    A3D/viewcontroller.h
    Dependencies/stb/stb_image_write.h
    A3D/camera.h
    A3D/common.h
    A3D/cubemap.h
    A3D/cubemapcache.h
    A3D/cubemapcacheogl.h
    A3D/entity.h
    A3D/group.h
    A3D/image.h
    A3D/material.h
    A3D/materialcache.h
    A3D/materialcacheogl.h
    A3D/materialproperties.h
    A3D/materialpropertiescache.h
    A3D/materialpropertiescacheogl.h
    A3D/mesh.h
    A3D/meshcache.h
    A3D/meshcacheogl.h
    A3D/model.h
    A3D/renderer.h
    A3D/rendererogl.h
    A3D/resource.h
    A3D/resourcemanager.h
    A3D/scene.h
    A3D/texture.h
    A3D/texturecache.h
    A3D/texturecacheogl.h
    A3D/view.h
    A3D/chart.h
    Dependencies/stb/stb_image.h
	keyeventmanager.h
)

set(RESOURCES
	A3D/A3D.qrc
    A3D/SampleResources.qrc
)

qt_add_big_resources(
    COMPILED_RESOURCE_FILES
	${RESOURCES}
)

# Create executable from these files...
qt_add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${COMPILED_RESOURCE_FILES}
	${RESOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${INCPATHS}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    ${DEFINES}
)

#target_sources(${PROJECT_NAME} PRIVATE ${SOURCES} ${HEADERS} ${RESOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGLWidgets
)

