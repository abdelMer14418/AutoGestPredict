cmake_minimum_required(VERSION 3.27)
project(AutoGestPredict C)

set(CMAKE_C_STANDARD 11)

add_executable(AutoGestPredict
        main.c)



#---------------------------------------------------------------------------------------------
# Ajoutez le chemin vers sqlite3.dll
set(SQLITE_DLL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/sqlite3.dll")


# Copier sqlite3.dll vers le répertoire de build
add_custom_command(TARGET AutoGestPredict POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SQLITE_DLL_PATH}
        $<TARGET_FILE_DIR:AutoGestPredict>)

# Lier la bibliothèque SQLite
target_link_libraries(AutoGestPredict PRIVATE "${SQLITE_DLL_PATH}")



#-------------------------------------------------------------------------------------------


