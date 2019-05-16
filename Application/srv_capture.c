/* srv_capture.c: This application is responsibe for sending the number of captures to be taken to 
                  the server to it can send it the clients.
__author__      = "Nitin Mohan
__copyright__   = "Copy Right 2018. NM Technologies"
*/


/*--------------------------------------------------/
/                   System Imports                  /
/--------------------------------------------------*/
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

/*--------------------------------------------------/
/                   Constants                       /
/--------------------------------------------------*/
#define DEFAULT_CAPTURES "1"
#define FIFO_WRITE_MESSAGE "python capture.pyc -c"
#define FIFO_PATH "/var/run/user/"
#define FIFO_FILE "multi.fifo"

//----------- Prototype ----------//
void print_usage(int es);
void write_fifo(char *captures);

//------ Start of Program ------//

int main (int argc, char *argv[])
{
    //Initialize Variables
    int opt;

    //Parse input arguments and take appropriate action
    if (argc == 2)
    {
        while ((opt = getopt(argc, argv, "hc:")) != -1)
        {
            switch (opt)
            {
                case 'c':
                    printf("Number of captures taken: %s\n", optarg);
                    write_fifo(optarg);
                    break;
                case 'h':
                    printf("Help Menu\n");
                    print_usage(0);
                    break;
                case '?':
                    printf("Unknown Argument Provided\n");
                    print_usage(1);
                    break;
            }
        }
    }
    else if (argc > 3)
    {
        printf("Too many arguments provided\n");
        print_usage(1);
    }    
    else
    {
        write_fifo(DEFAULT_CAPTURES);
    }
    return 0;
}

void print_usage(int es)
{
    printf("Usage: # of captures -c <captures> || help -h\n");
    exit(es);
}

void write_fifo(char *captures)
{
    //Initialize variables
    FILE *fp;
    char uid[6];
    int path_size = sizeof(FIFO_PATH) + sizeof(uid) + sizeof(FIFO_FILE);
    int fifo_size = sizeof(FIFO_WRITE_MESSAGE) + sizeof(captures);
    char *fifo_path = malloc(path_size);
    char *fifo_message = malloc(fifo_size);
    
    //Get folder path for fifo file
    sprintf(uid, "%d", getuid());
    snprintf(fifo_path, path_size, "%s%s/%s", FIFO_PATH, uid, FIFO_FILE);
    snprintf(fifo_message, fifo_size, "%s %s", FIFO_WRITE_MESSAGE, captures);

    //Write to Fifo File
    mkfifo(fifo_path, 0666);
    fp = fopen(fifo_path, "w");
    fprintf(fp, "%s", fifo_message);
    fclose(fp);

    //Free Allocated memmory
    free(fifo_path);
    free(fifo_message);
}
