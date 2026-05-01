cmake_minimum_required(VERSION 3.16)
project(OpenGLApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(libs/glfw-3.4)
add_subdirectory(libs/glad)
add_subdirectory(libs/glm)

add_executable(app 
    src/main.cpp 
    src/Shader.cpp 
    src/Material.cpp 
    src/Light.cpp
    src/Player.cpp
    src/models/Model.cpp 
    src/models/Cube.cpp 
    src/models/Plane.cpp 
    src/models/Pyramid.cpp 
    src/Camera.cpp
    src/net/ClientUdp.cpp
    src/net/Friend.cpp
)

target_compile_definitions(app PRIVATE ASIO_STANDALONE)

if(WIN32)
    # Ustawienie minimalnej wersji Windows na Windows 7 (0x0601)
    target_compile_definitions(app PRIVATE _WIN32_WINNT=0x0601)
endif()

target_include_directories(app PRIVATE ${CMAKE_SOURCE_DIR}/libs/asio/include)
target_include_directories(app PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(app PRIVATE glad glfw glm)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

if(UNIX AND NOT APPLE)
    target_link_libraries(app PRIVATE dl pthread X11 Xrandr Xi Xxf86vm Xinerama Xcursor rt)
endif()

if(WIN32)
    target_link_libraries(app PRIVATE ws2_32 mswsock)
endif()

if(WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
endif()
