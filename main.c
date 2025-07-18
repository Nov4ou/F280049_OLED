// #############################################################################
//
//  FILE:   empty_driverlib_main.c
//
//! \addtogroup driver_example_list
//! <h1>Empty Project Example</h1>
//!
//! This example is an empty project setup for Driverlib development.
//!
//
// #############################################################################
//
//
// $Copyright:
// Copyright (C) 2024 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the
//   distribution.
//
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
// #############################################################################

//
// Included Files
//
#include "board.h"
#include "c2000ware_libraries.h"
#include "device.h"
#include "driverlib.h"
#include "oled.h"

// NOTE: User-provided platform functions:
void SPI_WriteByte(uint32_t spiBase, uint8_t byte) {
  SPI_transmitByte(SPIA_BASE, byte);
}

void Delay_us(uint32_t micros) { DEVICE_DELAY_US(micros); }

//
// Main
//
void main(void) {
  //
  // Initialize device clock and peripherals
  //
  Device_init();

  //
  // Disable pin locks and enable internal pull-ups.
  //
  Device_initGPIO();

  //
  // Initialize PIE and clear PIE registers. Disables CPU interrupts.
  //
  Interrupt_initModule();

  //
  // Initialize the PIE vector table with pointers to the shell Interrupt
  // Service Routines (ISR).
  //
  Interrupt_initVectorTable();

  //
  // PinMux and Peripheral Initialization
  //
  Board_init();

  //
  // C2000Ware Library initialization
  //
  C2000Ware_libraries_init();

  //
  // Enable Global Interrupt (INTM) and real time interrupt (DBGM)
  //
  EINT;
  ERTM;

  oledConfig(OLED_RST, OLED_DC, mySPI0_BASE);
  oledInit();
  oledClear();
  char msg[] = "cyka";
  oledShowNum(30, 30, 910, 3, 16);
  oledShowString(0, 1, msg, 8);
  oledShowString(50, 10, msg, 16);
  while (1) {
  }
}

//
// End of File
//
