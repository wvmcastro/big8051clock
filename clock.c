#ifndef __DEF_PINOS__
	#define __DEF_PINOS__
	#include "def_pinos.h"
#endif

#ifndef __CONFIG__
	#define __CONFIG__
	#include "config.c"
#endif

#ifndef __DEFINES__
	#include "defines.h"
#endif

#ifndef __GLCD__
	#include "glcd.c"
#endif

#ifndef __EEPROM__
	#include "eeprom.c"
#endif

#ifndef __TIMER__
	#include "timer.c"
#endif

#ifndef __MISC__
	#include "misc.c"
#endif

//------------------------------- function headers -------------------------------
void setTimeDateMode();
void clockMode();
void setAlarmMode();
//--------------------------------------------------------------------------------

__bit exitMode(void)
{
	// Used between mode changes
	modePress = 0;
	clearGlcd();
	return 0;
}

void setTimeDateMode()
{
	//Sets the parameter of choice
	//Select Button: cycles through parameters
	//Increment Button: increments the current parameter
	//Mode Button: confirms current parameters and exits this mode

	unsigned char param, adjust;
	__bit timeOrDate, stay, _blink;
	timeOrDate = TIME;
	param = SEC;

	stay = 1;
	_blink = 0;

	// Stays in set-time-date-mode while user doesn't press Mode to confirm
	while(stay)
	{
		// ---------------------------------- DISPLAYING ON THE SCREEN ----------------------------------
		printf_fast_f("\x01 SET TIME/DATE");

		_blink = shouldBlink(500, _blink);

		if(_blink)
		{
			printf_fast_f("\x03 %2u:%2u:%2u", time[HR], time[MIN], time[SEC]);
			printf_fast_f("\x05 %2u/%2u/%4u", date[DAY], date[MON], date[YEAR]);
		}
		else
		{
			if(timeOrDate == TIME)
			{
				switch(param)
				{
					case HR:
						printf_fast_f("\x03   :%2u:%2u", time[MIN], time[SEC]);
						break;
					case MIN:
						printf_fast_f("\x03 %2u:  :%2u", time[HR], time[SEC]);
						break;
					case SEC:
						printf_fast_f("\x03 %2u:%2u:  ", time[HR], time[MIN]);
						break;
					default:
						printTimeDate();

				}
				printf_fast_f("\x05 %2u/%2u/%4u", date[DAY], date[MON], date[YEAR]);
			}

			if(timeOrDate == DATE)
			{
				printf_fast_f("\x03 %2u:%2u:%2u", time[HR], time[MIN], time[SEC]);

				switch(param)
				{
					case DAY:
						printf_fast_f("\x05   /%2u/%4u", date[MON], date[YEAR]);
						break;
					case MON:
						printf_fast_f("\x05 %2u/  /%4u", date[DAY], date[YEAR]);
						break;
					case YEAR:
						printf_fast_f("\x05 %2u/%2u/    ", date[DAY], date[MON]);
						break;
					default:
						printTimeDate();
				}
			}
		}
		// -------------------------------------------------------------------------------------------------

		if(selectPress)
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

			selectPress = 0;
		}
		if(incrementPress || decrementPress)
		{
			if(timeOrDate == TIME)
			{
				if(param == SEC) adjust = alterTime(&time[SEC], 60, INCREMENT & incrementPress);
				else if(param == MIN) adjust = alterTime(&time[MIN], 60, INCREMENT & incrementPress);
				else if(param == HR) adjust = alterTime(&time[HR], 24, INCREMENT & incrementPress);
			}
			else if(timeOrDate == DATE)
			{
					if(param == DAY) adjust  = alterDate(&date[DAY], monthDays[date[MON]-1], 1, INCREMENT & incrementPress);
					else if(param == MON) adjust = alterDate(&date[MON], 12, 1, INCREMENT & incrementPress);
					else if(param == YEAR)
					{
						adjust = alterDate(&date[YEAR], 65535, 0, INCREMENT & incrementPress);
						getDays(date[YEAR], monthDays);
					}
			}
			incrementPress = 0;
			decrementPress = 0;
		}

		if(modePress)
		{
			stay = exitMode();
		}
	}
	// stay = 0 => go to next mode
	clockMode();
}

void clockMode()
{
	// Displays current time and date; "default" mode
	__bit stay = 1;
	__bit alarmBuzz;

	printf_fast_f("\x01 BIG8051 CLOCK");

	while(stay)
	{
		printf_fast_f("\x03 %2u:%2u:%2u", time[HR], time[MIN], time[SEC]);
		printf_fast_f("\x05 %2u/%2u/%4u", date[DAY], date[MON], date[YEAR]);

		// Checks if current time and date matches alarm time and date
		if(alarmOn && time[SEC] == 0 && alarm[AL_MIN] == time[MIN]
		&& alarm[AL_HR] == time[HR]  && alarm[AL_DAY] == date[DAY]
		&& alarm[AL_MON] == date[MON] && alarm[AL_YEAR] == date[YEAR])
		{
			// Buzzes until user presses decrement
			alarmBuzz = 1;
			while(alarmBuzz)
			{
				if(BUZZER != shouldBlink(500, BUZZER))
				{
					BUZZER = !BUZZER;

					// makes alarm notification blinks
					if(BUZZER) printf_fast_f("\x07 ALARM !!");
					else printf_fast_f("\x07           ");
				}
				
				// continues printing current time and date
				printTimeDate();

				// Alarm off
				if(decrementPress) alarmBuzz = 0;
			}
		}

		if(modePress)
		{
				stay = exitMode();
		}
	}
	// stay = 0 => go to next mode
	setAlarmMode();
}

void setAlarmMode()
{
	// Sets an alarm and saves it to eeprom memory
	unsigned char param, adjust;
	__bit cycling = 0, stay = 1, _blink = 0;

	// At first, takes the currently saved alarm to display it
	readAlarm();

	param = AL_MIN;

	while(stay)
	{
		// ---------------------------------- DISPLAYING ON THE SCREEN ----------------------------------
		if(alarmOn) printf_fast_f("\x01 SET ALARM | on ");
		else
			printf_fast_f("\x01 SET ALARM | off");

		_blink = shouldBlink(500, _blink);
		if(_blink == 0 || cycling == 0)
		{
			printf_fast_f("\x03 %2u:%2u", alarm[AL_HR], alarm[AL_MIN]);
			printf_fast_f("\x05 %2u/%2u/%4u", alarm[AL_DAY], alarm[AL_MON], alarm[AL_YEAR]);
		}
		else if(cycling)
		{
			if(param == AL_HR || param == AL_MIN)
				printf_fast_f("\x05 %2u/%2u/%4u", alarm[AL_DAY], alarm[AL_MON], alarm[AL_YEAR]);
			else
				printf_fast_f("\x03 %2u:%2u", alarm[AL_HR], alarm[AL_MIN]);

			switch(param)
			{
				case AL_HR:
					printf_fast_f("\x03   :%2u", alarm[AL_MIN]);
					break;
				case AL_MIN:
					printf_fast_f("\x03 %2u:  ", alarm[AL_HR]);
					break;
				case AL_DAY:
					printf_fast_f("\x05   /%2u/%4u", alarm[AL_MON], alarm[AL_YEAR]);
					break;
				case AL_MON:
					printf_fast_f("\x05 %2u/  /%4u", alarm[AL_DAY], alarm[AL_YEAR]);
					break;
				case AL_YEAR:
					printf_fast_f("\x05 %2u/%2u/    ", alarm[AL_DAY], alarm[AL_MON]);
					break;
				default:
					printf_fast_f("\x03 %2u:%2u", alarm[AL_HR], alarm[AL_MIN]);
					printf_fast_f("\x05 %2u/%2u/%4u", alarm[AL_DAY], alarm[AL_MON], alarm[AL_YEAR]);
			}
		}
		// -------------------------------------------------------------------------------------------------

		// If select button is pressed, cycle through parameters
		if(selectPress)
		{
			if(cycling)
			{
				if(param == AL_YEAR)
				{
					param = AL_MIN;
					cycling = 0;
				}
				else param++;
			}
			else cycling = 1;

			selectPress = 0;
		}

		// If increment button is pressed, increment current paremeter, or sets alarm on/off
		if(incrementPress || decrementPress)
		{
			if(cycling)
			{
				if(param == AL_MIN) adjust = alterTime(&alarm[AL_MIN], 60, INCREMENT & incrementPress);
				else if(param == AL_HR) adjust = alterTime(&alarm[AL_HR], 24, INCREMENT & incrementPress);
				else if(param == AL_DAY) adjust  = alterDate(&alarm[AL_DAY], monthDays[date[MON]-1], 1, INCREMENT & incrementPress);
				else if(param == AL_MON) adjust = alterDate(&alarm[AL_MON], 12, 1, INCREMENT & incrementPress);
				else if(param == AL_YEAR) adjust = alterDate(&alarm[AL_YEAR], 65535, 0, INCREMENT & incrementPress);
			}
			else
			{
				alarmOn = !alarmOn;
			}
			incrementPress = 0;
			decrementPress = 0;
		}

		// If mode button is pressed, leave this mode and save alarm (if ON)
		if(modePress)
		{
			if(alarmOn) saveAlarm();
			stay = exitMode();
		}
	}
	setTimeDateMode();
}

int main(void)
{
	Init_Device();
	SFRPAGE = LEGACY_PAGE;

	initTimer3();
	Ini_glcd();
	clearGlcd();

	while(1)
	{
		clockMode();
	}
	return 0;
}
