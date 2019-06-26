/* pn_controller.h: Header file for pn_controller.c
__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technologies"
*/

/*--------------------------------------------------/
/                   System Imports                  /
/--------------------------------------------------*/
#include <stdbool.h>
#include <stdio.h>
#include <getopt.h>
#include <unistd.h>

/*--------------------------------------------------/
/                   Constants                       /
/--------------------------------------------------*/
#define SETTINGS_PATH "/etc/PhotoNetwork/network_settings.json"

/*---------Structures-------------*/
struct option long_options[] = 
{
    {"start", no_argument, 0, 's'},
    {"stop",  no_argument, 0, 'k'},
    {"help",  no_argument, 0, 'h'}
};

struct network_settings
{
    const char *network_mode;
    const char *test_path;
    const char *app_path;
    const char *py_ext;
    size_t     n_of_apps;
    bool       sim_mode;
    char       apps[][100];
};

//----------- Prototype ----------//
static void print_usage(int es);
static void read_file(char *filepath, char *file_content);
static void parse_json(char *file_content, struct network_settings *object_p);
static void start_pn(struct network_settings *object_p);
static void stop_pn();

//-----end of file--------------//
