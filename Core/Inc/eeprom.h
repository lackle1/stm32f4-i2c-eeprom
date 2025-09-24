#ifndef EEPROM
#define EEPROM

#include <stdint.h>
#include "stm32f439xx.h"

#define EEPROM_ADDRESS  0b1010000       // 7-bit address


#define PAGE_NUM        4096
#define PAGE_SIZE       1               // Counted in bytes

#endif