/* pn_controller.c: This application is used to start and stop the PhotoNetwork. 
__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technologies"
*/


/*--------------------------------------------------/
/                   System Imports                  /
/--------------------------------------------------*/
#include <string.h>
#include <json-c/json.h>

/*--------------------------------------------------/
/                   Local Imports                   /
/--------------------------------------------------*/
#include "pn_controller.h"

//------ Start of Program --------//
int main(int argc, char *argv[])
{
    //Initialize Variables
    int opt;
    char file_content[1000];
    struct network_settings settings;
    int long_index = 0;

    //Parse input arguments and take appropriate action
    if (argc == 2)
    {
        while ((opt = getopt_long(argc, argv, "skh", long_options, &long_index)) != -1)
        {
            switch (opt)
            {
                case 's':
                    printf("<------->Starting PhotoNetwork Application<------->\n");
                    read_file(SETTINGS_PATH, file_content);
                    parse_json(file_content, &settings);
                    start_pn(&settings);
                    break;
                case 'k':
                    printf("<------->Stopping PhotoNetwork Application<------->\n");
                    stop_pn();
                    break;
                case 'h':
                    printf("Help Menu:\n");
                    print_usage(0);
                    break;
                case '?':
                    printf("Unknown Argument Provided\n");
                    print_usage(1);
                    break;
            }
        }
    }
    else
    {
        printf("Invalid arguments provided\n");
        print_usage(1);
    }
}

static void print_usage(int es)
{
    printf("Usage: Start PhotoNetwork --start || Stop PhotoNetwork --stop || help --help/-h\n");
    exit(es);
}

static void read_file(char *filepath, char *file_content)
{
    //Input     : File Path, file_content pointer
    //Output    : N/A
    //Function  : Read file and populate file_content with contents

    FILE *fp;
    int c; 
    int index = 0;
    
    fp = fopen(filepath, "r");
    while((c = fgetc(fp)) != EOF)
    {
        file_content[index] = c;
        index++;
    }
    file_content[index] = '\0';

}

static void parse_json(char *file_content, struct network_settings *object_p)
{
    //Input     : file_content and structure pointer
    //Output    : N/A
    //Function  : populate network_settings struct with network_settings content

    //Initialize 
    struct json_object *parsed_json;
    struct json_object *jobj_network_mode;
    struct json_object *jobj_sim_mode;
    struct json_object *jobj_test_path;
    struct json_object *jobj_app_path;
    struct json_object *jobj_server_apps;
    struct json_object *jobj_client_apps;
    struct json_object *jobj_py_ext;
    struct json_object *app;


    //Parse json file_contents
    parsed_json = json_tokener_parse(file_content);

    //Get contents by json key
    json_object_object_get_ex(parsed_json, "network_mode", &jobj_network_mode);
    json_object_object_get_ex(parsed_json, "sim_mode",     &jobj_sim_mode);
    json_object_object_get_ex(parsed_json, "test_path",    &jobj_test_path);
    json_object_object_get_ex(parsed_json, "app_path",     &jobj_app_path);
    json_object_object_get_ex(parsed_json, "server_apps",  &jobj_server_apps);
    json_object_object_get_ex(parsed_json, "client_apps",  &jobj_client_apps);
    json_object_object_get_ex(parsed_json, "py_ext",       &jobj_py_ext);


    //Save contents to the structure
    object_p->network_mode = json_object_get_string(jobj_network_mode);
    object_p->sim_mode     = json_object_get_boolean(jobj_sim_mode);
    object_p->test_path    = json_object_get_string(jobj_test_path);
    object_p->app_path     = json_object_get_string(jobj_app_path);
    object_p->py_ext       = json_object_get_string(jobj_py_ext);

    //Check network_mode and get appropriate apps to start
    if (strcmp(object_p->network_mode, "server") == 0)
    {

        printf("Network Mode: %s\n", object_p->network_mode);
        object_p->n_of_apps = json_object_array_length(jobj_server_apps);
        for (int index = 0; index < object_p->n_of_apps; index++)
        {
            app = json_object_array_get_idx(jobj_server_apps, index);
            strcpy(object_p->apps[index], json_object_get_string(app));
        }
    }

    else if (strcmp(object_p->network_mode, "client") == 0)
    {

        printf("Network Mode: %s\n", object_p->network_mode);
        object_p->n_of_apps = json_object_array_length(jobj_client_apps);
        for (int index = 0; index < object_p->n_of_apps; index++)
        {
            app = json_object_array_get_idx(jobj_client_apps, index);
            strcpy(object_p->apps[index], json_object_get_string(app));
        }
    }
    else
    {
        printf("ERROR: Expected network_mode is server or client found %s\n", object_p->network_mode);
        exit(1);
    }
}

static void start_pn(struct network_settings *object_p)
{
    //Input     : network_settings structure
    //Output    : N/A
    //Function  : Start all PhotoNetwork Processes

    char *app;
    size_t path_size;

    for (int i = 0; i < object_p->n_of_apps; i++)
    {
        path_size = sizeof(object_p->app_path) + sizeof(object_p->py_ext) + sizeof(object_p->apps[i]);
        app = malloc(path_size);
        sprintf(app, "python %s%s.%s &", object_p->app_path, object_p->apps[i], object_p->py_ext);
        printf("Starting Application ....... %s\n", app);
        system(app);
    }

}

static void stop_pn()
{
    //Input     : N/A
    //Output    : N/A
    //Function  : Stop all PhotoNetwork Processes

    int  total_length = 100;
    char line[total_length];
    char *pid_str_ptr;
    char pid[100];

    strcpy(line, "");
    FILE *command = popen("pidof python", "r");
    fgets(line, total_length, command);
    pclose(command);

    if (strlen(line) > 1)
    {
        printf("The following pids will be killed: %s\n", line);
        pid_str_ptr = strtok(line, " ");

        while (pid_str_ptr != NULL)
        {
            sprintf(pid, "kill -9 %s", pid_str_ptr);
            printf("Killing PID .... %s\n", pid);
            system(pid);
            pid_str_ptr = strtok(NULL, " ");
        }
    }
    else
    {
        printf("ERROR: PhotoNetwork not running\n");
    }

}
