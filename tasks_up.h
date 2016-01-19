/*******************************************Copyright (c)*******************************************
 ɽ������ռ似����˾(�����ֲ�)                                                 
 **  ��   ��   ��: tasks_up.h
 **  ��   ��   ��: ������
 **  �� ��  �� ��: 2012.08.07 
 **  ��        ��: ����վ��ͨ������
 **  �� ��  �� ¼:   	
 *****************************************************************************************************/
#ifndef _TASKS_UP_H
#define _TASKS_UP_H

#define  XML_BUF_FILE_NUM   15

#pragma	pack(1)
typedef struct {
	uint8 GprsGvc;		// TRUE OR  FALSE��ΪTRUEʱ����ʾ��GPRS�Ĺ������漸��������Ч
	uint8 Device;			// �������ݵ��豸��GPRS, Zigbee, RS485
	uint16 ByteNum;		// �����ֽ���
	uint8 buff[1024];		//��������
} UP_COMM_MSG;
#pragma pack()

typedef struct {
	uint8 readpos;
	uint8 writepos;
	sem_t sem_read;
	sem_t sem_write;
	char *XMLFile[XML_BUF_FILE_NUM];
} XML_FILE;

extern XML_FILE gXML_Rcv_File;
extern XML_FILE gXML_Send_File;
extern char *RecFileName[];
extern char *SendFileName[];
extern uint8 gGPRS_XMLType;

void TaskUpRecGprs(void *pdata);
void ReportUp_main_proc(void *pdata);
void XMLFileInit(XML_FILE *XML_file, char *FileName[]);
#endif
/********************************************************************************************************
 **                                               End Of File										   **
 ********************************************************************************************************/
