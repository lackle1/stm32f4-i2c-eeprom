#ifndef RTC
#define RTC

#include "stm32f439xx.h"

typedef enum {
    Monday      = 0,
    Tuesday     = 1,
    Wednesday   = 2,
    Thursday    = 3,
    Friday      = 4,
    Saturday    = 5,
    Sunday      = 6
} DoW;

typedef struct ts {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t date;       // day of month
    uint8_t month;
    uint8_t year;
    uint8_t day; 
    uint8_t isDst;

} ts;

#endif