#define __SET_DAYS__

void getDays(unsigned int year, unsigned int *yearVector)
{
    /* Sets 28 or 29 days for february in the yearVector according
        to the year */
    unsigned leapYear;
    leapYear = 0;

    if(year % 4 == 0)
    {
        if(year % 100 != 0) leapYear = 1;
        else
        {
            if(year % 400 == 0) leapYear = 1;
        }
    }
    if(leapYear) yearVector[1] = 29;
    else yearVector[1] = 28;
}

int main(void)
{
    unsigned int monthDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    unsigned int i, year;
    while(scanf("%u", &year) != EOF)
    {
        getDays(year, monthDays);
        for(i = 0; i < 12; i++)
            printf("%d ", monthDays[i]);
        printf("\n");
    }
    return 0;
}
