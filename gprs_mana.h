#ifndef				_TASKS3_H
#define				_TASKS3_H				

#define  GPRS_CHECK_CYCLE	 			10

#define  GPRS_HEART_FRM_TIME		60
#define  GPRS_SELF_CHECK_CYCLE	 		5*GPRS_CHECK_CYCLE
//����֡���Ź������ֵ,ʱ�䵥λΪ��,������֡��������,�������ʱ������û���յ��κ�֡,������GPRSģ��
#define  GPRS_HEART_FRM_REC_OUTTIME		GPRS_HEART_FRM_TIME*4

extern UP_COMM_MSG gUpCommMsg;
extern uint8 XMLType;
#endif
