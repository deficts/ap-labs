#include <stdio.h>
#include <stdlib.h> 
extern 
/* month_day function's prototype*/
void month_day(int year, int yearday, int *pmonth, int *pday);

static char *months[12] = {"January","February","March","April","May","June","July","August","September","October","November","December"};
static char yearDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static char leapYearDays[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

int isLeap(int year){
    return year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
}

int main(int argc, char **argv ) {

    if(argc != 3){
        printf("You provided wrong input, plese make sure to provide a <year> and a <yearday> as parameters\n");
        return(0);
    }

    int year = atoi(argv[1]);
    int day = atoi(argv[2]);
    int isLeapYear = isLeap(year);

    int monthPointer, dayPointer;

    if(isLeapYear == 1 && day > 366){
        printf("You provided wrong input, the day must be between 0 and 366\n");
        return(0);
    }

    if(isLeapYear == 0 && day > 365){
        printf("You provided wrong input, the day must be between 0 and 365\n");
        return(0);
    }

    month_day(year,day,&monthPointer,&dayPointer);
    printf("%s %i, %i\n", months[monthPointer],dayPointer,year);
    return 0;
}

void month_day(int year, int yearday, int *pmonth, int *pday){
    int isLeapYear = isLeap(year);
    int i;
    if(isLeapYear){
        for(i = 0; i<yearday; i++){
            yearday -= leapYearDays[i];
        }
        if(yearday<=0){
            i-=1;
            *pmonth = i;
            *pday = yearday + leapYearDays[i];
        }else{
            *pmonth = i;
            *pday = yearday;
        }
    }else{
        for(i = 0; i<yearday; i++){
            yearday -= yearDays[i];
        }
        if(yearday<=0){
            i-=1;
            *pmonth = i;
            *pday = yearday + yearDays[i];
        }else{
            *pmonth = i;
            *pday = yearday;
        }
    }
}

