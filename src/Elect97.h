#ifndef __ELECT97_H
#define __ELECT97_H
#include "Centre_Controller.h"

/* 电表的命令长度基数*/
#define ELECT97_METER_COMMOND_BASE_LEN 10
/*命令前导FE 个数*/
#define ELECT97_METER_COMMOND_FE_NUMBER 4
/* 抄表命令的数据区长度*/
#define ELECT_METER_MAX_BUFF_SIZE 110
/* 抄表的超时时间*/
#define ELECT97_SEC_TIME_OUT 2
#define ELECT97_USEC_TIME_OUT 0
/* 接收命令BUFF的长度*/
#define ELECT97_RCV_BUFF_LEN 128
/**********************************************************************************************
 97协议电表抄表结构定义
 ***********************************************************************************************/
/* 电表97协议命令结构体*/
typedef struct {
	UINT8 ucFrameHead;UINT8 ucAddr[6];UINT8 ucSecondHead;
	/* 控制码*/
	UINT8 ucControlCode;
	/* 数据长度*/
	UINT8 ucDataLen;UINT8 ucDILow;UINT8 ucDIHigh;
	/* 数据区*/
	UINT8 aucData[ELECT_METER_MAX_BUFF_SIZE];
} ELECTR97_METER_CMD_T, *ELECTR97_METER_CMD_PT;

/************************************************************************************************/
extern UINT8 elect97_meter_com(INT32 u32DeviceFd, MeterFileType *pmf);

INT32 elect97_meter_com_rev(INT32 u32DeviceFd);
INT32 elect97_meter_com_send(INT32 u32DeviceFd, ELECTR97_METER_CMD_T *pmeter);
UINT8 elect97_write_date_into_sqlite(MeterFileType *pmf, char *sq_data);

#endif
