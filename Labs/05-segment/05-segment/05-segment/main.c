/***********************************************************************
 * 
 * Decimal counter with 7-segment output.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2018-Present Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include "segment.h"        // Seven-segment display library for AVR-GCC

volatile uint8_t value_pos0 = 0;
volatile uint8_t value_pos1 = 0;

/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Display decimal counter values on SSD (Seven-segment 
 *           display) when 16-bit Timer/Counter1 overflows.
 * Returns:  none
 **********************************************************************/
int main(void)
{
    // Configure SSD signals
    SEG_init();

    // Configure 16-bit Timer/Counter1 for Decimal counter
    // Set the overflow prescaler to 262 ms and enable interrupt
    
    TIM1_overflow_262ms();
    TIM1_overflow_interrupt_enable();
    
    // Configure 8-bit Timer/Counter0 for Decimal counter
    // Set the overflow prescaler to 4 ms and enable interrupt
    
    TIM0_overflow_4ms();
    TIM0_overflow_interrupt_enable();
    
    // Enables interrupts by setting the global interrupt mask
    
    sei();
   
    // Infinite loop
    while (1)
    {
        /* Empty loop. All subsequent operations are performed exclusively 
         * inside interrupt service routines ISRs */
    }

    // Will never reach this
    return 0;
}

/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter1 overflow interrupt
 * Purpose:  Increment decimal counter value and display it on SSD.
 **********************************************************************/
ISR(TIMER0_OVF_vect)
{
    static uint8_t pos = 0;
    if (pos == 0) {
        SEG_update_shift_regs(value_pos0, pos);
        pos++;
    }
    else {
        SEG_update_shift_regs(value_pos1, pos);
        pos = 0;
    }
}
    
ISR(TIMER1_OVF_vect)
{
    value_pos0++;
    
    if (value_pos0 > 9) {
        value_pos0 = 0;
        value_pos1++;
    }
    
    if (value_pos1 >= 6) {
        value_pos1 = 0;
    }
}