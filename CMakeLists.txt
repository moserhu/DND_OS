cmake_minimum_required(VERSION 3.15)
set(CMAKE_SYSTEM_NAME Generic)

if (WIN32)
    set(CMAKE_GENERATOR "MinGW Makefiles")
endif (WIN32)

#set(CMAKE_BUILD_TYPE Release)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# Cross-compilation toolchain configuration
set(CMAKE_CROSSCOMPILING false)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_LINKER /usr/bin/ld)

project(DND_Screen C)

# SDL Configuration
set(SDL_FOLDER sdl2-dev-rpi64)
set(CMAKE_FIND_ROOT_PATH ${SDL_FOLDER})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_BUILD_RPATH "${SDL_FOLDER}/include")
set(CMAKE_INSTALL_RPATH ${CMAKE_BUILD_RPATH})

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/${SDL_FOLDER}/include
    ${PROJECT_SOURCE_DIR}/lvgl
    ${PROJECT_SOURCE_DIR}/ui
    ${PROJECT_SOURCE_DIR}/src  # Include custom logic folder
)

# Gather source files
FILE(GLOB_RECURSE LVGL_Sources CONFIGURE_DEPENDS lvgl/*.c)
FILE(GLOB_RECURSE UI_Sources CONFIGURE_DEPENDS ui/*.c ui/*.cpp)
FILE(GLOB_RECURSE LOGIC_Sources CONFIGURE_DEPENDS src/*.c)

# Add executable
add_executable(${PROJECT_NAME} main.c ${LVGL_Sources} ${UI_Sources} ${LOGIC_Sources})

# Link required libraries
target_link_libraries(${PROJECT_NAME} -L"${PROJECT_SOURCE_DIR}/${SDL_FOLDER}/lib" SDL2 cjson)
