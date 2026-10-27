/*=====================================================================================
  This file is part of CANOpen Stack.

  Copyright (C): Fly-Lab Studio        
        
  Author: Horton Lu
===================================================================================== */

#ifndef __applicfg_h__
#define __applicfg_h__

#include <string.h>
#include <stdio.h>

// Integers
#define INTEGER8 signed char
#define INTEGER16 short
#define INTEGER24
#define INTEGER32 long
#define INTEGER40
#define INTEGER48
#define INTEGER56
#define INTEGER64

// Unsigned integers
#define UNS8   unsigned char
#define UNS16  unsigned short
#define UNS32  unsigned long
#define UNS64  long long
/*
#define UNS24
#define UNS40
#define UNS48
#define UNS56
#define UNS64
*/ 


// Reals
#define REAL32	float
#define REAL64 double
#include "can.h"


//---------------------------------------
//#define DEBUG_WAR_CONSOLE_ON 0
//#define DEBUG_ERR_CONSOLE_ON 0
//---------------------------------------

// MSG functions
// not finished, the strings have to be placed to the flash and printed out 
// using the printf_P function
/// Definition of MSG_ERR
// ---------------------
//#ifdef DEBUG_ERR_CONSOLE_ON
//#define MSG_ERR(num, str, val)      \
//          printf(num, ' ');	\
//          printf(str);		\
//          printf(val);		\
//          printf('\n');
//#else
//#    define MSG_ERR(num, str, val)
//#endif
#ifdef DEBUG_ERR_CONSOLE_ON
#define MSG_ERR(num, str, val) {printf(str); printf("\n");}
#else
#    define MSG_ERR(num, str, val)
#endif


/// Definition of MSG_WAR
// ---------------------
//#ifdef DEBUG_WAR_CONSOLE_ON
//#define MSG_WAR(num, str, val)      \
//          printf(num, ' ');	\
//          printf(str);		\
//          printf(val);		\
//          printf('\n');
//#else
//#    define MSG_WAR(num, str, val)
//#endif
#ifdef DEBUG_WAR_CONSOLE_ON
#define MSG_WAR(num, str, val) {printf(str); printf("\n");}
#else
#    define MSG_WAR(num, str, val)
#endif



typedef void* CAN_HANDLE;

typedef void* CAN_PORT;

#endif


