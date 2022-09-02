cmake_minimum_required(VERSION 3.16)
project(jackyshooter)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)
if (WIN32)
    find_package(Boost COMPONENTS thread REQUIRED)
endif ()
find_package(websocketpp REQUIRED)

include_directories(
        include
        glm
        ${OPENGL_INCLUDE_DIRS}
        ${GLFW3_INCLUDE_DIR}
        ${ASSIMP_INCLUDE_DIR}
        ${WEBSOCKETPP_INCLUDE_DIR}
)

file(GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/jcs/*.cpp)

add_executable(jackyshooter ${SOURCE_FILES})

target_link_libraries(jackyshooter
        ${OPENGL_LIBRARIES} ${GLFW3_LIBRARY} ${CMAKE_DL_LIBS} -lm)

if (WIN32)
    target_link_libraries(jackyshooter Boost::thread -lws2_32)
else ()
    target_link_libraries(jackyshooter -ldl -lGL -lGLU -lX11)
endif ()


file(GLOB_RECURSE SRV_SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/jcserver/*.cpp)
add_executable(jackyserver ${SRV_SOURCE_FILES})

target_link_libraries(jackyserver ${CMAKE_DL_LIBS} -lm)

if (WIN32)
    target_link_libraries(jackyserver Boost::thread -lws2_32 -lwsock32)
endif ()
