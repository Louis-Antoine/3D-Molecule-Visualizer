cmake_minimum_required(VERSION 3.24)
project(opengltest)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

set(ALL_LIBS
        OpenGL::GL
        OpenGL::GLU
        GLEW::glew
        glfw
        )

set(SRC
        src/main.cpp
        src/Shape.cpp
        src/Atom.cpp
        )


add_definitions(
        -DTW_STATIC
        -DTW_NO_LIB_PRAGMA
        -DTW_NO_DIRECT3D
        -DGLEW_STATIC
        -D_CRT_SECURE_NO_WARNINGS
        -DGLM_ENABLE_EXPERIMENTAL
        -DUSE_ZLIB_T
)

add_executable(opengltest src/main.cpp src/Atom.cpp src/Bond.h src/Bond.cpp src/Molecule.h src/Molecule.cpp src/TBCamera.h src/TBCamera.cpp Dependencies/imconfig.h Dependencies/imgui.cpp Dependencies/imgui.h Dependencies/imgui_demo.cpp Dependencies/imgui_draw.cpp Dependencies/imgui_internal.h Dependencies/imgui_tables.cpp Dependencies/imgui_widgets.cpp Dependencies/imstb_rectpack.h Dependencies/imstb_textedit.h Dependencies/imstb_truetype.h Dependencies/imgui_impl_glfw.h Dependencies/imgui_impl_glfw.cpp Dependencies/imgui_impl_opengl3.h Dependencies/imgui_impl_opengl3.cpp Dependencies/imgui_impl_opengl3_loader.h Dependencies/stb_image.h Dependencies/stb.cpp src/Skybox.h src/Skybox.cpp)
target_link_libraries(${PROJECT_NAME} ${ALL_LIBS})