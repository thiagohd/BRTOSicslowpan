//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

/* MCU and OS includes */
#include "BRTOS.h"
#include "xwdt.h"

/* Config. files */
void main_app(void);
//#include "tasks.h"        /* for tasks prototypes */


// ----------------------------------------------------------------------------
//
// KL25 led blink sample (trace via DEBUG).
//
// In debug configurations, demonstrate how to print a greeting message
// on the trace device. In release configurations the message is
// simply discarded.
//
// To demonstrate POSIX retargetting, reroute the STDOUT and STDERR to the
// trace device and display messages on both of them.
//
// Then demonstrates how to blink a led with 1 Hz, using a
// continuous loop and SysTick delays.
//
// On DEBUG, the uptime in seconds is also displayed on the trace device.
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//


// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int main(void)
{
  // Init system clock
  //xSysCtlClockSet(48000000, xSYSCTL_XTAL_8MHZ | xSYSCTL_OSC_MAIN);
  SystemInit();

  // Initialize system and tasks
  main_app();

  // Infinite loop
  while (1)
    {
    }
  // Infinite loop, never return.
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
