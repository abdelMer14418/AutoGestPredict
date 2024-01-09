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

GtkWidget *login_username_entry;
GtkWidget *login_password_entry;

GtkWidget * Vehicle_PredictionYear_entry;
GtkWidget * Vehicle_PredictionKm_entry;
GtkWidget * Vehicle_PredictionHP_entry;
GtkWidget * Vehicle_PredictionAcc_entry;
GtkWidget * Vehicle_PredictionContinent_entry;

GtkListStore *Vehiclesstore;



sqlite3 *db;  // Declare a sqlite Database.
char *err_msg = 0;
char sql_db[500];
sqlite_int64 userid = 0;
sqlite_int64 Rentalid = 0;
int isemailfound = -1;
int islogintrue = -1;
int current_user_id;
int isadminlogintrue = -1;





enum
{
    COL_Vehicle_Id = 0,
    COL_Brand,
    COL_Model,
    COL_Year,
    COL_Fuel,
    COL_Transmission,
    COL_Seating,
    NUM_Vehicle_COLS
} ;

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

static int client_CheckLoginCredintials_callback(void *data, int argc, char **argv,char **azColName)
{
    int result;
    char*pass_decrypt;
    pass_decrypt = argv[5];
    caesar_chiper_decrypt(pass_decrypt);
    result = strcmp((char*)data,pass_decrypt);
    printf("%s\n",(char*)data);
    printf("%s\n",pass_decrypt);
    if(result == 0)
    {
        islogintrue = result;
        current_user_id = atoi(argv[0]);
    }
    //printf("%s",argv[0]);
    return 0;
}

static int Admin_CheckLoginCredintials_callback(void *data, int argc, char **argv,char **azColName)
{
    int result;
    char*pass_decrypt;
    pass_decrypt = argv[2];
    caesar_chiper_decrypt(pass_decrypt);
    result = strcmp((char*)data,pass_decrypt);
    printf("%s\n",(char*)data);
    printf("%s\n",argv[2]);
    printf("%s\n",pass_decrypt);
    if(result == 0)
    {
        isadminlogintrue = result;
    }
    return 0;
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

static int client_CheckMail_callback(void *data, int argc, char **argv,char **azColName)
{
    int result = strcmp((char*)data,argv[0]);
    if (result == 0)
    {
        isemailfound = result;
    }
    return 0;
}


static int Vehicles_client_retrieve_callback (void *data, int argc, char **argv,char **azColName)
{
    GtkTreeIter iter;
    gtk_list_store_append (Vehiclesstore, &iter);
    gtk_list_store_set (Vehiclesstore, &iter,
                        COL_Vehicle_Id, atoi(argv[0]),
                        COL_Brand, argv[1],
                        COL_Model, argv[2],
                        COL_Year, argv[3],
                        COL_Fuel,argv[4],
                        COL_Transmission,argv[5],
                        COL_Seating,argv[6],
                        -1);
    return 0;
}

static GtkTreeModel *
createVehicle_and_fill_model (void)
{
    Vehiclesstore = gtk_list_store_new (NUM_Vehicle_COLS,
                                        G_TYPE_UINT,
                                        G_TYPE_STRING,
                                        G_TYPE_STRING,
                                        G_TYPE_STRING,
                                        G_TYPE_STRING,
                                        G_TYPE_STRING,
                                        G_TYPE_STRING);
    sprintf(sql_db, "SELECT * from Vehicles");
    sqlite3_exec(db, sql_db, Vehicles_client_retrieve_callback, 0, &err_msg);
    /* Append a row and fill in some data */
    return GTK_TREE_MODEL (Vehiclesstore);
}

static GtkWidget *
createvehicle_view_and_model (void)
{
    GtkWidget *view = gtk_tree_view_new ();

    GtkCellRenderer *renderer;

    /* --- Column #2 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "Vehicle Id",
                                                 renderer,
                                                 "text", COL_Vehicle_Id,
                                                 NULL);
    /* --- Column #1 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "Brand",
                                                 renderer,
                                                 "text", COL_Brand,
                                                 NULL);

    /* --- Column #1 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "Model",
                                                 renderer,
                                                 "text", COL_Model,
                                                 NULL);
    /* --- Column #1 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "Year",
                                                 renderer,
                                                 "text", COL_Year,
                                                 NULL);
    /* --- Column #1 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "Fuel",
                                                 renderer,
                                                 "text", COL_Fuel,
                                                 NULL);
    /* --- Column #1 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "Transmission",
                                                 renderer,
                                                 "text", COL_Transmission,
                                                 NULL);
    /* --- Column #1 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "Seating Capacity",
                                                 renderer,
                                                 "text", COL_Seating,
                                                 NULL);
    GtkTreeModel *model = createVehicle_and_fill_model ();

    gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);

    /* The tree view has acquired its own reference to the
     *  model, so we can drop ours. That way the model will
     *  be freed automatically when the tree view is destroyed
     */
    g_object_unref (model);


    return view;
}

static void Vehicle_database_callback(GtkWidget *widget,gpointer data)
{
    GtkWidget *vehicless_window;
    GtkBox *VehiclesBox;
    GtkWidget *Vehicles_Button_Selection;
    vehicless_window = gtk_window_new();
    VehiclesBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_window_set_child(GTK_WINDOW(vehicless_window),VehiclesBox);
    GtkWidget *Vehiclesview = createvehicle_view_and_model();
    Vehicles_Button_Selection = gtk_button_new_with_label("Available Vehicles");
    gtk_box_append(VehiclesBox,Vehiclesview);
    gtk_box_append(VehiclesBox,Vehicles_Button_Selection);
    //g_signal_connect(Vehicles_Button_Selection, "clicked", G_CALLBACK(VehicleSelection_database_callback), NULL);
    gtk_window_present(GTK_WINDOW(vehicless_window));
}

static void admin_prediction_callback(GtkWidget *widget, gpointer data)
{
    GtkWidget *PredictionWindow;
    GtkBox *PredictionBox;
    GtkWidget *Button_Predict;
    PredictionWindow = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(PredictionWindow), "Vehicle Prediction"); // Set the title of the window
    PredictionBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child(GTK_WINDOW(PredictionWindow), PredictionBox);
    Vehicle_PredictionYear_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(Vehicle_PredictionYear_entry),"Year");
    Vehicle_PredictionKm_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(Vehicle_PredictionKm_entry),"Kilometer");
    Vehicle_PredictionHP_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(Vehicle_PredictionHP_entry),"Horse Power");
    Vehicle_PredictionAcc_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(Vehicle_PredictionAcc_entry),"Accident");
    Vehicle_PredictionContinent_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(Vehicle_PredictionContinent_entry),"Continent");
    Button_Predict = gtk_button_new_with_label("Predict");
    gtk_box_append(PredictionBox, Vehicle_PredictionYear_entry);
    gtk_box_append(PredictionBox, Vehicle_PredictionKm_entry);
    gtk_box_append(PredictionBox, Vehicle_PredictionHP_entry);
    gtk_box_append(PredictionBox, Vehicle_PredictionAcc_entry);
    gtk_box_append(PredictionBox, Vehicle_PredictionContinent_entry);
    gtk_box_append(PredictionBox, Button_Predict);
    //g_signal_connect(Button_Predict, "clicked", G_CALLBACK(PredictionButton_callback), NULL);
    gtk_window_present(GTK_WINDOW(PredictionWindow));  // to show the window.
}

static void Administrator_callback(GtkWidget *widget, gpointer data) {
    GtkWidget *AdminWindow;
    GtkBox *AdminBox;
    GtkWidget *Button_Tables_selection;
    GtkWidget *Button_Car_Management;
    GtkWidget *Button_User_Management;
    GtkWidget *Button_Rental_History;
    AdminWindow = gtk_window_new(); // Create Admin window.
    gtk_window_set_title(GTK_WINDOW(AdminWindow), "Admin"); // Set the title of the window
    AdminBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0); // Create Admin box.
    gtk_window_set_child(GTK_WINDOW(AdminWindow), AdminBox);
    Button_Tables_selection = gtk_button_new_with_label("Table Selection");
    Button_Car_Management = gtk_button_new_with_label("Car Prediction");
    Button_User_Management = gtk_button_new_with_label("User Management");
    Button_Rental_History = gtk_button_new_with_label("Rental History");
    gtk_box_append(AdminBox, Button_Tables_selection);
    gtk_box_append(AdminBox, Button_Car_Management);
    gtk_box_append(AdminBox, Button_User_Management);
    gtk_box_append(AdminBox, Button_Rental_History);
    //g_signal_connect(Button_Tables_selection, "clicked", G_CALLBACK(Admin_tableselection_callback), NULL);
    //g_signal_connect(Button_Rental_History, "clicked", G_CALLBACK(Admin_RentalHistory_callback), NULL);
    //g_signal_connect(Button_User_Management, "clicked", G_CALLBACK(users_table_callback), NULL);
    g_signal_connect(Button_Car_Management, "clicked", G_CALLBACK(admin_prediction_callback), NULL);
    gtk_window_present(GTK_WINDOW(AdminWindow));  // to show the window.
}

static void Admin_login_database_callback(GtkWidget *widget,gpointer data)
{
    GtkWidget *Admin_loginwindow;
    GtkEntryBuffer *databuffer;
    char* datalogin;
    char* passlogin;
    GtkWidget *datalabel;
    GtkWidget *Admin_proceed_Button;
    GtkBox *Admin_loginBox;
    isadminlogintrue = -1;
    Admin_loginwindow = gtk_window_new(); // Create login window
    Admin_loginBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0); // Create login box.
    gtk_window_set_child(GTK_WINDOW(Admin_loginwindow),Admin_loginBox);
    databuffer = gtk_entry_get_buffer(login_Admin_username_entry);
    datalogin = gtk_entry_buffer_get_text(databuffer);
    databuffer = gtk_entry_get_buffer(login_Admin_password_entry);
    passlogin = gtk_entry_buffer_get_text(databuffer);
    sprintf(sql_db,"SELECT * from Administrator where Admin_Username='%s'",datalogin);
    sqlite3_exec(db,sql_db,Admin_CheckLoginCredintials_callback,passlogin,&err_msg);
    if (isadminlogintrue == 0)
    {
        datalabel = gtk_label_new("right credentials!");
        gtk_box_append(Admin_loginBox,datalabel);
        Admin_proceed_Button = gtk_button_new_with_label("Proceed!");
        gtk_box_append(Admin_loginBox,Admin_proceed_Button);
        g_signal_connect(Admin_proceed_Button, "clicked", G_CALLBACK(Administrator_callback), NULL);
    }
    else
    {
        datalabel = gtk_label_new("Wrong credentials!");
        gtk_box_append(Admin_loginBox,datalabel);
    }
    gtk_window_present(GTK_WINDOW(Admin_loginwindow));
}

static void login_database_callback(GtkWidget *widget,gpointer data)
{
    GtkEntryBuffer *databuffer;
    char* datalogin;
    char* passlogin;
    GtkWidget *LoginStatuswindow;
    GtkWidget *datalabel;
    GtkBox *User_MenuBox;
    GtkWidget *Vehicles_Button;
    GtkWidget *Vehicles_Prediction_Button;
    GtkCalendar *ReservationCalendar;
    LoginStatuswindow = gtk_window_new(); // Create login window.
    User_MenuBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0); // Create login box.
    gtk_window_set_child(GTK_WINDOW(LoginStatuswindow),User_MenuBox);
    Vehicles_Button = gtk_button_new_with_label("Available Vehicles");
    Vehicles_Prediction_Button = gtk_button_new_with_label("Predict Vehicle Cost");
    // ReservationCalendar = gtk_calendar_new();
    islogintrue = -1;
    databuffer = gtk_entry_get_buffer(login_username_entry);
    datalogin = gtk_entry_buffer_get_text(databuffer);
    databuffer = gtk_entry_get_buffer(login_password_entry);
    passlogin = gtk_entry_buffer_get_text(databuffer);
    sprintf(sql_db,"SELECT * from Clients where Email='%s'",datalogin);
    sqlite3_exec(db,sql_db,client_CheckLoginCredintials_callback,passlogin,&err_msg);

    if(islogintrue == 0)
    {
        // gtk_box_append(User_MenuBox,ReservationCalendar);
        gtk_box_append(User_MenuBox,Vehicles_Button);
        gtk_box_append(User_MenuBox,Vehicles_Prediction_Button);
        g_signal_connect(Vehicles_Button, "clicked", G_CALLBACK(Vehicle_database_callback), NULL);
        g_signal_connect(Vehicles_Prediction_Button, "clicked", G_CALLBACK(admin_prediction_callback), NULL);
    }
    else
    {
        datalabel = gtk_label_new("Wrong Email or Password!");
        gtk_box_append(User_MenuBox,datalabel);
    }

    gtk_window_present(GTK_WINDOW(LoginStatuswindow));
}

static void register_database_callback(GtkWidget *widget,gpointer data) {
    GtkWidget *RegisterStatuswindow;
    GtkEntryBuffer *register_databuffer;
    char *firstname, *lastname, *password, *email, *phonenumber;
    GtkWidget *datalabel;
    RegisterStatuswindow = gtk_window_new(); // Create login window.
    register_databuffer = gtk_entry_get_buffer(GTK_ENTRY(register_firstname_entry));
    firstname = gtk_entry_buffer_get_text(register_databuffer);
    register_databuffer = gtk_entry_get_buffer(GTK_ENTRY(register_lastname_entry));
    lastname = gtk_entry_buffer_get_text(register_databuffer);
    register_databuffer = gtk_entry_get_buffer(GTK_ENTRY(register_password_entry));
    password = gtk_entry_buffer_get_text(register_databuffer);
    caesar_chiper_encrypt(password);
    register_databuffer = gtk_entry_get_buffer(GTK_ENTRY(register_email_entry));
    email = gtk_entry_buffer_get_text(register_databuffer);
    register_databuffer = gtk_entry_get_buffer(GTK_ENTRY(register_phonenumber_entry));
    phonenumber = gtk_entry_buffer_get_text(register_databuffer);
    isemailfound = -1;
    sprintf(sql_db, "SELECT Email from Clients");
    sqlite3_exec(db, sql_db, client_CheckMail_callback, email, &err_msg);
    if (isemailfound == 0)
    {
        datalabel = gtk_label_new("Email is already used");
    }
    else
    {
        sprintf(sql_db,"SELECT User_Id from Clients");
        sqlite3_exec(db,sql_db,client_GetLastId_callback,0,&err_msg);
        userid = userid + 1;
        sprintf (sql_db,"INSERT INTO Clients VALUES(%lld,'%s','%s','%s','%s','%s');",userid,lastname,firstname,email,phonenumber,password);
        sqlite3_exec(db, sql_db, 0, 0, &err_msg);
        datalabel = gtk_label_new("Created Successfully!");
    }
    gtk_window_set_child(RegisterStatuswindow,datalabel);
    gtk_window_present(GTK_WINDOW(RegisterStatuswindow));
}

static void Login_callback(GtkWidget *widget, gpointer data)
{
    GtkWidget *loginwindow;
    GtkWidget *login_Button;
    GtkBox *loginBox;
    GtkPasswordEntry *pass;
    loginwindow = gtk_window_new(); // Create login window.
    gtk_window_set_title(GTK_WINDOW(loginwindow), "Log in Page"); // Set the title of the window
    loginBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0); // Create login box.
    gtk_window_set_child(GTK_WINDOW(loginwindow),loginBox);
    login_username_entry = gtk_entry_new();
    login_password_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(login_password_entry),FALSE);
    login_Button = gtk_button_new_with_label("Log in");
    gtk_entry_set_placeholder_text(GTK_ENTRY(login_username_entry),"Email Address");
    gtk_entry_set_placeholder_text(GTK_ENTRY(login_password_entry),"Password");
    gtk_box_append(loginBox,login_username_entry);
    gtk_box_append(loginBox,login_password_entry);
    gtk_box_append(loginBox,login_Button);
    g_signal_connect(login_Button, "clicked", G_CALLBACK(login_database_callback), NULL);
    gtk_window_present(GTK_WINDOW(loginwindow));
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
    g_signal_connect(register_Button, "clicked", G_CALLBACK(register_database_callback), NULL);
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
    g_signal_connect(Button_User_Login, "clicked", G_CALLBACK(Login_callback), NULL);
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
    g_signal_connect(Admin_login_Button, "clicked", G_CALLBACK(Admin_login_database_callback), NULL);
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