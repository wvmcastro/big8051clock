#ifndef __DEFINES__
    #include "defines.h"
#endif

#define __TIMER__

// TFx hardware overflow tag
// TRx enables os disables the timer
// SYSCLOCK = 25000000
// TC0

// ----------------------------- function headers --------------------------------------
// Initializes the TC0 in mode 1 and sets TH0 and TL0 in a way
// that the overflow will occur at each 1ms with SYSCLK = 25MHz
void initTimer0(void);

void isrTimer0(void) __interrupt 1;

// Copies the date, time and monthDays vector addresses for the _date, _time
// and _monthDays pointers
void setGlobals(unsigned int *date, unsigned char *time, unsigned char *monthDays);

unsigned char incrementTime(unsigned char *value, unsigned char limit);
unsigned int incrementDate(unsigned int *value, unsigned int limit, unsigned int rst);

//Updates the time
void updateTime();

//----------------------------------------------------------------------------------------
void initTimer0(void)
{
    //Initializes the TC0 in mode 1 and sets TH0 and TL0 in a way
    //that the overflow will occur at each 1ms with SYSCLK = 25MHz

    //Sets the TCO mode as 1
    TMOD |= 0x01;   #M0.0 = 1;
    TMOD &= ~0x02;  #M1.0 = 0;

    //Sets the overflow flag as 0 for ensurance
    TF0 = 0;

    //Loads the initial value
    TH0 = 0x9E;
    TL0 = 0x58;

    //Starts the timer
    TR0 = 1;
}

void isrTimer0(void) __interrupt 1
{
    //Sets the overflow tag to 0
    TF0 = 0;

    //Stops the timer
    TR0 = 1

    //Reloads the timer
    TH0 = 0x9E;
    TL0 = 0x58;

    //Starts the timer again
    TR0 = 1;

    //Calls the update time function to increment the time counter variable
    updateTime();
}

void updateTime()
{
    unsigned char i, increment;

    //Flag to increment time if occurs overflow in some measure
    increment = 0;

    mills += 1;

    if(mills == 1000)
    {
        mills = 0;

        //Increments seconds
        increment = incrementTime(&_time[SEC], 60);

        //Increments minutes
        if(increment) increment = incrementTime(&_time[MIN], 60);

        //Increments hours
        if(increment) increment = incrementTime(&_time[HR], 24);

        //Increment days
        if(increment) increment = incrementDate(&_date[DAY], _monthDays[_date[MON]-1], 0);

        //Increment month
        if(increment) increment = incrementDate(&_date[MON], 12, 1);

        //Increment year
        if(increment) increment = incrementDate(&_date[YEAR], 65535, 0);

    }
}

void setGlobals(unsigned int *date, unsigned char *time, unsigned char *monthDays)
{
    //Copies the date, time and monthDays vector addresses for the _date, _time
    //and _monthDays pointers

    _date = date;
    _time = time;
    _monthDays = monthDays;
}

unsigned char incrementTime(unsigned char *value, unsigned char limit)
{
    //Increments the time

    *value += 1;

    if(*value == limit)
    {
        *value = 0;
        return 1;
    }
    return 0;
}

unsigned char incrementDate(unsigned int *value, unsigned int limit, unsigned int rst)
{
    *value += 1;

    if(*value == limit)
    {
        *value = rst;
        return 1;
    }
    return 0;
}
