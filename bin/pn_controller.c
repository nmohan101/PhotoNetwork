/* pn_controller.c: The main process responsible for controlling all the
                    PhotoNetwork system processes
__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technologies"
*/


/*--------------------------------------------------/
/                   System Imports                  /
/--------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <json-c/json.h>

/*--------------------------------------------------/
/                   Local Imports                   /
/--------------------------------------------------*/
#include "pn_controller.h"

//------ Start of Program --------//
int main(int argc, char *argv[])
{
    char file_content[1000];
    struct network_settings settings;

    read_file(SETTINGS_PATH, file_content);
    parse_json(file_content, &settings);

}

static void read_file(char *filepath, char *file_content)
{
    //Input     : File Path, file_content pointer
    //Output    : N/A
    //Function  : Read file and populate file_content with contents

    FILE *fp;
    char c; 
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

    //Initialize json objects
    struct json_object *parsed_json;
    struct json_object *jobj_network_mode;
    struct json_object *jobj_sim_mode;
    struct json_object *jobj_test_path;
    struct json_object *jobj_app_path;
    struct json_object *jobj_apps;
    struct json_object *jobj_py_ext;

    //Parse json file_contents
    parsed_json = json_tokener_parse(file_content);

    //Get contents by json key
    json_object_object_get_ex(parsed_json, "network_mode", &jobj_network_mode);
    json_object_object_get_ex(parsed_json, "sim_mode",     &jobj_sim_mode);
    json_object_object_get_ex(parsed_json, "test_path",    &jobj_test_path);
    json_object_object_get_ex(parsed_json, "app_path",     &jobj_app_path);
    json_object_object_get_ex(parsed_json, "apps",         &jobj_apps);
    json_object_object_get_ex(parsed_json, "py_ext",       &jobj_py_ext);

    //Save contents to the structure
    object_p->network_mode = json_object_get_string(jobj_network_mode);
    object_p->sim_mode     = json_object_get_boolean(jobj_sim_mode);
    object_p->test_path    = json_object_get_string(jobj_test_path);
    object_p->app_path     = json_object_get_string(jobj_app_path);
    object_p->py_ext       = json_object_get_string(jobj_py_ext);

}
