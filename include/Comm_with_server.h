#ifndef __COMM_WITH_SERVER_H
#define __COMM_WITH_SERVER_H

#include "Centre_Controller.h"
#include "misc.h"

/* 电表的命令长度基数*/
#define SERVER_COMMOND_BASE_LEN 25
/*命令前导FE 个数*/
#define SERVER_COMMOND_FE_NUMBER 2
/* 抄表命令的数据区长度*/
#define SERVER_MAX_BUFF_SIZE 500
/* 抄表的超时时间*/
#define SERVER_SEC_TIME_OUT 1
#define SERVER_USEC_TIME_OUT 0
/* 接收命令BUFF的长度*/
#define SERVER_RCV_BUFF_LEN 527
/* 协议版本号*/
#define SERVER_VER_NUMBER 0x03

/*  消息类型定义*/
#define  SERVER_SET_CONTROL 0x02       //服务器设置集中器
#define  SERVER_SET_CONTROL_RET 0x03    //服务器设置集中器返回
#define  SERVER_SET_INSTRUMENT_ADDR 0x0C   //服务器发下给集中器表地址
#define  SERVER_SET_INSTRUMENT_ADDR_RET 0x0D  //服务器下发表地址后，集中器返回

#define  CONTROL_SEND_DATE 0x10                //集中器长传表数据
#define  CONTROL_SEND_DATE_RET 0x11         //集中器收到长传表数据返回
/**********************************************************************************************
 服务器下发给集中器命令结构定义
 ***********************************************************************************************/
typedef struct {
	/* 表地址7B*/
	UINT8 ucAddr0;UINT8 ucAddr1;UINT8 ucAddr2;UINT8 ucAddr3;UINT8 ucAddr4;UINT8 ucAddr5;UINT8 ucAddr6;

	/* 厂商代码*/
	UINT8 ucManufacturerCode;
	/* 协议版本*/
	UINT8 ucVerNumber;
	/* 设备类型 */
	UINT8 ucDeviceType;
	/* 通道号 */
	UINT8 ucPassNumber;
} INSTRUMENT_ADDR_T, *INSTRUMENT_ADDR_PT;
/* 服务器端下发的命令结构体*/
typedef struct {
	/* 起始符*/
	UINT8 ucHead;
	/* Ver 号*/
	UINT8 ucVerNumber;
	/* Sadd 目的地址*/
	UINT8 ucSaddr0;UINT8 ucSaddr1;UINT8 ucSaddr2;UINT8 ucSaddr3;UINT8 ucSaddr4;UINT8 ucSaddr5;
	/* Oadd 源地址*/
	UINT8 ucOaddr0;UINT8 ucOaddr1;UINT8 ucOaddr2;UINT8 ucOaddr3;UINT8 ucOaddr4;UINT8 ucOaddr5;
	/* 消息的序号MID*/
	UINT8 ucFrameMid;
	/* 数据长度*/
	UINT8 ucDateLenL;UINT8 ucDateLenH;
	/* 消息类型*/
	UINT8 ucFrameMT;
	/* 消息时间*/
	UINT8 ucRealTimeSec;UINT8 ucRealTimeMin;UINT8 ucRealTimeHour;UINT8 ucRealTimeDay;UINT8 ucRealTimeMonth;UINT8 ucRealTimeYear;
	/* 消息头校验*/
	UINT8 ucFrameCrc;
	/* 数据区 */
	UINT8 aucDate[SERVER_MAX_BUFF_SIZE];
} SERVER_HEAD_CMD_T, *SERVER_HEAD_CMD_PT;

/* 服务器端下发的命令结构体*/
typedef struct {
	/* 前导符号*/
	UINT8 ucLeadFe1;UINT8 ucLeadFe2;
	/*  命令结构体*/
	SERVER_HEAD_CMD_T stServerHeadCmd;
} SERVER_CMD_T, *SERVER_CMD_PT;

/* 电表用到杂项misc结构体 */
typedef struct {
	UINT8 ucLeadFe1;
} SERVER_INFO_T;
/************************************************************************************************/

extern uint8 Gprs_QueueCreate(void);
extern INT32 server_com_rev(INT32 u32DeviceFd);
INT32 Gprs_com_send(INT32 u32DeviceFd);
#endif
