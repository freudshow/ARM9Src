/*******************************************Copyright (c)*******************************************
 **									ɽ������ռ似����˾(�����ֲ�)                                **                            
 **  ��   ��   ��: tasks3.cpp																      **
 **  ��   ��   ��: ������																		  **
 **	��   ��   ��: 0.1																		      **
 **  �� ��  �� ��: 2012��12��07�� 													     	      **
 **  ��        ��: GPRS��������        			      						                      **
 **	�� ��  �� ¼:   																			  **
 ****************************************************************************************************/
/********************************************** include *********************************************/
#include "includes.h"
#include "gprs_mana.h"
#include "tasks_up.h"

/********************************************** static **********************************************/
#define TEST_SELF_CHECK 0
#define TEST_GPRS_EN	0

static char gRecBuf_xmz[MAX_REC_MC52i_BYTENUM + 100]; //����+100������

/************************************************ gobale ***********************************************/
UP_COMM_MSG gUpCommMsg;

uint8 PRIO_TASK_GPRS_IPD = 0;

/************************************************ extern ***********************************************/
uint16 gUpdateBegin = 0;
uint8 gGPRSBusy = 0;
/********************************************************************************************************
 **  �� ��  �� ��: Fun_GprsIpd_xmz       									                           **
 **	�� ��  �� ��: ������ģ�� 								                                           **			
 **	�� ��  �� ��: pdata												                                   **
 **	�� ��  �� ��: none											                                       **
 **  ��   ��   ֵ: none																			   	   **
 **	��		  ע: 						                                                               **
 ********************************************************************************************************/
void Fun_GprsIpd_xmz(void) {
	//������û������,ԭ��֮һ����������һ��ʱ�䴦�ڹ���״̬
	uint8 err;
	uint16 len;
	uint8 Count = 0;
	while (PRIO_TASK_GPRS_IPD == 1) {
		if (gUpdateBegin == 1) {
			err = 0;
			UpQueueFlush(UP_COMMU_DEV_ATIPD);
		} else {
			err = GprsGetIPDATA_xmz(gRecBuf_xmz, 0, &len);
		}
		if (err == 0) {
			debug_debug(gDebugModule[GPRS_MODULE], "enter jh\n");
			do {
				err = GprsGetIPDATA_jh(gRecBuf_xmz, 0, &len);
				if (err) {
					//����һ�� 
					//���ܶԵ��Թ����жϵ�(�򱾵ؿ���������)���´���������ж�������ģ�鷢��^SISR: 1, 1(���ݻ�û����ȫ����ʱ)
					debug_err(gDebugModule[GPRS_MODULE], "%s %d err=%d\n",
							__FUNCTION__, __LINE__, err);
					/*begin:yangfei added 2013-02-28 needed deleted*/
					OSTimeDly(OS_TICKS_PER_SEC / 8);
					/*end:yangfei added 2013-02-28 needed deleted*/
					err = GprsGetIPDATA_jh(gRecBuf_xmz, 0, &len);
					if (err) {
						debug_err(gDebugModule[GPRS_MODULE], "%s %d err=%d\n",
								__FUNCTION__, __LINE__, err);
					}
				}
				/*begin:yangfei added 2013-02-28 needed deleted*/
				debug_debug(gDebugModule[GPRS_MODULE],
						"[%s][%s][%d] GprsIPREC len=%d ok \n", FILE_LINE, len);
				if (len > 400) {
					gUpdateBegin = 1;
				}/*����������ȡ����ģʽ*/
				if (len == 0) {
					Count++;
				} else {
					Count = 0;
				}
				if (Count > 5) {
					gUpdateBegin = 0;
				}/*����10�������ݽ����ѯ��������ģʽ*/
				/*end:yangfei added 2013-02-28 needed deleted*/
				OSTimeDly(OS_TICKS_PER_SEC / 8);
				UpdGprsRunSta_FeedRecDog();
				UpdGprsRunSta_AddFrmRecTimes(len);
				UpRecQueueWrite(UP_COMMU_DEV_GPRS, (uint8*) gRecBuf_xmz, len);
				if (len) {
					debug_debug(gDebugModule[GPRS_MODULE], "[%s][%s][%d] %s\n",
							FILE_LINE, gRecBuf_xmz);
					memset(gRecBuf_xmz, 0, sizeof(gRecBuf_xmz));
				}
			} while (err == 0 && len > 0);
			debug_debug(gDebugModule[GPRS_MODULE], "leave jh \n");
		} else {
			debug_err(gDebugModule[GPRS_MODULE], "%s %d err=%d\n", __FUNCTION__,
					__LINE__, err);
		}
	}
}

/********************************************************************************************************
 **  �� ��  �� ��: TaskGprsIpd       									                               **
 **	�� ��  �� ��: ������ģ�� 								                                           **
 **	�� ��  �� ��: pdata												                                   **
 **	�� ��  �� ��: none											                                       **
 **  ��   ��   ֵ: none																			   	   **
 **	��		  ע: Ϊ���ݶ���ģ�����	                                                               **
 ********************************************************************************************************/
void GPRS_Rec_proc(void *pdata) {
	//������û������,ԭ��֮һ����������һ��ʱ�䴦�ڹ���״̬

	pdata = pdata;
	//OSTaskSuspend(OS_PRIO_SELF);	
	while (1) {
		Fun_GprsIpd_xmz();
	}
}

/********************************************************************************************************
 **  �� ��  �� ��: DlyLandFail       									                               **
 **	�� ��  �� ��: ��¼ʧ����ʱ 								                                           **			
 **	�� ��  �� ��: none												                                   **
 **	�� ��  �� ��: none											                                       **
 **  ��   ��   ֵ: none																			   	   **
 **	��		  ע: 						                                                               **
 ********************************************************************************************************/
void DlyLandFail(void) {
	uint8 i;
	for (i = 0; i < 6; i++) {
		OSTimeDly(10 * OS_TICKS_PER_SEC);
	}
}

/********************************************************************************************************
 **  �� ��  �� ��: TaskGprsIpd       									                               **
 **	�� ��  �� ��: ��¼ʧ����ʱ���							                                           **
 **	�� ��  �� ��: none												                                   **
 **	�� ��  �� ��: none											                                       **
 **  ��   ��   ֵ: none																			   	   **
 **	��		  ע: 						                                                               **
 ********************************************************************************************************/
void DlyGprsCheck(void) {
	uint8 n, m;
	n = GPRS_CHECK_CYCLE / 10;
	m = GPRS_CHECK_CYCLE % 10;

	debug_debug(gDebugModule[GPRS_MODULE],
			"INFO: <DlyGprsCheck> Wait for GPRS Check Time Delay!\n");
	while (n--) {
		OSTimeDly(10 * OS_TICKS_PER_SEC);
	}
	if (m) {
		OSTimeDly(m * OS_TICKS_PER_SEC);
	}
}
/*add by yangmin 20130918*/
void setGprsXmlType(uint8 type) {
	// gjt add 0607 for xml send
	sem_wait(&Xml_Send_Sem);
	// gjt add end
	gGPRS_XMLType = type;

	sem_post(&ReportUp_Sem);
}
/********************************************************************************************************
 **  �� ��  �� ��: ConnectConfirm       									                               **
 **	�� ��  �� ��: ��¼ȷ�ϣ�ʧ�ܲ�����						                                           **
 **	�� ��  �� ��: none												                                   **
 **	�� ��  �� ��: none											                                       **
 **  ��   ��   ֵ: none																			   	   **
 **	��		  ע:    					                                                               **
 ********************************************************************************************************/
uint8 ConnectConfirm(void) {
	uint8 err, TryT;
	TryT = 0;
	int ret = 0;
	while (1) {
		if (TryT++ >= 5) {
			debug_err(gDebugModule[GPRS_MODULE],
					"ERROR: <ConnectConfirm> Retry five Times! GPRS UpLand Failure!\n");
			return FALSE; //5��û���յ���½֡��Ӧ
		}
		/*yangfei added 20130909 for ȷ���ź���Ϊ0*/
		ret = sem_init(&Sequence_XML_sem, 0, 0);
		if (ret != 0) {
			perror("USART3RecQueSem_AtIPD  error\n");
		}
		ret = sem_init(&Result_XML_sem, 0, 0);
		if (ret != 0) {
			perror("USART3RecQueSem_AtIPD  error\n");
		}
		/**/
		OSTimeDly(5 * OS_TICKS_PER_SEC);
		debug_err(gDebugModule[GPRS_MODULE],
				"GPRS Start Send Confirm frame !!!!\n");

		setGprsXmlType(REQUEST);/*�ɼ������������֤�������ݰ�Ϊ�ɼ������͸���������*/
		OSSemPend(SEQUEUE_XML, OS_TICKS_PER_SEC * 30, &err);/*�ȴ�sequence:����������һ���������*/
		if (err != OS_ERR_NONE) {
			debug_err(gDebugModule[GPRS_MODULE],
					"[%s][%s][%d]OSSemPend recive sequence err=%d\n", FILE_LINE,
					err);
			continue;
		}
		setGprsXmlType(MD5_XML);

		debug_info(gDebugModule[GPRS_MODULE], "INFO: sem_post MD5_XML!\n");/*�ɼ������ͼ����MD5��md5��Ԫ����Ч*/
		OSSemPend(RESULT_XML, OS_TICKS_PER_SEC * 30, &err);
		if (err != OS_ERR_NONE) {
			debug_err(gDebugModule[GPRS_MODULE],
					"[%s][%s][%d]OSSemPend recive result  err=%d\n", FILE_LINE,
					err);
			//TryT = 0;
			continue;
		} else { //�յ���Ӧ
			debug_info(gDebugModule[GPRS_MODULE],
					"INFO: <ConnectConfirm> Recvie GPRS UpLand Confirm Frame OK!\n");
			break;
		}

		debug_info(gDebugModule[GPRS_MODULE],
				"INFO: <ConnectConfirm> Retry Times is %d\n", TryT);
	}
	return TRUE;
}

/********************************************************************************************************
 **  �� ��  �� ��: SuspendGprsRecTask       									                           **
 **	�� ��  �� ��: ����GPRS��������							                                           **
 **	�� ��  �� ��: none												                                   **
 **	�� ��  �� ��: none											                                       **
 **  ��   ��   ֵ: none																			   	   **
 **	��		  ע:                                                                                      **
 ********************************************************************************************************/
void SuspendGprsRecTask(void) {
	//uint8 err;
	//�ڹ���GPRS��������ǰ��ȡ����������ռ�õ��ź���GprsXmzSem,���ź������з�������Ҳ��������,
	//��������˴���,���ܵ������з�����������

}

/********************************************************************************************************
 **  �� ��  �� ��: SuspendGprsRecTask       									                           **
 **	�� ��  �� ��: ����GPRS��������							                                           **
 **	�� ��  �� ��: none												                                   **
 **	�� ��  �� ��: none											                                       **
 **  ��   ��   ֵ: none																			   	   **
 **	��		  ע:                                                                                      **
 ********************************************************************************************************/
uint8 gGprsFirst;
void GPRS_Mana_main_proc(void *pdata) {
	uint8 tmpmid;
	//uint8 err;
	uint32 ReStartCounter = 0x00;
	uint32 HeartFrmSndCycles;
	uint32 Cycles;
	uint32 selfchecknum;

	GPRS_RUN_STA GprsRunSta;

	/*begin:yangfei added 2013-05-13 for GPRS��ʼ�ϵ縴λ*/
	//GprsOn_xmz();
	//gjt test 06131531
	GprsRestart_xmz();
	//gjt test end
	/*end:yangfei added 2013-05-13 for GPRS��ʼ�ϵ縴λ*/
	pdata = pdata;
#if TEST_SELF_CHECK ==0
	uint32 heartcy_sec;    //�������ڣ���λ��
#endif

	gGprsFirst = TRUE;

#if GPRS_HEART_FRM_TIME%GPRS_CHECK_CYCLE == 0
	HeartFrmSndCycles = GPRS_HEART_FRM_TIME / GPRS_CHECK_CYCLE;
#else
	HeartFrmSndCycles=GPRS_HEART_FRM_TIME/GPRS_CHECK_CYCLE+1;
#endif

	UpdGprsRunSta_ModuId(MODU_ID_UNKNOWN);
	InitGprsRunSta();    //GPRSû�о���
	tmpmid = ModelIdentify();
	UpdGprsRunSta_ModuId(tmpmid);
	printf("[%s][%s][%d] \n", FILE_LINE);
	for (;;) {
		ReStartCounter++;
		if (ReStartCounter > 1) {
			debug_info(gDebugModule[GPRS_MODULE],
					"WARNING: [TaskGprsMana] GPRS Modul Restart, Retry Times is %2d\n",
					ReStartCounter);
		} else {
			debug_info(gDebugModule[GPRS_MODULE],
					"INFO: [TaskGprsMana] GPRS Modul Start!\n");
		}
		/*yangfei added for ÿ������ʱҪ����GPRS��������*/
		PRIO_TASK_GPRS_IPD = 0;/*����GPRS��������*/

		InitGprsRunSta();    //GPRSû�о���ʱ ��TaskUpSend����ʹ���豸 UP_COMMU_DEV_AT

		//SuspendGprsRecTask();//����GPRS�������� 

		tmpmid = GetGprsRunSta_ModuId();

		if (tmpmid == MODU_ID_XMZ) {
			GprsInit_xmz(); //ʹ���豸 UP_COMMU_DEV_AT
			debug_info(gDebugModule[GPRS_MODULE], "GPRS  XMZ  init OK!! \n");
		} else {
			debug_err(gDebugModule[GPRS_MODULE], "GPRS MODU_ID_UNKNOWN!! \n");
		}
		UpdGprsRunSta_Ready(TRUE);
		//gprs ready���豸 UP_COMMU_DEV_AT��ʹ��Ȩ����TaskUpSend�����汾����
		//��ʱ���GPRS״̬��ȷ���Ƿ���Ҫ������������ѯGPRS״̬ʹ�õ����豸UP_COMMU_DEV_AT���ʱ�����ֱ��ʹ�ã�
		//������TaskUpSend��������TaskUpSend�����д��ȫ��״̬��Ϣ
		//�Ƿ񱾵ؿ�Ҳ���ƣ�Ҳ������ʾд����1

		PRIO_TASK_GPRS_IPD = 1;	  	//�ָ�GPRS��������
		//gjt modifed  test
		//#if TEST_SELF_CHECK ==0 && TEST_GPRS_EN == 0
		//#if 0
		if (ConnectConfirm() == FALSE) {
			//DlyLandFail();
			continue;
		}
		//#endif
		//#endif
		//OSMboxPost(HeartFrmMbox,(void*)1);//֪ͨ�����ϱ���������֡,��1֡����½֡��
		//==========��������½֡�����ϻ�Ҫ��1������֡,��Ӧ����������,����,��1���������ں�ſ�ʼ������֡
		//OSTimeDly(OS_TICKS_PER_SEC);
		//OSMboxPost(HeartFrmMbox,(void*)3);////֪ͨ�����ϱ���������֡
		//==========
		////
		UpdGprsRunSta_Cont(TRUE);
		selfchecknum = 0;

		//gjt added 06141644
		g_uiDataDealStatus |= STATU_CONTINUOUS;
		sem_post(&Xml_Deal_Sem);
		//gjt added end

		while (1) {
			DlyGprsCheck();
			UpdGprsRunSta_AddSndDog();
			UpdGprsRunSta_AddRecDog();
			selfchecknum = (selfchecknum + 1)
					% (GPRS_SELF_CHECK_CYCLE / GPRS_CHECK_CYCLE);
			if (selfchecknum == 0 && gGPRSBusy == 0) {
				//========������Ϣ��֪ͨ����������GPRS״̬���������gGprsRunSta
				gGPRSBusy = 1;
				/*begin:yangfei deleted 2013-08-29 for ��ʱ����Ҫ���Ժ��ٿ�������*/
#if 1
				gUpCommMsg.GprsGvc = TRUE;
#endif
				/*end:yangfei deleted 2013-08-29 for */
				printf("[%s][%s][%d] GprsGvc=TRUE \n", FILE_LINE);
				sem_post(&UpSend_Sem);
				sem_wait(&GprsMana_Sem);
			}

			ReadGprsRunSta(&GprsRunSta);

			if (GprsRunSta.IpCloseNum >= 2) {
				debug_err(gDebugModule[GPRS_MODULE],
						"WARNING: [TaskGprsMana] GprsRunSta.IpCloseNum>=2! Modul Restart!\n");
				goto restart;
			}

#if   TEST_SELF_CHECK == 0
			OS_ENTER_CRITICAL();
			//heartcy_sec=GlobalVariable.HeartCycle;
			heartcy_sec = 60;
			OS_EXIT_CRITICAL();
			if (heartcy_sec != 0 && heartcy_sec < 60) {
				HeartFrmSndCycles = (heartcy_sec * 60) / GPRS_CHECK_CYCLE;
			} else {
				HeartFrmSndCycles = GPRS_HEART_FRM_TIME / GPRS_CHECK_CYCLE;
			}
#endif

			Cycles = GPRS_HEART_FRM_REC_OUTTIME / GPRS_CHECK_CYCLE;
			Cycles += HeartFrmSndCycles;
			if (GprsRunSta.RecDog > Cycles) {
				debug_err(gDebugModule[GPRS_MODULE],
						"WARNING: [TaskGprsMana] Heart Frame Recive Out Time! Module will Restart!\n");
				goto restart;
			}
			/*yangfei added for */
			if (gGprsRunSta.IpSendFailNum > 2) {
				debug_err(gDebugModule[GPRS_MODULE],
						"WARNING: [TaskGprsMana] IpSendFailNum >2! Module will Restart!\n");
				goto restart;
			}

			//��һ����Ҫ ��ģ�鷵�ط���ʧ��ʱ������ι�������ֵ�ͻᳬ��HeartFrmSndCycles��Խ��Խ��
#if 0
			if(GprsRunSta.SndDog>HeartFrmSndCycles+5) {
#if TEST_SELF_CHECK == 0
				debug_err(gDebugModule[GPRS_MODULE],"WARNING: [TaskGprsMana] Heart Frame Send Failure! Modul Restart!\n");
				break;	  	//��������GPRS
#endif
			}
#endif

#if 0  /*�������ݵ�ʱ��Ҳ��������*/
			if(GprsRunSta.SndDog>=HeartFrmSndCycles)
#else
			if (GprsRunSta.SndDog >= HeartFrmSndCycles
					&& GprsRunSta.RecDog >= HeartFrmSndCycles)
#endif
							{

#if 1    /*yf:needed */
				debug_info(gDebugModule[GPRS_MODULE],
						"[%s][%s][%d] sem_post NOTIFY ����\n", FILE_LINE);
				gGPRSBusy = 0;
				// gjt add 0607 for xml send
				sem_wait(&Xml_Send_Sem);
				// gjt add end
				gGPRS_XMLType = NOTIFY;
				sem_post(&ReportUp_Sem);

#endif
			}
			continue;

			restart: UpdGprsRunSta_Cont(FALSE);/*GPRS ����*/
#if   TEST_SELF_CHECK == 0 && TEST_GPRS_EN == 0
			break;/*��������GPRS*/
#endif

		}
	}
}

