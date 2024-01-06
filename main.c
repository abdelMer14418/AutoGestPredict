//
// Created by abdelhak on 06/01/2024.
//


#include <stdio.h>
#include <ctype.h>
#include ".\src\sqlite3.h"
#include <curl/curl.h>


sqlite3 *db;  // Declare a sqlite Database.
char *err_msg = 0;
char sql_db[500];
sqlite_int64 userid = 0;
sqlite_int64 Rentalid = 0;



void caesar_chiper_encrypt(char* data)
{
    int key = 6;
    char ch;
    for (int i = 0; data[i] != '\0';++i)
    {
        ch = data[i];
        printf("%c",ch);
        //Lowercase characters.
        if (islower(ch)) {
            ch = (ch - 'a' + key) % 26 + 'a';
        }
            // Uppercase characters.
        else if (isupper(ch)) {
            ch = (ch - 'A' + key) % 26 + 'A';
        }

            // Numbers.
        else if (isdigit(ch)) {
            ch = (ch - '0' + key) % 10 + '0';
        }
        data[i] = ch;
    }
}

void caesar_chiper_decrypt(char* data)
{
    int key = 6;
    char ch;
    for (int i = 0; data[i] != '\0';++i) {
        ch = data[i];
        //Lowercase characters.
        if (islower(ch)) {
            ch = (ch - 'a' - key + 26) % 26 + 'a';
        } else if (isupper(ch)) {
            ch = (ch - 'A' - key + 26) % 26 + 'A';
        } else if (isdigit(ch)) {
            ch = (ch - '0' - key + 10) % 10 + '0';
        }
        data[i] = ch;
    }
}


static int client_GetLastId_callback(void *data, int argc, char **argv,char **azColName)
{
    userid = atoi(argv[0]);
    return 0;
}
static int Rental_GetLastId_callback(void *data, int argc, char **argv,char **azColName)
{
    Rentalid = atoi(argv[0]);
    return 0;
}



int main(int argc,
         char **argv)
{
    int status;  // Show the application status
    char admin_pass[] = "abdelmer_1234";
    int rc = sqlite3_open("system.db", &db); // Create system database sql file.
    /* Check that the database is created or not*/
    if (rc != SQLITE_OK) {

        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    /*
     **  Initialize the System Databases
     */


    /* Create Vehicles Data base*/
    char *sql = "CREATE TABLE IF NOT EXISTS Vehicles(Vehicle_Id INT PRIMARY KEY,"
                " Brand TEXT, Model TEXT, Year TEXT, Fuel_type TEXT,"
                " Transmission TEXT,"
                " Seating_Capacity TEXT)";
    sqlite3_exec(db, sql, 0, 0, &err_msg);
    sql = "INSERT INTO Vehicles VALUES(1,'BMW','X3','2012','Gasoline','Manual','4');"
          "INSERT INTO Vehicles VALUES(2,'AUDI','A6','2022','Petrol','Automatic','4');"
          "INSERT INTO Vehicles VALUES(3,'Peugeout','301','2015','Gasoline','Manual','4');";
    sqlite3_exec(db, sql, 0, 0, &err_msg);

    /*Create Administrator Data base*/
    sql =  "CREATE TABLE IF NOT EXISTS Administrator(Admin_Id INT PRIMARY KEY,"
           " Admin_Username TEXT, Admin_Password TEXT)";
    sqlite3_exec(db, sql, 0, 0, &err_msg);

    /* Create Clients Database*/
    sql = "CREATE TABLE IF NOT EXISTS Clients(User_Id INT PRIMARY KEY, Last_Name TEXT,"
          " First_Name TEXT,Email TEXT,Phone_Number TEXT,Password TEXT )";
    sqlite3_exec(db, sql, 0, 0, &err_msg);
    caesar_chiper_encrypt(admin_pass);
    sprintf(sql_db,"INSERT INTO Administrator VALUES(1,'abdlmer','%s')",admin_pass);
    printf("%s",sql_db);
    sqlite3_exec(db, sql_db, 0, 0, &err_msg);
    /* Create Rentals Database*/
    sql = "CREATE TABLE IF NOT EXISTS Rentals(Rental_Id INT PRIMARY KEY,"
          "User_Id INT,"
          "Start_date TEXT,End_date TEXT, Cost INT, Rental_Status TEXT,"
          "Vehicle_Id INT, FOREIGN KEY(Vehicle_Id) REFERENCES Vehicles(Vehicle_Id),"
          " FOREIGN KEY(User_Id) REFERENCES Clients(User_Id))";
    sqlite3_exec(db, sql, 0, 0, &err_msg);
    /*Get the number of inserted rows in Clients table*/
    sprintf(sql_db,"SELECT User_Id from Clients");
    sqlite3_exec(db,sql_db,client_GetLastId_callback,0,&err_msg);
    printf("\n%lld\n",userid);
    sprintf(sql_db,"SELECT Rental_Id from Rentals");
    sqlite3_exec(db,sql_db,Rental_GetLastId_callback,0,&err_msg);
    printf("%lld\n",Rentalid);



    return 0;
}