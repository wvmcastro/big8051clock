#define __TIMER__

#ifndef __DEFINES__
    #include "defines.h"
#endif

#ifndef __MISC__
    #include "misc.c"
#endif

// ----------------------------- function headers --------------------------------------
//Initializes the TC3 in auto reload mode and sets TMR3H and TMR3L in a way
//that the overflow will occur at each 20ms with SYSCLK/12
void initTimer3(void);

unsigned char alterTime(unsigned char*, unsigned char, __bit);
unsigned char alterDate(unsigned int*, unsigned int, unsigned int, __bit);

//Updates the time
void updateTime() __reentrant;

//----------------------------------------------------------------------------------------
void initTimer3(void)
{
    //Initializes the TC3 in auto reload mode and sets TMR3H and TMR3L in a way
    //that the overflow will occur at each 20ms with SYSCLK/12

    // Change the page so it can use TMR3CN register
    SFRPAGE = TMR3_PAGE;

	TF3 = 0;

    // Sets TC3 in auto-reload mode
    TMR3CN &= ~0x01;

    // Sets TC3 initial values
    TMR3H = 0x5D;
    TMR3L = 0x3D;

    // Uses the same values above as reload values
    RCAP3H = 0x5D;
    RCAP3L = 0x3D;

    // Enables TC3
    TR3 = 1;

	// Enables TC3 interrupt
	EIE2 |= 0x01;

    // Gives TC3 interrupt priority HIGH
    EIP2 |= 0x01;

    //Change back to LEGACY_PAGE
    SFRPAGE = LEGACY_PAGE;
}

void isrTimer3(void) __interrupt 14
{
    // Change to TMR3_PAGE
    SFRPAGE = TMR3_PAGE;

    //Sets the overflow tag to 0
    TF3 = 0;

    // Change back to LEGACY_PAGE
    SFRPAGE = LEGACY_PAGE;

    //Calls the update time function to increment the time counter variable
    updateTime();
}

void updateTime() __reentrant
{
    unsigned char increment;

    mills += 20;

    if(mills == 1000)
    {
		//Flag to increment time if occurs overflow in some measure
	    increment = 0;

        mills = 0;

        //Increments seconds
        increment = alterTime(&time[SEC], 60, INCREMENT);

        //Increments minutes
        if(increment) increment = alterTime(&time[MIN], 60, INCREMENT);

        //Increments hours
        if(increment) increment = alterTime(&time[HR], 24, INCREMENT);

        //Increment days
        if(increment) increment = alterDate(&date[DAY], monthDays[date[MON]-1], 1, INCREMENT);

        //Increment month
        if(increment) increment = alterDate(&date[MON], 12, 1, INCREMENT);

        //Increment year
        if(increment)
        {
            increment = alterDate(&date[YEAR], 65535, 0, INCREMENT);
            getDays(date[YEAR]);
        }
    }
}

unsigned char alterTime(unsigned char *value, unsigned char limit, __bit increment)
{
    // This function is made to alter the seconds, minutes and hours
    // Increments or decrements by one and resets the values when they
    // cross the upper and lower limits

    if(increment) *value += 1;
    else *value -= 1;

    if(*value == limit)
    {
        *value = 0;
        return 1;
    }
    else
    {
        if(*value == 0xFF) *value = limit -1;
    }
    return 0;

}

unsigned char alterDate(unsigned int *value, unsigned int limit, unsigned int rst, __bit increment)
{
    // This function is made to alter the days, months and years
    // Increments or decrements by one and resets the values when they
    // cross the upper and lower limits

    if(increment) *value += 1;
    else *value -= 1;

    if(*value > limit)
    {
        *value = rst;
        return 1;
    }
    else
    {
        // Year
        if(*value == 0xFFFF && rst == 0) *value = limit;
        // Day and months
        if(*value == 0 && rst == 1) *value = limit;
    }
    return 0;
}
