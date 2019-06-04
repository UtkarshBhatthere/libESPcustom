/**************************************************************************************************************************
 *  @author : Utkarsh Bhatt, Rajdeep Burman                                                                                                *
 *  @name   : libTestSimple.cpp                                                                                           *
 *  @brief  : Test Ground for continuously developed API's in simple state.                                               *
 **************************************************************************************************************************/
#if 1
#include "includes.hpp"
#include "MPU_essentials.h"
#include "files_essentials.h"

extern "C"{
    void app_main();
}

void app_main() {
    //Setting up handler for MPU one.
    xTaskCreate(storeHandlerOne, "STORE 1 Handler", 16 * 1024, NULL, 5, NULL);
    xTaskCreate(storeHandlerTwo, "STORE 2 Handler", 16 * 1024, NULL, 5, NULL);
    while(1){
        //Forever is not a lie.
    }
}
#endif