#ifndef				_COM_MAP_H
#define				_COM_MAP_H

#define		ZIGBEE_SEND_BYTES	100
#define		UP_COMMU_DEV_ARRAY	8

#define GPRS_REC_QUEUES_LENGTH				1000000
#define USART3_REC_QUEUE_AT_LENGTH   		1000000
#define USART3_REC_QUEUE_ATIPD_LENGTH   	1000000
//=============上行通信设备ID ，这些常量的值直接作为全局数组的下标使用，因此不能随便修改
#define     UP_COMMU_DEV_ZIGBEE	0
#define	 UP_COMMU_DEV_485		1
#define	 UP_COMMU_DEV_232		2
#define	 UP_COMMU_DEV_GPRS   3
#define    UP_COMMU_DEV_AT		    4
#define    UP_COMMU_DEV_ATIPD	5
#define    SEQUEUE_XML	    6
#define    RESULT_XML	    7

//============================
extern sem_t QueueSems[UP_COMMU_DEV_ARRAY];
extern sem_t UpRecQueSem_Gprs;
extern sem_t USART3RecQueSem_At;
extern sem_t USART3RecQueSem_AtIPD;
extern sem_t Sequence_XML_sem;
extern sem_t Result_XML_sem;

extern uint8 *pQueues[UP_COMMU_DEV_ARRAY];
extern uint8 UpRecQueue_Gprs[GPRS_REC_QUEUES_LENGTH];
extern uint8 USART3RecQueue_At[USART3_REC_QUEUE_AT_LENGTH];
extern uint8 USART3RecQueue_AtIPD[USART3_REC_QUEUE_ATIPD_LENGTH];

//===============下行通信设备ID
#define		DOWN_COMM_DEV_MBUS		0
#define		DOWN_COMM_DEV_485		1
#define	    DOWN_COMM_DEV_RESERVE	2

//============================================================================

extern uint8 UpcomInit(void);
extern uint8 UpGetEnd(uint8 dev, uint32 OutTime);
extern uint8 UpGetStart(uint8 dev, uint32 OutTime);
extern uint8 UpGetch(uint8 dev, uint8* data, uint16 OutTime);
extern uint8 UpQueueFlush(uint8 dev);
extern uint8 UpRecQueueWrite(uint8 dev, uint8* buf, uint32 n);
extern void UpDevSend(uint8 dev, uint8 *Data, uint32 n);

extern uint8 DuGetch(uint8 dev, uint8* data, uint16 OutTime);
extern void ChangeDuBps(uint8 dev, uint32 bps);
extern void DuSend(uint8 dev, uint8* buf, uint32 n);
extern uint8 DuQueueFlush(uint8 dev);   //清空缓冲区

extern uint16 TestUpRec(uint8 dev, uint8* buff, uint16 max);
extern uint16 TestDownRec(uint8 dev, uint8* buff, uint16 max);

void GprsSend(int32 fd, uint8 *Data, uint32 n);

#endif
/*********************************************************************************************************
 **                            End Of File
 ********************************************************************************************************/
