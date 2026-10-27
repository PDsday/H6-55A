/*=====================================================================================
  This file is part of CANOpen Stack.

  Copyright (C): Fly-Lab Studio        
        
  Author: Horton Lu
===================================================================================== */

#ifndef __TIMERSCFG_H__
#define __TIMERSCFG_H__

// Whatever your microcontroller, the timer wont work if
// TIMEVAL is not at least on 32 bits
#define TIMEVAL UNS32

// The timer of the AVR counts from 0000 to 0xFFFF in CTC mode (it can be
// shortened setting OCR3A eg. to get 2ms instead of 2.048ms)
#define TIMEVAL_MAX 0xFFFF

// The timer is incrementing every 4 us.
//#define MS_TO_TIMEVAL(ms) (ms * 250)
//#define US_TO_TIMEVAL(us) (us>>2)

// The timer is incrementing every 8 us.
//#define MS_TO_TIMEVAL(ms) ((ms) * 125)
//#define US_TO_TIMEVAL(us) ((us)>>3)

// The timer is incrementing every 1 us.
#define MS_TO_TIMEVAL(ms) ((ms) * 1000)
#define US_TO_TIMEVAL(us) (us)


#endif
