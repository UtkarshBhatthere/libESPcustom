/**************************************************************************************************************************
 *  @author : Utkarsh Bhatt                                                                                               *
 *  @date   : 12.02.2019                                                                                                  *
 *  @name   : Includes.hpp                                                                                                *
 *  @brief  : This file deals with the internal includes required by 'main.cpp'. This is done to keep the main code neat. *
 **************************************************************************************************************************/
#pragma once
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_err.h"
#include "esp_spiffs.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "tcpip_adapter.h"

#include "sdkconfig.h"

//handler One File storage name.
#define _ONE_STORE "one_store_file.roghaari"
//handler Two File storage name.
#define _TWO_STORE "two_store_file.roghaari"