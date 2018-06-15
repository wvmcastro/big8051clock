#ifndef __DEFINES__
    #include "defines.h"
#endif


void I2CStart(void)
{
    // Sends a start signal to the I2C bus

    // Generates a start in the I2C bus
    STA = 1;

    // Sets SI to 0 because when it goes to 1 it means that the I2C status
    // has changed
    SI = 0;

    // Waits until the status change occurs
    while(SI == 0);

    STA = 0;
    return SMB0STA;
}

void I2Cstop(void)
{
    // Sends a stop signal to the I2C bus
    STO = 1;

    // Sets SI to 0 so this way de bus is unlocked
    SI = 0;

    // Waits until the status change occurs
    while(STO == 1);

    return 0;
}

int I2CReadByte(__bit NACK)
{
    // Configures 8051 to send a NACK after receive the byte
    AA = ~NACK;

    SI = 0;

    // Checks when the status changes
    while(SI == 0);

    // Data byte received NACK transmited
    if(NACK && SMB0STA == 0x58)
        return (unsigned char) SMB0DAT;

    // Data byte received ACK transmited
    if(~NACK && SMB0STA == 0x50)
        return (unsigned char) SMB0DAT;

    // If some error occurs return -1
    return -1;
}

void I2CSendByte(unsigned char byte, __bit afterStart)
{
    // Stores the transmiting byte at SB0DAT
    SM0DAT = byte;

    if(afterStart) STA = 0;

    SI = 0;

    // Waits until the byte is sended
    while(SI == 0);

    return SMB0STA;
}

void char I2CWriteControlByte(unsigned char deviceAdrres, __bit RW)
{
    unsigned char status;

    status = I2CStart();

    // If the start is not successful return the status error
    if(status != 0x08 && status != 0x10)
        return status;

    status = I2CSendByte(deviceAdrres | (unsigned char) RW, 1);

    // Checks if Save Address + W transmitted ACK received, if not returns
    // status error
    if(RW == WRITE && status != 0x18) return status;

    // Checks if Save Address + R transmited and ACK received, if not returns
    // status error
    if(RW == READ && status != 0x40) return status

    // In case everything goes fine
    return 0;
}

int writeEeprom(unsigned char deviceAdrres, unsigned char memAddress, unsigned char *data, unsigned int numBytes)
{
    unsigned char ret, i;

    ret = I2CWriteControlByte(deviceAdrres, WRITE);
    if(ret == 0) ret = I2CSendByte(memAddress, 0);

    // Sends all bytes inside data[]
    for(i = 0; i < numBytes && ret != 0x28; i++)
    {
        ret = I2CSendByte(*(data+i), 0);
    }

    if(ret != 0x28)
    {
        I2CStop();
        return 0;
    }

    return -1;
}

int readEeprom(unsigned char deviceAdrres, unsigned char memAddress, unsigned char *dst, unsigned int numBytes)
{
    unsigned char ret, i, k;
    int data;

    ret = I2CWriteControlByte();
    if(ret == 0x28) ret = I2CSendByte(memAddress, 0);

    k = 1;
    for(i = 0; i < numBytes && k; i++)
    {
        if(i < numBytes-1) // Reads and transmites ACK
            data = I2CReadByte(0);
        else // Reads and transmites NACK
            data = I2CReadByte(1);

        if(data != -1)
            *(dst+i) = (unsigned char) data;
        else
            k = 0;
    }

    if(~k) return -1;

    return k;
}

void saveAlarm()
{
    unsigned int ret;
    unsigned char i, aux[6];

    for(i = 0; i < 5; i++)
    {
        aux[i] = (unsigned char) (alarm[i] % 256);
    }
    aux[5] = (unsigned char) (alarm[4] / 256);

    ret = writeEeprom(EEPROM_ADDRESS, MEM_ADDRESS, aux, 6);

    return ret;
}

void readAlarm()
{
    unsigned int ret;
    unsigned char i, aux[6];

    ret = readEeprom(EEPROM_ADDRESS, MEM_ADDRESS, aux, 6);

    if(ret >= 0)
    {
        for(i = 0; i < 4; i++)
        {
            alarm[i] = aux[i];
        }
        alarm[4] = (aux[4] << 8) | aux[5];
    }
}
