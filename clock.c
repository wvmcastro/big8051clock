#include "def_pinos.h"
#include "config.c"
#include "delay.c"
#include "glcd.c"
#include "eeprom.c"

/*
#define YEAR 2
#define MON 1
#define DAY 0

#define HR 2
#define MIN 1
#define SEC 0

#define TIME 0
#define DATE 1

#define MODEBUTTON P0_0
#define SELECTBUTTON P0_1
#define INCREMENTBUTTON P0_2

#define MODE 0
#define SELECT 1
#define INCREMENT 2

__bit modePress;
__bit selectPress;
__bit incrementPress;
__bit state;

unsigned int date[3];
unsigned char time[3];
*/

unsigned char readKey()
{
	// returns which key is being pressed
	if (P0 != 0xFF)
	{
		if (!MODEBUTTON) return MODE;
		if (!SELECTBUTTON) return SELECT;
		if (!INCREMENTBUTTON) return INCREMENT;
	}
	// no button is being pressed
	else
		return 21;
}

void isrTimer2() __interrupt 5
{
	// Polling: checks if buttons are being pressed

	unsigned char key;

	// Sets overflow flag to 0
	TF2 = 0;

	// If no button is being pressed, state is 0
	if(PO == 0xFF)
		state = 0;

	// If state is 0, check if a key is being pressed
	if(state == 0)
	{
		key = readKey();
	}

	// If we did read some key, sets global variables accordingly
	if(key != 21 && state == 0)
	{
		state = 1;
		if (key == MODE) modePress = 1;
		if (key == SELECT) selectPress = 1;
		if (key == INCREMENT) incrementPress = 1;
	}

}

//adaptar para usar incrementTime e incrementDate
void setTimeDateMode()
{
	//Sets the parameter of choice
	//Select Button: cycles through parameters
	//Increment Button: increments the current parameter
	//Mode Button: confirms current parameters and exists this mode

	unsigned char param;
	__bit timeOrDate;

	timeOrDate = TIME;
	param = SEC;

	// Stays in set-time-date-mode while user doesn't press Mode to confirm
	while(1)
	{
		blink(param);
		if (selectPress)
		{
			if(param == HR && timeOrDate == TIME)
			{
				timeOrDate = DATE;
				param = DAY;
			}
			else if(param == YEAR && timeOrDate == DATE)
			{
				timeOrDate = TIME;
				param = SEC;
			}
			else
				param++;
		}
		if (incrementPress)
		{
			incrementParam(timeOrDate, param);
		}
		if (modePress)
		{
			clockMode();
		}

	}
}

/*
void incrementParam(__bit timeOrDate, unsigned char param)
{
	// Increments a given parameter

	if (timeOrDate == TIME)
	{
		if(param == DAY)
		{
			if(date[DAY] == monthDays[date[MONTH]-1])
				date[DAY] = 1;
			else
				date[DAY] += 1;
		}
		if(param == MON)
		{
			if(date[MON] == 12)
				date[MON] = 1;
			else
				date[MON] += 1; 
		}
		if(param == YEAR)
		{
			date[YEAR] += 1;
			getDays(date[YEAR], monthDays);
		}
		
	}
	else 
	{
		if(param == HR)
		{
			if(time[HR] == 23)
				time[HR] = 0;
			else
				time[HR] += 1;
		}
		if(param == MIN)
		{
			if(time[MIN] == 59)
				time[MIN] = 0;
			else
				time[MIN] += 1;
		}
		if(param == SEC)
		{
			if(time[SEC] == 59)
				time[SEC] = 0;
			else
				time[SEC] += 1;
		}

	}
		
}
*/