#ifndef __DEFINES__
    #define __DEFINES__
#endif

// ------------------- Timer defines ----------------------
unsigned int mills = 0;
unsigned int *_date;
unsigned char *_time;
unsigned char *_monthDays;
// --------------------------------------------------------

// ------------------- Caio defines -----------------------
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
// --------------------------------------------------------
