// ******** Timer.c **************
// Authors: Ryan Mitchell and Ben Richards
// Initial Creation Date: 9/11/2018
// Description: Used to initialize a timer
// for general purposes
// Lab Number: Lab02
// TA: Zee Lv
// Date of last revision: 9/17/2018
// Hardware Configuration: N/A

#include "Timer.h"

#include "tm4c123gh6pm.h"
#include <stdint.h>

/**
 * Initialize a General timer in the TM4C123
 * @param: reloadValue: Value to count down from for the timer
 */
void Timer1_Init(int reloadValue)
{
    volatile uint32_t delay;
    SYSCTL_RCGCTIMER_R |= 0x02; // 0) activate TIMER1
    delay = SYSCTL_RCGCTIMER_R; // allow time to finish activating
    TIMER1_CTL_R = 0x00000000; // 1) disable TIMER1A during setup
    TIMER1_CFG_R = 0x00000000; // 2) configure for 32-bit mode
    TIMER1_TAMR_R = 0x00000002; // 3) configure for periodic mode, down-count
    TIMER1_TAILR_R = reloadValue; // 4) reload value
    TIMER1_TAPR_R = 0; // 5) bus clock resolution
    TIMER1_CTL_R = 0x00000001; // 10) enable TIMER1A
}
