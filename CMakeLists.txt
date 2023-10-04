cmake_minimum_required(VERSION 3.10)

project(OpenGL_teste)

set(CMAKE_CXX_STANDARD 20)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/dep/glfw-3.3.8.bin.WIN64/include
	${CMAKE_CURRENT_SOURCE_DIR}/dep/glew-2.1.0/include
	${CMAKE_CURRENT_SOURCE_DIR}/dep/glm
	${CMAKE_CURRENT_SOURCE_DIR}/dep/imgui
)

link_libraries(
    ${CMAKE_CURRENT_SOURCE_DIR}/dep/glfw-3.3.8.bin.WIN64/lib-vc2022/glfw3.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/dep/glfw-3.3.8.bin.WIN64/lib-vc2022/glfw3_mt.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/dep/glew-2.1.0/lib/Release/x64/glew32s.lib
    OpenGL32.lib
)

#################################################
add_executable(OpenGL_teste
    # opengl
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/stb_image/stb_image.cpp # load image
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/Texture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/Renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/VertexArray.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/VertexBufferLayout.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/VertexBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/IndexBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/Shader.cpp
    # imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/dep/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dep/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dep/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dep/imgui/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dep/imgui/imgui_impl_opengl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dep/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dep/imgui/imgui_widgets.cpp
    # main
    ${CMAKE_CURRENT_SOURCE_DIR}/src/App.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)
#################################################