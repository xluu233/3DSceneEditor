cmake_minimum_required(VERSION 3.10)
project(3DSceneEditor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "C:/App/Qt/Qt6/6.4.3/mingw_64")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        OpenGLWidgets
        Quick
        REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/libs)

add_executable(3DSceneEditor
        main.cpp

        assimp/camera.cpp
        assimp/camera.h
        assimp/LoadModelWidget.cpp
        assimp/LoadModelWidget.h
        assimp/model/AssModel.cpp
        assimp/model/AssModel.h
        assimp/model/assimp_helper.h
        assimp/model/AssMesh.cpp
        assimp/model/AssMesh.h

        )

set(shaders_resource_files
        "resource/shader/assimp_model_fs.glsl"
        "resource/shader/assimp_model_vs.glsl"
        )
qt6_add_resources(3DSceneEditor "shaders"
        PREFIX "/"
        FILES ${shaders_resource_files}
        )



########    获取 qml 文件    ########
file(GLOB_RECURSE QML_PATHS *.qml)
#file(GLOB QML_PATHS qml/*.qml)

foreach(filepath ${QML_PATHS})
    message(${filepath} "\\t" ${CMAKE_CURRENT_SOURCE_DIR})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" filename ${filepath}) #.+/(.+)\\\\..*" "\\\\1
    list(APPEND QML_FILES ${filename})
endforeach(filepath)
list(REMOVE_DUPLICATES QML_FILES) #表示删除非第一个重名值

qt_add_qml_module(3DSceneEditor
        URI gl
        VERSION 1.0
        QML_FILES
        ${QML_FILES}
        )


#在内部配置 CMake 以便为 Qt 项目生成一个更简洁的目录结构
qt_standard_project_setup()

target_link_libraries(3DSceneEditor PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Quick
        Qt::OpenGL
        Qt::Widgets
        Qt::OpenGLWidgets
        ${PROJECT_SOURCE_DIR}/libs/libassimp-5.dll
        ${PROJECT_SOURCE_DIR}/libs/libzlibstatic.a
        )



if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
