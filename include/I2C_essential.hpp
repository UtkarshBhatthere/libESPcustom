/**************************************************************************************************************************
 *  @author : Utkarsh Bhatt                                                                                               *
 *  @date   : 12.02.2019                                                                                                  *
 *  @name   : I2C_essentials.h                                                                                            *
 *  @brief  : This file deals with essential level definitions for the I2C system MPU's.                                  *
 **************************************************************************************************************************/
#pragma once
#include "driver/i2c.h"
#include "I2Cbus.hpp"

/*     Hardware level definitions     */

// General definitions
constexpr uint32_t   CLOCK_SPEED     = 400000;

// I2C TWO
constexpr gpio_num_t i2cTwo_SCL      = GPIO_NUM_26;
constexpr gpio_num_t i2cTwo_SDA      = GPIO_NUM_25;


// I2C ONE
constexpr gpio_num_t i2cOne_SCL      = GPIO_NUM_19;
constexpr gpio_num_t i2cOne_SDA      = GPIO_NUM_18;

// API's
void i2cOneInit();
void i2cTwoInit();