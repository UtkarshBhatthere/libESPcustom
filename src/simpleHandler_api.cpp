/**************************************************************************************************************************
 *  @author : Utkarsh Bhatt, Rajdeep Burman                                                                                               *
 *  @name   : simpleHandler_api.cpp                                                                                             *
 *  @brief  : This file contains the Task Handler Definitions.                                                            *
 **************************************************************************************************************************/
#include "includes.hpp"
#include "MPU_essentials.h"

//Flags

// Counter vars.
static const uint8_t mpuCollectLength = 5;

void mpuHandlerOne(void* switchTask)
{
    i2cOneInit();
    //Var's for data extraction.
    mpud::raw_axes_t accelRaw;   // x, y, z axes as int16
    mpud::raw_axes_t gyroRaw;    // x, y, z axes as int16
    mpud::float_axes_t accelG;   // accel axes in (g) gravity format
    mpud::float_axes_t gyroDPS;  // gyro axes in (DPS) º/s format
    //MPU initialisation.
    MPU_t MPU1, MPU3;
    mpuInit(MPU1, i2c0, mpud::MPU_I2CADDRESS_AD0_LOW);
    mpuInit(MPU3, i2c0, mpud::MPU_I2CADDRESS_AD0_HIGH);
    mpuCalibrate(MPU1);
    mpuCalibrate(MPU3);
    while (true) {
        //MPU1 Cycle
        for(int i = 0; i < mpuCollectLength; i++){
            // Read
            MPU1.motion(&accelRaw, &gyroRaw);
            // Convert
            accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_16G);
            gyroDPS = mpud::gyroDegPerSec(gyroRaw, mpud::GYRO_FS_500DPS);
            // Debug
            ESP_LOGI(mpuONE_TAG,"accel: [%+6.2f %+6.2f %+6.2f ] (G) \t", accelG.x, accelG.y, accelG.z);
            ESP_LOGI(mpuONE_TAG,"gyro: [%+7.2f %+7.2f %+7.2f ] (º/s)\n", gyroDPS[0], gyroDPS[1], gyroDPS[2]);
            taskYIELD();
        }
        taskYIELD();
        
        //MPU2 Cycle
        for(int i = 0; i < mpuCollectLength; i++){
            // Read
            MPU3.motion(&accelRaw, &gyroRaw);
            // Convert
            accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_16G);
            gyroDPS = mpud::gyroDegPerSec(gyroRaw, mpud::GYRO_FS_500DPS);
            // Debug
            ESP_LOGI(mpuTHREE_TAG,"accel: [%+6.2f %+6.2f %+6.2f ] (G) \t", accelG.x, accelG.y, accelG.z);
            ESP_LOGI(mpuTHREE_TAG,"gyro: [%+7.2f %+7.2f %+7.2f ] (º/s)\n", gyroDPS[0], gyroDPS[1], gyroDPS[2]);
            taskYIELD();
        }
        taskYIELD();
    }
    vTaskDelete(nullptr);
}

void mpuHandlerTwo(void* switchTask)
{
    i2cTwoInit();
    //Var's for data extraction.
    mpud::raw_axes_t accelRaw;   // x, y, z axes as int16
    mpud::raw_axes_t gyroRaw;    // x, y, z axes as int16
    mpud::float_axes_t accelG;   // accel axes in (g) gravity format
    mpud::float_axes_t gyroDPS;  // gyro axes in (DPS) º/s format
    //MPU initialisation.
    MPU_t MPU4, MPU2;
    mpuInit(MPU4, i2c0, mpud::MPU_I2CADDRESS_AD0_LOW);
    mpuInit(MPU2, i2c0, mpud::MPU_I2CADDRESS_AD0_HIGH);
    mpuCalibrate(MPU4);
    mpuCalibrate(MPU2);
    while (true) {
        //MPU2 Cycle
        for(int i = 0; i < mpuCollectLength; i++){
            // Read
            MPU2.motion(&accelRaw, &gyroRaw);
            // Convert
            accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_16G);
            gyroDPS = mpud::gyroDegPerSec(gyroRaw, mpud::GYRO_FS_500DPS);
            // Debug
            ESP_LOGI(mpuTWO_TAG,"accel: [%+6.2f %+6.2f %+6.2f ] (G) \t", accelG.x, accelG.y, accelG.z);
            ESP_LOGI(mpuTWO_TAG,"gyro: [%+7.2f %+7.2f %+7.2f ] (º/s)\n", gyroDPS[0], gyroDPS[1], gyroDPS[2]);
            taskYIELD();
        }
        taskYIELD();
        //MPU4 Cycle
        for(int i = 0; i < mpuCollectLength; i++){
            // Read
            MPU2.motion(&accelRaw, &gyroRaw);
            // Convert
            accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_16G);
            gyroDPS = mpud::gyroDegPerSec(gyroRaw, mpud::GYRO_FS_500DPS);
            // Debug
            ESP_LOGI(mpuFOUR_TAG,"accel: [%+6.2f %+6.2f %+6.2f ] (G) \t", accelG.x, accelG.y, accelG.z);
            ESP_LOGI(mpuFOUR_TAG,"gyro: [%+7.2f %+7.2f %+7.2f ] (º/s)\n", gyroDPS[0], gyroDPS[1], gyroDPS[2]);
            taskYIELD();
        }
        taskYIELD();
    }
    vTaskDelete(nullptr);
}