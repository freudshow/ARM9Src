/*******************************************Copyright (c)*******************************************
 山东华宇空间技术公司(西安分部)                                                 
 **  文   件   名: tasks_up.h
 **  创   建   人: 勾江涛
 **  创 建  日 期: 2012.08.07 
 **  描        述: 与主站的通信任务
 **  修 改  记 录:   	
 *****************************************************************************************************/
#ifndef _TASKS_UP_H
#define _TASKS_UP_H

#define  XML_BUF_FILE_NUM   15

#pragma	pack(1)
typedef struct {
	uint8 GprsGvc;		// TRUE OR  FALSE，为TRUE时，表示对GPRS的管理，下面几个参数无效
	uint8 Device;			// 发送数据的设备，GPRS, Zigbee, RS485
	uint16 ByteNum;		// 发送字节数
	uint8 buff[1024];		//发送数据
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
