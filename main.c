//
// Created by abdelhak on 06/01/2024.
//


/*#include <stdio.h>
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
*/
/*
#include <gtk/gtk.h>

static void
print_hello (GtkWidget *widget,
             gpointer   data)
{
    g_print ("Hello World\n");
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
    GtkWidget *window;
    GtkWidget *button;

    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Hello");
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

    button = gtk_button_new_with_label ("Hello World");
    g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
    gtk_window_set_child (GTK_WINDOW (window), button);

    gtk_window_present (GTK_WINDOW (window));
}

int
main (int    argc,
      char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}*/

#include <stdio.h>
#include <curl/curl.h>
struct memory {
    char *response;
    size_t size;
};
static size_t cb(void *data, size_t size, size_t nmemb, void *clientp)
{
    size_t realsize = size * nmemb;
    char carcost[10];
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
    return nmemb;
}
int main() {
    CURL *curl;
    CURLcode res;
    char* searchresult;
    char request[50];
    const char* s2 =  "Prediction:";
    sprintf(request,"a=%d&b=%d&c=%d&d=%d&e=%d",2015,43000,231.0,0,"Europe");
    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:5000/predict");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        else
        {
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 0;

}