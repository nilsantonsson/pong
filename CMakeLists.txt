cmake_minimum_required(VERSION 3.10)

project (pong-project LANGUAGES CXX C)

find_package(OpenGL REQUIRED)

# Make sure Release build
if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif ()

# Other compile flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build libs
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

add_subdirectory(unittest-cpp)

# Build pong-app
include_directories(src/main)
include_directories(src/main/glad)
include_directories(glm)
include_directories(stb)
include_directories(glfw/include)
include_directories(unittest-cpp/UnitTest++)

file(GLOB PONG_APP_SOURCES
    "src/main/*.cpp"
    "src/main/glad/*.c"
)

file(GLOB TEST_SOURCES
    "src/main/glad/*.c"
    "src/test/*.cpp"
)

link_directories(build/glfw/src)
link_directories(build/unittest-cpp)

if (WIN32)

    set(PROJECT_LINK_LIBS
        libglfw3.a
        libUnitTest++.a
        OpenGL::GL
        libwinmm.a
    )

    add_executable(pong-app ${PONG_APP_SOURCES})
    target_link_libraries(pong-app ${PROJECT_LINK_LIBS})

    add_executable(pong-test ${TEST_SOURCES})
    target_link_libraries(pong-test ${PROJECT_LINK_LIBS})

elseif (APPLE)

    include_directories(${OPENGL_INCLUDE_DIR}/Headers)

    set(PROJECT_LINK_LIBS
        libglfw3.a
        libUnitTest++.a
    )

    add_executable(pong-app ${PONG_APP_SOURCES})
    target_link_libraries(pong-app ${PROJECT_LINK_LIBS})

    add_executable(pong-test ${TEST_SOURCES})
    target_link_libraries(pong-test ${PROJECT_LINK_LIBS})

    target_link_libraries(pong-app
        "-framework OpenGL"
        "-framework IOKit"
        "-framework Cocoa"
        "-framework System"
    )
else ()
    set(PROJECT_LINK_LIBS
        libglfw3.a
        libUnitTest++.a
        OpenGL::GL
    )

    add_executable(pong-app ${PONG_APP_SOURCES})
    target_link_libraries(pong-app ${PROJECT_LINK_LIBS})

    add_executable(pong-test ${TEST_SOURCES})
    target_link_libraries(pong-test ${PROJECT_LINK_LIBS})    
endif ()
