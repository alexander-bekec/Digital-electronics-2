/***********************************************************************
 * 
 * Stopwatch with LCD display output.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2017-Present Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include "lcd.h"            // Peter Fleury's LCD library
#include <stdlib.h>         // C library. Needed for conversion function

uint8_t euroChar[48] = {
    0b00110,
    0b01001,
    0b11100,
    0b01000,
    0b11100,
    0b01001,
    0b00110,
    0b00000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b11000,
    0b11000,
    0b11000,
    0b11000,
    0b11000,
    0b11000,
    0b11000,
    0b11000,
    0b11100,
    0b11100,
    0b11100,
    0b11100,
    0b11100,
    0b11100,
    0b11100,
    0b11100,
    0b11110,
    0b11110,
    0b11110,
    0b11110,
    0b11110,
    0b11110,
    0b11110,
    0b11110,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111
};

/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Update stopwatch value on LCD display when 8-bit 
 *           Timer/Counter2 overflows.
 * Returns:  none
 **********************************************************************/
int main(void)
{
    // Initialize LCD display
    lcd_init(LCD_DISP_ON);
    
    
    lcd_command(1 << LCD_CGRAM);
    for (uint8_t i = 0; i < 48; i++)
    {
        // Store all new chars to memory line by line
        lcd_data(euroChar[i]);
    }
    // Set DDRAM address
    lcd_command(1 << LCD_DDRAM);
    
    lcd_gotoxy(1, 0);
    // Display first custom character
    // lcd_putc(5);

    // Configure 8-bit Timer/Counter2 for Stopwatch
    // Set the overflow prescaler to 16 ms and enable interrupt
    TIM2_overflow_16ms();
    TIM2_overflow_interrupt_enable();

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
 * Function: Timer/Counter2 overflow interrupt
 * Purpose:  Update the stopwatch on LCD display every sixth overflow,
 *           ie approximately every 100 ms (6 x 16 ms = 100 ms).
 **********************************************************************/
ISR(TIMER2_OVF_vect)
{
    static uint8_t number_of_overflows = 0;
    static uint8_t tens_sec = 0;
    static uint8_t sec = 0;
    static uint8_t tenths_sec = 0;
    
    char das [1] = " ";
    char s[1] = " ";
    char ds[1] = " ";

    number_of_overflows++;
    if (number_of_overflows >= 6)
    {
        // Do this every 6 x 16 ms = 100 ms
        number_of_overflows = 0;
        tenths_sec++;
        
        if (tenths_sec > 9) {
            tenths_sec = 0;
            sec++;
        }
        
        if (sec > 9) {
            sec = 0;
            tens_sec++;
        }
        
        if (tens_sec > 5) {
            tens_sec = 0;
        }
        
        
        itoa(tens_sec, das, 10);
        itoa(sec, s, 10);
        itoa(tenths_sec, ds, 10);
        
        
        lcd_gotoxy(1, 0);
        lcd_puts("00:");
        lcd_gotoxy(4, 0);
        lcd_puts(das);
        lcd_gotoxy(5, 0);
        lcd_puts(s);
        lcd_gotoxy(6, 0);
        lcd_putc('.');
        lcd_gotoxy(7, 0);
        lcd_puts(ds);
    }
    // Else do nothing and exit the ISR
}
