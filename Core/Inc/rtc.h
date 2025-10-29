#ifndef RTC_AUS
#define RTC_AUS

#include "stm32f439xx.h"

#define RTC_WRITE_PROTECTION_UNLOCK_1 0xCAU
#define RTC_WRITE_PROTECTION_UNLOCK_2 0x53U

enum DoW {
    Monday      = 0,
    Tuesday     = 1,
    Wednesday   = 2,
    Thursday    = 3,
    Friday      = 4,
    Saturday    = 5,
    Sunday      = 6
};

typedef struct ts {
    uint8_t secs;
    uint8_t mins;
    uint8_t hours;
    uint8_t date;       // day of month
    uint8_t month;
    uint16_t year;
    uint8_t day; 
    uint8_t isDst;

} ts;

void RTC_init(ts *ts);
void RTC_setTime(ts *ts);
void RTC_getTime(ts *ts);

#endif