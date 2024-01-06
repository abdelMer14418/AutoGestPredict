//
// Created by abdelhak on 06/01/2024.
//


#include <stdio.h>
#include ".\src\sqlite3.h"







int main(int argc, char* argv[]) {


    sqlite3 *db;
    char *err_msg = 0;

    int rc = sqlite3_open("system.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Impossible d'ouvrir la base de données : %s\n", sqlite3_errmsg(db));
        return rc;
    }

    char *sql = "CREATE TABLE Amis(Id INT, Nom TEXT);";

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Erreur SQL : %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);

        return rc;
    }
    sqlite3_close(db);



    return 0;
}
