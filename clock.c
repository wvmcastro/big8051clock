#ifndef __DEF_PINOS__
	#define __DEF_PINOS__
	#include "def_pinos.h"
#endif

#ifndef __CONFIG__
	#define __CONFIG__
	#include "config.c"
#endif

#ifndef __DELAY__
	#include "delay.c"
#endif

#ifndef __GLCD__
	#include "glcd.c"
#endif


//#include "eeprom.c"


#ifndef __DEFINES__
	#include "defines.h"
#endif

#ifndef __TIMER__
	#include "timer.c"
#endif

//------------------------------- function headers -------------------------------
unsigned char readKey();
void isrTimer2() __interrupt 5;
void setTimeDateMode();
void clockMode();
void setAlarmMode();
//--------------------------------------------------------------------------------


unsigned char readKey()
{
	// returns which key is being pressed
	// some button is being pressed
	if (P0 != 0xFF)
	{
		if (!MODEBUTTON) return MODE;
		if (!SELECTBUTTON) return SELECT;
		if (!INCREMENTBUTTON) return INCREMENT;
	}
	// no button is being pressed
	return 21;
}

void isrTimer2() __interrupt 5
{
	// Polling: checks if buttons are being pressed

	unsigned char key = 21;

	// Sets overflow flag to 0
	TF2 = 0;

	// If no button is being pressed, state is 0
	if(P0 == 0xFF)
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

void setTimeDateMode()
{
	//Sets the parameter of choice
	//Select Button: cycles through parameters
	//Increment Button: increments the current parameter
	//Mode Button: confirms current parameters and exits this mode

	unsigned char param, adjust;
	__bit timeOrDate, stay;
	timeOrDate = TIME;
	param = SEC;

	stay = 1;
	// Stays in set-time-date-mode while user doesn't press Mode to confirm
	while(stay)
	{
		// ---------------------------------- DISPLAYING ON THE SCREEN ----------------------------------
		printf_fast_f("\x01 SET TIME OR DATE");
		blink = blink();
		if(blink == 0)
		{
			printf_fast_f("\x02 %2d:%2d:%2d", time[HR], time[MIN], time[SEC]);
			printf_fast_f("\x03 %2d/%2d/%4d", date[DAY], date[MON], date[YEAR]);
		}
		else
		{
			switch(param)
			{
				case HR:
					printf_fast_f("\x02   :%2d:%2d", time[MIN], time[SEC]);
					printf_fast_f("\x03 %2d/%2d/%4d", date[DAY], date[MON], date[YEAR]);
				case MIN:
					printf_fast_f("\x02 %2d:  :%2d", time[HR], time[SEC]);
					printf_fast_f("\x03 %2d/%2d/%4d", date[DAY], date[MON], date[YEAR]);
				case SEC:
					printf_fast_f("\x02 %2d:%2d:  ", time[HR], time[MIN]);
					printf_fast_f("\x03 %2d/%2d/%4d", date[DAY], date[MON], date[YEAR]);
				case DAY:
					printf_fast_f("\x02 %2d:%2d:%2d", time[HR], time[MIN], time[SEC]);
					printf_fast_f("\x03   /%2d/%4d", date[MON], date[YEAR]);
				case MON:
					printf_fast_f("\x02 %2d:%2d:%2d", time[HR], time[MIN], time[SEC]);
					printf_fast_f("\x03 %2d/  /%4d", date[DAY], date[YEAR]);
				case YEAR:
					printf_fast_f("\x02 %2d:%2d:%2d", time[HR], time[MIN], time[SEC]);
					printf_fast_f("\x03 %2d/%2d/    ", date[DAY], date[MON]);
				default:
					printf_fast_f("\x02 %2d:%2d:%2d", time[HR], time[MIN], time[SEC]);
					printf_fast_f("\x03 %2d/%2d/%4d", date[DAY], date[MON], date[YEAR]);
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
		if(incrementPress)
		{
			if(timeOrDate == TIME)
			{
				if(param == SEC) adjust = incrementTime(&time[SEC], 60);
				if(param == MIN) adjust = incrementTime(&time[MIN], 60);
				if(param == HR) adjust = incrementTime(&time[HR], 24);
			}
			if(timeOrDate == DATE)
			{
					if(param == DAY) adjust  = incrementDate(&date[DAY], monthDays[date[MON]-1], 0);
					if(param == MON) adjust = incrementDate(&date[MON], 12, 1);
					if(param == YEAR)
					{
						adjust = incrementDate(&date[YEAR], 65535, 0);
						getDays(date[YEAR], monthDays);
					}

			}
			incrementPress = 0;
		}
		if(modePress)
		{
			stay = 0;
			modePress = 0;
		}
	}
	// stay = 0 => go to next mode
	clockMode();
}

void clockMode()
{
	// Displays current time and date; "default" mode
	__bit blink, stay = 1;
	unsigned char i;

	printf_fast_f("\x01 BIG8051 CLOCK");
	while(stay)
	{
		printf_fast_f("\x02 %2d:%2d:%2d", time[HR], time[MIN], time[SEC]);
		printf_fast_f("\x03 %2d/%2d/%4d", date[DAY], date[MON], date[YEAR]);
		printf_fast_f("\x04  ");

		// Checks if current time and date matches alarm time and date
		if(alarmOn && time[SEC] == 0 && alarm[AL_MIN] == time[MIN]
		&& alarm[AL_HR] == time[HR]  && alarm[AL_DAY] == date[DAY]
		&& alarm[AL_MON] == date[MON] && alarm[AL_YEAR] == date[YEAR])
		{
			// Buzzes for 3 seconds
			i = 0;
			while(i <= 35)
			{
				blink = blink();
				if (blink && i%10 = 0)
				{
					P2_7 = 1;
				}
				if (i%10 != 0 && i%5 = 0)
				{
					P2_7 = 0;
				}
				if(blink) i++;
			}
		}

		if(modePress)
		{
				stay = 0;
				modePress = 0;
		}
	}
	// stay = 0 => go to next mode
	setAlarmMode();
}

void setAlarmMode()
{
	// Sets an alarm and saves it to eeprom memory
	unsigned int end_mem;
	unsigned char param, adjust;
	__bit cycling, stay = 0;

	// At first, takes the currently saved alarm to display it
	readAlarm(alarm);

	param = AL_MIN;
	while(stay)
	{
		// ---------------------------------- DISPLAYING ON THE SCREEN ----------------------------------
		if(alarmOn) printf_fast_f("\x01 SET ALARM | on");
		else printf_fast_f("\x01 SET ALARM | off")

		blink = blink();
		if(blink == 0 || cycling = 0)
		{
			printf_fast_f("\x02 %2d:%2d", alarm[AL_HR], alarm[AL_MIN]);
			printf_fast_f("\x03 %2d/%2d/%4d", alarm[AL_DAY], alarm[AL_MON], alarm[AL_YEAR]);
		}
		else if(cycling)
		{
			switch(param)
			{
				case AL_HR:
					printf_fast_f("\x02   :%2d", alarm[AL_MIN]);
					printf_fast_f("\x03 %2d/%2d/%4d", alarm[AL_DAY], alarm[AL_MON], alarm[AL_YEAR]);
				case AL_MIN:
					printf_fast_f("\x02 %2d:  ", alarm[AL_HR]);
					printf_fast_f("\x03 %2d/%2d/%4d", alarm[AL_DAY], alarm[AL_MON], alarm[AL_YEAR]);
				case AL_DAY:
					printf_fast_f("\x02 %2d:%2d", alarm[AL_HR], alarm[AL_MIN]);
					printf_fast_f("\x03   /%2d/%4d", alarm[AL_MON], alarm[AL_YEAR]);
				case AL_MON:
					printf_fast_f("\x02 %2d:%2d", alarm[AL_HR], alarm[AL_MIN]);
					printf_fast_f("\x03 %2d/  /%4d", alarm[AL_DAY], alarm[AL_YEAR]);
				case AL_YEAR:
					printf_fast_f("\x02 %2d:%2d", alarm[AL_HR], alarm[AL_MIN]);
					printf_fast_f("\x03 %2d/%2d/    ", alarm[AL_DAY], alarm[AL_MON]);
				default:
					printf_fast_f("\x02 %2d:%2d", alarm[AL_HR], alarm[AL_MIN]);
					printf_fast_f("\x03 %2d/%2d/%4d", alarm[AL_DAY], alarm[AL_MON], alarm[AL_YEAR]);
			}

		}
		// -------------------------------------------------------------------------------------------------

		// If select button is pressed, cycle through parameters
		if(selectPress)
		{
			if(cycling)
			{
				param++;
				if(param == AL_YEAR) cycling = 0;
			}
			else cycling = 1;

			selectPress = 0;
		}

		// If increment button is pressed, increment current paremeter, or sets alarm on/off
		if(incrementPress)
		{
			if(cycling)
			{
				if(param == AL_MIN) adjust = incrementTime(&alarm[AL_MIN], 60);
				if(param == AL_HR) adjust = incrementTime(&alarm[AL_HR], 24);
				if(param == AL_DAY) adjust  = incrementDate(&alarm[AL_DAY], monthDays[date[MON]-1], 0);
				if(param == AL_MON) adjust = incrementDate(&alarm[AL_MON], 12, 1);
				if(param == AL_YEAR) adjust = incrementDate(&alarm[AL_YEAR], 65535, 0);
			}
			else
			{
				alarmOn = !alarmOn;
			}

			incrementPress = 0;
		}

		// If mode button is pressed, leave this mode and save alarm (if ON)
		if(modePress)
		{
			if(alarmOn)
			{
				saveAlarm(alarm);
			}
			stay = 0;
			modePress = 0;
		}
	}
	setTimeDateMode();
}

void blink()
{
	// Returns 1 to blink, 0 not to blink; blinks every 100 ms
	static unsigned int oldMills;
	unsigned int aux;

	if(mills - oldMills > 0) aux = mills;
	else aux = mills + 1000;

	if(aux - oldMills > 100)
		{
			oldMills = mills;
			return 1;
		}
	return 0;
}

int main(void)
{
	Init_Device();
	SFRPAGE = LEGACY_PAGE;

	Ini_glcd();
	limpa_glcd(ESQ);
	limpa_glcd(DIR);

	initTimer3();

	while(1)
	{
		clockMode();
	}
	return 0;
}
