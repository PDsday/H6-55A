/*=====================================================================================
  This file is part of CANOpen Stack.

  Copyright (C): Fly-Lab Studio        
        
  Author: Horton Lu
===================================================================================== */


// STM32 implementation of the  CANopen timer driver, uses Timer 2 (16 bit)

// Includes for the CANopen driver

// #include "stm32f10x_lib.h"
#include "stm32f4xx_conf.h"
#include "store_STM32.h"

#include "CANopen.h"
#include "applicfg.h"
#include "data.h"


#define PageSize    ((u16)0x20000)		 // ((u16)0x400)	,x C/D/E , 2k Byte/Page	!128K 存储区变大 20170510
#define StartAddr   ((u32)0x08060000)	 //CANopen标准保存区，起始地址	,201K<第7个128K
#define StartAddr1  (StartAddr+(PageSize*0))		//Communication_Parameters	OD: 1000~1fff	   ,1Page
//#define StartAddr2  (StartAddr+(PageSize*1))		//Manufacturer_Parameters	OD: 2000~5fff	   ,1Page
//#define StartAddr3  (StartAddr+(PageSize*2))	    //Application_Parameters	OD: 6000~9fff	   ,1Page
//#define StartAddr1  ((u32)0x0801E800)		//Communication_Parameters	OD: 1000~1fff	   ,122K<
//#define StartAddr2  ((u32)0x0801F000)		//Manufacturer_Parameters	OD: 2000~5fff	   ,124K<
//#define StartAddr3  ((u32)0x0801F800)		//Application_Parameters	OD: 6000~9fff	   ,126K<

#define ChangedFlag_Addr  (StartAddr1+(4*500))		//Word  12




//==================================Save_Parameters to FLASH========================================================
UNS32 CO_FLASH_Save_Parameters(CO_Data* d, UNS8 target)		 //target  100:6000~9fff ;   010:2000~5fff;   001:1000~1fff;  111:All
{
 UNS32 errorCode;
 volatile FLASH_Status FLASHStatus;

 UNS8 currentNodeId;


 FLASHStatus = FLASH_COMPLETE;

  if(target & 0x01)	  //Save Communication_Parameters, 001:1000~1fff;	&NodeID  &BaudRate
  {
   FLASH_Unlock();
//    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
		  FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR |FLASH_FLAG_PGSERR | FLASH_FLAG_WRPERR);	//f405 m4 
//   FLASHStatus = FLASH_ErasePage(StartAddr1); //ErasePage
		  FLASHStatus = FLASH_EraseSector(GetSector(StartAddr1 ),VoltageRange_3);
   while(FLASHStatus != FLASH_COMPLETE);

   FLASHStatus = FLASH_ProgramWord(ChangedFlag_Addr, 12); //----- Changed Flag-------
   while(FLASHStatus != FLASH_COMPLETE);


   currentNodeId = getNodeId(d);	//-----NodeID
   FLASHStatus = FLASH_ProgramHalfWord(StartAddr1, (UNS16)currentNodeId);
   while(FLASHStatus != FLASH_COMPLETE);
   //StartAddr1 = StartAddr1 + 2;

   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*1), FD_ObjDict_obj1006); /* index 0x1006 :   Communication / Cycle Period. */
   while(FLASHStatus != FLASH_COMPLETE);
//    FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*2), FD_ObjDict_obj100C); /* index 0x100C :   Guard Time. */
//    while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*3), FD_ObjDict_obj100D); /* index 0x100D :   Life Time Factor. */
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*4), FD_ObjDict_obj1017); /* index 0x1017 :   Producer Heartbeat Time. */
   while(FLASHStatus != FLASH_COMPLETE);

   /* index 0x1400 :   Receive PDO 1 Parameter. */
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*5), FD_ObjDict_obj1400_COB_ID_used_by_PDO); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*6), FD_ObjDict_obj1400_Transmission_Type); 
   while(FLASHStatus != FLASH_COMPLETE);
//    FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*7), FD_ObjDict_obj1400_Inhibit_Time); 
//    while(FLASHStatus != FLASH_COMPLETE);
//    FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*8), FD_ObjDict_obj1400_Event_Timer); 
//    while(FLASHStatus != FLASH_COMPLETE);

   /* index 0x1401 :   Receive PDO 2 Parameter. */
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*9), FD_ObjDict_obj1401_COB_ID_used_by_PDO); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*10), FD_ObjDict_obj1401_Transmission_Type); 
    while(FLASHStatus != FLASH_COMPLETE);
//    FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*11), FD_ObjDict_obj1401_Inhibit_Time); 
//    while(FLASHStatus != FLASH_COMPLETE);
//    FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*12), FD_ObjDict_obj1401_Event_Timer); 
//    while(FLASHStatus != FLASH_COMPLETE);

   /* index 0x1402 :   Receive PDO 3 Parameter. */
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*13), FD_ObjDict_obj1402_COB_ID_used_by_PDO); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*14), FD_ObjDict_obj1402_Transmission_Type); 
   while(FLASHStatus != FLASH_COMPLETE);
//    FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*15), FD_ObjDict_obj1402_Inhibit_Time); 
//    while(FLASHStatus != FLASH_COMPLETE);
//    FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*16), FD_ObjDict_obj1402_Event_Timer); 
//    while(FLASHStatus != FLASH_COMPLETE);

   /* index 0x1403 :   Receive PDO 4 Parameter. */
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*17), FD_ObjDict_obj1403_COB_ID_used_by_PDO); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*18), FD_ObjDict_obj1403_Transmission_Type); 
   while(FLASHStatus != FLASH_COMPLETE);
//    FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*19), FD_ObjDict_obj1403_Inhibit_Time); 
//    while(FLASHStatus != FLASH_COMPLETE);
//    FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*20), FD_ObjDict_obj1403_Event_Timer); 
//    while(FLASHStatus != FLASH_COMPLETE);

   /* index 0x1600 :   Receive PDO 1 Mapping. */
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*21), FD_ObjDict_obj1600[0]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*22), FD_ObjDict_obj1600[1]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*23), FD_ObjDict_obj1600[2]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*24), FD_ObjDict_obj1600[3]); 
   while(FLASHStatus != FLASH_COMPLETE);

   /* index 0x1601 :   Receive PDO 2 Mapping. */
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*25), FD_ObjDict_obj1601[0]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*26), FD_ObjDict_obj1601[1]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*27), FD_ObjDict_obj1601[2]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*28), FD_ObjDict_obj1601[3]); 
   while(FLASHStatus != FLASH_COMPLETE);

   /* index 0x1602 :   Receive PDO 3 Mapping. */
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*29), FD_ObjDict_obj1602[0]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*30), FD_ObjDict_obj1602[1]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*31), FD_ObjDict_obj1602[2]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*32), FD_ObjDict_obj1602[3]); 
   while(FLASHStatus != FLASH_COMPLETE);

   /* index 0x1603 :   Receive PDO 4 Mapping. */
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*33), FD_ObjDict_obj1603[0]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*34), FD_ObjDict_obj1603[1]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*35), FD_ObjDict_obj1603[2]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*36), FD_ObjDict_obj1603[3]); 
   while(FLASHStatus != FLASH_COMPLETE);








   /* index 0x1800 :   Transmit PDO 1 Parameter. */
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*37), FD_ObjDict_obj1800_COB_ID_used_by_PDO); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*38), FD_ObjDict_obj1800_Transmission_Type); 
   while(FLASHStatus != FLASH_COMPLETE);
    FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*39), FD_ObjDict_obj1800_Inhibit_Time); 
    while(FLASHStatus != FLASH_COMPLETE);
    FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*40), FD_ObjDict_obj1800_Event_Timer); 
    while(FLASHStatus != FLASH_COMPLETE);

   /* index 0x1801 :   Transmit PDO 2 Parameter. */
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*41), FD_ObjDict_obj1801_COB_ID_used_by_PDO); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*42), FD_ObjDict_obj1801_Transmission_Type); 
   while(FLASHStatus != FLASH_COMPLETE);
    FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*43), FD_ObjDict_obj1801_Inhibit_Time); 
    while(FLASHStatus != FLASH_COMPLETE);
    FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*44), FD_ObjDict_obj1801_Event_Timer); 
    while(FLASHStatus != FLASH_COMPLETE);

   /* index 0x1802 :   Transmit PDO 3 Parameter. */
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*45), FD_ObjDict_obj1802_COB_ID_used_by_PDO); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*46), FD_ObjDict_obj1802_Transmission_Type); 
   while(FLASHStatus != FLASH_COMPLETE);
    FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*47), FD_ObjDict_obj1802_Inhibit_Time); 
    while(FLASHStatus != FLASH_COMPLETE);
    FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*48), FD_ObjDict_obj1802_Event_Timer); 
    while(FLASHStatus != FLASH_COMPLETE);

   /* index 0x1803 :   Transmit PDO 4 Parameter. */
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*49), FD_ObjDict_obj1803_COB_ID_used_by_PDO); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*50), FD_ObjDict_obj1803_Transmission_Type); 
   while(FLASHStatus != FLASH_COMPLETE);
    FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*51), FD_ObjDict_obj1803_Inhibit_Time); 
    while(FLASHStatus != FLASH_COMPLETE);
    FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*52), FD_ObjDict_obj1803_Event_Timer); 
    while(FLASHStatus != FLASH_COMPLETE);

   /* index 0x1A00 :   Transmit PDO 1 Mapping. */
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*53), FD_ObjDict_obj1A00[0]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*54), FD_ObjDict_obj1A00[1]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*55), FD_ObjDict_obj1A00[2]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*56), FD_ObjDict_obj1A00[3]); 
   while(FLASHStatus != FLASH_COMPLETE);

   /* index 0x1A01 :   Transmit PDO 2 Mapping. */
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*57), FD_ObjDict_obj1A01[0]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*58), FD_ObjDict_obj1A01[1]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*59), FD_ObjDict_obj1A01[2]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*60), FD_ObjDict_obj1A01[3]); 
   while(FLASHStatus != FLASH_COMPLETE);

   /* index 0x1A02 :   Transmit PDO 3 Mapping. */
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*61), FD_ObjDict_obj1A02[0]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*62), FD_ObjDict_obj1A02[1]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*63), FD_ObjDict_obj1A02[2]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*64), FD_ObjDict_obj1A02[3]); 
   while(FLASHStatus != FLASH_COMPLETE);

   /* index 0x1A03 :   Transmit PDO 4 Mapping. */
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*65), FD_ObjDict_obj1A03[0]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*66), FD_ObjDict_obj1A03[1]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*67), FD_ObjDict_obj1A03[2]); 
   while(FLASHStatus != FLASH_COMPLETE);
   FLASHStatus = FLASH_ProgramWord(StartAddr1+(4*68), FD_ObjDict_obj1A03[3]); 
   while(FLASHStatus != FLASH_COMPLETE);





    /* 锁定FLASH编写擦除控制器 */
    FLASH_Lock();

  }


  if(target & 0x02)	  //Save Manufacturer_Parameters, 010:2000~5fff;
  {

  }

  if(target & 0x07)	  //Save Application_Parameters, 100:6000~9fff;
  {

  }

errorCode=0;
return errorCode;

}




//---------Load parameters from FLASH--------
void CO_FLASH_Load_Parameters(void)
{
// u16* Addr_u16;
// u32* Addr_u32;

if(*(u32*)(ChangedFlag_Addr)==12)	 //--Changed Flag---  曾经SAVE过才加载，否则字典默认值
{

  FD_ObjDict_obj1006 = *(u32*)(StartAddr1+(4*1)); /* index 0x1006 :   Communication / Cycle Period. */
//  FD_ObjDict_obj100C = *(u32*)(StartAddr1+(4*2)); /* index 0x100C :   Guard Time. */
 FD_ObjDict_obj100D = *(u32*)(StartAddr1+(4*3)); /* index 0x100D :   Life Time Factor. */
 FD_ObjDict_obj1017 = *(u32*)(StartAddr1+(4*4)); /* index 0x1017 :   Producer Heartbeat Time. */

  /* index 0x1400 :   Receive PDO 1 Parameter. */
  FD_ObjDict_obj1400_COB_ID_used_by_PDO = *(u32*)(StartAddr1+(4*5));
  FD_ObjDict_obj1400_Transmission_Type = *(u32*)(StartAddr1+(4*6));
//   FD_ObjDict_obj1400_Inhibit_Time = *(u32*)(StartAddr1+(4*7));
//   FD_ObjDict_obj1400_Event_Timer = *(u32*)(StartAddr1+(4*8));

  /* index 0x1401 :   Receive PDO 2 Parameter. */
  FD_ObjDict_obj1401_COB_ID_used_by_PDO = *(u32*)(StartAddr1+(4*9));
  FD_ObjDict_obj1401_Transmission_Type = *(u32*)(StartAddr1+(4*10));
//   FD_ObjDict_obj1401_Inhibit_Time = *(u32*)(StartAddr1+(4*11));
//   FD_ObjDict_obj1401_Event_Timer = *(u32*)(StartAddr1+(4*12));

  /* index 0x1402 :   Receive PDO 3 Parameter. */
  FD_ObjDict_obj1402_COB_ID_used_by_PDO = *(u32*)(StartAddr1+(4*13));
  FD_ObjDict_obj1402_Transmission_Type = *(u32*)(StartAddr1+(4*14));
//   FD_ObjDict_obj1402_Inhibit_Time = *(u32*)(StartAddr1+(4*15));
//   FD_ObjDict_obj1402_Event_Timer = *(u32*)(StartAddr1+(4*16));

  /* index 0x1403 :   Receive PDO 4 Parameter. */
  FD_ObjDict_obj1403_COB_ID_used_by_PDO = *(u32*)(StartAddr1+(4*17));
  FD_ObjDict_obj1403_Transmission_Type = *(u32*)(StartAddr1+(4*18));
//   FD_ObjDict_obj1403_Inhibit_Time = *(u32*)(StartAddr1+(4*19));
//   FD_ObjDict_obj1403_Event_Timer = *(u32*)(StartAddr1+(4*20));

  /* index 0x1600 :   Receive PDO 1 Mapping. */
  FD_ObjDict_obj1600[0] = *(u32*)(StartAddr1+(4*21));
  FD_ObjDict_obj1600[1] = *(u32*)(StartAddr1+(4*22));
  FD_ObjDict_obj1600[2] = *(u32*)(StartAddr1+(4*23));
  FD_ObjDict_obj1600[3] = *(u32*)(StartAddr1+(4*24));

  /* index 0x1601 :   Receive PDO 2 Mapping. */
  FD_ObjDict_obj1601[0] = *(u32*)(StartAddr1+(4*25));
  FD_ObjDict_obj1601[1] = *(u32*)(StartAddr1+(4*26));
  FD_ObjDict_obj1601[2] = *(u32*)(StartAddr1+(4*27));
  FD_ObjDict_obj1601[3] = *(u32*)(StartAddr1+(4*28));

  /* index 0x1602 :   Receive PDO 3 Mapping. */
  FD_ObjDict_obj1602[0] = *(u32*)(StartAddr1+(4*29));
  FD_ObjDict_obj1602[1] = *(u32*)(StartAddr1+(4*30));
  FD_ObjDict_obj1602[2] = *(u32*)(StartAddr1+(4*31));
  FD_ObjDict_obj1602[3] = *(u32*)(StartAddr1+(4*32));

  /* index 0x1603 :   Receive PDO 4 Mapping. */
  FD_ObjDict_obj1603[0] = *(u32*)(StartAddr1+(4*33));
  FD_ObjDict_obj1603[1] = *(u32*)(StartAddr1+(4*34));
  FD_ObjDict_obj1603[2] = *(u32*)(StartAddr1+(4*35));
  FD_ObjDict_obj1603[3] = *(u32*)(StartAddr1+(4*36));





  /* index 0x1800 :   Transmit PDO 1 Parameter. */
  FD_ObjDict_obj1800_COB_ID_used_by_PDO = *(u32*)(StartAddr1+(4*37));
  FD_ObjDict_obj1800_Transmission_Type = *(u32*)(StartAddr1+(4*38));
   FD_ObjDict_obj1800_Inhibit_Time = *(u32*)(StartAddr1+(4*39));
   FD_ObjDict_obj1800_Event_Timer = *(u32*)(StartAddr1+(4*40));

  /* index 0x1801 :   Transmit PDO 2 Parameter. */
  FD_ObjDict_obj1801_COB_ID_used_by_PDO = *(u32*)(StartAddr1+(4*41));
  FD_ObjDict_obj1801_Transmission_Type = *(u32*)(StartAddr1+(4*42));
   FD_ObjDict_obj1801_Inhibit_Time = *(u32*)(StartAddr1+(4*43));
   FD_ObjDict_obj1801_Event_Timer = *(u32*)(StartAddr1+(4*44));

  /* index 0x1802 :   Transmit PDO 3 Parameter. */
  FD_ObjDict_obj1802_COB_ID_used_by_PDO = *(u32*)(StartAddr1+(4*45));
  FD_ObjDict_obj1802_Transmission_Type = *(u32*)(StartAddr1+(4*46));
   FD_ObjDict_obj1802_Inhibit_Time = *(u32*)(StartAddr1+(4*47));
   FD_ObjDict_obj1802_Event_Timer = *(u32*)(StartAddr1+(4*48));

  /* index 0x1803 :   Transmit PDO 4 Parameter. */
  FD_ObjDict_obj1803_COB_ID_used_by_PDO = *(u32*)(StartAddr1+(4*49));
  FD_ObjDict_obj1803_Transmission_Type = *(u32*)(StartAddr1+(4*50));
   FD_ObjDict_obj1803_Inhibit_Time = *(u32*)(StartAddr1+(4*51));
   FD_ObjDict_obj1803_Event_Timer = *(u32*)(StartAddr1+(4*52));


  /* index 0x1A00 :   Transmit PDO 1 Mapping. */
  FD_ObjDict_obj1A00[0] = *(u32*)(StartAddr1+(4*53));
  FD_ObjDict_obj1A00[1] = *(u32*)(StartAddr1+(4*54));
  FD_ObjDict_obj1A00[2] = *(u32*)(StartAddr1+(4*55));
  FD_ObjDict_obj1A00[3] = *(u32*)(StartAddr1+(4*56));

  /* index 0x1A01 :   Transmit PDO 2 Mapping. */
  FD_ObjDict_obj1A01[0] = *(u32*)(StartAddr1+(4*57));
  FD_ObjDict_obj1A01[1] = *(u32*)(StartAddr1+(4*58));
  FD_ObjDict_obj1A01[2] = *(u32*)(StartAddr1+(4*59));
  FD_ObjDict_obj1A01[3] = *(u32*)(StartAddr1+(4*60));

  /* index 0x1A02 :   Transmit PDO 3 Mapping. */
  FD_ObjDict_obj1A02[0] = *(u32*)(StartAddr1+(4*61));
  FD_ObjDict_obj1A02[1] = *(u32*)(StartAddr1+(4*62));
  FD_ObjDict_obj1A02[2] = *(u32*)(StartAddr1+(4*63));
  FD_ObjDict_obj1A02[3] = *(u32*)(StartAddr1+(4*64));

  /* index 0x1A03 :   Transmit PDO 4 Mapping. */
  FD_ObjDict_obj1A03[0] = *(u32*)(StartAddr1+(4*65));
  FD_ObjDict_obj1A03[1] = *(u32*)(StartAddr1+(4*66));
  FD_ObjDict_obj1A03[2] = *(u32*)(StartAddr1+(4*67));
  FD_ObjDict_obj1A03[3] = *(u32*)(StartAddr1+(4*68));


}


}
//---------------------------------------------------------------












