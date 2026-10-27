/*=====================================================================================
  This file is part of CANOpen Stack.

  Copyright (C): Fly-Lab Studio        
        
  Author: Horton Lu
===================================================================================== */


#ifndef __canopen_h__
#define __canopen_h__

#include "applicfg.h"
#include "data.h"


extern UNS8 CANopen_LifeGuard_TimeOut;
extern INTEGER8 FD_ObjDict_obj1008[10]; // ¥”’æ–Õ∫≈
// ---------  to be called by user app ---------
void initTimer(void);
UNS8 canSend(CAN_PORT notused, Message *m);
UNS8 canChangeBaudRate(CAN_PORT port, char* baud);

#endif
