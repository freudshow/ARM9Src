/*******************************************Copyright (c)*******************************************
 ɽ������ռ似����˾(�����ֲ�)                                                 
 **  ��   ��   ��: tasks_up.cpp
 **  ��   ��   ��: ������
 **  �� ��  �� ��: 2012.08.07 
 **  ��        ��: ����վ��ͨ������
 **  �� ��  �� ¼:   	
 *****************************************************************************************************/

/********************************************** include *********************************************/
#include "includes.h"
#include "tasks_up.h"

/********************************************** extern *********************************************/
extern GPRS_RUN_STA gGprsRunSta;

/********************************************** global *********************************************/
char *RecFileName[] = { "buff0.xml", "buff1.xml", "buff2.xml", "buff3.xml",
		"buff4.xml", "buff5.xml", "buff6.xml", "buff7.xml", "buff8.xml",
		"buff9.xml", "buff10.xml", "buff11.xml", "buff12.xml", "buff13.xml",
		"buff14.xml" };

char *SendFileName[] = { "send0.xml", "send1.xml", "send2.xml", "send3.xml",
		"send4.xml", "send5.xml", "send6.xml", "send7.xml", "send8.xml",
		"send9.xml", "send10.xml", "send11.xml", "send12.xml", "send13.xml",
		"send14.xml" };

uint8 gGPRS_XMLType;

XML_FILE gXML_Rcv_File;
XML_FILE gXML_Send_File;
uint8 gGPRSSendBuf[1500];
/********************************************** static *********************************************/

//static uint8 testHeartFrm_xmz[]={"����!������CZ513-6�ͼ������Լ����!GPRS����ͨ���Ѿ��ɹ�����!�Ǻ�.����������������һЩ���ֻ�����.�����Լ����������Ƿ��յ�.---����!�����ǳ�̩�������Լ����!GPRS����ͨ���Ѿ��ɹ�����!�Ǻ�.����������������һЩ���ֻ�����.�����Լ����������Ƿ��յ�.---����!�����ǳ�̩�������Լ����!GPRS����ͨ���Ѿ��ɹ�����!�Ǻ�.����������������һЩ���ֻ�����.�����Լ����������Ƿ��յ�.---����!�����ǳ�̩�������Լ����!GPRS����ͨ���Ѿ��ɹ�����!�Ǻ�.����������������һЩ���ֻ�����.�����Լ����������Ƿ��յ�."};

/****************************************************************************************************
 **	�� ��  �� ��: TaskUpSend
 **	�� ��  �� ��: ����λ���������ݣ������κ����ݣ�����ͨ��������������
 **	�� ��  �� ��: ��
 **	�� ��  �� ��: �� 
 **  ��   ��   ֵ: 
 **	��		  ע: ��
 *****************************************************************************************************/
void GPRS_Upsend_main_proc(void *pdata) {
	uint8 Err = 0x00;

	uint16 BufNum = 0;
	GPRS_RUN_STA GprsRunSta;
	pdata = pdata; /* ������뾯��*/
	FILE *fp;
	int FileSize;
	int i;
	int SendNum;
	gUpCommMsg.GprsGvc = FALSE;
	while (1) {
		sem_wait(&UpSend_Sem);

		debug_debug(gDebugModule[GPRS_MODULE],
				"INFO: [TaskUpSend] Recived a UpSend_Sem, Ready to UpSend Data !\n");

		if (gUpCommMsg.GprsGvc == TRUE) {
			debug_info(gDebugModule[GPRS_MODULE],
					"[%s][%s][%d] GprsGvc == TRUE \n", FILE_LINE);
			ReadGprsRunSta(&GprsRunSta);
			if (GprsRunSta.Ready == TRUE) {
				if (CMD_IpClose() == FALSE) {
					printf("[%s][%s][%d] UpdGprsRunSta_IpCloseNum\n", FILE_LINE
							);
					UpdGprsRunSta_IpCloseNum(0);
					debug_info(gDebugModule[GPRS_MODULE],
							"UpdGprsRunSta_IpCloseNum(0)!\n");
				} else {
					UpdGprsRunSta_AddIpCloseNum();
					debug_err(gDebugModule[GPRS_MODULE],
							"UpdGprsRunSta_AddIpCloseNum()\n");
				}

				OSTimeDly(OS_TICKS_PER_SEC / 20);
#if  0   /*��ʱû�У����ε�*/
				if(CMD_Csq(&csq)==0)
				{
					UpdGprsRunSta_Csq(csq);
				}
#endif
			}
			sem_post(&GprsMana_Sem);
			gUpCommMsg.GprsGvc = FALSE;
		} else {
			ReadGprsRunSta(&GprsRunSta);
			if (GprsRunSta.Ready == FALSE) {
				printf("[%s][%s][%d] GprsRunSta.Ready==FALSE)\n", FILE_LINE);
				continue;
			}
			sem_wait(&gXML_Send_File.sem_read);
			debug_debug(gDebugModule[GPRS_MODULE],
					"[%s][%s][%d] sem_wait gXML_Send_File.sem_read ok\n",
					FILE_LINE);

			/*����XML�ļ�*/
			fp = fopen(gXML_Send_File.XMLFile[gXML_Send_File.readpos], "r");
			fseek(fp, 0, 2);/*λ��ָ�뵽�ļ�ĩβ*/
			FileSize = ftell(fp);/*�����ļ���С*/
			fseek(fp, 0, 0);/*λ��ָ�뵽�ļ���ʼ*/
			SendNum = MAX_IPSEND_MC52i_BYTENUM;
			for (i = 0; i < FileSize / MAX_IPSEND_MC52i_BYTENUM + 1; i++) {
				if (i == FileSize / MAX_IPSEND_MC52i_BYTENUM) /*���һ��*/
				{
					SendNum = FileSize % MAX_IPSEND_MC52i_BYTENUM;
				}
				memset(gGPRSSendBuf, 0, sizeof(gGPRSSendBuf));
				fread(gGPRSSendBuf, 1, SendNum, fp);
				debug_info(gDebugModule[GPRS_MODULE], "send xml:  %s\n",
						gGPRSSendBuf);
				debug_info(gDebugModule[GPRS_MODULE],
						"INFO: [TaskUpSend] SendNum=%d\n", SendNum);
				Err = GprsIPSEND(gGPRSSendBuf, SendNum, &BufNum);
				//OSTimeDly(OS_TICKS_PER_SEC/4);
				if (Err == 0) {
					//UpdGprsRunSta_IpSendRemnBufNum((uint8)BufNum);
					UpdGprsRunSta_FeedSndDog();
					UpdGprsRunSta_FeedIpSendFailNum();
					UpdGprsRunSta_AddFrmSndTimes(SendNum);
					debug_info(gDebugModule[GPRS_MODULE],
							"INFO: [TaskUpSend] IP Send Successful!SendNum=%d\n",
							SendNum);
					// gjt add 06081743 for xmldeal data report

					if ((gGPRS_XMLType == REPLY) || (gGPRS_XMLType == REPORT)) {
						gGPRS_Send_Succ_Flag = TRUE;
						debug_info(gDebugModule[GPRS_MODULE],
								"INFO: [TaskUpSend] IP Send Successful! then post gGPRS_Send_Succ_Sem \n");
						sem_post(&gGPRS_Send_Succ_Sem);
					}
				} else {
					debug_warn(gDebugModule[GPRS_MODULE],
							"WARNING: [TaskUpSend] IP Send Failure!\n");
					UpdGprsRunSta_AddIpSendFailNum();
					if ((gGPRS_XMLType == REPLY) || (gGPRS_XMLType == REPORT)) {
						gGPRS_Send_Succ_Flag = FALSE;
						debug_info(gDebugModule[GPRS_MODULE],
								"INFO: [TaskUpSend] IP Send Failure! then post Failure");
						sem_post(&gGPRS_Send_Succ_Sem);
					}
					break;
				}
			}
			fclose(fp);

			gXML_Send_File.readpos = (gXML_Send_File.readpos + 1)
					% XML_BUF_FILE_NUM;
			printf("[%s][%s][%d]gXML_Send_File.readpos = %d \n", FILE_LINE,
					gXML_Send_File.readpos);
			sem_post(&gXML_Send_File.sem_write);
		}
		debug_debug(gDebugModule[GPRS_MODULE],
				"INFO: [TaskUpSend] Waiting for UpSend_Q!\n");
	}
}

/*****************************************************************************************
 **	�� ��  �� ��: TaskUpRecGPRS
 **	�� ��  �� ��: �������GPRS�����ݣ�����Ч����ȷ, ֮���ɷ���֡�������ִ��
 **	�� ��  �� ��: ��
 **	�� ��  �� ��: �� 
 **  ��   ��   ֵ: 
 **	��		  ע: ��
 ******************************************************************************************/
//char gGPRSTestData[1024] = {0x00};
void TaskUpRecGprs(void *pdata) {
	uint8 err;
	int value;

	while (1) {

		err = UpGetStart(UP_COMMU_DEV_GPRS, (uint32) NULL);
		if (err) {
			debug_err(gDebugModule[GPRS_MODULE],
					"[%s][%s][%d] get file start err\n", FILE_LINE);
		} else {
			debug_info(gDebugModule[GPRS_MODULE],
					"[%s][%s][%d] get file start ok\n", FILE_LINE);
		}

		err = UpGetEnd(UP_COMMU_DEV_GPRS, OS_TICKS_PER_SEC * 20);
		if (err) {
			debug_err(gDebugModule[GPRS_MODULE],
					"[%s][%s][%d] get file end err\n", FILE_LINE);
		} else {
			//gjt test 06121432 start
			debug_info(gDebugModule[XML_MODULE],
					"[%s][%s][%d] get file end ok\n", FILE_LINE);
			//debug_info(gDebugModule[GPRS_MODULE],"[%s][%s][%d] get file end ok\n",FILE_LINE);

			//sem_post(&XMLRcvFile_Sem);
			if ((gXML_Rcv_File.writepos + 1) % XML_BUF_FILE_NUM
					== gXML_Rcv_File.readpos) {
				debug_err(gDebugModule[XML_MODULE],
						"[%s][%s][%d] gXML_Rcv_File is full writepos=%d readpos=%d\n",
						FILE_LINE, gXML_Rcv_File.writepos,
						gXML_Rcv_File.readpos);
			}
			sem_wait(&gXML_Rcv_File.sem_write);

			sem_getvalue(&gXML_Rcv_File.sem_write, &value);
			debug_info(gDebugModule[XML_MODULE],
					"[%s][%s][%d]gXML_Rcv_File.sem_write=%d \n", FILE_LINE,
					value);
			gXML_Rcv_File.writepos = (gXML_Rcv_File.writepos + 1)
					% XML_BUF_FILE_NUM;
			sem_getvalue(&gXML_Rcv_File.sem_read, &value);
			debug_info(gDebugModule[XML_MODULE],
					"[%s][%s][%d]gXML_Rcv_File.sem_read=%d \n", FILE_LINE,
					value);

			debug_info(gDebugModule[XML_MODULE],
					"[%s][%s][%d]gXML_Rcv_File.writepos = %d \n", FILE_LINE,
					gXML_Rcv_File.writepos);
			sem_post(&gXML_Rcv_File.sem_read);
			//gjt test end
		}

	}
}

/****************************************************************************************************
 **	�� ��  �� ��: TaskReportUp
 **	�� ��  �� ��: ����ϵͳ�������ϱ�����
 **	�� ��  �� ��: ��
 **	�� ��  �� ��: �� 
 **  ��   ��   ֵ: 
 **	��		  ע: ��
 *****************************************************************************************************/
void ReportUp_main_proc(void *pdata) {
	pdata = pdata;
	int ret;
	int value;

	while (1) {
		sem_wait(&ReportUp_Sem);
		ret = makexml(gGPRS_XMLType,
				gXML_Send_File.XMLFile[gXML_Send_File.writepos]);
		/******* gjt add for xml send data *****/
		sem_post(&Xml_Send_Sem);
		ret = sem_getvalue(&Xml_Send_Sem, &value);
		if (ret != 0) {
			printf("[%s][%s][%d] %s \n", FILE_LINE, strerror(errno));
		}
		printf("[%s][%s][%d]Xml_Send_Sem=%d *************\n", FILE_LINE, value);

		/******* gjt add end ******************/
		if (ret == 0) {
			if ((gXML_Send_File.writepos + 1) % XML_BUF_FILE_NUM
					== gXML_Send_File.readpos) {
				debug_err(gDebugModule[GPRS_MODULE],
						"[%s][%s][%d] XML_BUF_FILE is full \n", FILE_LINE);
			}
			sem_wait(&gXML_Send_File.sem_write);
			gXML_Send_File.writepos = (gXML_Send_File.writepos + 1)
					% XML_BUF_FILE_NUM;

			sem_post(&UpSend_Sem);
			debug_info(gDebugModule[GPRS_MODULE],
					"[%s][%s][%d]gXML_Send_File.writepos = %d \n", FILE_LINE,
					gXML_Send_File.writepos);
			sem_post(&gXML_Send_File.sem_read);
		} else {
			debug_err(gDebugModule[GPRS_MODULE], "[%s][%s][%d]makexml err \n",
					FILE_LINE);
		}
	}

}
/********************************************************************************************************
 **                                               End Of File										   **
 ********************************************************************************************************/
