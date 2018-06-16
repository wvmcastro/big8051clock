#define __GLCD__

#include <stdio.h>

#ifndef __DEF_PINOS__
	#define __DEF_PINOS__
	#include "def_pinos.h"
#endif

#ifndef __CONFIG__
	#define __CONFIG__
	#include "config.c"
#endif

#define NOP4() NOP(); NOP(); NOP(); NOP()
#define NOP8() NOP4(); NOP4()
#define NOP12() NOP8(); NOP4()

#define CO 0
#define DA 1
#define ESQ 0
#define DIR 1

#define RST	P2_5
#define E	P2_4
#define RW	P2_3
#define RS	P2_2
#define DB	P4
#define CS1	P2_0
#define CS2	P2_1

unsigned char charCount = 0;
unsigned char currentPage = 0;

//Essa matriz contem dados para montar 96 caracteres (codigos ASCII 32 a 127)  
__xdata const char fonte[96][5] = {
{0x00, 0x00, 0x00, 0x00, 0x00},// (space)
{0x00, 0x00, 0x5F, 0x00, 0x00},// !
{0x00, 0x07, 0x00, 0x07, 0x00},// "
{0x14, 0x7F, 0x14, 0x7F, 0x14},// #
{0x24, 0x2A, 0x7F, 0x2A, 0x12},// $
{0x23, 0x13, 0x08, 0x64, 0x62},// %
{0x36, 0x49, 0x55, 0x22, 0x50},// &
{0x00, 0x05, 0x03, 0x00, 0x00},// '
{0x00, 0x1C, 0x22, 0x41, 0x00},// (
{0x00, 0x41, 0x22, 0x1C, 0x00},// )
{0x08, 0x2A, 0x1C, 0x2A, 0x08},// *
{0x08, 0x08, 0x3E, 0x08, 0x08},// +
{0x00, 0x50, 0x30, 0x00, 0x00},// ,
{0x08, 0x08, 0x08, 0x08, 0x08},// -
{0x00, 0x30, 0x30, 0x00, 0x00},// .
{0x20, 0x10, 0x08, 0x04, 0x02},// /
{0x3E, 0x51, 0x49, 0x45, 0x3E},// 0
{0x00, 0x42, 0x7F, 0x40, 0x00},// 1
{0x42, 0x61, 0x51, 0x49, 0x46},// 2
{0x21, 0x41, 0x45, 0x4B, 0x31},// 3
{0x18, 0x14, 0x12, 0x7F, 0x10},// 4
{0x27, 0x45, 0x45, 0x45, 0x39},// 5
{0x3C, 0x4A, 0x49, 0x49, 0x30},// 6
{0x01, 0x71, 0x09, 0x05, 0x03},// 7
{0x36, 0x49, 0x49, 0x49, 0x36},// 8
{0x06, 0x49, 0x49, 0x29, 0x1E},// 9
{0x00, 0x36, 0x36, 0x00, 0x00},// :
{0x00, 0x56, 0x36, 0x00, 0x00},// ;
{0x00, 0x08, 0x14, 0x22, 0x41},// <
{0x14, 0x14, 0x14, 0x14, 0x14},// =
{0x41, 0x22, 0x14, 0x08, 0x00},// >
{0x02, 0x01, 0x51, 0x09, 0x06},// ?
{0x32, 0x49, 0x79, 0x41, 0x3E},// @
{0x7E, 0x11, 0x11, 0x11, 0x7E},// A
{0x7F, 0x49, 0x49, 0x49, 0x36},// B
{0x3E, 0x41, 0x41, 0x41, 0x22},// C
{0x7F, 0x41, 0x41, 0x22, 0x1C},// D
{0x7F, 0x49, 0x49, 0x49, 0x41},// E
{0x7F, 0x09, 0x09, 0x01, 0x01},// F
{0x3E, 0x41, 0x41, 0x51, 0x32},// G
{0x7F, 0x08, 0x08, 0x08, 0x7F},// H
{0x00, 0x41, 0x7F, 0x41, 0x00},// I
{0x20, 0x40, 0x41, 0x3F, 0x01},// J
{0x7F, 0x08, 0x14, 0x22, 0x41},// K
{0x7F, 0x40, 0x40, 0x40, 0x40},// L
{0x7F, 0x02, 0x04, 0x02, 0x7F},// M
{0x7F, 0x04, 0x08, 0x10, 0x7F},// N
{0x3E, 0x41, 0x41, 0x41, 0x3E},// O
{0x7F, 0x09, 0x09, 0x09, 0x06},// P
{0x3E, 0x41, 0x51, 0x21, 0x5E},// Q
{0x7F, 0x09, 0x19, 0x29, 0x46},// R
{0x46, 0x49, 0x49, 0x49, 0x31},// S
{0x01, 0x01, 0x7F, 0x01, 0x01},// T
{0x3F, 0x40, 0x40, 0x40, 0x3F},// U
{0x1F, 0x20, 0x40, 0x20, 0x1F},// V
{0x7F, 0x20, 0x18, 0x20, 0x7F},// W
{0x63, 0x14, 0x08, 0x14, 0x63},// X
{0x03, 0x04, 0x78, 0x04, 0x03},// Y
{0x61, 0x51, 0x49, 0x45, 0x43},// Z
{0x00, 0x00, 0x7F, 0x41, 0x41},// [
{0x02, 0x04, 0x08, 0x10, 0x20},// "\"
{0x41, 0x41, 0x7F, 0x00, 0x00},// ]
{0x04, 0x02, 0x01, 0x02, 0x04},// ^
{0x40, 0x40, 0x40, 0x40, 0x40},// _
{0x00, 0x01, 0x02, 0x04, 0x00},// `
{0x20, 0x54, 0x54, 0x54, 0x78},// a
{0x7F, 0x48, 0x44, 0x44, 0x38},// b
{0x38, 0x44, 0x44, 0x44, 0x20},// c
{0x38, 0x44, 0x44, 0x48, 0x7F},// d
{0x38, 0x54, 0x54, 0x54, 0x18},// e
{0x08, 0x7E, 0x09, 0x01, 0x02},// f
{0x08, 0x14, 0x54, 0x54, 0x3C},// g
{0x7F, 0x08, 0x04, 0x04, 0x78},// h
{0x00, 0x44, 0x7D, 0x40, 0x00},// i
{0x20, 0x40, 0x44, 0x3D, 0x00},// j
{0x00, 0x7F, 0x10, 0x28, 0x44},// k
{0x00, 0x41, 0x7F, 0x40, 0x00},// l
{0x7C, 0x04, 0x18, 0x04, 0x78},// m
{0x7C, 0x08, 0x04, 0x04, 0x78},// n
{0x38, 0x44, 0x44, 0x44, 0x38},// o
{0x7C, 0x14, 0x14, 0x14, 0x08},// p
{0x08, 0x14, 0x14, 0x18, 0x7C},// q
{0x7C, 0x08, 0x04, 0x04, 0x08},// r
{0x48, 0x54, 0x54, 0x54, 0x20},// s
{0x04, 0x3F, 0x44, 0x40, 0x20},// t
{0x3C, 0x40, 0x40, 0x20, 0x7C},// u
{0x1C, 0x20, 0x40, 0x20, 0x1C},// v
{0x3C, 0x40, 0x30, 0x40, 0x3C},// w
{0x44, 0x28, 0x10, 0x28, 0x44},// x
{0x0C, 0x50, 0x50, 0x50, 0x3C},// y
{0x44, 0x64, 0x54, 0x4C, 0x44},// z
{0x00, 0x08, 0x36, 0x41, 0x00},// {
{0x00, 0x00, 0x7F, 0x00, 0x00},// |
{0x00, 0x41, 0x36, 0x08, 0x00},// }
{0x08, 0x08, 0x2A, 0x1C, 0x08},// ->
{0x08, 0x1C, 0x2A, 0x08, 0x08} // <-
};

// ex02
unsigned char le_glcd(__bit cd, __bit cs)
{
	unsigned char byte;

	RW = 1;
	CS1 = cs;
	CS2 = !cs;
	RS = cd;
	NOP4(); // aprox 140ns
	E = 1;
	NOP8(); // aprox 320ns
	SFRPAGE = CONFIG_PAGE;
	byte = DB;
	SFRPAGE = LEGACY_PAGE;
	NOP4(); // aprox 160ns (160 + 320 = 480 ~= 450)
	E = 0;
	RW = 0;
	RS = 0;
	// final
	NOP12(); // aprox 480ns (TWL)
	
	return byte;
}

// ex03
void esc_glcd(unsigned char byte, __bit cd, __bit cs)
{
	while(le_glcd(CO, cs) & 0x80); // checa status (ocupado / disponivel)

	RW = 0;
	CS1 = cs;
	CS2 = !cs;
	RS = cd;
	SFRPAGE = CONFIG_PAGE;
	DB = byte;
	NOP4(); // 140ns Tasu
	SFRPAGE = LEGACY_PAGE;
	E = 1;
	NOP12(); // 450ns Twh
	E = 0;
	SFRPAGE = CONFIG_PAGE;
	DB = 0xff;
	SFRPAGE = LEGACY_PAGE;
	//RW = 0;
	//RS = !RS;
	//CS1 = !CS1;
	//CS2 = !CS2;
	NOP12();
}

// ex04
void Ini_glcd()
{
	// condi��es iniciais
	E = 0;
	CS1 = 1;
	CS2 = 1;
	SFRPAGE = CONFIG_PAGE;
	DB = 0xff;
	SFRPAGE = LEGACY_PAGE;

	// aguardando controladores do LCD terminarem reset por power up
	RST = 1;
	while(le_glcd(CO, ESQ) & 0x10);
	while(le_glcd(CO, DIR) & 0x10);

	// ligando display
	// RS = 0; fun�oes de leitura e escrita ja fazem
	// RW = 0;
	esc_glcd(0x3f, CO, ESQ);
	esc_glcd(0x3f, CO, DIR);

	// zerando indicadores de posi��o X, Y, Z
	esc_glcd(0x40, CO, ESQ); // Y
	esc_glcd(0x40, CO, DIR);
	esc_glcd(0xb8, CO, ESQ); // X
	esc_glcd(0xb8, CO, DIR);
	esc_glcd(0xc0, CO, ESQ); // Z
	esc_glcd(0xc0, CO, DIR);
}

// ex05
void conf_y(unsigned char y, __bit cs)
{
	unsigned char byte;
	byte = y;
	byte |= 0x40;
	byte &= 0x7f;
	esc_glcd(byte, CO, cs);
}

void conf_pag(unsigned char pag, __bit cs)
{
	unsigned char byte;
	byte = pag;
	byte |= 0xb8;
	byte &= ~0x40;
	esc_glcd(byte, CO, cs);
}

// ex06
void limpa_glcd(__bit cs)
{
	unsigned char i, j;

	for(i = 0; i < 8; i++)
	{	
		conf_pag(i, cs);
		conf_y(0, cs);
		for(j = 0; j < 64; j++)
		{
			esc_glcd(0x00, DA, cs);
		}
	}
	charCount = 0;
	conf_pag(0, cs);
	conf_y(0, cs);
}

void clearGlcd()
{
	limpa_glcd(ESQ);
	limpa_glcd(DIR);
}

void putchar(char c)
{
	unsigned char cs, i;
	
	if(c < 9)
	{
		conf_pag(c-1, DIR);
		conf_y(0x00, DIR);
		conf_pag(c-1, ESQ);
		conf_y(0x00, ESQ);
		charCount = 0;
	}
	else
	{
		if(charCount < 8)
			cs = ESQ;
		else
			cs = DIR;

		if(charCount == 16) // ?
		{
			currentPage += 1;
			charCount = 1;
			conf_pag(currentPage, DIR);
			conf_y(0x00, DIR);
			conf_pag(currentPage, ESQ);
			conf_y(0x00, ESQ);
			cs = ESQ;
		}
		else
		{
			charCount++;
		}
	
		for(i = 0; i < 5; i++)
		{
			esc_glcd(fonte[c-32][i], DA, cs);
		}
		esc_glcd(0, DA, cs);
		esc_glcd(0, DA, cs);
		esc_glcd(0, DA, cs);
	}

}
