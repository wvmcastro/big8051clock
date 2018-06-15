#define __DEFINES__

// ------------------ Eeprom defines ----------------------
#define SDA P9_0
#define SCL P0_1

#define WRITE       0
#define READ        1
#define END_EEPROM  0xA0
// --------------------------------------------------------

// ------------------------- Time-Date Managing defines -----------------------------
unsigned int mills = 0;

#define YEAR 2
#define MON 1
#define DAY 0

#define HR 2
#define MIN 1
#define SEC 0

#define TIME 0
#define DATE 1

volatile unsigned int date[3] = {1, 1, 2000};
volatile unsigned char time[3];
volatile unsigned int alarm[6];
volatile unsigned char monthDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


#define AL_YEAR 5
#define AL_MON 4
#define AL_DAY 3

// -----------------------------------------------------------------------------

// ------------------ Button defines ------------------

#define MODEBUTTON P0_0
#define SELECTBUTTON P0_1
#define INCREMENTBUTTON P0_2

#define MODE 0
#define SELECT 1
#define INCREMENT 2

volatile __bit modePress = 0;
volatile __bit selectPress = 0;
volatile __bit incrementPress = 0;
__bit state = 0;

// ----------------------------------------------------



