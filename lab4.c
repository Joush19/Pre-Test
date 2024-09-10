//*****************************************************************************
//
// blinky.c - Simple example to blink the on-board LED.
//
// Copyright (c) 2013-2020 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.2.0.295 of the EK-TM4C1294XL Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Blinky (blinky)</h1>
//!
//! A very simple example that blinks the on-board LED using direct register
//! access.
//
//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

//*****************************************************************************
//
// Blink the on-board LED.
//
//*****************************************************************************

uint32_t FS = (120000000*3)/2;
void timer0A_handler(void);
uint8_t switch_state = 0;
uint32_t button = 1;
uint8_t n[16] = {0x0,0x0,0x0,0x0,0x01,0x01,0x01,0x01,0x02,0x02,0x02,0x02,0x03,0x03,0x03,0x03};
uint8_t f[16] = {0x0,0x01,0x10,0x11,0x0,0x01,0x10,0x11,0x0,0x01,0x10,0x11,0x0,0x01,0x10,0x11};

int main(void)
{
    //volatile uint32_t ui32Loop;
    
    //
 
    //Clock Configuration
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ|SYSCTL_OSC_MAIN|SYSCTL_USE_PLL|SYSCTL_CFG_VCO_480), 120000000); //Enables system clock
    
    // Enable the GPIO port that is used for the on-board LED.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    //
    // Check if the peripheral access is enabled.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }
    
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }
    
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ))
    {
    }
    
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
    {
    }
    
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, FS);
    IntMasterEnable();
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER0_BASE, TIMER_A);
    
    //
    // Enable the GPIO pin for the LED (PN0).  Set the direction as output, and
    // enable the GPIO pin for digital function.
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x03);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x11);
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, 0x01);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, 0X01, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    //
    // Loop forever.
    //
    while(1)
    {   
        //
        // Turn on the LED.
        //
                
        //
        // Delay for a bit.
        //
        /*for(ui32Loop = 0; ui32Loop < 2000000; ui32Loop++)
        {
        }
	*/
        //
        // Turn off the LED.
        //
        
        //
        // Delay for a bit.
        //
        /*for(ui32Loop = 0; ui32Loop < 2000000; ui32Loop++)
        {
        }*/
    }
}

void timer0A_handler(void){
    
    switch_state ++;
    if (switch_state > 15){
        switch_state = 0;
    }
    if (GPIOPinRead(GPIO_PORTJ_BASE, 0x01) == 0){
        button ++;
    }
    if (button % 2 != 0){
        FS = (120000000*3)/2;
        TimerLoadSet(TIMER0_BASE, TIMER_A, FS);
    }
    else if (button % 2 == 0){
        FS = 120000000*3;
        TimerLoadSet(TIMER0_BASE, TIMER_A, FS);
    }
    TimerIntClear(TIMER0_BASE, TIMER_A);
    
    GPIOPinWrite(GPIO_PORTN_BASE, 0x03, n[switch_state]);
    GPIOPinWrite(GPIO_PORTF_BASE, 0x11, f[switch_state]);
}

