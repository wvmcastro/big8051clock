#define __MISC__

#ifndef __DEFINES__
	#include "defines.h"
#endif

void getDays(unsigned int year, unsigned int *yearVector)
{
    /* Sets 28 or 29 days for february in the yearVector according
        to the year */
    unsigned leapYear;
    leapYear = 0;

    if(year % 4 == 0)
    {
        if(year % 100 != 0) leapYear = 1;
        else
        {
            if(year % 400 == 0) leapYear = 1;
        }
    }
    if(leapYear) yearVector[1] = 29;
    else yearVector[1] = 28;
}

__bit shouldBlink(unsigned int t, __bit s)
{
	static int oldMills;

	if(oldMills > mills)
	{
		oldMills -= 1000;
	}

	if(mills - oldMills >= t)
	{
		oldMills = mills;
		return !s;
	}

	return s;
}


__bit blink()
{
	// Returns 1 to blink, 0 not to blink; blinks every 200 ms
	static unsigned int oldMills;
	unsigned int aux;

	if(mills - oldMills > 0) aux = mills;
	else aux = mills + 1000;

	if(aux - oldMills > 200)
		{
			oldMills = mills;
			return 1;
		}
	return 0;
}
