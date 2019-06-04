#if 1

#include "MPU_essentials.h"

mpud::raw_axes_t accelBias, gyroBias;

/*     Static API's for unexposed operation     */
// @brief : Use this to verify the connection to the MPU.
static void mpuVerify(MPU_t mpu, const char *mpuTag){
    while(esp_err_t err = mpu.testConnection()){
        ESP_LOGE(mpuTag, "Connection failed, err=%#X", err);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(mpuTag, "Connection Succeeded!");
}

// @brief   : Use this to test the Gyro and Accelerometers of the MPU.
// @warning : It is recommended that the MPU's be as horizontal as possible
//            facing upwards.
static void mpuSelfTest(MPU_t mpu, char *mpuTag){
    mpud::selftest_t retSelfTest;
    while(esp_err_t err = mpu.selfTest(&retSelfTest)){
        ESP_LOGE(mpuTag, "Test Failed, err=%#X", err);
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
    ESP_LOGI(mpuTag, "Self Test Result: Gyro=%s Accel=%s", //
            ( retSelfTest & mpud::SELF_TEST_GYRO_FAIL ? "FAIL" : "OK"),
            ( retSelfTest & mpud::SELF_TEST_ACCEL_FAIL ? "FAIL" : "OK"));
}

/*     API's     */

/*************************************************************************************************************************
 * @brief : API's for initialising the MPU's setup of I2c PERIPH and verification.                                       *
 *************************************************************************************************************************/
void mpuInit(MPU_t &mpuONE, I2C_t &busAddr, mpud::types::mpu_i2caddr_t addr){
    mpuONE.setBus(busAddr);
    mpuONE.setAddr(addr);
    mpuVerify(mpuONE, mpuONE_TAG);
    ESP_ERROR_CHECK(mpuONE.initialize());
    mpuSelfTest(mpuONE, mpuONE_TAG);
}

void mpuCalibrate(MPU_t &mpu){
    
    ESP_ERROR_CHECK(mpu.computeOffsets(&accelBias, &gyroBias));
    ESP_ERROR_CHECK(mpu.setAccelOffset(accelBias));
    ESP_ERROR_CHECK(mpu.setGyroOffset(gyroBias));
    ESP_LOGI("MPU Calibration", "Calibration Done!");
}

void mpuScaleConfig(MPU_t &mpu){
    ESP_ERROR_CHECK(mpu.setAccelFullScale(AccelFS));
    ESP_ERROR_CHECK(mpu.setGyroFullScale(GyroFS));
    ESP_ERROR_CHECK(mpu.setSampleRate(SampleRate));
    ESP_ERROR_CHECK(mpu.setDigitalLowPassFilter(DLPF));
}

void mpuFIFoSetup(MPU_t &mpu){
    ESP_ERROR_CHECK(mpu.setFIFOConfig(mpud::FIFO_CFG_ACCEL | mpud::FIFO_CFG_GYRO));
    ESP_ERROR_CHECK(mpu.setFIFOEnabled(true));
}

#endif