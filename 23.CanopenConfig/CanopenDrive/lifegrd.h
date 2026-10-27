/*=====================================================================================
  This file is part of CANOpen Stack.

  Copyright (C): Fly-Lab Studio        
        
  Author: Horton Lu
===================================================================================== */

/** @defgroup heartbeato Heartbeat Object
 *  The heartbeat mechanism for a device is established through cyclically transmitting a message by a
 *	heartbeat producer. One or more devices in the network are aware of this heartbeat message. If the
 *	heartbeat cycle fails for the heartbeat producer the local application on the heartbeat consumer will be
 *	informed about that event.
 *  @ingroup comobj
 */
					 
#ifndef __lifegrd_h__
#define __lifegrd_h__


#include <applicfg.h>

typedef void (*heartbeatError_t)(CO_Data*, UNS8);
void _heartbeatError(CO_Data* d, UNS8 heartbeatID);

typedef void (*post_SlaveBootup_t)(CO_Data*, UNS8);
void _post_SlaveBootup(CO_Data* d, UNS8 SlaveID);

#include "data.h"

/*************************************************************************
 * Functions
 *************************************************************************/

/** 
 * @ingroup statemachine
 * @brief To read the state of a node
 * This can be used by the master after having sent a life guard request,
 * of by any node if it is waiting for heartbeat.
 * @param *d Pointer on a CAN object data structure
 * @param nodeId Id of a node
 * @return e_nodeState State of the node corresponding to the nodeId
 */
e_nodeState getNodeState (CO_Data* d, UNS8 nodeId);

/** 
 * @brief Start heartbeat consumer and producer
 * with respect to 0x1016 and 0x1017
 * object dictionary entries
 * @param *d Pointer on a CAN object data structure
 */
void heartbeatInit(CO_Data* d);

/** 
 * @brief Stop heartbeat consumer and producer
 * @param *d Pointer on a CAN object data structure
 */
void heartbeatStop(CO_Data* d);

/** 
 * @brief This function is responsible to process a canopen-message which seams to be an NMT Error Control
 * Messages. At them moment we assume that every NMT error control message
 * is a heartbeat message.
 * If a BootUp message is detected, it will return the nodeId of the Slave who booted up
 * @param *d Pointer on a CAN object data structure 
 * @param *m Pointer on the CAN-message which has to be analysed.
 */
void proceedNODE_GUARD (CO_Data* d, Message* m);

#endif /*__lifegrd_h__ */
