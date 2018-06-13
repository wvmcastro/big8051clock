#ifndef __DEF_PINOS__
	#define __DEF_PINOS__
	#include "def_pinos.h"
#endif

#ifndef __CONFIG__
	#define __CONFIG__
	#include "config.c"
#endif

void delay_ms(unsigned char t)
{
	TMOD |= 0x01;
	TMOD &= ~0x02; 
	while(t>0)
	{
		TR0 = 0;
		TF0 = 0;
		TH0 = 0x9E; 
		TL0 = 0x58;
		TR0 = 1;
		while(TF0 != 1);
		t--;
	}
}

void delay_us(unsigned int t)
{
	TR0=0;
	TF0=0;
	TMOD = TMOD | 0x02;
	TMOD = TMOD & ~0x01;
	TL0=231; //TL0 é o contador do modo 2
	TH0=231; //valor que é recarregado no TL0
	TR0=1;
	//no modo 2 o contador recarrega sozinho o valor setado no TH0
	for(; t>0; t--)
	{
		TF0=0;
		while(TF0 == 0);
	}

}
