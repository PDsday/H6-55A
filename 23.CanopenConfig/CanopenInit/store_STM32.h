/*=====================================================================================
  This file is part of CANOpen Stack.

  Copyright (C): Fly-Lab Studio        
        
  Author: Horton Lu
===================================================================================== */

#ifndef __store_STM32__
#define __store_STM32__

// the CANopen driver includes
// #include "stm32f10x_lib.h"
#include "stm32f4xx_conf.h"
#include "config.h"
#include "applicfg.h"
#include "data.h"



/************************* To be called by user app ***************************/

extern UNS32 FD_ObjDict_obj1006;	/* index 0x1006 :   Communication / Cycle Period. */
//extern UNS16 FD_ObjDict_obj100C;  /* index 0x100C :   Guard Time. */
extern UNS8 FD_ObjDict_obj100D;  /* index 0x100D :   Life Time Factor. */
extern UNS16 FD_ObjDict_obj1017;  /* index 0x1017 :   Producer Heartbeat Time. */


/* index 0x1400 :   Receive PDO 1 Parameter. */
extern UNS32 FD_ObjDict_obj1400_COB_ID_used_by_PDO;
extern UNS8 FD_ObjDict_obj1400_Transmission_Type;
extern UNS16 FD_ObjDict_obj1400_Inhibit_Time;
//extern UNS8 FD_ObjDict_obj1400_Compatibility_Entry;
extern UNS16 FD_ObjDict_obj1400_Event_Timer;

/* index 0x1401 :   Receive PDO 2 Parameter. */
extern UNS32 FD_ObjDict_obj1401_COB_ID_used_by_PDO;
extern UNS8 FD_ObjDict_obj1401_Transmission_Type;
extern UNS16 FD_ObjDict_obj1401_Inhibit_Time;
//extern UNS8 FD_ObjDict_obj1401_Compatibility_Entry;
extern UNS16 FD_ObjDict_obj1401_Event_Timer;

/* index 0x1402 :   Receive PDO 3 Parameter. */
extern UNS32 FD_ObjDict_obj1402_COB_ID_used_by_PDO;
extern UNS8 FD_ObjDict_obj1402_Transmission_Type;
extern UNS16 FD_ObjDict_obj1402_Inhibit_Time;
//extern UNS8 FD_ObjDict_obj1402_Compatibility_Entry;
extern UNS16 FD_ObjDict_obj1402_Event_Timer;

/* index 0x1403 :   Receive PDO 4 Parameter. */
extern UNS32 FD_ObjDict_obj1403_COB_ID_used_by_PDO;
extern UNS8 FD_ObjDict_obj1403_Transmission_Type;
extern UNS16 FD_ObjDict_obj1403_Inhibit_Time;
//extern UNS8 FD_ObjDict_obj1403_Compatibility_Entry;
extern UNS16 FD_ObjDict_obj1403_Event_Timer;

/* index 0x1600 :   Receive PDO 1 Mapping. */
extern UNS32 FD_ObjDict_obj1600[];

/* index 0x1601 :   Receive PDO 2 Mapping. */
extern UNS32 FD_ObjDict_obj1601[];

/* index 0x1602 :   Receive PDO 3 Mapping. */
extern UNS32 FD_ObjDict_obj1602[];

/* index 0x1603 :   Receive PDO 4 Mapping. */
extern UNS32 FD_ObjDict_obj1603[];



/* index 0x1800 :   Transmit PDO 1 Parameter. */
extern UNS32 FD_ObjDict_obj1800_COB_ID_used_by_PDO;
extern UNS8 FD_ObjDict_obj1800_Transmission_Type;
extern UNS16 FD_ObjDict_obj1800_Inhibit_Time;
//extern UNS8 FD_ObjDict_obj1800_Compatibility_Entry;
extern UNS16 FD_ObjDict_obj1800_Event_Timer;

/* index 0x1801 :   Transmit PDO 2 Parameter. */
extern UNS32 FD_ObjDict_obj1801_COB_ID_used_by_PDO;
extern UNS8 FD_ObjDict_obj1801_Transmission_Type;
extern UNS16 FD_ObjDict_obj1801_Inhibit_Time;
//extern UNS8 FD_ObjDict_obj1801_Compatibility_Entry;
extern UNS16 FD_ObjDict_obj1801_Event_Timer;

/* index 0x1802 :   Transmit PDO 3 Parameter. */
extern UNS32 FD_ObjDict_obj1802_COB_ID_used_by_PDO;
extern UNS8 FD_ObjDict_obj1802_Transmission_Type;
extern UNS16 FD_ObjDict_obj1802_Inhibit_Time;
//extern UNS8 FD_ObjDict_obj1802_Compatibility_Entry;
extern UNS16 FD_ObjDict_obj1802_Event_Timer;

/* index 0x1803 :   Transmit PDO 4 Parameter. */
extern UNS32 FD_ObjDict_obj1803_COB_ID_used_by_PDO;
extern UNS8 FD_ObjDict_obj1803_Transmission_Type;
extern UNS16 FD_ObjDict_obj1803_Inhibit_Time;
//extern UNS8 FD_ObjDict_obj1803_Compatibility_Entry;
extern UNS16 FD_ObjDict_obj1803_Event_Timer;

/* index 0x1804 :   Transmit PDO 5 Parameter. */
extern UNS32 FD_ObjDict_obj1804_COB_ID_used_by_PDO;
extern UNS8 FD_ObjDict_obj1804_Transmission_Type;
extern UNS16 FD_ObjDict_obj1804_Inhibit_Time;
//extern UNS8 FD_ObjDict_obj1803_Compatibility_Entry;
extern UNS16 FD_ObjDict_obj1804_Event_Timer;
/* index 0x1A00 :   Transmit PDO 1 Mapping. */
extern UNS32 FD_ObjDict_obj1A00[];

/* index 0x1A01 :   Transmit PDO 2 Mapping. */
extern UNS32 FD_ObjDict_obj1A01[];

/* index 0x1A02 :   Transmit PDO 3 Mapping. */
extern UNS32 FD_ObjDict_obj1A02[];

/* index 0x1A03 :   Transmit PDO 4 Mapping. */
extern UNS32 FD_ObjDict_obj1A03[];





UNS32 CO_FLASH_Save_Parameters(CO_Data* d, UNS8 target);		//Save parameters to FLASH

void CO_FLASH_Load_Parameters(void);		//Load parameters from FLASH




#endif

