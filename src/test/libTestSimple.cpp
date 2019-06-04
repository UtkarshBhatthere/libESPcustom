/**************************************************************************************************************************
 *  @author : Utkarsh Bhatt, Rajdeep Burman                                                                                                *
 *  @name   : libTestSimple.cpp                                                                                           *
 *  @brief  : Test Ground for continuously developed API's in simple state.                                               *
 **************************************************************************************************************************/
#if 0
#include "includes.hpp"
#include "MPU_essentials.h"

//Taskhandles.
TaskHandle_t *testHandlerOne;
TaskHandle_t *testHandlerTwo;

extern "C"{
    void app_main();
}

void app_main() {
    //Setting up handler for MPU one.
    xTaskCreate(mpuHandlerOne, "MPU 1 Handler", 16 * 1024, (void*)testHandlerTwo, 5, testHandlerOne);
    xTaskCreate(mpuHandlerTwo, "MPU 2 Handler", 16 * 1024, (void*)testHandlerOne, 5, testHandlerTwo);
    while(1){
        //Forever is not a lie.
    }
}
#endif