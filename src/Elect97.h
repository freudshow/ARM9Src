#ifndef __ELECT97_H
#define __ELECT97_H
#include "Centre_Controller.h"

/* ��������Ȼ���*/
#define ELECT97_METER_COMMOND_BASE_LEN 10
/*����ǰ��FE ����*/
#define ELECT97_METER_COMMOND_FE_NUMBER 4
/* �������������������*/
#define ELECT_METER_MAX_BUFF_SIZE 110
/* ����ĳ�ʱʱ��*/
#define ELECT97_SEC_TIME_OUT 2
#define ELECT97_USEC_TIME_OUT 0
/* ��������BUFF�ĳ���*/
#define ELECT97_RCV_BUFF_LEN 128
/**********************************************************************************************
 97Э������ṹ����
 ***********************************************************************************************/
/* ���97Э������ṹ��*/
typedef struct {
	UINT8 ucFrameHead;UINT8 ucAddr[6];UINT8 ucSecondHead;
	/* ������*/
	UINT8 ucControlCode;
	/* ���ݳ���*/
	UINT8 ucDataLen;UINT8 ucDILow;UINT8 ucDIHigh;
	/* ������*/
	UINT8 aucData[ELECT_METER_MAX_BUFF_SIZE];
} ELECTR97_METER_CMD_T, *ELECTR97_METER_CMD_PT;

/************************************************************************************************/
extern UINT8 elect97_meter_com(INT32 u32DeviceFd, MeterFileType *pmf);

INT32 elect97_meter_com_rev(INT32 u32DeviceFd);
INT32 elect97_meter_com_send(INT32 u32DeviceFd, ELECTR97_METER_CMD_T *pmeter);
UINT8 elect97_write_date_into_sqlite(MeterFileType *pmf, char *sq_data);

#endif
