
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*******************************************************************************/
/*
 * Returns day of week for any given date
 * PARAMS: year y > 1752 (in the U.K.)
 *  month 1 <= m <= 12 
 *  day 1 <= d <= 31
 *
 * RETURNS: day of week (0-6 is Sun-Sat)
 * NOTES: Sakamoto's Algorithm
 *   http://en.wikipedia.org/wiki/Calculating_the_day_of_the_week#Sakamoto.27s_algorithm
 *
 */

int t_dow[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };

int dayOfWeek(int d, int m, int y)
{
       y -= m < 3;
       return (y + y/4 - y/100 + y/400 + t_dow[m-1] + d) % 7;
}

/*******************************************************************************/

const char *days[] = {
    // 0    1      2      3      4      5      6      7
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};

const char *DOW(int d,int m,int y)
{
    return days[dayOfWeek(d,m,y)];
}

/*******************************************************************************/

int isLeapYear(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}


int daysInMonth(int month,int year)
{
    if (month == 4 || month == 6 || month == 9 || month == 11)
	return 30;
    if (month == 2) {
	return isLeapYear(year) ? 29 : 28;
    }
    return 31;
}

/*******************************************************************************/

int dayOfYear(int day, int mon, int year)
{
    static const int daysm[2][13] = {
	//  1  2   3   4   5    6    7    8    9    10   11   12
        {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
        {0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335}
    };
    int leap = isLeapYear(year);
    return daysm[leap][mon] + day;
}

/*******************************************************************************/
// The ISO 8601 week number (see NOTES) of the current year as a
//  decimal number, range 01 to 53, where week 1 is the first week
//  that has at least 4 days in the new year.
// or, synonymously, week 01 is: the first week of the year that contains a Thursday;
// or, the week that has 4 January in it

int weekNumber(int d,int m,int y)
{
int dow1Jan = dayOfWeek(1,1,y);
int firstMonday = 1;
    if(dow1Jan == 5 || dow1Jan == 6 || dow1Jan == 0) {
	for(;;) {
	    int dow = dayOfWeek(firstMonday,1,y);
	    if(dow == 1) break;
	    firstMonday++;
	}
    }
    int dy = dayOfYear(d,m,y);
    if(dy < firstMonday) return weekNumber(31,12,y-1);	//last week of previous year
    dy -= firstMonday;
    return dy / 7 + 1;
}

/*******************************************************************************/

#ifdef TEST

int main(int argc,char **argv)
{
int d,m,y;
    y = 2016;
    m = 1;
    d = 1;
    printf("%02d-%02d-%02d DOW %d %s WK %d\n",d,m,y,dayOfWeek(d,m,y),DOW(d,m,y),weekNumber(d,m,y));

    d = 18;
    m = 11;
    printf("%02d-%02d-%02d DOW %d %s WK %d\n",d,m,y,dayOfWeek(d,m,y),DOW(d,m,y),weekNumber(d,m,y));

    m = 1;
    d = 19;
    printf("%02d-%02d-%02d DOW %d %s WK %d\n",d,m,y,dayOfWeek(d,m,y),DOW(d,m,y),weekNumber(d,m,y));

    m = 11;
    d = 19;
    printf("%02d-%02d-%02d DOW %d %s WK %d\n",d,m,y,dayOfWeek(d,m,y),DOW(d,m,y),weekNumber(d,m,y));

    m = 12;
    d = 31;
    printf("%02d-%02d-%02d DOW %d %s WK %d\n",d,m,y,dayOfWeek(d,m,y),DOW(d,m,y),weekNumber(d,m,y));

    m = 1;
    d = 1;
    y++;
    printf("%02d-%02d-%02d DOW %d %s WK %d\n",d,m,y,dayOfWeek(d,m,y),DOW(d,m,y),weekNumber(d,m,y));

    m = 11;
    d = 20;
    y--;
    printf("%02d-%02d-%02d DOW %d %s WK %d\n",d,m,y,dayOfWeek(d,m,y),DOW(d,m,y),weekNumber(d,m,y));


    return 0;
}

#endif
