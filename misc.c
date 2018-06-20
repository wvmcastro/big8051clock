#define __MISC__

#ifndef __DEFINES__
	#include "defines.h"
#endif

#ifndef __EEPROM__
	#include "eeprom.c"
#endif

#ifndef __DELAY__
	#include "delay.c"
#endif

void getDays(unsigned int year)
{
    /* Sets 28 or 29 days for february in the yearVector according
        to the year */
    __bit leapYear;
    leapYear = 0;

    if(year % 4 == 0)
    {
        if(year % 100 != 0) leapYear = 1;
        else
        {
            if(year % 400 == 0) leapYear = 1;
        }
    }
    if(leapYear) monthDays[1] = 29;
    else monthDays[1] = 28;
	
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

unsigned char readKey()
{
	// returns which key is being pressed
	// some button is being pressed

	if (KEYBOARD_ROW != 0xFF)
	{

		if (!MODEBUTTON) return MODE;
		if (!SELECTBUTTON) return SELECT;
		if (!INCREMENTBUTTON) return INCREMENT;
		if (!DECREMENTBUTTON) return DECREMENT;
	}
	// no button is being pressed
	return 21;
}

void printTimeDate(void)
{
	printf_fast_f("\x03 %2u:%2u:%2u", time[HR], time[MIN], time[SEC]);
	printf_fast_f("\x05 %2u/%2u/%4u", date[DAY], date[MON], date[YEAR]);
}

void isrTimer2() __interrupt 5
{
	// Polling: checks if buttons are being pressed

	unsigned char key = 21;

	// Sets overflow flag to 0
	TF2 = 0;

	// If no button is being pressed, state is 0
	if(KEYBOARD_ROW == 0xFF)
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
		if (key == DECREMENT) decrementPress = 1;
	}

}

void saveAlarm()
{
    int ret;
    unsigned char i, aux[7];
	
	alarm[AL_STATUS] = alarmOn;

    for(i = 0; i < 6; i++)
    {
        aux[i] = (unsigned char) (alarm[i] % 256);
    }
    aux[6] = (unsigned char) (alarm[5] / 256);

    ret = writeEeprom(EEPROM_ADDRESS, MEM_ADDRESS, aux, 7);
}

void readAlarm()
{
    int ret;
    unsigned char i, aux[7];
    ret = readEeprom(EEPROM_ADDRESS, MEM_ADDRESS, aux, 7);
	
    if(ret >= 0)
    {
        for(i = 0; i < 5; i++)
        {
            alarm[i] = aux[i];
        }
        alarm[5] = (aux[6] << 8) | aux[5];
    }

	alarmOn = alarm[AL_STATUS] & 0x01;

}

void beep()
{
	// Beeps the buzzer at 4KHz
	BUZZER = 1;
	delay_us(25);
	BUZZER = 0;
	delay_us(25);
}
