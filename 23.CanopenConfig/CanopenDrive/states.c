/*=====================================================================================
  This file is part of CANOpen Stack.

  Copyright (C): Fly-Lab Studio        
        
  Author: Horton Lu
===================================================================================== */

/*!
** @file   states.c
** @author Horton Lu
** @date   Tue Jun  5 09:32:32 2007
**
** @brief
**
**
*/
#include "can_STM32.h"	   //LED HW
#include "store_STM32.h"

#include "data.h"
#include "sysdep.h"
extern unsigned char CANOpenOffLineFlag;//离线标志

extern UNS8 HeartBeatCheckFlag;//心跳方式断线故障检测允许标志
extern UNS8 HeartBeatCheckFlag1;//翻转位方式断线故障检测允许标志
extern UNS32 HeartBeatCheckCnt;//心跳方式断线故障计数器
extern UNS32 HeartBeatStopCnt;	//心跳停止计数器
extern UNS32 NoSyncFrameCnt;
extern UNS32 FD_ObjDict_obj1016[]; /* number of subindex - 1*/
extern UNS16 FD_ObjDict_obj1016_L;
extern u16 CanLineOffCnt;

void StoreInit(CO_Data* d);

/** Prototypes for internals functions */
/*!                                                                                                
**                                                                                                 
**                                                                                                 
** @param d                                                                                        
** @param newCommunicationState                                                                    
**/     
void switchCommunicationState(CO_Data* d, 
	s_state_communication *newCommunicationState);
	
/*!                                                                                                
**                                                                                                 
**                                                                                                 
** @param d                                                                                        
**                                                                                                 
** @return                                                                                         
**/    
e_nodeState getState(CO_Data* d)
{
  return d->nodeState;
}

/*!                                                                                                
**                                                                                                 
**                                                                                                 
** @param d                                                                                        
** @param m                                                                                        
**/  
void canDispatch(CO_Data* d, Message *m)
{
	UNS16 cob_id = UNS16_LE(m->cob_id);
	 switch(cob_id >> 7)
	{
		case SYNC:		/* can be a SYNC or a EMCY message */
		//	CANOpenOffLineFlag = 0;
			if(cob_id == 0x080)	/* SYNC */
			{
				CanLineOffCnt = 0;
				NoSyncFrameCnt = 0;
				if(d->CurrentCommunicationState.csSYNC)
					proceedSYNC(d);
			} else 		/* EMCY */
				if(d->CurrentCommunicationState.csEmergency)
					proceedEMCY(d,m);
			break;
		/* case TIME_STAMP: */
		case PDO1tx:
		case PDO1rx:
		case PDO2tx:
		case PDO2rx:
		case PDO3tx:
		case PDO3rx:
		case PDO4tx:
		case PDO4rx:
//				if (d->CurrentCommunicationState.csPDO)
//				proceedPDO(d,m);
				if((cob_id&0x7f)==*d->bDeviceNodeId)
				{
					CANOpenOffLineFlag = 0;
					if (d->CurrentCommunicationState.csPDO)
					proceedPDO(d,m);
				}
				break;
		case SDOtx:
		case SDOrx:
//				if (d->CurrentCommunicationState.csSDO)
//				proceedSDO(d,m);
				if((cob_id&0x7f)==*d->bDeviceNodeId)
				{	
					if (d->CurrentCommunicationState.csSDO)
					proceedSDO(d,m);
		//			CANOpenOffLineFlag = 0;
				}
			break;
		case NODE_GUARD:
			if(cob_id == 0x701)
			{
				if((d->nodeState == 5) && (FD_ObjDict_obj1016_L))
				{	
					;
				}
			}	
			if (d->CurrentCommunicationState.csHeartbeat)
				proceedNODE_GUARD(d,m);
			break;
		case NMT:
			if (*(d->iam_a_slave))
			{
				proceedNMTstateChange(d,m);
			}
#ifdef CO_ENABLE_LSS
		case LSS:
			if (!d->CurrentCommunicationState.csLSS)break;
			if ((*(d->iam_a_slave)) && cob_id==MLSS_ADRESS)
			{
				proceedLSS_Slave(d,m);
			}
			else if(!(*(d->iam_a_slave)) && cob_id==SLSS_ADRESS)
			{
				proceedLSS_Master(d,m);
			}
			break;
#endif
	}
}

#define StartOrStop(CommType, FuncStart, FuncStop) \
	if(newCommunicationState->CommType && d->CurrentCommunicationState.CommType == 0){\
		MSG_WAR(0x9999,#FuncStart, 9999);\
		d->CurrentCommunicationState.CommType = 1;\
		FuncStart;\
	}else if(!newCommunicationState->CommType && d->CurrentCommunicationState.CommType == 1){\
		MSG_WAR(0x9999,#FuncStop, 9999);\
		d->CurrentCommunicationState.CommType = 0;\
		FuncStop;\
	}
#define None

/*!                                                                                                
**                                                                                                 
**                                                                                                 
** @param d                                                                                        
** @param newCommunicationState                                                                    
**/  	
void switchCommunicationState(CO_Data* d, s_state_communication *newCommunicationState)
{
#ifdef CO_ENABLE_LSS
	StartOrStop(csLSS,	startLSS(d),	stopLSS(d))
#endif
	StartOrStop(csSDO,	None,		resetSDO(d))
//	StartOrStop(csSDO,	StoreInit(d),		resetSDO(d))				//StoreInit(d),s.
	StartOrStop(csSYNC,	startSYNC(d),		stopSYNC(d))
	StartOrStop(csHeartbeat,	heartbeatInit(d),	heartbeatStop(d))
	StartOrStop(csEmergency,	emergencyInit(d),	emergencyStop(d)) 
	StartOrStop(csPDO,	PDOInit(d),	PDOStop(d))
	StartOrStop(csBoot_Up,	None,	slaveSendBootUp(d))
}

/*!                                                                                                
**                                                                                                 
**                                                                                                 
** @param d                                                                                        
** @param newState                                                                                 
**                                                                                                 
** @return                                                                                         
**/  
UNS8 setState(CO_Data* d, e_nodeState newState)
{
	if(newState != d->nodeState){
		switch( newState ){
			case Initialisation:
			{
				s_state_communication newCommunicationState = {1, 0, 0, 0, 0, 0, 0};
				d->nodeState = Initialisation;
				switchCommunicationState(d, &newCommunicationState);
				/* call user app init callback now. */
				/* d->initialisation MUST NOT CALL SetState */
				(*d->initialisation)(d);				
			}
			//break;   //not need
			/* Automatic transition - No break statement ! */

			/* Transition from Initialisation to Pre_operational */
			/* is automatic as defined in DS301. */
			/* App don't have to call SetState(d, Pre_operational) */
								
			case Pre_operational:
			{
				
				s_state_communication newCommunicationState = {0, 1, 1, 1, 1, 0, 1};
				d->nodeState = Pre_operational;
				switchCommunicationState(d, &newCommunicationState);
				if (!(*(d->iam_a_slave)))
				{
					masterSendNMTstateChange (d, 0, NMT_Reset_Node);
				}
                (*d->preOperational)(d);
			}
			break;
								
			case Operational:
			if(d->nodeState == Initialisation) return 0xFF;
			{
				s_state_communication newCommunicationState = {0, 1, 1, 1, 1, 1, 0};
				d->nodeState = Operational;
				newState = Operational;
				switchCommunicationState(d, &newCommunicationState);
				(*d->operational)(d);
			}
			break;
						
			case Stopped:
			if(d->nodeState == Initialisation) return 0xFF;
			{
				s_state_communication newCommunicationState = {0, 0, 0, 0, 1, 0, 1};
				d->nodeState = Stopped;
				newState = Stopped;
				switchCommunicationState(d, &newCommunicationState);
				(*d->stopped)(d);
			}
			break;
			default:
				return 0xFF;

		}/* end switch case */
	
	}
	/* d->nodeState contains the final state */
	/* may not be the requested state */
    return d->nodeState;  
}

/*!                                                                                                
**                                                                                                 
**                                                                                                 
** @param d                                                                                        
**                                                                                                 
** @return                                                                                         
**/ 
UNS8 getNodeId(CO_Data* d)
{
  return *d->bDeviceNodeId;
}

/*!                                                                                                
**                                                                                                 
**                                                                                                 
** @param d                                                                                        
** @param nodeId                                                                                   
**/   
void setNodeId(CO_Data* d, UNS8 nodeId)
{
  UNS16 offset = d->firstIndex->SDO_SVR;
  
#ifdef CO_ENABLE_LSS
  d->lss_transfer.nodeID=nodeId;
  if(nodeId==0xFF){
  	*d->bDeviceNodeId = nodeId;
  	return;
  }
  else
#endif
  if(!(nodeId>0 && nodeId<=127)){
	  MSG_WAR(0x2D01, "Invalid NodeID",nodeId);
	  return;
  }

  if(offset){
    /* Adjust COB-ID Client->Server (rx) only id already set to default value or id not valid (id==0xFF)*/
    if((*(UNS32*)d->objdict[offset].pSubindex[1].pObject == 0x600 + *d->bDeviceNodeId)||(*d->bDeviceNodeId==0xFF)){
      /* cob_id_client = 0x600 + nodeId; */
      *(UNS32*)d->objdict[offset].pSubindex[1].pObject = 0x600 + nodeId;
    }
    /* Adjust COB-ID Server -> Client (tx) only id already set to default value or id not valid (id==0xFF)*/
    if((*(UNS32*)d->objdict[offset].pSubindex[2].pObject == 0x580 + *d->bDeviceNodeId)||(*d->bDeviceNodeId==0xFF)){
      /* cob_id_server = 0x580 + nodeId; */
      *(UNS32*)d->objdict[offset].pSubindex[2].pObject = 0x580 + nodeId;
    }
  }

  /* 
   	Initialize the server(s) SDO parameters
  	Remember that only one SDO server is allowed, defined at index 0x1200	
 		
  	Initialize the client(s) SDO parameters 	
  	Nothing to initialize (no default values required by the DS 401)	
  	Initialize the receive PDO communication parameters. Only for 0x1400 to 0x1403 
  */
  {
    UNS8 i = 0;
    UNS16 offset = d->firstIndex->PDO_RCV;
    UNS16 lastIndex = d->lastIndex->PDO_RCV;
    UNS32 cobID[] = {0x200, 0x300, 0x400, 0x500};
    if( offset ) while( (offset <= lastIndex) && (i < 4)) {
      //if((*(UNS32*)d->objdict[offset].pSubindex[1].pObject == cobID[i] + *d->bDeviceNodeId)||(*d->bDeviceNodeId==0xFF))
	      *(UNS32*)d->objdict[offset].pSubindex[1].pObject = cobID[i] + nodeId;
      i ++;
      offset ++;
    }
  }
  /* ** Initialize the transmit PDO communication parameters. Only for 0x1800 to 0x1803 */
  {
    UNS8 i = 0;
    UNS16 offset = d->firstIndex->PDO_TRS;
    UNS16 lastIndex = d->lastIndex->PDO_TRS;
    //UNS32 cobID[] = {0x180, 0x280, 0x380, 0x480};
	UNS32 cobID[] = {0x180, 0x280, 0x380, 0x480,0x490};//20160603修改，增加充电器参数到TPDO5
    i = 0;
    //if( offset ) while ((offset <= lastIndex) && (i < 4)) {
	if( offset ) while ((offset <= lastIndex) && (i < 5)) {//20160603修改，增加充电器参数到TPDO5
	      *(UNS32*)d->objdict[offset].pSubindex[1].pObject = cobID[i] + nodeId;
      i ++;
      offset ++;
    }
  }

  /* Update EMCY COB-ID if already set to default*/
  if((*d->error_cobid == *d->bDeviceNodeId + 0x80)||(*d->bDeviceNodeId==0xFF))
    *d->error_cobid = nodeId + 0x80;

  /* bDeviceNodeId is defined in the object dictionary. */
  *d->bDeviceNodeId = nodeId;
}



//==============================================================================
void StoreInit(CO_Data* d)
{
RegisterSetODentryCallBack(d, 0x1010, 0x01, &Save_All_Parameters_callback);
RegisterSetODentryCallBack(d, 0x1010, 0x02, &Save_Communication_Parameters_callback);
RegisterSetODentryCallBack(d, 0x1010, 0x03, &Save_Application_Parameters_callback);
RegisterSetODentryCallBack(d, 0x1010, 0x04, &Save_Manufacturer_Parameters_callback);

//Init , Read All_Parameters from FLASH to OD	...
CO_FLASH_Load_Parameters();

}

//----------------------------------------Save Parameters callback-----------------------------------------------
UNS32 Save_All_Parameters_callback(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
 UNS32 temp;
 UNS32 errorCode;

 temp = *d->obj1010_Save_All_Parameters;

  if (temp!=0x65766173)		  // "save"
  {
   *d->obj1010_Save_All_Parameters = 0;
   return SDOABT_NOT_SAVE_ERROR;
  }

  //Save All_Parameters to FLASH ...
  errorCode = CO_FLASH_Save_Parameters(d,0x7);   //111
    if (errorCode != 0)
    return errorCode;

  *d->obj1010_Save_All_Parameters = 0;
  return 0;
}

UNS32 Save_Communication_Parameters_callback(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
 UNS32 temp;
 UNS32 errorCode;

 temp = *d->obj1010_Save_Communication_Parameters;
  if (temp!=0x65766173)		  // "save"
  {
   *d->obj1010_Save_Communication_Parameters = 0;
   return SDOABT_NOT_SAVE_ERROR;
  }
  //Save Communication_Parameters to FLASH ...
  errorCode = CO_FLASH_Save_Parameters(d,0x01);  //001
    if (errorCode != 0)
    return errorCode;

  *d->obj1010_Save_Communication_Parameters = 0;
  return 0;
}

UNS32 Save_Application_Parameters_callback(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
 UNS32 temp;
 UNS32 errorCode;

 temp = *d->obj1010_Save_Application_Parameters;
  if (temp!=0x65766173)		  // "save"
  {
   *d->obj1010_Save_Application_Parameters = 0;
   return SDOABT_NOT_SAVE_ERROR;
  }
  //Save Application_Parameters to FLASH ...
  errorCode = CO_FLASH_Save_Parameters(d,0x04);	//100
    if (errorCode != 0)
    return errorCode;

  *d->obj1010_Save_Application_Parameters = 0;
  return 0;
}

UNS32 Save_Manufacturer_Parameters_callback(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
 UNS32 temp;
 UNS32 errorCode;

 temp = *d->obj1010_Save_Manufacturer_Parameters;
  if (temp!=0x65766173)		  // "save"
  {
   *d->obj1010_Save_Manufacturer_Parameters = 0;
   return SDOABT_NOT_SAVE_ERROR;
  }
  //Save Manufacturer_Parameters to FLASH ...
  errorCode = CO_FLASH_Save_Parameters(d,0x02);	//010
    if (errorCode != 0)
    return errorCode;

  *d->obj1010_Save_Manufacturer_Parameters = 0;
  return 0;
}

//--------------------------------------------------------
void _initialisation(CO_Data* d)
{
//LED1_OFF();   //----------LED TEST
//LED2_OFF();   //----------LED TEST
//LED3_OFF();   //----------LED TEST
//LED1_ON();   //----------LED TEST
//LED_OFF();   //----------LED TEST
}

void _preOperational(CO_Data* d)
{
//LED_ON();   //----------LED TEST
//LED_OFF();   //----------LED TEST
}

void _operational(CO_Data* d)
{
//LED_ON();   //----------LED TEST
//LED2_OFF();   //----------LED TEST
}

void _stopped(CO_Data* d)
{
//LED_OFF();   //----------LED TEST
//LED1_ON();   //----------LED TEST
//LED2_ON();   //----------LED TEST
//LED3_ON();   //----------LED TEST
}


