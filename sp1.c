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
#include <string.h>
#include <stdlib.h>  
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "utils/uartstdio.c"

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

//uint32_t FS = (120000000*3)/2;
bool state = false;
void printData(void);
//void blink(uint32_t distance);
uint32_t ind = 0;
char data[50];

void uart(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, 0x03);
    UARTStdioConfig(0,9600,120000000);
}

//
// Check if the peripheral access is enabled.
//

void checking(void){
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)){}
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOH)){}
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM)){}
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)){}
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){}
}

void pinConfiguration(void){
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x03);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x11);
    GPIOPinTypeGPIOInput(GPIO_PORTH_BASE, 0x03);
    GPIOPinTypeGPIOInput(GPIO_PORTM_BASE, 0x07);

    GPIOPadConfigSet(GPIO_PORTH_BASE, 0X03, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTM_BASE, 0X07, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

int main(void)
{
    //Clock Configuration
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ|SYSCTL_OSC_MAIN|SYSCTL_USE_PLL|SYSCTL_CFG_VCO_480), 120000000); //Enables system clock
    
    // Enable the GPIO port that is used for the on-board LED.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    
    uart(); 
    checking();
    pinConfiguration();    
    //
    
    // Loop forever.
    //
    while(1)
    {
        printData();
        if(UARTCharsAvail(UART0_BASE)){
            UARTgets(data, 50);
            ind = atoi(data);
            if (ind > 2000){
                GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x0);
                if (state == false){
                    GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x0);
                }
                else{
                    GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x03);
                }
            }
            if ((ind < 2000) && (ind > 1500)){
                GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x0);
                GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x10);
            }
            else if (ind < 1000){
                GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x03);
                GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x11);
            }
            state = !state;
        }
        
    }
}

void printData(void){
    if (GPIOPinRead(GPIO_PORTM_BASE, 0x01) == 0){
        UARTprintf("0\n");
    }
    else if (GPIOPinRead(GPIO_PORTM_BASE, 0x02) == 0){
        UARTprintf("1\n");
    }
    else if (GPIOPinRead(GPIO_PORTM_BASE, 0x04) == 0){
        UARTprintf("2\n");
    }
    else if (GPIOPinRead(GPIO_PORTH_BASE, 0x01) == 0){
        UARTprintf("3\n");
    }
    else if (GPIOPinRead(GPIO_PORTH_BASE, 0x02) == 0){
        UARTprintf("4\n");
    }
    SysCtlDelay(20000000);
}
