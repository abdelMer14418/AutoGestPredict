//
// Created by abdelhak on 06/01/2024.
//


#include <stdio.h>
#include <gtk/gtk.h>
#include <ctype.h>
#include ".\src\sqlite3.h"
#include <curl/curl.h>

GtkWidget *login_Admin_username_entry;
GtkWidget *login_Admin_password_entry;

GtkWidget *register_firstname_entry;
GtkWidget *register_lastname_entry;
GtkWidget *register_password_entry;
GtkWidget *register_email_entry;
GtkWidget *register_phonenumber_entry;

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

static void register_callback(GtkWidget *widget, gpointer data)
{
    GtkWidget *registerwindow;
    GtkWidget *register_Button;
    GtkBox *registerBox;
    registerwindow = gtk_window_new(); // Create login window.
    gtk_window_set_title(GTK_WINDOW(registerwindow), "Register Page"); // Set the title of the window
    registerBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0); // Create register box.
    gtk_window_set_child(GTK_WINDOW(registerwindow),registerBox);
    register_firstname_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(register_firstname_entry),"First Name");
    register_lastname_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(register_lastname_entry),"Last Name");
    register_password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(register_password_entry),"Password");
    gtk_entry_set_visibility(GTK_ENTRY(register_password_entry),FALSE);
    register_email_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(register_email_entry),"Email Address");
    register_phonenumber_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(register_phonenumber_entry),"Phone Number");
    register_Button = gtk_button_new_with_label("Sign Up");
    gtk_box_append(registerBox,register_firstname_entry);
    gtk_box_append(registerBox,register_lastname_entry);
    gtk_box_append(registerBox,register_email_entry);
    gtk_box_append(registerBox,register_phonenumber_entry);
    gtk_box_append(registerBox,register_password_entry);
    gtk_box_append(registerBox,register_Button);
    //g_signal_connect(register_Button, "clicked", G_CALLBACK(register_database_callback), NULL);
    gtk_window_present(GTK_WINDOW(registerwindow));
}


static void User_callback(GtkWidget *widget, gpointer data)
{
    GtkWidget *UserWindow;
    GtkBox *UserBox;
    GtkWidget *Button_User_Login;
    GtkWidget *Button_User_Register;

    UserWindow = gtk_window_new(); // Create Admin window.
    gtk_window_set_title(GTK_WINDOW(UserWindow), "User"); // Set the title of the window
    UserBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0); // Create Admin box.
    gtk_window_set_child(GTK_WINDOW(UserWindow),UserBox);
    Button_User_Login = gtk_button_new_with_label("Log in");
    Button_User_Register = gtk_button_new_with_label("Register");
    gtk_box_append(UserBox,Button_User_Login);
    gtk_box_append(UserBox,Button_User_Register);
    gtk_window_present(GTK_WINDOW(UserWindow));  // to show the window.
    g_signal_connect(Button_User_Register, "clicked", G_CALLBACK(register_callback), NULL);
    //g_signal_connect(Button_User_Login, "clicked", G_CALLBACK(Login_callback), NULL);
}
static void Admin_Login_callback(GtkWidget *widget, gpointer data)
{
    GtkWidget *Admin_loginwindow;
    GtkWidget *Admin_login_Button;
    GtkBox *Admin_loginBox;
    GtkPasswordEntry *pass;
    Admin_loginwindow = gtk_window_new(); // Create login window.
    gtk_window_set_title(GTK_WINDOW(Admin_loginwindow), "Log in Page"); // Set the title of the window
    Admin_loginBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0); // Create login box.
    gtk_window_set_child(GTK_WINDOW(Admin_loginwindow),Admin_loginBox);
    login_Admin_username_entry = gtk_entry_new();
    login_Admin_password_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(login_Admin_password_entry),FALSE);
    Admin_login_Button = gtk_button_new_with_label("Log in");
    gtk_entry_set_placeholder_text(GTK_ENTRY(login_Admin_username_entry),"Admin Username");
    gtk_entry_set_placeholder_text(GTK_ENTRY(login_Admin_password_entry),"Admin Password");
    gtk_box_append(Admin_loginBox,login_Admin_username_entry);
    gtk_box_append(Admin_loginBox,login_Admin_password_entry);
    gtk_box_append(Admin_loginBox,Admin_login_Button);
    //g_signal_connect(Admin_login_Button, "clicked", G_CALLBACK(Admin_login_database_callback), NULL);
    gtk_window_present(GTK_WINDOW(Admin_loginwindow));
}
static void
activate(GtkApplication *app,
         gpointer user_data)
{
    GtkWidget *MainWindow;
    GtkWidget *Button_Administrator;
    GtkWidget *Button_User;
    GtkBox *Mainbox;
    GtkWidget *Mainlabel;

    MainWindow = gtk_application_window_new(app); // create new window
    gtk_window_set_title(GTK_WINDOW(MainWindow), "Home"); // Set the title of the window
    gtk_window_set_default_size(GTK_WINDOW(MainWindow), 200, 200); // set the size of window
    Mainbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_window_set_child(GTK_WINDOW(MainWindow),Mainbox);
    Button_Administrator = gtk_button_new_with_label("Administrator");
    Button_User = gtk_button_new_with_label("User");
    gtk_box_append(Mainbox,Button_Administrator);
    gtk_box_append(Mainbox,Button_User);
    g_signal_connect(Button_Administrator, "clicked", G_CALLBACK(Admin_Login_callback), NULL);
    g_signal_connect(Button_User, "clicked", G_CALLBACK(User_callback), NULL);
    gtk_window_present(GTK_WINDOW(MainWindow));  // to show the window.
}




int main(int argc,
         char **argv)
{
    GtkApplication *app; // Create GTK application
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
    printf("%lld\n",userid);
    sprintf(sql_db,"SELECT Rental_Id from Rentals");
    sqlite3_exec(db,sql_db,Rental_GetLastId_callback,0,&err_msg);
    printf("%lld\n",Rentalid);
    /* Initialize the GTK application*/
    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);


    return status;
}