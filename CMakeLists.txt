cmake_minimum_required(VERSION 3.27)
project(AutoGestPredict C)

set(CMAKE_C_STANDARD 11)

#add_executable(AutoGestPredict
#        main.c)



#---------------------------------------------------------------------------------------------
# Ajoutez le chemin vers sqlite3.dll
#set(SQLITE_DLL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/sqlite3.dll")


# Copier sqlite3.dll vers le répertoire de build
#add_custom_command(TARGET AutoGestPredict POST_BUILD
 #       COMMAND ${CMAKE_COMMAND} -E copy_if_different
 #       ${SQLITE_DLL_PATH}
 #       $<TARGET_FILE_DIR:AutoGestPredict>)

# Lier la bibliothèque SQLite
#target_link_libraries(AutoGestPredict PRIVATE "${SQLITE_DLL_PATH}")



#-------------------------------------------------------------------------------------------




# Ajouter le répertoire d'inclusion pour votre projet
include_directories(${CMAKE_SOURCE_DIR}/src)

# Ajouter les répertoires d'inclusion de GTK
set(GTK_INCLUDE_DIRS "C:/msys64/mingw64/include/gtk-4.0")
include_directories(${GTK_INCLUDE_DIRS})
include_directories("C:/msys64/mingw64/include/pango-1.0")
include_directories("C:/msys64/mingw64/include/gdk-pixbuf-2.0")
include_directories("C:/msys64/mingw64/include/cairo")
include_directories("C:/msys64/mingw64/include/harfbuzz")
include_directories("C:/msys64/mingw64/include/freetype2")
include_directories("C:/msys64/mingw64/include/graphene-1.0")
include_directories("C:/msys64/mingw64/lib/graphene-1.0/include")
include_directories("C:/msys64/mingw64/include")
include_directories("C:/msys64/mingw64/include/glib-2.0")
include_directories("C:/msys64/mingw64/lib/glib-2.0/include")
include_directories("C:/msys64/mingw64/include/fribidi")
include_directories("C:/msys64/mingw64/include/webp")
include_directories("C:/msys64/mingw64/include/libpng16")
include_directories("C:/msys64/mingw64/include/pixman-1")

# Ajouter la bibliothèque SQLite3
add_library(sqlite3 STATIC src/sqlite3.c src/sqlite3.h src/sqlite3ext.h)

# Ajouter l'exécutable du shell SQLite
add_executable(sqlite src/shell.c)
target_link_libraries(sqlite LINK_PUBLIC sqlite3)

# Ajouter l'exécutable de SQLtest
add_executable(AutoGestPredict main.c)
target_link_libraries(AutoGestPredict LINK_PUBLIC sqlite3)

# Ajouter les bibliothèques GTK
target_link_libraries(AutoGestPredict LINK_PUBLIC
        -LC:/msys64/mingw64/lib
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


