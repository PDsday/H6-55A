/*=====================================================================================
  This file is part of CANOpen Stack.

  Copyright (C): Fly-Lab Studio        
        
  Author: Horton Lu
===================================================================================== */

/** 
 * @brief Send the consise dcf to node corresping to nodeId
 * @param *d Pointer on a CAN object data structure
 * @param nodeId Id of the slave node
 * @return
 */
UNS8 send_consise_dcf(CO_Data* d, UNS8 nodeId);
