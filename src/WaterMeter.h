#ifndef __WATER_METER_H
#define __WATER_METER_H
#include "Centre_Controller.h"
#include "CommandDeal.h"

/* ˮ���豸����*/
#define WATER_METER_TYPE 0x10
/* ��������BUFF�ĳ���*/
#define WATERMETER_LEN 128
/* �������������������*/
#define WATER_METER_MAX_BUFF_SIZE 110
/*ˮ�������Ȼ���*/
#define WATER_METER_COMMOND_BASE_LEN 11
/*ˮ�������ǰ��FE ����*/
#define WATER_METER_COMMOND_FE_NUMBER 2
/* ����ĳ�ʱʱ��*/
#define WATER_SEC_TIME_OUT 2
#define WATER_USEC_TIME_OUT 0

/**************************************��������********************************************/
extern UINT8 Water_meter_com(INT32 u32DeviceFd, MeterFileType *pmf);

#endif

