/*=====================================================================================
  This file is part of CANOpen Stack.

  Copyright (C): Fly-Lab Studio        
        
  Author: Horton Lu
===================================================================================== */

/*!
** @file   lifegrd.c
** @author Horton Lu
** @date   Mon Jun  4 17:19:24 2010
**
** @brief
**
**
*/

#include "data.h"
#include "lifegrd.h"
#include "CANopen.h"
#include "dcf.h"
#include "sysdep.h"

extern UNS16 UserPara8;		/* Mapped at index 0x2108, subindex 0x00 */
extern UNS16 UserPara9;		/* Mapped at index 0x2109, subindex 0x00 */
extern UNS16 UserPara10;		/* Mapped at index 0x210A, subindex 0x00 */


void ConsumerHearbeatAlarm(CO_Data* d, UNS32 id);


void ProducerHearbeatAlarm(CO_Data* d, UNS32 id);

void LifeGuardAlarm(CO_Data* d, UNS32 id);	  //LifeGuardAlarm
UNS8 CANopen_LifeGuard_TimeOut; //CANopen寿命保护超时标志
// UNS8 CANopen_LifeGuard_TimeOut = 1; //【上电报断线
UNS32 OnHearbeatProducerUpdate(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex);

/*!
**
**
** @param d
** @param nodeId
**
** @return
**/
e_nodeState getNodeState (CO_Data* d, UNS8 nodeId)
{
  e_nodeState networkNodeState = d->NMTable[nodeId];
  return networkNodeState;
}

/*! 
** The Consumer Timer Callback
**
** @param d
** @param id
**/
void ConsumerHearbeatAlarm(CO_Data* d, UNS32 id)
{
  /*MSG_WAR(0x00, "ConsumerHearbeatAlarm", 0x00);*/

  /* timer have been notified and is now free (non periodic)*/
  /* -> avoid deleting re-assigned timer if message is received too late*/
  d->ConsumerHeartBeatTimers[id]=TIMER_NONE;
  /*! call heartbeat error with NodeId */
  (*d->heartbeatError)(d, (UNS8)( ((d->ConsumerHeartbeatEntries[id]) & (UNS32)0x00FF0000) >> (UNS8)16 ));
}

/*!
**
**
** @param d
** @param m
**/
void proceedNODE_GUARD(CO_Data* d, Message* m )
{
  UNS8 nodeId = (UNS8) GET_NODE_ID((*m));
  TIMEVAL life_guard_time;

  if((m->rtr == 1) )
    /*!
    ** Notice that only the master can have sent this
    ** node guarding request
    */
    {
      /*!
      ** Receiving a NMT NodeGuarding (request of the state by the
      ** master)
      ** Only answer to the NMT NodeGuarding request, the master is
      ** not checked (not implemented)
      */
      if (nodeId == *d->bDeviceNodeId )
        {
          Message msg;
          UNS16 tmp = *d->bDeviceNodeId + 0x700;
          msg.cob_id = UNS16_LE(tmp);
          msg.len = (UNS8)0x01;
          msg.rtr = 0;
          msg.data[0] = d->nodeState;
          if (d->toggle)
            {
              msg.data[0] |= 0x80 ;
              d->toggle = 0 ;
            }
          else
            d->toggle = 1 ;
          /* send the nodeguard response. */
          MSG_WAR(0x3130, "Sending NMT Nodeguard to master, state: ", d->nodeState);
          canSend(d->canHandle,&msg );


		 /* Renew alarm for next LifeGuard. */
		   if ( *d->GuardTime && *d->LifeTimeFactor)
            {
              CANopen_LifeGuard_TimeOut=0; //Clear LifeGuard_TimeOut Flag
			  DelAlarm(d->LifeGuardTimer);
			  life_guard_time = (*d->GuardTime)*(*d->LifeTimeFactor);
              d->LifeGuardTimer = SetAlarm(d, 0, &LifeGuardAlarm, MS_TO_TIMEVAL(life_guard_time), 0);            
			}

        }

    }else{ /* Not a request CAN */

      MSG_WAR(0x3110, "Received NMT nodeId : ", nodeId);
      /* the slave's state receievd is stored in the NMTable */
      /* The state is stored on 7 bit */
      d->NMTable[nodeId] = (e_nodeState) ((*m).data[0] & 0x7F) ;

      /* Boot-Up frame reception */
      if ( d->NMTable[nodeId] == Initialisation)
        {
          /*
          ** The device send the boot-up message (Initialisation)
          ** to indicate the master that it is entered in
          ** pre_operational mode
          ** Because the  device enter automaticaly in pre_operational
          ** mode,
          ** the pre_operational mode is stored
          ** NMTable[bus_id][nodeId] = Pre_operational
          */
          MSG_WAR(0x3100, "The NMT is a bootup from node : ", nodeId);
          
          if(!send_consise_dcf(d,nodeId)){
             /* call post SlaveBootup with NodeId */
		  	(*d->post_SlaveBootup)(d, nodeId);
          }
        }

      if( d->NMTable[nodeId] != Unknown_state ) {
        UNS8 index, ConsummerHeartBeat_nodeId ;
        for( index = (UNS8)0x00; index < *d->ConsumerHeartbeatCount; index++ )
          {
            ConsummerHeartBeat_nodeId = (UNS8)( ((d->ConsumerHeartbeatEntries[index]) & (UNS32)0x00FF0000) >> (UNS8)16 );
            if ( nodeId == ConsummerHeartBeat_nodeId )
              {
                TIMEVAL time = ( (d->ConsumerHeartbeatEntries[index]) & (UNS32)0x0000FFFF ) ;
                /* Renew alarm for next heartbeat. */
                DelAlarm(d->ConsumerHeartBeatTimers[index]);
                d->ConsumerHeartBeatTimers[index] = SetAlarm(d, index, &ConsumerHearbeatAlarm, MS_TO_TIMEVAL(time), 0);
              }
          }
      }
    }
}

/*! The Consumer Timer Callback
**
**
** @param d
** @param id
**/
void ProducerHearbeatAlarm(CO_Data* d, UNS32 id)
{
  if(*d->ProducerHeartBeatTime)
    {
      Message msg;
      /* Time expired, the heartbeat must be sent immediately
      ** generate the correct node-id: this is done by the offset 1792
      ** (decimal) and additionaly
      ** the node-id of this device.
      */
      UNS16 tmp = *d->bDeviceNodeId + 0x700;
      msg.cob_id = UNS16_LE(tmp);
      msg.len = (UNS8)0x01;
      msg.rtr = 0;
      msg.data[0] = d->nodeState; /* No toggle for heartbeat !*/
      /* send the heartbeat */
      MSG_WAR(0x3130, "Producing heartbeat: ", d->nodeState);
      canSend(d->canHandle,&msg );

    }else{
      d->ProducerHeartBeatTimer = DelAlarm(d->ProducerHeartBeatTimer);
    }
}

/*! This is called when Index 0x1017 is updated.
**
**
** @param d
** @param unsused_indextable
** @param unsused_bSubindex
**
** @return
**/
UNS32 OnHeartbeatProducerUpdate(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
  heartbeatStop(d);
  heartbeatInit(d);
  return 0;
}

/*!
**
**
** @param d
**/
void heartbeatInit(CO_Data* d)
{

  UNS8 index; /* Index to scan the table of heartbeat consumers */
  RegisterSetODentryCallBack(d, 0x1017, 0x00, &OnHeartbeatProducerUpdate);

  d->toggle = 0;

  for( index = (UNS8)0x00; index < *d->ConsumerHeartbeatCount; index++ )
    {
      TIMEVAL time = (UNS16) ( (d->ConsumerHeartbeatEntries[index]) & (UNS32)0x0000FFFF ) ;
      /* MSG_WAR(0x3121, "should_time : ", should_time ) ; */
      if ( time )
        {
          d->ConsumerHeartBeatTimers[index] = SetAlarm(d, index, &ConsumerHearbeatAlarm, MS_TO_TIMEVAL(time), 0);
        }
    }

  if ( *d->ProducerHeartBeatTime )
    {
      TIMEVAL time = *d->ProducerHeartBeatTime;
      d->ProducerHeartBeatTimer = SetAlarm(d, 0, &ProducerHearbeatAlarm, MS_TO_TIMEVAL(time), MS_TO_TIMEVAL(time));
    }
}

/*!
**
**
** @param d
**/
void heartbeatStop(CO_Data* d)
{
  UNS8 index;
  for( index = (UNS8)0x00; index < *d->ConsumerHeartbeatCount; index++ )
    {
      d->ConsumerHeartBeatTimers[index] = DelAlarm(d->ConsumerHeartBeatTimers[index]);
    }

  d->ProducerHeartBeatTimer = DelAlarm(d->ProducerHeartBeatTimer);
}

/*!
**
**
** @param heartbeatID
**/
void _heartbeatError(CO_Data* d, UNS8 heartbeatID){}
void _post_SlaveBootup(CO_Data* d, UNS8 SlaveID){}
void LifeGuardAlarm(CO_Data* d, UNS32 id)
{
 MSG_WAR(0x00, "LifeGuardAlarm", 0x00);
 CANopen_LifeGuard_TimeOut=1;
//  UserPara8 |= 0x0004;//【翻转位类型导致的断线置位UserPara8的Bit0，系统心跳类型导致的断线置位UserPara8的Bit1，节点守护类型的断线置位UserPara8的Bit2
}


