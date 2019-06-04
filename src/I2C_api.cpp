#if 1 
#include "I2C_essential.hpp"

// I2C wrapper
I2C_t& i2cOne = i2c0;
I2C_t& i2cTwo = i2c1;

// API's
void i2cOneInit()
{
    i2cOne.begin(i2cOne_SDA, i2cOne_SCL, CLOCK_SPEED);
}

void i2cTwoInit()
{
    i2cTwo.begin(i2cTwo_SDA, i2cTwo_SCL, CLOCK_SPEED);
}

#endif