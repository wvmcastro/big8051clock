#ifndef __DEFINES__
    #include "defines.h"
#endif


__bit I2CStart(void)
{
    // Sends a start signal to the I2C bus

    // Generates a start in the I2C bus
    STA = 1;

    // Sets SI to 0 because when it goes to 1 it means that the I2C status
    // has changed
    SI = 0;

    // Waits until the status change occurs
    while(SI == 0);

    return SI;
}

__bit I2Cstop(void)
{
    // Sends a stop signal to the I2C bus
    STO = 1;

    // Sets SI to 0 so this way de bus is unlocked
    SI = 0;

    // Waits until the status change occurs
    while(STO == 1);

    return SI;
}

unsigned char I2CReadByte(void)
{
    
}
