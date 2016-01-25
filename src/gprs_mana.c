/*******************************************Copyright (c)*******************************************
 **									山东华宇空间技术公司(西安分部)                                **                            
 **  文   件   名: tasks3.cpp																      **
 **  创   建   人: 勾江涛																		  **
 **	版   本   号: 0.1																		      **
 **  创 建  日 期: 2012年12月07日 													     	      **
 **  描        述: GPRS管理任务        			      						                      **
 **	修 改  记 录:   																			  **
 ****************************************************************************************************/
/********************************************** include *********************************************/
#include "includes.h"
#include "gprs_mana.h"
#include "tasks_up.h"

/********************************************** static **********************************************/
#define TEST_SELF_CHECK 0
#define TEST_GPRS_EN	0

static char gRecBuf_xmz[MAX_REC_MC52i_BYTENUM + 100]; //这里+100是冗余

/************************************************ gobale ***********************************************/
UP_COMM_MSG gUpCommMsg;

uint8 PRIO_TASK_GPRS_IPD = 0;

/************************************************ extern ***********************************************/
uint16 gUpdateBegin = 0;
uint8 gGPRSBusy = 0;
/********************************************************************************************************
 **  函 数  名 称: Fun_GprsIpd_xmz       									                           **
 **	函 数  功 能: 西门子模块 								                                           **			
 **	输 入  参 数: pdata												                                   **
 **	输 出  参 数: none											                                       **
 **  返   回   值: none																			   	   **
 **	备		  注: 						                                                               **
 ********************************************************************************************************/
void Fun_GprsIpd_xmz(void) {
	//本任务没有任务狗,原因之一是它可能在一段时间处在挂起状态
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
					//重试一次 
					//可能对调试过程中断点(或本地口来的数据)导致错误而彻底中断西门子模块发的^SISR: 1, 1(数据还没有完全读空时)
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
				}/*进入主动读取数据模式*/
				if (len == 0) {
					Count++;
				} else {
					Count = 0;
				}
				if (Count > 5) {
					gUpdateBegin = 0;
				}/*连续10次无数据进入查询接受数据模式*/
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
 **  函 数  名 称: TaskGprsIpd       									                               **
 **	函 数  功 能: 西门子模块 								                                           **
 **	输 入  参 数: pdata												                                   **
 **	输 出  参 数: none											                                       **
 **  返   回   值: none																			   	   **
 **	备		  注: 为兼容多种模块设计	                                                               **
 ********************************************************************************************************/
void GPRS_Rec_proc(void *pdata) {
	//本任务没有任务狗,原因之一是它可能在一段时间处在挂起状态

	pdata = pdata;
	//OSTaskSuspend(OS_PRIO_SELF);	
	while (1) {
		Fun_GprsIpd_xmz();
	}
}

/********************************************************************************************************
 **  函 数  名 称: DlyLandFail       									                               **
 **	函 数  功 能: 登录失败延时 								                                           **			
 **	输 入  参 数: none												                                   **
 **	输 出  参 数: none											                                       **
 **  返   回   值: none																			   	   **
 **	备		  注: 						                                                               **
 ********************************************************************************************************/
void DlyLandFail(void) {
	uint8 i;
	for (i = 0; i < 6; i++) {
		OSTimeDly(10 * OS_TICKS_PER_SEC);
	}
}

/********************************************************************************************************
 **  函 数  名 称: TaskGprsIpd       									                               **
 **	函 数  功 能: 登录失败延时检测							                                           **
 **	输 入  参 数: none												                                   **
 **	输 出  参 数: none											                                       **
 **  返   回   值: none																			   	   **
 **	备		  注: 						                                                               **
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
 **  函 数  名 称: ConnectConfirm       									                               **
 **	函 数  功 能: 登录确认，失败并重试						                                           **
 **	输 入  参 数: none												                                   **
 **	输 出  参 数: none											                                       **
 **  返   回   值: none																			   	   **
 **	备		  注:    					                                                               **
 ********************************************************************************************************/
uint8 ConnectConfirm(void) {
	uint8 err, TryT;
	TryT = 0;
	int ret = 0;
	while (1) {
		if (TryT++ >= 5) {
			debug_err(gDebugModule[GPRS_MODULE],
					"ERROR: <ConnectConfirm> Retry five Times! GPRS UpLand Failure!\n");
			return FALSE; //5次没有收到登陆帧回应
		}
		/*yangfei added 20130909 for 确保信号量为0*/
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

		setGprsXmlType(REQUEST);/*采集器请求身份验证（该数据包为采集器发送给服务器）*/
		OSSemPend(SEQUEUE_XML, OS_TICKS_PER_SEC * 30, &err);/*等待sequence:服务器发送一串随机序列*/
		if (err != OS_ERR_NONE) {
			debug_err(gDebugModule[GPRS_MODULE],
					"[%s][%s][%d]OSSemPend recive sequence err=%d\n", FILE_LINE,
					err);
			continue;
		}
		setGprsXmlType(MD5_XML);

		debug_info(gDebugModule[GPRS_MODULE], "INFO: sem_post MD5_XML!\n");/*采集器发送计算的MD5，md5子元素有效*/
		OSSemPend(RESULT_XML, OS_TICKS_PER_SEC * 30, &err);
		if (err != OS_ERR_NONE) {
			debug_err(gDebugModule[GPRS_MODULE],
					"[%s][%s][%d]OSSemPend recive result  err=%d\n", FILE_LINE,
					err);
			//TryT = 0;
			continue;
		} else { //收到回应
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
 **  函 数  名 称: SuspendGprsRecTask       									                           **
 **	函 数  功 能: 挂起GPRS接收任务							                                           **
 **	输 入  参 数: none												                                   **
 **	输 出  参 数: none											                                       **
 **  返   回   值: none																			   	   **
 **	备		  注:                                                                                      **
 ********************************************************************************************************/
void SuspendGprsRecTask(void) {
	//uint8 err;
	//在挂起GPRS接收任务前获取它可能正在占用的信号量GprsXmzSem,此信号量上行发送任务也可能申请,
	//如果不做此处理,可能导致上行发送任务死掉

}

/********************************************************************************************************
 **  函 数  名 称: SuspendGprsRecTask       									                           **
 **	函 数  功 能: 挂起GPRS接收任务							                                           **
 **	输 入  参 数: none												                                   **
 **	输 出  参 数: none											                                       **
 **  返   回   值: none																			   	   **
 **	备		  注:                                                                                      **
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

	/*begin:yangfei added 2013-05-13 for GPRS开始上电复位*/
	//GprsOn_xmz();
	//gjt test 06131531
	GprsRestart_xmz();
	//gjt test end
	/*end:yangfei added 2013-05-13 for GPRS开始上电复位*/
	pdata = pdata;
#if TEST_SELF_CHECK ==0
	uint32 heartcy_sec;    //心跳周期，单位秒
#endif

	gGprsFirst = TRUE;

#if GPRS_HEART_FRM_TIME%GPRS_CHECK_CYCLE == 0
	HeartFrmSndCycles = GPRS_HEART_FRM_TIME / GPRS_CHECK_CYCLE;
#else
	HeartFrmSndCycles=GPRS_HEART_FRM_TIME/GPRS_CHECK_CYCLE+1;
#endif

	UpdGprsRunSta_ModuId(MODU_ID_UNKNOWN);
	InitGprsRunSta();    //GPRS没有就绪
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
		/*yangfei added for 每次重启时要挂起GPRS接收任务*/
		PRIO_TASK_GPRS_IPD = 0;/*挂起GPRS接收任务*/

		InitGprsRunSta();    //GPRS没有就绪时 ，TaskUpSend不会使用设备 UP_COMMU_DEV_AT

		//SuspendGprsRecTask();//挂起GPRS接收任务 

		tmpmid = GetGprsRunSta_ModuId();

		if (tmpmid == MODU_ID_XMZ) {
			GprsInit_xmz(); //使用设备 UP_COMMU_DEV_AT
			debug_info(gDebugModule[GPRS_MODULE], "GPRS  XMZ  init OK!! \n");
		} else {
			debug_err(gDebugModule[GPRS_MODULE], "GPRS MODU_ID_UNKNOWN!! \n");
		}
		UpdGprsRunSta_Ready(TRUE);
		//gprs ready后，设备 UP_COMMU_DEV_AT的使用权交给TaskUpSend，下面本任务将
		//定时检查GPRS状态，确定是否需要重新启动，查询GPRS状态使用的是设备UP_COMMU_DEV_AT，故本任务不直接使用，
		//而是向TaskUpSend发出请求，TaskUpSend将结果写入全局状态信息
		//是否本地口也限制？也就是显示写串口1

		PRIO_TASK_GPRS_IPD = 1;	  	//恢复GPRS接收任务
		//gjt modifed  test
		//#if TEST_SELF_CHECK ==0 && TEST_GPRS_EN == 0
		//#if 0
		if (ConnectConfirm() == FALSE) {
			//DlyLandFail();
			continue;
		}
		//#endif
		//#endif
		//OSMboxPost(HeartFrmMbox,(void*)1);//通知主动上报任务发心跳帧,第1帧（登陆帧）
		//==========如果发完登陆帧后马上还要发1个心跳帧,则应加下面两句,否则,在1个心跳周期后才开始发心跳帧
		//OSTimeDly(OS_TICKS_PER_SEC);
		//OSMboxPost(HeartFrmMbox,(void*)3);////通知主动上报任务发心跳帧
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
				//========发送消息，通知发送任务检查GPRS状态，结果填入gGprsRunSta
				gGPRSBusy = 1;
				/*begin:yangfei deleted 2013-08-29 for 暂时不需要，以后再看情况添加*/
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

			//不一定需要 当模块返回发送失败时，不会喂狗，这个值就会超过HeartFrmSndCycles，越来越大
#if 0
			if(GprsRunSta.SndDog>HeartFrmSndCycles+5) {
#if TEST_SELF_CHECK == 0
				debug_err(gDebugModule[GPRS_MODULE],"WARNING: [TaskGprsMana] Heart Frame Send Failure! Modul Restart!\n");
				break;	  	//重新启动GPRS
#endif
			}
#endif

#if 0  /*接收数据的时候也不发心跳*/
			if(GprsRunSta.SndDog>=HeartFrmSndCycles)
#else
			if (GprsRunSta.SndDog >= HeartFrmSndCycles
					&& GprsRunSta.RecDog >= HeartFrmSndCycles)
#endif
							{

#if 1    /*yf:needed */
				debug_info(gDebugModule[GPRS_MODULE],
						"[%s][%s][%d] sem_post NOTIFY 心跳\n", FILE_LINE);
				gGPRSBusy = 0;
				// gjt add 0607 for xml send
				sem_wait(&Xml_Send_Sem);
				// gjt add end
				gGPRS_XMLType = NOTIFY;
				sem_post(&ReportUp_Sem);

#endif
			}
			continue;

			restart: UpdGprsRunSta_Cont(FALSE);/*GPRS 断线*/
#if   TEST_SELF_CHECK == 0 && TEST_GPRS_EN == 0
			break;/*重新启动GPRS*/
#endif

		}
	}
}

