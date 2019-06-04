/******************************************************************************************************************
 *  @author : Utkarsh Bhatt                                                                                       *
 *  @name   : storeHandler_api.cpp                                                                                *
 *  @brief  : This file contains the Task Handler Definitions with file saving functionality.                     *
 *  @TO-DO  : Implement the storeToFile() function for storing as a file in SPIFFS.
 ******************************************************************************************************************/
#include "includes.hpp"
#include "MPU_essentials.h"
#include "files_essentials.h"

//Struct
typedef struct{
    bool iD;
    uint16_t filledSize;
    mpud::float_axes_t *bufferPtr;
} buffer;

// Size matters.
int blockSize = sizeof(mpud::float_axes_t);

// Buffers.
static buffer frontBuffOne, backBuffOne;  //Buffers for Handler One.
static buffer frontBuffTwo, backBuffTwo;  //Buffers for Handler Two.

// Buffer Pointers.
static buffer *oneBuffer, *oneStore;
static buffer *twoBuffer, *twoStore;

//Static API's.
static void storeToFileHandlerOne(buffer* bfr);
static void storeToFileHandlerTwo(buffer* bfr);

// Counter vars.
static const uint8_t mpuCollectLength = 10;

void storeHandlerOne(void* switchTask)
{
    i2cOneInit();
    //Var's for data extraction.
    mpud::raw_axes_t accelRaw;   // x, y, z axes as int16
    mpud::raw_axes_t gyroRaw;    // x, y, z axes as int16
    mpud::float_axes_t accelG;   // accel axes in (g) gravity format
    mpud::float_axes_t gyroDPS;  // gyro axes in (DPS) º/s format
    //Cleaning up previous files if found.
    remove(_ONE_STORE);
    //Buffer Allocation.
    frontBuffOne.iD = 0;                                                //Init front buffer for handler one.
    frontBuffOne.filledSize = 0;
    frontBuffOne.bufferPtr = (mpud::float_axes_t*)malloc(sizeof(mpud::float_axes_t)*512);
    backBuffOne.iD = 1;                                                //Init back buffer for handler one.
    backBuffOne.filledSize = 0;
    backBuffOne.bufferPtr = (mpud::float_axes_t*)malloc(sizeof(mpud::float_axes_t)*512);
    oneBuffer = &frontBuffOne;                                         //Init handler buffer.
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
            // Store.
            *(oneBuffer -> bufferPtr++) = accelG;
            *(oneBuffer -> bufferPtr++) = gyroDPS;
            oneBuffer -> filledSize += 2;
        }
        taskYIELD();
        
        //MPU2 Cycle
        for(int i = 0; i < mpuCollectLength; i++){
            // Read
            MPU3.motion(&accelRaw, &gyroRaw);
            // Convert
            accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_16G);
            gyroDPS = mpud::gyroDegPerSec(gyroRaw, mpud::GYRO_FS_500DPS);
            // Store
            *(oneBuffer -> bufferPtr++) = accelG;
            *(oneBuffer -> bufferPtr++) = gyroDPS;
            oneBuffer -> filledSize += 2;
            
        }
        // Buffer change if required.
        if(oneBuffer -> filledSize > 500){
            oneStore = oneBuffer;               // Buffer for storing Data.
            if(oneBuffer->iD == 0)
                oneBuffer = &backBuffOne;
            if(oneBuffer->iD == 1)
                oneBuffer = &frontBuffOne;
            oneBuffer->filledSize = 0;
        }
        // Start Writing file to file.
        storeToFileHandlerOne(oneStore);
        taskYIELD();
    }
    vTaskDelete(nullptr);
}

void storeHandlerTwo(void* switchTask)
{
    i2cTwoInit();
    //Var's for data extraction.
    mpud::raw_axes_t accelRaw;   // x, y, z axes as int16
    mpud::raw_axes_t gyroRaw;    // x, y, z axes as int16
    mpud::float_axes_t accelG;   // accel axes in (g) gravity format
    mpud::float_axes_t gyroDPS;  // gyro axes in (DPS) º/s format
    //Cleaning up previous files if found.
    remove(_TWO_STORE);
    //Buffer Allocation.
    frontBuffTwo.iD = 0;                                              //Init front buffer for handler Two.
    frontBuffTwo.filledSize = 0;
    frontBuffTwo.bufferPtr = (mpud::float_axes_t*)malloc(sizeof(mpud::float_axes_t)*512);
    backBuffTwo.iD = 1;                                              //Init back buffer for handler Two.
    backBuffTwo.filledSize = 0;
    backBuffTwo.bufferPtr = (mpud::float_axes_t*)malloc(sizeof(mpud::float_axes_t)*512);
    twoBuffer = &frontBuffTwo;                                       //Init handler buffer.
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
            // Store
            // Store
            *(twoBuffer -> bufferPtr++) = accelG;
            *(twoBuffer -> bufferPtr++) = gyroDPS;
            twoBuffer -> filledSize += 2; 
        }
        taskYIELD();
        //MPU4 Cycle
        for(int i = 0; i < mpuCollectLength; i++){
            // Read
            MPU2.motion(&accelRaw, &gyroRaw);
            // Convert
            accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_16G);
            gyroDPS = mpud::gyroDegPerSec(gyroRaw, mpud::GYRO_FS_500DPS);
            // Store
            *(twoBuffer -> bufferPtr++) = accelG;
            *(twoBuffer -> bufferPtr++) = gyroDPS;
            twoBuffer -> filledSize += 2;
        }
        // Buffer change if required.
        if(twoBuffer -> filledSize > 500){
            twoStore = twoBuffer;
            if(twoBuffer->iD == 0)
                twoBuffer = &backBuffTwo;
            if(twoBuffer->iD == 1)
                twoBuffer = &frontBuffTwo;
            twoBuffer->filledSize = 0;
        }
        // Start Writing file to file.
        storeToFileHandlerTwo(twoStore);
        taskYIELD();
    }
    vTaskDelete(nullptr);
}

static void storeToFileHandlerOne(buffer* bfr)
{
    // implement this.
    FILE* storeFile = fopen(_ONE_STORE, "a");
    int res = fwrite(bfr->bufferPtr, blockSize, bfr->filledSize, storeFile);
    ESP_LOGI("[STORE1]", "RET : %d", res);
    fclose(storeFile);
}

static void storeToFileHandlerTwo(buffer* bfr)
{
    // implement this.
    FILE* storeFile = fopen(_TWO_STORE, "a");
    int res = fwrite(bfr->bufferPtr, blockSize, bfr->filledSize, storeFile);
    ESP_LOGI("[STORE1]", "RET : %d", res);
    fclose(storeFile);
}