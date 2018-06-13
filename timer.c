#ifndef __DEFINES__
    #include "defines.h"
#endif

#ifndef __SET_DAYS__
    #include __SET_DAYS__
#endif

#define _timeR__

// TFx hardware overflow tag
// TRx enables os disables the timer
// SYSCLOCK = 25000000
// TC0

// ----------------------------- function headers --------------------------------------
//Initializes the TC3 in auto reload mode and sets TMR3H and TMR3L in a way
//that the overflow will occur at each 20ms with SYSCLK/12
void initTimer3(void);

void isrTimer3(void) __interrupt 13;

unsigned char incrementTime(unsigned char *value, unsigned char limit);
unsigned int incrementDate(unsigned int *value, unsigned int limit, unsigned int rst);

//Updates the time
void updateTime();

//----------------------------------------------------------------------------------------
void initTimer3(void)
{
    //Initializes the TC3 in auto reload mode and sets TMR3H and TMR3L in a way
    //that the overflow will occur at each 20ms with SYSCLK/12

    // Change the page so it can use TMR3CN register
    SFRPAGE = TMR3_PAGE;

    // Sets TC3 in auto-reload mode
    TMR3CN &= ~0x01;

    // Sets TC3 initial values
    TMR3H = 0x5D;
    TMR3L = 0x3E;

    // Uses the same values above as reload values
    RCAP3H = 0x5D;
    RCAP3L = 0x3E;

    // Enables TC3
    TR3 = 1;

    //Change back to LEGACY_PAGE
    SFRPAGE = LEGACY_PAGE;
}

void isrTimer3(void) __interrupt 13
{
    // Change to TMR3_PAGE
    SFRPAGE = TMR3_PAGE;

    //Sets the overflow tag to 0
    TF4 = 0;

    // Change back to LEGACY_PAGE
    SFRPAGE = LEGACY_PAGE;

    //Calls the update time function to increment the time counter variable
    updateTime();
}

void updateTime()
{
    unsigned char i, increment;

    //Flag to increment time if occurs overflow in some measure
    increment = 0;

    mills += 20;

    if(mills == 1000)
    {
        mills = 0;

        //Increments seconds
        increment = incrementTime(&time[SEC], 60);

        //Increments minutes
        if(increment) increment = incrementTime(&time[MIN], 60);

        //Increments hours
        if(increment) increment = incrementTime(&time[HR], 24);

        //Increment days
        if(increment) increment = incrementDate(&date[DAY], monthDays[date[MON]-1], 0);

        //Increment month
        if(increment) increment = incrementDate(&date[MON], 12, 1);

        //Increment year
        if(increment)
        {
            increment = incrementDate(&date[YEAR], 65535, 0);
            getDays(date[YEAR], monthDays)

        }

    }
}

void setGlobals(unsigned int *date, unsigned char *time, unsigned char *monthDays)
{
    //Copies the date, time and monthDays vector addresses for the date, time
    //and monthDays pointers

    date = date;
    time = time;
    monthDays = monthDays;
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
