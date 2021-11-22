/* Show the CGI (Common Gateway Interface) environment variables */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql.h>

/* Print a basic HTTP header. */

static void
print_http_header(const char *content_type)
{
    printf("Content-Type: %s\n\n", content_type);
}

void getTemperature()
{
    MYSQL *con = mysql_init(NULL);

    if (con == NULL)
    {
        printf("MySQL initialization failed");
        return;
    }
    if (mysql_real_connect(con, "localhost", "root", "abc,123",
                           "MQTT_tc74", 0, NULL, 0) == NULL)
    {
        printf("%s\n", mysql_error(con));
        mysql_close(con);
        return;
    }

    const char *query = "SELECT temperatuur FROM tempValues";

    if (mysql_query(con, query) != 0)
    {
        printf("%s\n", mysql_error(con));
        exit(-1);
    }
    else
    {
        MYSQL_RES *query_results = mysql_store_result(con);
        if (query_results)
        { // make sure there *are* results..
            MYSQL_ROW row;

            while ((row = mysql_fetch_row(query_results)) != 0)
            {
               float f = row[0] ? atof(row[0]) : 0.0f;

                printf("<tr><td>%.1f", f);

            }

            /* Free results when done */
            mysql_free_result(query_results);
        }
    }
}

/* Handle errors by printing a plain text message. */

static void cgi_fail(const char *message)
{
    print_http_header("text/plain");
    printf("%s\n", message);
    exit(0);
}

int main()
{
    print_http_header("text/html");
    printf("<h1>Temperatuur</h1>\n");
    printf("<table>\n");
    getTemperature();
    printf("</table>");
    return 0;
}