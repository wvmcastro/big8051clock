#define __MISC__

#ifndef __DEFINES__
	#include "defines.h"
#endif

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