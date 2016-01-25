#ifndef __WATER_METER_H
#define __WATER_METER_H
#include "Centre_Controller.h"
#include "CommandDeal.h"

/* 水表设备类型*/
#define WATER_METER_TYPE 0x10
/* 接收命令BUFF的长度*/
#define WATERMETER_LEN 128
/* 抄表命令的数据区长度*/
#define WATER_METER_MAX_BUFF_SIZE 110
/*水表的命令长度基数*/
#define WATER_METER_COMMOND_BASE_LEN 11
/*水表的命令前导FE 个数*/
#define WATER_METER_COMMOND_FE_NUMBER 2
/* 抄表的超时时间*/
#define WATER_SEC_TIME_OUT 2
#define WATER_USEC_TIME_OUT 0

/**************************************函数声明********************************************/
extern UINT8 Water_meter_com(INT32 u32DeviceFd, MeterFileType *pmf);

#endif

