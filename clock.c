#include "def_pinos.h"
#include "config.c"
#include "delay.c"
#include "glcd.c"
#include "eeprom.c"
#include "defines.h"

#ifndef __TIMER__
	#include "timer.c"
#endif

//------------------------------- function headers -------------------------------
unsigned char readKey();
void isrTimer2();
void setTimeDateMode();
void clockMode();
void setAlarmMode()
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

void setTimeDateMode()
{
	//Sets the parameter of choice
	//Select Button: cycles through parameters
	//Increment Button: increments the current parameter
	//Mode Button: confirms current parameters and exits this mode

	unsigned char param, adjust;
	__bit timeOrDate;
	timeOrDate = TIME;
	param = SEC;
	bool stay = 1;

	// Stays in set-time-date-mode while user doesn't press Mode to confirm
	while(stay)
	{
		blink(param);
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
					if(param = DAY) adjust = = incrementDate(&date[DAY], monthDays[date[MON]-1], 0);
					if(param == MON) adjust = incrementDate(&date[MON], 12, 1);
					if(param == YEAR)
					{
						adjust = incrementDate(&date[YEAR], 65535, 0);
						getDays(date[YEAR], monthDays);
					}

			}
		}
		if(modePress)
		{
			stay = 0;
		}
	}
	// stay = 0 => go to next mode
	clockMode();
}

void clockMode()
{
	// Displays current time and date; "default" mode
	bool stay = 1;
	while(stay)
	{
		void updateTime();
		printf_fast_f("BIG8051 CLOCK\n");
		printf_fast_f("%d : %d : %d\n", time[HR], time[MIN], time[SEC]);
		printf_fast_f("%d/%d/%d\n", date[DAY], date[MON], date[YEAR]);
		if(modePress) stay = 0;
	}
	setAlarmMode();
}

void setAlarmMode()
{
	// definir padrão de representação de hora-data na eeprom
	// definir padrão de uso das posições da eeprom
	// usar esc_eeprom, colocar verificação em clockMode caso horário bata com o do alarme
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
