cmake_minimum_required(VERSION 3.26)
project(AutoGestPredict C)
set(CMAKE_C_STANDARD 11)
find_package(CURL REQUIRED)

# Set the path to msys64 relative to the project directory
set(MSYS64_RELATIVE_PATH "msys64/mingw64")

# Include directories relative to the project directory
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/${MSYS64_RELATIVE_PATH}/include/gtk-4.0)
include_directories(${CMAKE_SOURCE_DIR}/${MSYS64_RELATIVE_PATH}/include/gtk-4.0/gtk)
include_directories(${CMAKE_SOURCE_DIR}/${MSYS64_RELATIVE_PATH}/include/pango-1.0)
include_directories(${CMAKE_SOURCE_DIR}/${MSYS64_RELATIVE_PATH}/include/gdk-pixbuf-2.0)
include_directories(${CMAKE_SOURCE_DIR}/${MSYS64_RELATIVE_PATH}/include/cairo)
include_directories(${CMAKE_SOURCE_DIR}/${MSYS64_RELATIVE_PATH}/include/harfbuzz)
include_directories(${CMAKE_SOURCE_DIR}/${MSYS64_RELATIVE_PATH}/include/freetype2)
include_directories(${CMAKE_SOURCE_DIR}/${MSYS64_RELATIVE_PATH}/include/graphene-1.0)
include_directories(${CMAKE_SOURCE_DIR}/${MSYS64_RELATIVE_PATH}/lib/graphene-1.0/include)
include_directories(${CMAKE_SOURCE_DIR}/${MSYS64_RELATIVE_PATH}/include)
include_directories(${CMAKE_SOURCE_DIR}/${MSYS64_RELATIVE_PATH}/include/glib-2.0)
include_directories(${CMAKE_SOURCE_DIR}/${MSYS64_RELATIVE_PATH}/lib/glib-2.0/include)
include_directories(${CMAKE_SOURCE_DIR}/${MSYS64_RELATIVE_PATH}/include/fribidi)
include_directories(${CMAKE_SOURCE_DIR}/${MSYS64_RELATIVE_PATH}/include/webp)
include_directories(${CMAKE_SOURCE_DIR}/${MSYS64_RELATIVE_PATH}/include/libpng16)
include_directories(${CMAKE_SOURCE_DIR}/${MSYS64_RELATIVE_PATH}/include/pixman-1)

# Add the SQLite3 library
add_library(sqlite3 STATIC src/sqlite3.c src/sqlite3.h src/sqlite3ext.h)

# Add the SQLite shell executable
add_executable(sqlite src/sqlite3.c src/shell.c src/sqlite3.h src/sqlite3ext.h)

# Add the AutoGestPredict executable
add_executable(AutoGestPredict main.c)

# Link libraries
target_link_libraries(AutoGestPredict LINK_PUBLIC sqlite3
        -L${CMAKE_SOURCE_DIR}/${MSYS64_RELATIVE_PATH}/lib
        -lgtk-4
        -lpangowin32-1.0
        -lpangocairo-1.0
        -lpango-1.0
        -lharfbuzz
        -lgdk_pixbuf-2.0
        -lcairo-gobject
        -lcairo
        -lgraphene-1.0
        -lgio-2.0
        -lgobject-2.0
        -lglib-2.0
        -lintl
)

# Link the CURL library
target_link_libraries(AutoGestPredict PRIVATE CURL::libcurl)