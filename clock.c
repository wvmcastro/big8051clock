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
		//blink(param);
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
					if(param == DAY) adjust  = incrementDate(&date[DAY], monthDays[date[MON]-1], 0);
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
	__bit stay = 1;
	printf_fast_f("\x01 BIG8051 CLOCK");
	while(stay)
	{
		printf_fast_f("\x02 %2d:%2d:%2d", time[HR], time[MIN], time[SEC]);
		printf_fast_f("\x03 %2d/%2d/%4d", date[DAY], date[MON], date[YEAR]);
		printf_fast_f("\x04  ");


		/*if(alarmSignal)
		{
			printf_fast_f("\x04 ALARM");
		}*/
	
		if(modePress) stay = 0;
	}
	// stay = 0 => go to next mode
	setAlarmMode();
}

void setAlarmMode()
{
	// Sets an alarm and saves it to eeprom memory
	unsigned int end_mem;
	int ret;
	unsigned char param, adjust;
	__bit timeOrDate, stay = 0;

	// At first, takes the currently saved alarm and displays it
	// OBS: last iteration copies only LOW part of the YEAR
	for(end_mem = 0; end_mem < 6; end_mem++)
	{
		alarm[param] = le_eeprom(END_EEPROM, end_mem);
		param++;
	}

	// Copying the HIGH part of the YEAR
	alarm[param] = le_eeprom(END_EEPROM, end_mem)/256;

	param = SEC;
	while(stay)
	{
		// ---------------------------------- DISPLAYING ON THE SCREEN ----------------------------------
		printf_fast_f("\x01 SET ALARM");
		blink = blink();
		if(blink == 0)
		{
			printf_fast_f("\x02 %2d:%2d:%2d", alarm[HR], alarm[MIN], alarm[SEC]);
			printf_fast_f("\x03 %2d/%2d/%4d", alarm[AL_DAY], alarm[AL_MON], alarm[AL_YEAR]);
		}
		else
		{
			switch(param)
			{
				case SEC:
					printf_fast_f("\x02   :%2d:%2d", alarm[HR], alarm[MIN], alarm[SEC]);
					printf_fast_f("\x03 %2d/%2d/%4d", alarm[AL_DAY], alarm[AL_MON], alarm[AL_YEAR]);
				case MIN:
					printf_fast_f("\x02 %2d:  :%2d", alarm[HR], alarm[MIN], alarm[SEC]);
					printf_fast_f("\x03 %2d/%2d/%4d", alarm[AL_DAY], alarm[AL_MON], alarm[AL_YEAR]);
				case HR:
					printf_fast_f("\x02 %2d:%2d:  ", alarm[HR], alarm[MIN], alarm[SEC]);
					printf_fast_f("\x03 %2d/%2d/%4d", alarm[AL_DAY], alarm[AL_MON], alarm[AL_YEAR]);
				case AL_DAY:
					printf_fast_f("\x02 %2d:%2d:%2d", alarm[HR], alarm[MIN], alarm[SEC]);
					printf_fast_f("\x03   /%2d/%4d", alarm[AL_DAY], alarm[AL_MON], alarm[AL_YEAR]);
				case AL_MON:
					printf_fast_f("\x02 %2d:%2d:%2d", alarm[HR], alarm[MIN], alarm[SEC]);
					printf_fast_f("\x03 %2d/  /%4d", alarm[AL_DAY], alarm[AL_MON], alarm[AL_YEAR]);
				case AL_YEAR:
					printf_fast_f("\x02 %2d:%2d:%2d", alarm[HR], alarm[MIN], alarm[SEC]);
					printf_fast_f("\x03 %2d/%2d/    ", alarm[AL_DAY], alarm[AL_MON], alarm[AL_YEAR]);
				default:
					printf_fast_f("\x02 %2d:%2d:%2d", alarm[HR], alarm[MIN], alarm[SEC]);
					printf_fast_f("\x03 %2d/%2d/%4d", alarm[AL_DAY], alarm[AL_MON], alarm[AL_YEAR]);
			}

		}
		
		// -------------------------------------------------------------------------------------------------
		if(selectPress)
		{
			if (param > AL_YEAR) param = SEC;
			else param++;
		}

		if(incrementPress)
		{
			if(param == SEC) adjust = incrementTime(&alarm[SEC], 60);
			if(param == MIN) adjust = incrementTime(&alarm[MIN], 60);
			if(param == HR) adjust = incrementTime(&alarm[HR], 24);
			if(param == AL_DAY) adjust  = incrementDate(&alarm[AL_DAY], monthDays[date[MON]-1], 0);
			if(param == AL_MON) adjust = incrementDate(&alarm[AL_MON], 12, 1);
			if(param == AL_YEAR) adjust = incrementDate(&alarm[AL_YEAR], 65535, 0);
		}

		if(modePress) stay = 0;
	}
	setTimeDateMode();

	// definir padrão de representação de hora-data na eeprom
	// definir padrão de uso das posições da eeprom
	// usar esc_eeprom, colocar verificação em clockMode caso horário bata com o do alarme
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
			oldMills = aux;
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
