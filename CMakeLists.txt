cmake_minimum_required(VERSION 3.10)

project (pong-project LANGUAGES CXX C)

find_package(OpenGL REQUIRED)

# Set default build type
if (NOT CMAKE_BUILD_TYPE)
    set( CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif (NOT CMAKE_BUILD_TYPE)

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

file(GLOB APP_SOURCES
    "src/main/*.cpp"
    "src/main/glad/*.c"
)

file(GLOB TEST_SOURCES
    "src/main/glad/*.c"
    "src/test/*.cpp"
)

link_directories(build/glfw/src)
link_directories(build/unittest-cpp)

set(COMMON_PROJECT_LINK_LIBS
    libglfw3.a
    libUnitTest++.a
)

if (WIN32)

    set(PROJECT_LINK_LIBS
        OpenGL::GL
        libwinmm.a
    )

elseif (APPLE)

    include_directories(${OPENGL_INCLUDE_DIR}/Headers)
    
else ()

    set(PROJECT_LINK_LIBS
        OpenGL::GL
    )

endif ()

add_executable(pong-app ${APP_SOURCES})
target_link_libraries(pong-app ${PROJECT_LINK_LIBS} ${COMMON_PROJECT_LINK_LIBS})

add_executable(pong-test ${TEST_SOURCES})
target_link_libraries(pong-test ${PROJECT_LINK_LIBS} ${COMMON_PROJECT_LINK_LIBS})

if (APPLE)
    target_link_libraries(pong-app
        "-framework OpenGL"
        "-framework IOKit"
        "-framework Cocoa"
        "-framework System"
    )
endif()
