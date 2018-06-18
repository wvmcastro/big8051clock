#define __DEFINES__

// ------------------ Eeprom defines ----------------------
#define SDA P0_0
#define SCL P0_1

#define WRITE           0
#define READ            1
#define EEPROM_ADDRESS  0xA0
#define MEM_ADDRESS     0x00
// --------------------------------------------------------

// ------------------------- Time-Date Managing defines -----------------------------
volatile unsigned int mills = 0;

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
volatile unsigned int alarm[5] = {11, 9, 16, 6, 2018 }; // ALARM[] = {MIN, HOUR, DAY, MONTH, YEAR}
volatile unsigned char monthDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

__bit alarmOn = 0;
#define AL_YEAR 4
#define AL_MON 3
#define AL_DAY 2
#define AL_HR 1
#define AL_MIN 0
#define BUZZER P2_7
// -----------------------------------------------------------------------------

// ------------------ Button defines ------------------
#define KEYBOARD_ROW P1
#define MODEBUTTON P1_0
#define SELECTBUTTON P1_1
#define INCREMENTBUTTON P1_2
#define DECREMENTBUTTON P1_3

#define DECREMENT 0
#define INCREMENT 1
#define MODE 2
#define SELECT 3

volatile __bit modePress = 0;
volatile __bit selectPress = 0;
volatile __bit incrementPress = 0;
volatile __bit decrementPress = 0;
__bit state = 0;

// ----------------------------------------------------
