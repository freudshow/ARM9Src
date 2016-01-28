#ifndef				_TASKS3_H
#define				_TASKS3_H				

#define  GPRS_CHECK_CYCLE	 			10

#define  GPRS_HEART_FRM_TIME		60
#define  GPRS_SELF_CHECK_CYCLE	 		5*GPRS_CHECK_CYCLE
//接收帧看门狗的溢出值,时间单位为秒,在心跳帧间隔到达后,经过这个时间若仍没有收到任何帧,则重启GPRS模块
#define  GPRS_HEART_FRM_REC_OUTTIME		GPRS_HEART_FRM_TIME*4

extern UP_COMM_MSG gUpCommMsg;
extern uint8 XMLType;
#endif
