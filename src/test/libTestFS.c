#if 0
#include "includes.hpp"
#include "files_essentials.h"

uint8_t* writeBuffer = (uint8_t*)"This is my file test string.\n";
uint8_t* readBuffer = NULL;

void app_main(){
    esp_err_t ret;
    filesSystemInit();
    vTaskDelay(500/portTICK_PERIOD_MS);
    ret = filesWrite("/spiffs/output.txt", writeBuffer);
    printf("The Write Operation resulted %s\n", esp_err_to_name(ret));
    vTaskDelay(500/portTICK_PERIOD_MS);
    readBuffer = filesRead("/spiffs/output.txt");
    char* arr = (char*)readBuffer;
    printf(" %s", arr);
    free(readBuffer);
    while(1){
        taskYIELD();
    }
}


#endif // Main Control