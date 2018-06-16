#define __SET_DAYS__

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
