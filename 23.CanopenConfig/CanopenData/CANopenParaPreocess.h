#ifndef  __CANOPENPARAPREOCESS_H__
#define  __CANOPENPARAPREOCESS_H__

#ifdef CANopen_GLOBALS 
    #define  CANopen_EXT 
#else 
    #define  CANopen_EXT   extern 
#endif

#define co_guard_time                FD_ObjDict_obj100C  // u16 CANopen Guard Time 守护时间	  Index:0x100C
#define co_life_time_factor          FD_ObjDict_obj100D  // u8 CANopen Life Time Factor 寿命保护因子   Index:0x100D
#define co_producer_heartbeat_time   FD_ObjDict_obj1017  // u16 CANopen Producer Heartbeat Time 心跳产生时间  Index:0x1017

//Receive PDO Communication Parameter
#define co_rpdo1_cob_id              FD_ObjDict_obj1400_COB_ID_used_by_PDO  // u32 CANopen RPDO1 Cob-ID 高16位
#define co_rpdo1_transmission_type   FD_ObjDict_obj1400_Transmission_Type   // u8  CANopen RPDO1 Transmission Type 传输类型
#define co_rpdo2_cob_id              FD_ObjDict_obj1401_COB_ID_used_by_PDO  // u32 CANopen RPDO2 Cob-ID 高16位
#define co_rpdo2_transmission_type   FD_ObjDict_obj1401_Transmission_Type   // u8  CANopen RPDO2 Transmission Type 传输类型
#define co_rpdo3_cob_id              FD_ObjDict_obj1402_COB_ID_used_by_PDO  // u32 CANopen RPDO3 Cob-ID 高16位
#define co_rpdo3_transmission_type   FD_ObjDict_obj1402_Transmission_Type   // u8  CANopen RPDO3 Transmission Type 传输类型
#define co_rpdo4_cob_id              FD_ObjDict_obj1403_COB_ID_used_by_PDO  // u32 CANopen RPDO4 Cob-ID 高16位
#define co_rpdo4_transmission_type   FD_ObjDict_obj1403_Transmission_Type   // u8  CANopen RPDO4 Transmission Type 传输类型

//Transmit PDO Communication Parameter
#define co_tpdo1_cob_id              FD_ObjDict_obj1800_COB_ID_used_by_PDO  // u32 CANopen TPDO1 Cob-ID 高16位
#define co_tpdo1_transmission_type   FD_ObjDict_obj1800_Transmission_Type   // u8  CANopen TPDO1 Transmission Type 传输类型
#define co_tpdo1_inhibit_time        FD_ObjDict_obj1800_Inhibit_Time        // u16 CANopen TPDO1 Inhibit Time 抑制时间
#define co_tpdo1_event_timer         FD_ObjDict_obj1800_Event_Timer         // u16 CANopen TPDO1 Event Timer 事件时间
#define co_tpdo2_cob_id              FD_ObjDict_obj1801_COB_ID_used_by_PDO  // u32 CANopen TPDO2 Cob-ID 高16位
#define co_tpdo2_transmission_type   FD_ObjDict_obj1801_Transmission_Type   // u8  CANopen TPDO2 Transmission Type 传输类型
#define co_tpdo2_inhibit_time        FD_ObjDict_obj1801_Inhibit_Time        // u16 CANopen TPDO2 Inhibit Time 抑制时间
#define co_tpdo2_event_timer         FD_ObjDict_obj1801_Event_Timer         // u16 CANopen TPDO2 Event Timer 事件时间
#define co_tpdo3_cob_id              FD_ObjDict_obj1802_COB_ID_used_by_PDO  // u32 CANopen TPDO3 Cob-ID 高16位
#define co_tpdo3_transmission_type   FD_ObjDict_obj1802_Transmission_Type   // u8  CANopen TPDO3 Transmission Type 传输类型
#define co_tpdo3_inhibit_time        FD_ObjDict_obj1802_Inhibit_Time        // u16 CANopen TPDO3 Inhibit Time 抑制时间
#define co_tpdo3_event_timer         FD_ObjDict_obj1802_Event_Timer         // u16 CANopen TPDO3 Event Timer 事件时间
#define co_tpdo4_cob_id              FD_ObjDict_obj1803_COB_ID_used_by_PDO  // u32 CANopen TPDO4 Cob-ID 高16位
#define co_tpdo4_transmission_type   FD_ObjDict_obj1803_Transmission_Type   // u8  CANopen TPDO4 Transmission Type 传输类型
#define co_tpdo4_inhibit_time        FD_ObjDict_obj1803_Inhibit_Time        // u16 CANopen TPDO4 Inhibit Time 抑制时间
#define co_tpdo4_event_timer         FD_ObjDict_obj1803_Event_Timer         // u16 CANopen TPDO4 Event Timer 事件时间

typedef struct {
    u16		Index;	     	// CANopen参数对应于系统参数的索引号
    u16*	pCANopenData;	// 指向CANopen参数的指针
}CANopenParaType;

CANopen_EXT u8  ExCANopen_RxBuf[EXUART_TX_MAX_FRAMELEN];
CANopen_EXT u8  ExCANopenRxFullFlag;   						// 模拟ARM接收CANopen一帧完成标志
CANopen_EXT u16 SSI_LinkFlag; 								/* SSI连接标志：1连接 */
CANopen_EXT u16 PitchPLCState; 								/* 驱动器状态机状态 */
CANopen_EXT s32 UserTargetPosition; 						// 用户32位位置给定

extern bool CANopenErrFlag;									//  CANopen通讯故障标志：1故障;

void CANopenParaInit(void);
void CANopen_Para_Precoss(void);  

#endif
