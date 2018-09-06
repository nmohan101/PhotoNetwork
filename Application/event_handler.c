#include <stdio.h>
#include <sqlite3.h>
#include "jsmn.h"

#define JSON_FILE_PATH "/main_server/c/Users/nmohan/Documents/GitHub/Photo_Network/Application/bcmessage.json"
#define BUFFER_SIZE 5000
#define MAX_TOKEN_COUNT 128

// Read files
void readfile(char* filepath, char* fileContent)
{
    FILE *f;
    char c;
    int index;
    f = fopen(filepath, "rt");
    while((c = fgetc(f)) != EOF){
        fileContent[index] = c;
        index++;
    }
    fileContent[index] = '\0';
}

// This is where the magic happens
int parseJSON(char *filepath, void callback(char *, char*)){

    char JSON_STRING[BUFFER_SIZE];

    char value[1024];
    char key[1024];

    readfile(filepath, JSON_STRING);

   int i;
   int r;

   jsmn_parser p;
   jsmntok_t t[MAX_TOKEN_COUNT];

   jsmn_init(&p);

   r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/(sizeof(t[0])));

   if (r < 0) {
       printf("Failed to parse JSON: %d\n", r);
       return 1;
   }

   /* Assume the top-level element is an object */
   if (r < 1 || t[0].type != JSMN_OBJECT) {
       printf("Object expected\n");
       return 1;
   }

   for (i = 1; i < r; i++){

       jsmntok_t json_value = t[i+1];
       jsmntok_t json_key = t[i];


       int string_length = json_value.end - json_value.start;
       int key_length = json_key.end - json_key.start;

       int idx;

       for (idx = 0; idx < string_length; idx++){
           value[idx] = JSON_STRING[json_value.start + idx ];
       }

       for (idx = 0; idx < key_length; idx++){
           key[idx] = JSON_STRING[json_key.start + idx];
       }

       value[string_length] = '\0';
       key[key_length] = '\0';

       callback(key, value);

       i++;
   }

   return 0;
}

// Only prints the key and value
void mycallback(char *key, char* value){
    printf("%s : %s\n", key, value);
}
void data_base(void)
{
   sqlite3 *db;
   int rc;

   rc = sqlite3_open("client_list.db", &db);

   if(rc)
   {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
   }
   else
   {
      fprintf(stderr, "Opened database successfully\n");
   }
   sqlite3_close(db);
}


int main()
{
    //data_base();
    parseJSON(JSON_FILE_PATH, mycallback);
    return 0;


}

