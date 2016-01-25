#ifndef __DELU_HEAT_METER_H
#define __DELU_HEAT_METER_H
//#include "Centre_Controller.h"
#include "CommandDeal.h"

/* 德鲁的热表设备类型*/
#define DELU_HEAT_METER_TYPE 0x20
/* 接收命令BUFF的长度*/
#define HEATMETER_LEN 128
/* 抄表命令的数据区长度*/
#define DELU_HEAT_METER_MAX_BUFF_SIZE 110
/*热表的命令长度基数*/
#define DELU_HEAT_METER_COMMOND_BASE_LEN 11
/*热表的命令前导FE 个数*/
#define DELU_HEAT_METER_COMMOND_FE_NUMBER 2
/* 抄表的超时时间*/
#define DELU_SEC_TIME_OUT 2
#define DELU_USEC_TIME_OUT 0

/**********************************************************************************************
 德鲁热表抄表结构定义
 ***********************************************************************************************/
/* 德鲁热表的命令结构体 */
typedef struct {
	/* 起始符*/
	UINT8 ucHead;
	/* 仪表类型*/
	UINT8 ucDeviceType;
	/* 七位地址*/
	UINT8 ucAddr[7];

	/* 控制码*/
	UINT8 ucControlCode;
	/* 数据长度*/
	UINT8 ucDataLen;
	/* 数据标识*/
	UINT8 ucDILow;
	UINT8 ucDIHigh;
	/* 序列号*/
	UINT8 ucSer;
	/* 数据区*/
	UINT8 aucData[DELU_HEAT_METER_MAX_BUFF_SIZE];
} DELU_HEAT_METER_CMD_T, *DELU_HEAT_METER_CMD_PT;
/******************************************************************************************/
/* 德鲁热表冷量 */
typedef struct {
	INT8 ucCoolYieldDecimal;
	INT8 ucCoolYieldInteger1;
	INT8 ucCoolYieldInteger2;
	INT8 ucCoolYieldInteger3;
	INT8 ucNumber;
} DELU_HEAT_METER_LENGLIANG_T;
/* 德鲁热表当前热量 */
typedef struct {
	INT8 ucHeatYieldDecimal;
	INT8 ucHeatYieldInteger1;
	INT8 ucHeatYieldInteger2;
	INT8 ucHeatYieldInteger3;
	INT8 ucNumber;
} DELU_HEAT_METER_RELIANG_T;
/* 德鲁热表热功率 */
typedef struct {
	INT8 ucHeatPowerDecimal;
	INT8 ucHeatPowerInteger1;
	INT8 ucHeatPowerInteger2;
	INT8 ucHeatPowerInteger3;
	INT8 ucNumber;
} DELU_HEAT_METER_REGONGLV_T;
/* 德鲁热表流速 */
typedef struct {
	INT8 ucFlowRateDecimal;
	INT8 ucFlowRateInteger1;
	INT8 ucFlowRateInteger2;
	INT8 ucFlowRateInteger3;
	INT8 ucNumber;
} DELU_HEAT_METER_LIUSU_T;
/* 德鲁热表累计流量 */
typedef struct {
	INT8 ucAllFlowDecimal;
	INT8 ucAllFlowInteger1;
	INT8 ucAllFlowInteger2;
	INT8 ucAllFlowInteger3;
	INT8 ucNumber;
} DELU_HEAT_METER_LEIJILIULIANG_T;
/* 德鲁热表	供水温度 */
typedef struct {
	INT8 ucSupplyWaterTemperDecimal;
	INT8 ucSupplyWaterTemperInteger;
	INT8 ucNumber;
} DELU_HEAT_METER_GONGSHUIWENDU_T;
/* 德鲁热表	回水温度 */
typedef struct {
	INT8 ucReturnWaterTemperDecimal;
	INT8 ucReturnWaterTemperInteger;
	INT8 ucNumber;
} DELU_HEAT_METER_HUISHUIWENDU_T;
/* 德鲁热表	供水累计工作时间 */
typedef struct {
	INT8 ucAllWorkTimeDecimal;
	INT8 ucAllWorkTimeInteger;
	INT8 ucNumber;
} DELU_HEAT_METER_ALL_WORK_TIME_T;
/* 德鲁热表	实时时间 */
typedef struct {
	INT8 ucSecond;
	INT8 ucMinute;
	INT8 ucHour;
	INT8 ucDay;
	INT8 ucMonth;
	INT8 ucYearL;
	INT8 ucYearH;
} DELU_HEAT_METER_REALTIME_T;
typedef struct {
	INT8 ucStL;
	INT8 ucStH;
} DELU_HEAT_METER_ST_T;

/* 德鲁热表抄表数据结构体 */
typedef struct {
	DELU_HEAT_METER_LENGLIANG_T stCoolYield;
	DELU_HEAT_METER_RELIANG_T stHeatYield;
	DELU_HEAT_METER_REGONGLV_T stHeatPower;
	DELU_HEAT_METER_LIUSU_T stFlowRate;
	DELU_HEAT_METER_LEIJILIULIANG_T stAllFlow;
	DELU_HEAT_METER_GONGSHUIWENDU_T stSupplyWaterTemper;
	DELU_HEAT_METER_HUISHUIWENDU_T stReturnWaterTemper;
	DELU_HEAT_METER_ALL_WORK_TIME_T stAllWorkTime;
	DELU_HEAT_METER_REALTIME_T stRealTime;
	DELU_HEAT_METER_ST_T stStStatus;
	INT8 ucCrc;
	INT8 ucOverByte;
} DELU_HEAT_METER_DATE_T;

/* 德鲁热表用到杂项misc结构体 */
typedef struct {

} DELU_HEAT_METER_INFO_T;

/**************************************函数声明********************************************/
INT32 delu_heat_meter_com_rev(INT32 u32DeviceFd);

INT32 delu_heat_meter_com_send(INT32 u32DeviceFd);

extern UINT8 Heat_meter_com(INT32 u32DeviceFd, MeterFileType *pmf);

#endif

