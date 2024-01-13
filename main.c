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

GtkWidget *admin_deleteId_entry;
GtkWidget *admin_Readuser_entry;


GtkListStore *Vehiclesstore;
GtkListStore *store;
GtkListStore *Readstore;
GtkListStore *RentalHistory;

GtkWidget *Update_Id_entry;
GtkWidget *Update_firstname_entry;
GtkWidget *Update_lastname_entry;
GtkWidget *Update_password_entry;
GtkWidget *Update_email_entry;
GtkWidget *Update_phonenumber_entry;





sqlite3 *db;  // Declare a sqlite Database.
char *err_msg = 0;
char sql_db[500];
sqlite_int64 userid = 0;
sqlite_int64 Rentalid = 0;
int isemailfound = -1;
int islogintrue = -1;
int current_user_id;
int isadminlogintrue = -1;
char carcost[10];





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

enum
{
    COL_Id = 0,
    COL_First_NAME,
    COL_Last_Name,
    COL_Email,
    COL_Number,
    NUM_COLS
} ;

enum
{
    COL_RentalHistory_RentalId = 0,
    COL_RentalHistory_UserId,
    COL_RentalHistory_StartDate,
    COL_RentalHistory_EndData,
    COL_RentalHistory_Cost,
    COL_RentalHistory_Status,
    COL_RentalHistory_VehicleId,
    NUM_RentalHistory_COLS
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

static int admin_client_retrieve_callback (void *data, int argc, char **argv,char **azColName)
{
    GtkTreeIter iter;
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter,
                        COL_Id, atoi(argv[0]),
                        COL_First_NAME, argv[2],
                        COL_Last_Name, argv[1],
                        COL_Email,argv[3],
                        COL_Number,argv[4],
                        -1);
    return 0;
}

static size_t cb(void *data, size_t size, size_t nmemb, void *clientp)
{
    int j =0;
    for (size_t i = 1445; i <1470;i++)
    {
        if (isdigit(((char *)data)[i]) || ((char *)data)[i] == '$' || ((char *)data)[i] == '.')
        {
            carcost[j] = ((char *)data)[i];
            j++;
        }
    }
    carcost[j] = '\0';
    printf("%s",carcost);
    // Predictionlabel = gtk_label_new(carcost);
    // gtk_box_append(PredictionBox, Predictionlabel);
    return nmemb;
}


static void PredictionButton_callback(GtkWidget *widget, gpointer data)
{
    CURL *curl;
    CURLcode res;
    GtkEntryBuffer *Prediction_databuffer;
    char *Year, *Km, *Hp, *Acc, *Cont;
    char request[50];
    GtkWidget *PredictionResultWindow;
    GtkWidget *PredictionResultlabel;
    PredictionResultWindow = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(PredictionResultWindow), "Vehicle Predicted Cost"); // Set the title of the window
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    Prediction_databuffer = gtk_entry_get_buffer(GTK_ENTRY(Vehicle_PredictionYear_entry));
    Year = gtk_entry_buffer_get_text(Prediction_databuffer);

    Prediction_databuffer = gtk_entry_get_buffer(GTK_ENTRY(Vehicle_PredictionKm_entry));
    Km = gtk_entry_buffer_get_text(Prediction_databuffer);

    Prediction_databuffer = gtk_entry_get_buffer(GTK_ENTRY(Vehicle_PredictionHP_entry));
    Hp = gtk_entry_buffer_get_text(Prediction_databuffer);

    Prediction_databuffer = gtk_entry_get_buffer(GTK_ENTRY(Vehicle_PredictionAcc_entry));
    Acc = gtk_entry_buffer_get_text(Prediction_databuffer);

    Prediction_databuffer = gtk_entry_get_buffer(GTK_ENTRY(Vehicle_PredictionContinent_entry));
    Cont = gtk_entry_buffer_get_text(Prediction_databuffer);
    sprintf(request,"a=%s&b=%s&c=%s&d=%s&e=%s",Year,Km,Hp,Acc,Cont);
    printf("%s",request);
    if(curl) {
        /* First set the URL that is about to receive our POST. This URL can
           just as well be an https:// URL if that is what should receive the
           data. */

        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:5000/predict");
        /* Now specify the POST data */
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        else
        {
            PredictionResultlabel = gtk_label_new(carcost);
            gtk_window_set_child(GTK_WINDOW(PredictionResultWindow), PredictionResultlabel);
            gtk_window_present(GTK_WINDOW(PredictionResultWindow));  // to show the window.
        }


        /* always cleanup */
        curl_easy_cleanup(curl);
    }
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

static GtkTreeModel *
create_and_fill_model (void)
{
    store = gtk_list_store_new (NUM_COLS,
                                G_TYPE_UINT,
                                G_TYPE_STRING,
                                G_TYPE_STRING,
                                G_TYPE_STRING,
                                G_TYPE_STRING);
    sprintf(sql_db, "SELECT * from Clients");
    sqlite3_exec(db, sql_db, admin_client_retrieve_callback, 0, &err_msg);
    /* Append a row and fill in some data */
    return GTK_TREE_MODEL (store);
}
static int admin_RentalHistory_retrieve_callback (void *data, int argc, char **argv,char **azColName)
{
    GtkTreeIter iter;
    gtk_list_store_append (RentalHistory, &iter);
    gtk_list_store_set (RentalHistory, &iter,
                        COL_RentalHistory_RentalId, atoi(argv[0]),
                        COL_RentalHistory_UserId, atoi(argv[2]),
                        COL_RentalHistory_StartDate, argv[1],
                        COL_RentalHistory_EndData,argv[3],
                        COL_RentalHistory_Cost,atoi(argv[4]),
                        COL_RentalHistory_Status,argv[5],
                        COL_RentalHistory_VehicleId,atoi(argv[6]),
                        -1);
    return 0;
}
static GtkTreeModel *
RantalHistory_create_and_fill_model (void)
{
    RentalHistory = gtk_list_store_new (NUM_RentalHistory_COLS,
                                        G_TYPE_UINT,
                                        G_TYPE_UINT,
                                        G_TYPE_STRING,
                                        G_TYPE_STRING,
                                        G_TYPE_UINT,
                                        G_TYPE_STRING,
                                        G_TYPE_UINT);
    sprintf(sql_db, "SELECT * from Rentals");
    sqlite3_exec(db, sql_db, admin_RentalHistory_retrieve_callback, 0, &err_msg);
    /* Append a row and fill in some data */
    return GTK_TREE_MODEL (RentalHistory);
}

static GtkWidget *
create_view_and_model (void)
{
    GtkWidget *view = gtk_tree_view_new ();

    GtkCellRenderer *renderer;

    /* --- Column #2 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "User Id",
                                                 renderer,
                                                 "text", COL_Id,
                                                 NULL);
    /* --- Column #1 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "First Name",
                                                 renderer,
                                                 "text", COL_First_NAME,
                                                 NULL);

    /* --- Column #1 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "Last Name",
                                                 renderer,
                                                 "text", COL_Last_Name,
                                                 NULL);
    /* --- Column #1 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "Email",
                                                 renderer,
                                                 "text", COL_Email,
                                                 NULL);
    /* --- Column #1 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "Phone Number",
                                                 renderer,
                                                 "text", COL_Number,
                                                 NULL);

    GtkTreeModel *model = create_and_fill_model ();

    gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);

    /* The tree view has acquired its own reference to the
     *  model, so we can drop ours. That way the model will
     *  be freed automatically when the tree view is destroyed
     */
    g_object_unref (model);



    return view;
}
static void Admin_users_Delete_Request_callback(GtkWidget *widget,gpointer data)
{
    GtkEntryBuffer *deleted_id_databuffer;
    const char* deleted_id;
    deleted_id_databuffer = gtk_entry_get_buffer(GTK_ENTRY(admin_deleteId_entry));
    deleted_id = gtk_entry_buffer_get_text(deleted_id_databuffer);
    printf("%s\n",deleted_id);
    sprintf(sql_db,"DELETE from Clients where User_Id=%s",deleted_id);
    sqlite3_exec(db, sql_db, 0, 0, &err_msg);
    printf("%s",sql_db);
}
static void admin_user_Delete(GtkWidget *widget, gpointer data)
{
    GtkWidget *users_delete_window;
    GtkBox *UsersdeleteBox;
    GtkWidget *User_Delete_Button;
    users_delete_window = gtk_window_new();
    UsersdeleteBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_window_set_child(GTK_WINDOW(users_delete_window),UsersdeleteBox);
    admin_deleteId_entry = gtk_entry_new();
    User_Delete_Button = gtk_button_new_with_label("Delete");
    gtk_entry_set_placeholder_text(GTK_ENTRY(admin_deleteId_entry),"Enter the ID");
    gtk_box_append(UsersdeleteBox,admin_deleteId_entry);
    gtk_box_append(UsersdeleteBox,User_Delete_Button);
    g_signal_connect(User_Delete_Button, "clicked", G_CALLBACK(Admin_users_Delete_Request_callback), NULL);
    gtk_window_present(GTK_WINDOW(users_delete_window));
}

static GtkWidget *
RentalHistory_create_view_and_model (void)
{
    GtkWidget *view = gtk_tree_view_new ();

    GtkCellRenderer *renderer;

    /* --- Column #2 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "Rental Id",
                                                 renderer,
                                                 "text", COL_RentalHistory_RentalId,
                                                 NULL);
    /* --- Column #1 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "User Id",
                                                 renderer,
                                                 "text", COL_RentalHistory_UserId,
                                                 NULL);

    /* --- Column #1 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "Start Date",
                                                 renderer,
                                                 "text", COL_RentalHistory_StartDate,
                                                 NULL);
    /* --- Column #1 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "End Date",
                                                 renderer,
                                                 "text", COL_RentalHistory_EndData,
                                                 NULL);
    /* --- Column #1 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "Cost",
                                                 renderer,
                                                 "text", COL_RentalHistory_Cost,
                                                 NULL);
    /* --- Column #1 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "Status",
                                                 renderer,
                                                 "text", COL_RentalHistory_Status,
                                                 NULL);
    /* --- Column #1 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "Vehicle Id",
                                                 renderer,
                                                 "text", COL_RentalHistory_VehicleId,
                                                 NULL);

    GtkTreeModel *model = RantalHistory_create_and_fill_model();

    gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);

    /* The tree view has acquired its own reference to the
     *  model, so we can drop ours. That way the model will
     *  be freed automatically when the tree view is destroyed
     */
    g_object_unref (model);

    return view;
}
static void Admin_RentalHistory_callback(GtkWidget *widget, gpointer data)
{
    GtkWidget *AdminRentalHistory_window;
    GtkBox *RentalHistoryBox;
    AdminRentalHistory_window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(AdminRentalHistory_window), "Rental History"); // Set the title of the window
    RentalHistoryBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_window_set_child(GTK_WINDOW(AdminRentalHistory_window), RentalHistoryBox);
    GtkWidget *view = RentalHistory_create_view_and_model();
    gtk_box_append(RentalHistoryBox, view);
    gtk_window_present(GTK_WINDOW(AdminRentalHistory_window));  // to show the window.
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
static int admin_client_Read_callback (void *data, int argc, char **argv,char **azColName)
{
    GtkTreeIter iter;
    gtk_list_store_append (Readstore, &iter);
    gtk_list_store_set (Readstore, &iter,
                        COL_Id, atoi(argv[0]),
                        COL_First_NAME, argv[2],
                        COL_Last_Name, argv[1],
                        COL_Email,argv[3],
                        COL_Number,argv[4],
                        -1);
    return 0;
}


static GtkTreeModel *
UserReadcreate_and_fill_model (char*data)
{
    Readstore = gtk_list_store_new (NUM_COLS,
                                    G_TYPE_UINT,
                                    G_TYPE_STRING,
                                    G_TYPE_STRING,
                                    G_TYPE_STRING,
                                    G_TYPE_STRING);
    sprintf(sql_db, "SELECT * from Clients WHERE User_Id = %s ",data);
    sqlite3_exec(db, sql_db, admin_client_Read_callback, 0, &err_msg);
    /* Append a row and fill in some data */
    return GTK_TREE_MODEL (Readstore);
}

static GtkWidget *
UserReadcreate_view_and_model (char*data)
{
    GtkWidget *view = gtk_tree_view_new ();

    GtkCellRenderer *renderer;

    /* --- Column #2 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "User Id",
                                                 renderer,
                                                 "text", COL_Id,
                                                 NULL);
    /* --- Column #1 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "First Name",
                                                 renderer,
                                                 "text", COL_First_NAME,
                                                 NULL);

    /* --- Column #1 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "Last Name",
                                                 renderer,
                                                 "text", COL_Last_Name,
                                                 NULL);
    /* --- Column #1 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "Email",
                                                 renderer,
                                                 "text", COL_Email,
                                                 NULL);
    /* --- Column #1 --- */
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                                 -1,
                                                 "Phone Number",
                                                 renderer,
                                                 "text", COL_Number,
                                                 NULL);

    GtkTreeModel *model = UserReadcreate_and_fill_model (data);

    gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);

    /* The tree view has acquired its own reference to the
     *  model, so we can drop ours. That way the model will
     *  be freed automatically when the tree view is destroyed
     */
    g_object_unref (model);

    return view;
}
static void Admin_users_Read_Request_callback(GtkWidget *widget,gpointer data)
{
    GtkWidget *users_window;
    GtkBox *UsersTableBox;
    GtkEntryBuffer *Read_databuffer;
    users_window = gtk_window_new();
    char* id;
    UsersTableBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    Read_databuffer = gtk_entry_get_buffer(GTK_ENTRY(admin_Readuser_entry));
    id = gtk_entry_buffer_get_text(Read_databuffer);
    GtkWidget *view = UserReadcreate_view_and_model(id);
    gtk_window_set_child(GTK_WINDOW(users_window),UsersTableBox);
    gtk_box_append(UsersTableBox,view);
    gtk_window_present(GTK_WINDOW(users_window));
}
static void admin_user_Read_entry(GtkWidget *widget,gpointer data)
{
    GtkWidget *users_Read_window;
    GtkBox *UsersReadBox;
    GtkWidget *User_Read_Button;
    users_Read_window = gtk_window_new();
    UsersReadBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_window_set_child(GTK_WINDOW(users_Read_window),UsersReadBox);
    admin_Readuser_entry = gtk_entry_new();
    User_Read_Button = gtk_button_new_with_label("Read");
    gtk_entry_set_placeholder_text(GTK_ENTRY(admin_Readuser_entry),"Enter the ID");
    gtk_box_append(UsersReadBox,admin_Readuser_entry);
    gtk_box_append(UsersReadBox,User_Read_Button);
    g_signal_connect(User_Read_Button, "clicked", G_CALLBACK(Admin_users_Read_Request_callback), NULL);
    gtk_window_present(GTK_WINDOW(users_Read_window));
}
static void Update_database_callback(GtkWidget *widget, gpointer data)
{
    GtkWidget *UpdateStatuswindow;
    GtkEntryBuffer *Update_databuffer;
    char *firstname, *lastname, *password, *email, *phonenumber, *id;
    GtkWidget *Updatedatalabel;
    UpdateStatuswindow = gtk_window_new(); // Create login window.
    Update_databuffer = gtk_entry_get_buffer(GTK_ENTRY(Update_Id_entry));
    id = gtk_entry_buffer_get_text(Update_databuffer);
    Update_databuffer = gtk_entry_get_buffer(GTK_ENTRY(Update_firstname_entry));
    firstname = gtk_entry_buffer_get_text(Update_databuffer);
    Update_databuffer = gtk_entry_get_buffer(GTK_ENTRY(Update_lastname_entry));
    lastname = gtk_entry_buffer_get_text(Update_databuffer);
    Update_databuffer = gtk_entry_get_buffer(GTK_ENTRY(Update_password_entry));
    password = gtk_entry_buffer_get_text(Update_databuffer);
    Update_databuffer = gtk_entry_get_buffer(GTK_ENTRY(Update_email_entry));
    email = gtk_entry_buffer_get_text(Update_databuffer);
    Update_databuffer = gtk_entry_get_buffer(GTK_ENTRY(Update_phonenumber_entry));
    phonenumber = gtk_entry_buffer_get_text(Update_databuffer);
    sprintf(sql_db, "UPDATE Clients SET Email='%s',First_Name ='%s',"
                    "Last_Name = '%s',"
                    " Phone_Number = '%s',"
                    " Password = '%s' WHERE User_Id=%d",email,firstname,lastname,phonenumber,password,atoi(id));
    sqlite3_exec(db, sql_db, 0, 0, &err_msg);
    printf("%s",sql_db);
}
static void admin_user_Update_entry(GtkWidget *widget, gpointer data)
{
    GtkWidget *Updatewindow;
    GtkWidget *Update_Button;
    GtkBox *UpdateBox;
    Updatewindow = gtk_window_new(); // Create login window.
    gtk_window_set_title(GTK_WINDOW(Updatewindow), "Update Page"); // Set the title of the window
    UpdateBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0); // Create register box.
    gtk_window_set_child(GTK_WINDOW(Updatewindow),UpdateBox);
    Update_Id_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(Update_Id_entry),"User Id");
    Update_firstname_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(Update_firstname_entry),"First Name");
    Update_lastname_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(Update_lastname_entry),"Last Name");
    Update_password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(Update_password_entry),"Password");
    gtk_entry_set_visibility(GTK_ENTRY(Update_password_entry),FALSE);
    Update_email_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(Update_email_entry),"Email Address");
    Update_phonenumber_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(Update_phonenumber_entry),"Phone Number");
    Update_Button = gtk_button_new_with_label("Update");
    gtk_box_append(UpdateBox,Update_Id_entry);
    gtk_box_append(UpdateBox,Update_firstname_entry);
    gtk_box_append(UpdateBox,Update_lastname_entry);
    gtk_box_append(UpdateBox,Update_email_entry);
    gtk_box_append(UpdateBox,Update_phonenumber_entry);
    gtk_box_append(UpdateBox,Update_password_entry);
    gtk_box_append(UpdateBox,Update_Button);
    g_signal_connect(Update_Button, "clicked", G_CALLBACK(Update_database_callback), NULL);
    gtk_window_present(GTK_WINDOW(Updatewindow));
}
static void users_table_callback(GtkWidget *widget,gpointer data)
{
    GtkWidget *users_window;
    GtkBox *UsersTableBox;
    GtkWidget *User_Create_Button;
    GtkWidget *User_Read_Button;
    GtkWidget *User_Delete_Button;
    GtkWidget *User_Update_Button;
    users_window = gtk_window_new();
    UsersTableBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    GtkWidget *view = create_view_and_model();
    gtk_window_set_child(GTK_WINDOW(users_window),UsersTableBox);
    User_Create_Button = gtk_button_new_with_label("Create");
    User_Read_Button = gtk_button_new_with_label("Read");
    User_Delete_Button = gtk_button_new_with_label("Delete");
    User_Update_Button = gtk_button_new_with_label("Update");
    gtk_box_append(UsersTableBox,view);
    gtk_box_append(UsersTableBox,User_Create_Button);
    gtk_box_append(UsersTableBox,User_Read_Button);
    gtk_box_append(UsersTableBox,User_Delete_Button);
    gtk_box_append(UsersTableBox,User_Update_Button);
    g_signal_connect(User_Create_Button, "clicked", G_CALLBACK(register_callback), NULL);
    g_signal_connect(User_Delete_Button, "clicked", G_CALLBACK(admin_user_Delete), NULL);
    g_signal_connect(User_Update_Button, "clicked", G_CALLBACK(admin_user_Update_entry), NULL);
    g_signal_connect(User_Read_Button, "clicked", G_CALLBACK(admin_user_Read_entry), NULL);
    gtk_window_present(GTK_WINDOW(users_window));
}
static void Admin_tableselection_callback(GtkWidget *widget,gpointer data)
{
    GtkWidget *TablesWindow;
    GtkBox *TablesBox;
    GtkWidget *User_Button;
    GtkWidget *Rentals_Button;
    GtkWidget *Vehicles_Button;
    TablesWindow = gtk_window_new(); // Create login window.
    gtk_window_set_title(GTK_WINDOW(TablesWindow), "Tables"); // Set the title of the window
    TablesBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0); // Create login box.
    gtk_window_set_child(GTK_WINDOW(TablesWindow),TablesBox);
    User_Button = gtk_button_new_with_label("Users");
    Rentals_Button = gtk_button_new_with_label("Rentals");
    Vehicles_Button = gtk_button_new_with_label("Vehicles");
    gtk_box_append(TablesBox,User_Button);
    gtk_box_append(TablesBox,Rentals_Button);
    gtk_box_append(TablesBox,Vehicles_Button);
    g_signal_connect(User_Button, "clicked", G_CALLBACK(users_table_callback), NULL);
    //g_signal_connect(Rentals_Button, "clicked", G_CALLBACK(Rentals_table_callback), NULL);
    //g_signal_connect(Vehicles_Button, "clicked", G_CALLBACK(Vehicles_table_callback), NULL);
    gtk_window_present(GTK_WINDOW(TablesWindow));
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
    g_signal_connect(Button_Predict, "clicked", G_CALLBACK(PredictionButton_callback), NULL);
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
    g_signal_connect(Button_Tables_selection, "clicked", G_CALLBACK(Admin_tableselection_callback), NULL);
    g_signal_connect(Button_Rental_History, "clicked", G_CALLBACK(Admin_RentalHistory_callback), NULL);
    g_signal_connect(Button_User_Management, "clicked", G_CALLBACK(users_table_callback), NULL);
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