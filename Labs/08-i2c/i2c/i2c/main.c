/***********************************************************************
 * 
 * The I2C bus scanner detects the addresses of the modules that are 
 * connected to the SDA and SCL signals. A simple description of FSM is 
 * used.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2017-Present Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/

/* Defines -----------------------------------------------------------*/
#ifndef F_CPU
# define F_CPU 16000000  // CPU frequency in Hz required for UART_BAUD_SELECT
#endif

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <stdlib.h>         // C library. Needed for conversion function
#include "uart.h"           // Peter Fleury's UART library
#include "twi.h"            // TWI library for AVR-GCC

/* Variables ---------------------------------------------------------*/
typedef enum {              // FSM declaration
    STATE_IDLE = 1,
    STATE_SEND,
    STATE_ACK
} state_t;

/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Use Timer/Counter1 and send I2C (TWI) address every 33 ms.
 *           Send information about scanning process to UART.
 * Returns:  none
 **********************************************************************/
int main(void)
{
    // Initialize I2C (TWI)
    twi_init();

    // Initialize UART to asynchronous, 8N1, 9600
    uart_init(UART_BAUD_SELECT(9600, F_CPU));

    // Configure 16-bit Timer/Counter1 to update FSM
    // Set prescaler to 33 ms and enable interrupt
    TIM1_overflow_33ms();
    TIM1_overflow_interrupt_enable();

    // Enables interrupts by setting the global interrupt mask
    sei();

    // Put strings to ringbuffer for transmitting via UART
    uart_puts("\r\nScan I2C-bus for devices:\r\n");
    uart_puts("\r\n");
    uart_puts("\r\n      .0 .1 .2 .3 .4 ");
    //uart_puts(".5 .6 .7 .8 .9 .a .b .c .d .e .f");
    uart_puts("\r\n0x0.: RA RA RA RA RA RA RA RA ");
    

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
 * Purpose:  Update Finite State Machine and test I2C slave addresses 
 *           between 8 and 119.
 **********************************************************************/
ISR(TIMER1_OVF_vect)
{
    
    static state_t state = STATE_IDLE;  // Current state of the FSM
    static uint8_t addr = 7;            // I2C slave address
    uint8_t result = 1;                 // ACK result from the bus
    char uart_string_line[] = "000";     // String for converting numbers by itoa()
    char uart_string[] = "000";
    
    /*
    char uart_humidity_integer[] = "000";
    char uart_humidity_fractional[] = "000";
    char uart_temperature_integer[] = "000";
    char uart_temperature_fractional[] = "000";
    char uart_checksum[] = "000";
    
    static uint8_t humidity_integer = 0;
    static uint8_t humidity_fractional = 0;
    static uint8_t temperature_integer = 0;
    static uint8_t temperature_fractional = 0;
    static uint8_t checksum = 0;
    
    if (addr == 92) {
        twi_start((addr<<1) + TWI_READ);
        humidity_integer = twi_read_ack();
        humidity_fractional = twi_read_ack();
        temperature_integer = twi_read_ack();
        temperature_fractional = twi_read_ack();
        checksum = twi_read_ack();
        twi_stop();
        itoa(humidity_integer, uart_humidity_integer, 10);
        itoa(humidity_fractional, uart_humidity_fractional, 10);
        itoa(temperature_integer, uart_temperature_integer, 10);
        itoa(temperature_fractional, uart_temperature_fractional, 10);
        itoa(checksum, uart_checksum, 10);
        uart_puts("Humidity: ");
        uart_puts(uart_humidity_integer);
        uart_puts(".");
        uart_puts(uart_humidity_fractional);
        uart_puts("\n\r");
        uart_puts("Temperature: ");
        uart_puts(uart_temperature_integer);
        uart_puts(".");
        uart_puts(uart_temperature_fractional);
        uart_puts("\n\r");
        uart_puts("Checksum: ");
        uart_puts(uart_checksum);
        uart_puts("\n\r");
        }
    */
    
    // FSM
    switch (state)
    {
    // Increment I2C slave address
    case STATE_IDLE:
        addr++;
        // If slave address is between 8 and 119 then move to SEND state
        if (addr > 7 && addr < 120) {
            state = STATE_SEND;
        }
        else {
            addr = 7;
            state = STATE_IDLE;
            uart_puts("RA RA RA RA RA RA RA RA ");
            uart_puts("\r\n");
            uart_puts("\r\n      .0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .a .b .c .d .e .f");
            uart_puts("\r\n0x0.: RA RA RA RA RA RA RA RA ");
        }
        
        break;
    
    // Transmit I2C slave address and get result
    case STATE_SEND:
        // I2C address frame:
        // +------------------------+------------+
        // |      from Master       | from Slave |
        // +------------------------+------------+
        // | 7  6  5  4  3  2  1  0 |     ACK    |
        // |a6 a5 a4 a3 a2 a1 a0 R/W|   result   |
        // +------------------------+------------+
        result = twi_start((addr<<1) + TWI_WRITE);
        twi_stop();
        /* Test result from I2C bus. If it is 0 then move to ACK state, 
         * otherwise move to IDLE */
        if (addr % 16 == 0) {
            uart_puts("\n\r0x");
            itoa(addr / 16, uart_string_line, 16);
            uart_puts(uart_string_line);
            uart_puts(".: ");
        }
        
        if (result == 0) {
            state = STATE_ACK;
            
        }
        else {
            state = STATE_IDLE;
            uart_puts("-- ");
        }
        break;

    // A module connected to the bus was found
    case STATE_ACK:
        // Send info about active I2C slave to UART and move to IDLE
        itoa(addr, uart_string, 16);
        uart_puts(uart_string);
        uart_puts(" ");
        
        /*
        uart_puts("\n\r      .0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .a .b .c .d .e .f");
        uart_puts("\n\r0x0.: RA RA RA RA RA RA RA RA ");
        itoa(addr, uart_string_dec, 10);
        itoa(addr, uart_string_bin, 16);
        uart_puts("Address found: ");
        uart_puts(uart_string_dec);
        uart_puts("[0x");
        uart_puts(uart_string_bin);
        uart_puts("]");
        uart_puts("\n\r");
        */
               
        state = STATE_IDLE;
        break;
        
    // If something unexpected happens then move to IDLE
    default:
        state = STATE_IDLE;
        break;
    }
}