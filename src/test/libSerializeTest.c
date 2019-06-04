#if 0
#include "includes.hpp"
#include "files_essentials.h"

typedef struct
{
    int number;
    char text[10];
    double real_number;
} example;

uint8_t* writeBuffer = (uint8_t*)"This is my file test string.\n";
uint8_t* readBuffer = NULL;

void app_main(){
    esp_err_t ret;
    filesSystemInit();
    example data = {1, "Hello!", 3.14159265}, read_data;
    printf("%d %s %lf from Write\n", data.number, data.text, data.real_number);
    FILE* fout = fopen("/spiffs/blah.txt", "w");
    fwrite(&data, sizeof(example), 1, fout);
    fclose(fout);
    FILE* fin = fopen("/spiffs/blah.txt", "r");
    fread(&read_data, sizeof(example), 1, fout);
    printf("%d %s %lf from Read\n", read_data.number, read_data.text, read_data.real_number);
    fclose(fin);

    while(1){
        taskYIELD();
    }
}


#endif // Main Control