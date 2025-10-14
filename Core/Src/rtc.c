/***********************************************************************************
 * @file        rtc.c                                                              *
 * @author      Lachie Keane                                                       *
 * @addtogroup  RTC                                                                *
 * @brief       Implements RTC integration.                                        *
 ***********************************************************************************/

#include "rtc.h"

void RTC_init() {

    /*  Enable RTC
    * 1. Enable access to PWR
    * 2. Enable write access to backup domain
    * 3. Enable LSE and wait for it to be ready
    * 4. Set RTC source to LSE
    * 5. Enable RTC
    */

    // Enable access to PWR
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;

    // Enable write access to backup domain
    PWR->CR |= PWR_CR_DBP;

    // Enable LSE and wait for it to be ready
    RCC->BDCR |= RCC_BDCR_LSEON;
    while ((RCC->BDCR & RCC_BDCR_LSERDY) == 0)

    // Set RTC source to LSE
    RCC->BDCR |= RCC_BDCR_RTCSEL_0;
    RCC->BDCR &= ~RCC_BDCR_RTCSEL_1;

    // Enable RTC
    RCC->BDCR |= RCC_BDCR_RTCEN;

    /*  Initialise
    * 1. Unlock write protection
    * 2. Enter initialisation mode and wait for the init flag to be set, signalling the RTC is ready
    * 3. Set sync prescaler then async prescaler (manual specifically says in this order)
    * 4. Load initial time and date values in the shadow registers and configure time mode (12h or 24h)
    * 5. Exit initialisation mode
    */

    // Unlock write protection
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    // Enter initialisation mode
    RTC->ISR |= RTC_ISR_INIT;
    while ((RTC->ISR & RTC_ISR_INITF) == 0);

    // Set sync prescaler then async prescaler (manual specifically says in this order)
    RTC->PRER |= 256;
    RTC->PRER |= 128 << RTC_PRER_PREDIV_A_Pos;

    // Load initial time and date values in the shadow registers and configure time mode (12h or 24h)


    // Exit initialisation mode
    RTC->ISR &= ~RTC_ISR_INIT;
}

void RTC_setTime() {

}