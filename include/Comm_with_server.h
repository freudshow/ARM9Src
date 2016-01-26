#ifndef __COMM_WITH_SERVER_H
#define __COMM_WITH_SERVER_H

#include "Centre_Controller.h"
#include "misc.h"

/* ��������Ȼ���*/
#define SERVER_COMMOND_BASE_LEN 25
/*����ǰ��FE ����*/
#define SERVER_COMMOND_FE_NUMBER 2
/* �������������������*/
#define SERVER_MAX_BUFF_SIZE 500
/* ����ĳ�ʱʱ��*/
#define SERVER_SEC_TIME_OUT 1
#define SERVER_USEC_TIME_OUT 0
/* ��������BUFF�ĳ���*/
#define SERVER_RCV_BUFF_LEN 527
/* Э��汾��*/
#define SERVER_VER_NUMBER 0x03

/*  ��Ϣ���Ͷ���*/
#define  SERVER_SET_CONTROL 0x02       //���������ü�����
#define  SERVER_SET_CONTROL_RET 0x03    //���������ü���������
#define  SERVER_SET_INSTRUMENT_ADDR 0x0C   //���������¸����������ַ
#define  SERVER_SET_INSTRUMENT_ADDR_RET 0x0D  //�������·����ַ�󣬼���������

#define  CONTROL_SEND_DATE 0x10                //����������������
#define  CONTROL_SEND_DATE_RET 0x11         //�������յ����������ݷ���
/**********************************************************************************************
 �������·�������������ṹ����
 ***********************************************************************************************/
typedef struct {
	/* ���ַ7B*/
	UINT8 ucAddr0;UINT8 ucAddr1;UINT8 ucAddr2;UINT8 ucAddr3;UINT8 ucAddr4;UINT8 ucAddr5;UINT8 ucAddr6;

	/* ���̴���*/
	UINT8 ucManufacturerCode;
	/* Э��汾*/
	UINT8 ucVerNumber;
	/* �豸���� */
	UINT8 ucDeviceType;
	/* ͨ���� */
	UINT8 ucPassNumber;
} INSTRUMENT_ADDR_T, *INSTRUMENT_ADDR_PT;
/* ���������·�������ṹ��*/
typedef struct {
	/* ��ʼ��*/
	UINT8 ucHead;
	/* Ver ��*/
	UINT8 ucVerNumber;
	/* Sadd Ŀ�ĵ�ַ*/
	UINT8 ucSaddr0;UINT8 ucSaddr1;UINT8 ucSaddr2;UINT8 ucSaddr3;UINT8 ucSaddr4;UINT8 ucSaddr5;
	/* Oadd Դ��ַ*/
	UINT8 ucOaddr0;UINT8 ucOaddr1;UINT8 ucOaddr2;UINT8 ucOaddr3;UINT8 ucOaddr4;UINT8 ucOaddr5;
	/* ��Ϣ�����MID*/
	UINT8 ucFrameMid;
	/* ���ݳ���*/
	UINT8 ucDateLenL;UINT8 ucDateLenH;
	/* ��Ϣ����*/
	UINT8 ucFrameMT;
	/* ��Ϣʱ��*/
	UINT8 ucRealTimeSec;UINT8 ucRealTimeMin;UINT8 ucRealTimeHour;UINT8 ucRealTimeDay;UINT8 ucRealTimeMonth;UINT8 ucRealTimeYear;
	/* ��ϢͷУ��*/
	UINT8 ucFrameCrc;
	/* ������ */
	UINT8 aucDate[SERVER_MAX_BUFF_SIZE];
} SERVER_HEAD_CMD_T, *SERVER_HEAD_CMD_PT;

/* ���������·�������ṹ��*/
typedef struct {
	/* ǰ������*/
	UINT8 ucLeadFe1;UINT8 ucLeadFe2;
	/*  ����ṹ��*/
	SERVER_HEAD_CMD_T stServerHeadCmd;
} SERVER_CMD_T, *SERVER_CMD_PT;

/* ����õ�����misc�ṹ�� */
typedef struct {
	UINT8 ucLeadFe1;
} SERVER_INFO_T;
/************************************************************************************************/

extern uint8 Gprs_QueueCreate(void);
extern INT32 server_com_rev(INT32 u32DeviceFd);
INT32 Gprs_com_send(INT32 u32DeviceFd);
#endif
