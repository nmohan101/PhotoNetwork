/* pn_controller.h: Header file for pn_controller.c
__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technologies"
*/

/*--------------------------------------------------/
/                   System Imports                  /
/--------------------------------------------------*/
#include <stdbool.h>

/*--------------------------------------------------/
/                   Constants                       /
/--------------------------------------------------*/
#define SETTINGS_PATH "/etc/PhotoNetwork/network_settings.json"

/*---------Structures-------------*/
struct network_settings
{
    const char *network_mode;
    bool sim_mode;
    const char *test_path;
    const char *app_path;
    const char apps[100][5];
    const char *py_ext;
};

//----------- Prototype ----------//
static void read_file(char *filepath, char *file_content);
static void parse_json(char *file_content, struct network_settings *object_p);
