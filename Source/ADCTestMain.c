// ADCTestMain.c
// Runs on TM4C123
// This program periodically samples ADC channel 0 and stores the
// result to a global variable that can be accessed with the JTAG
// debugger and viewed with the variable watch feature.
// Daniel Valvano
// September 5, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// center of X-ohm potentiometer connected to PE3/AIN0
// bottom of X-ohm potentiometer connected to ground
// top of X-ohm potentiometer connected to +3.3V
#include "ADCSWTrigger.h"
#include "Fixed.h"
#include "PLL.h"
#include "ST7735.h"
#include "Timer.h"
#include "Types.h"
#include "heap.h"
#include "tm4c123gh6pm.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define PF2 (*((volatile uint32_t*)0x40025010))
#define PF1 (*((volatile uint32_t*)0x40025008))

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void); // Enable interrupts
long StartCritical(void); // previous I bit, disable interrupts
void EndCritical(long sr); // restore I bit to previous value
void WaitForInterrupt(void); // low power mode

static const uint16_t ADCMaxNumValues = 1000;
volatile uint32_t ADCTimeStamps[ADCMaxNumValues] = { 0 };
volatile uint32_t ADCValues[ADCMaxNumValues] = { 0 };
volatile uint32_t ADCJitter[ADCMaxNumValues - 1] = { 0 };
volatile uint32_t ADCCursor = 0;
HashMap* ADCValueOccurances = NULL;

// This debug function initializes Timer0A to request interrupts
// at a 100 Hz frequency.  It is similar to FreqMeasure.c.
void Timer0A_Init100HzInt(void)
{
    volatile uint32_t delay;
    DisableInterrupts();
    // **** general initialization ****
    SYSCTL_RCGCTIMER_R |= 0x01; // activate timer0
    delay = SYSCTL_RCGCTIMER_R; // allow time to finish activating
    TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
    TIMER0_CFG_R = 0; // configure for 32-bit timer mode
    // **** timer0A initialization ****
    // configure for periodic mode
    TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
    TIMER0_TAILR_R = 799999; // start value for 100 Hz interrupts
    TIMER0_IMR_R |= TIMER_IMR_TATOIM; // enable timeout (rollover) interrupt
    TIMER0_ICR_R = TIMER_ICR_TATOCINT; // clear timer0A timeout flag
    TIMER0_CTL_R |= TIMER_CTL_TAEN; // enable timer0A 32-b, periodic, interrupts
    // **** interrupt initialization ****
    // Timer0A=priority 2
    NVIC_PRI4_R = (NVIC_PRI4_R & 0x00FFFFFF) | 0x40000000; // top 3 bits
    NVIC_EN0_R = 1 << 19; // enable interrupt 19 in NVIC
}

void Timer0A_Handler(void)
{
    static const uint32_t* defaultValue = 0;
    if (ADCCursor < sizeof(ADCValues)) {
        TIMER0_ICR_R = TIMER_ICR_TATOCINT; // acknowledge timer0A timeout
        PF2 ^= 0x04; // profile
        PF2 ^= 0x04; // profile
        ADCTimeStamps[ADCCursor] = TIMER1_TAR_R;
        uint32_t value = ADC0_InSeq3();
        ADCValues[ADCCursor] = value;
        uint32_t* occurances = getOrDefault(ADCValueOccurances, &value, &defaultValue);
        ++(*occurances);
        ++ADCCursor;
        PF2 ^= 0x04; // profile
    }
}

void ProcessADCValues(void)
{
    for (int i = 0; i < ADCMaxNumValues - 1; ++i)
        ADCJitter[i] = ADCTimeStamps[i] - ADCTimeStamps[i + 1];

    uint32_t maxValue = ADCValues[0];
    uint32_t minValue = ADCValues[0];
    uint32_t maxOccurances = 0;
    uint32_t minOccurances = 0;
    entry* e = iterate(ADCValueOccurances);
    do {
        uint32_t value = *(uint32_t*)getKey(e);
        uint32_t occurances = *(uint32_t*)getValue(e);
        maxValue = value > maxValue ? value : maxValue;
        minValue = value < minValue ? value : minValue;
        maxOccurances = occurances > maxOccurances ? occurances : maxOccurances;
        minOccurances = occurances < minOccurances ? occurances : minOccurances;
    } while ((e = iterate(NULL)) != NULL);
    ST7735_PMFplotInit(minValue, maxValue, minOccurances, maxOccurances, 1);
    ST7735_PlotADCPMF(ADCValueOccurances);
    clear(ADCValueOccurances);
    ADCCursor = 0;
    TIMER1_TAILR_R = 0xFFFFFFFF;
}

int main(void)
{
    PLL_Init(Bus80MHz); // 80 MHz
    SYSCTL_RCGCGPIO_R |= 0x20; // activate port F
    ADC0_InitSWTriggerSeq3_Ch9(); // allow time to finish activating
    Timer0A_Init100HzInt(); // set up Timer0A for 100 Hz
    Timer1_Init(0xFFFFFFFF);

    // configure PF2 as GPIO
    GPIO_PORTF_DIR_R |= 0x06; // make PF2, PF1 out (built-in LED)
    GPIO_PORTF_AFSEL_R &= ~0x06; // disable alt funct on PF2, PF1
    GPIO_PORTF_DEN_R |= 0x06; // enable digital I/O on PF2, PF1
    GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R & 0xFFFFF00F) + 0x00000000;
    GPIO_PORTF_AMSEL_R = 0; // disable analog functionality on PF
    PF2 = 0; // turn off LED
    Output_Init();
	  Heap_Init();
    ADCValueOccurances = new(HASHMAP, 2, NUMBER, NUMBER);
    EnableInterrupts();
    while (true) {
        EnableInterrupts();
        while (ADCCursor < 1000) {
            PF1 ^= 0x02; // toggles when running in main
        }
        DisableInterrupts();
        ProcessADCValues();
    }
}
