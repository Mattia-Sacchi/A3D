cmake_minimum_required(VERSION 3.16)
project(A3D)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Enable Qt modules
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGLWidgets)

# Define the executable or library
add_executable(${PROJECT_NAME})

# Add preprocessor definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    STBI_NO_STDIO=1
    STBI_NO_ZLIB=1
    STBI_ONLY_HDR=1
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/stb
)

# Source files
set(SOURCES
    main.cpp
    A3D/entitycontroller.cpp
    A3D/keycameracontroller.cpp
    A3D/linegroup.cpp
    A3D/linegroupcache.cpp
    A3D/linegroupcacheogl.cpp
    A3D/meshgenerator.cpp
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
    A3D/view.cpp
)

# Header files
set(HEADERS
    A3D/entitycontroller.h
    A3D/keycameracontroller.h
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
    Dependencies/stb/stb_image.h
)

# Add resources (with BIG mode)
qt6_add_big_resources(RESOURCES
    A3D/A3D.qrc
    A3D/SampleResources.qrc
)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES} ${HEADERS} ${RESOURCES})

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGLWidgets
)

# Install rules
if(QNX)
    set(INSTALL_PATH /tmp/${PROJECT_NAME}/bin)
elseif(UNIX AND NOT ANDROID)
    set(INSTALL_PATH /opt/${PROJECT_NAME}/bin)
endif()

if(INSTALL_PATH)
    install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_PATH})
endif()
