#ifndef __DELU_HEAT_METER_H
#define __DELU_HEAT_METER_H
//#include "Centre_Controller.h"
#include "CommandDeal.h"

/* ��³���ȱ��豸����*/
#define DELU_HEAT_METER_TYPE 0x20
/* ��������BUFF�ĳ���*/
#define HEATMETER_LEN 128
/* �������������������*/
#define DELU_HEAT_METER_MAX_BUFF_SIZE 110
/*�ȱ������Ȼ���*/
#define DELU_HEAT_METER_COMMOND_BASE_LEN 11
/*�ȱ������ǰ��FE ����*/
#define DELU_HEAT_METER_COMMOND_FE_NUMBER 2
/* ����ĳ�ʱʱ��*/
#define DELU_SEC_TIME_OUT 2
#define DELU_USEC_TIME_OUT 0

/**********************************************************************************************
 ��³�ȱ���ṹ����
 ***********************************************************************************************/
/* ��³�ȱ������ṹ�� */
typedef struct {
	/* ��ʼ��*/
	UINT8 ucHead;
	/* �Ǳ�����*/
	UINT8 ucDeviceType;
	/* ��λ��ַ*/
	UINT8 ucAddr[7];

	/* ������*/
	UINT8 ucControlCode;
	/* ���ݳ���*/
	UINT8 ucDataLen;
	/* ���ݱ�ʶ*/
	UINT8 ucDILow;
	UINT8 ucDIHigh;
	/* ���к�*/
	UINT8 ucSer;
	/* ������*/
	UINT8 aucData[DELU_HEAT_METER_MAX_BUFF_SIZE];
} DELU_HEAT_METER_CMD_T, *DELU_HEAT_METER_CMD_PT;
/******************************************************************************************/
/* ��³�ȱ����� */
typedef struct {
	INT8 ucCoolYieldDecimal;
	INT8 ucCoolYieldInteger1;
	INT8 ucCoolYieldInteger2;
	INT8 ucCoolYieldInteger3;
	INT8 ucNumber;
} DELU_HEAT_METER_LENGLIANG_T;
/* ��³�ȱ�ǰ���� */
typedef struct {
	INT8 ucHeatYieldDecimal;
	INT8 ucHeatYieldInteger1;
	INT8 ucHeatYieldInteger2;
	INT8 ucHeatYieldInteger3;
	INT8 ucNumber;
} DELU_HEAT_METER_RELIANG_T;
/* ��³�ȱ��ȹ��� */
typedef struct {
	INT8 ucHeatPowerDecimal;
	INT8 ucHeatPowerInteger1;
	INT8 ucHeatPowerInteger2;
	INT8 ucHeatPowerInteger3;
	INT8 ucNumber;
} DELU_HEAT_METER_REGONGLV_T;
/* ��³�ȱ����� */
typedef struct {
	INT8 ucFlowRateDecimal;
	INT8 ucFlowRateInteger1;
	INT8 ucFlowRateInteger2;
	INT8 ucFlowRateInteger3;
	INT8 ucNumber;
} DELU_HEAT_METER_LIUSU_T;
/* ��³�ȱ��ۼ����� */
typedef struct {
	INT8 ucAllFlowDecimal;
	INT8 ucAllFlowInteger1;
	INT8 ucAllFlowInteger2;
	INT8 ucAllFlowInteger3;
	INT8 ucNumber;
} DELU_HEAT_METER_LEIJILIULIANG_T;
/* ��³�ȱ�	��ˮ�¶� */
typedef struct {
	INT8 ucSupplyWaterTemperDecimal;
	INT8 ucSupplyWaterTemperInteger;
	INT8 ucNumber;
} DELU_HEAT_METER_GONGSHUIWENDU_T;
/* ��³�ȱ�	��ˮ�¶� */
typedef struct {
	INT8 ucReturnWaterTemperDecimal;
	INT8 ucReturnWaterTemperInteger;
	INT8 ucNumber;
} DELU_HEAT_METER_HUISHUIWENDU_T;
/* ��³�ȱ�	��ˮ�ۼƹ���ʱ�� */
typedef struct {
	INT8 ucAllWorkTimeDecimal;
	INT8 ucAllWorkTimeInteger;
	INT8 ucNumber;
} DELU_HEAT_METER_ALL_WORK_TIME_T;
/* ��³�ȱ�	ʵʱʱ�� */
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

/* ��³�ȱ������ݽṹ�� */
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

/* ��³�ȱ��õ�����misc�ṹ�� */
typedef struct {

} DELU_HEAT_METER_INFO_T;

/**************************************��������********************************************/
INT32 delu_heat_meter_com_rev(INT32 u32DeviceFd);

INT32 delu_heat_meter_com_send(INT32 u32DeviceFd);

extern UINT8 Heat_meter_com(INT32 u32DeviceFd, MeterFileType *pmf);

#endif

