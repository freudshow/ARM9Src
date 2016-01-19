#include "includes.h"
#include "commap.h"

//考虑IPDATA连续两次以上发过来,由于对列没有做满了等待而是丢掉,这里定义的比较大

uint8 *pQueues[UP_COMMU_DEV_ARRAY];
uint8 UpRecQueue_Gprs[GPRS_REC_QUEUES_LENGTH];
uint8 USART3RecQueue_At[USART3_REC_QUEUE_AT_LENGTH];
uint8 USART3RecQueue_AtIPD[USART3_REC_QUEUE_ATIPD_LENGTH];

sem_t QueueSems[UP_COMMU_DEV_ARRAY];
sem_t UpRecQueSem_Gprs;
sem_t USART3RecQueSem_At;
sem_t USART3RecQueSem_AtIPD;
sem_t Sequence_XML_sem;
sem_t Result_XML_sem;

#if 0
pthread_mutex_t QueueMutex[UP_COMMU_DEV_ARRAY];
pthread_mutex_t UpRecQueMutex_Gprs;
pthread_mutex_t USART3RecQueMutex_At;
pthread_mutex_t USART3RecQueMutex_AtIPD;
pthread_mutex_t Sequence_XML_mutex;
pthread_mutex_t Result_XML_mutex;
#endif

int8 gGprsRec[1024];

extern uint8 Gprs_QueueCreate(void);

/*------extern----*/
extern INT32 g_uiGprsFd;

uint8 UpcomInit(void) {
	pQueues[UP_COMMU_DEV_GPRS] = UpRecQueue_Gprs;
	pQueues[UP_COMMU_DEV_AT] = USART3RecQueue_At;
	pQueues[UP_COMMU_DEV_ATIPD] = USART3RecQueue_AtIPD;

	QueueSems[UP_COMMU_DEV_GPRS] = UpRecQueSem_Gprs;
	QueueSems[UP_COMMU_DEV_AT] = USART3RecQueSem_At;
	QueueSems[UP_COMMU_DEV_ATIPD] = USART3RecQueSem_AtIPD;
	QueueSems[SEQUEUE_XML] = Sequence_XML_sem;
	QueueSems[RESULT_XML] = Result_XML_sem;

	return 0;
}
/**/
uint8 UpGetEnd(uint8 dev, uint32 OutTime) {
	uint8 err = 0x00;
	int8 XMLEnd[] = "</root>"; /*</test>*/
	uint8 DataTemp = 0x00;
	uint8 Flag = 0x01;
	uint8 i;
	int j = 0;
	FILE *fp;

	while (Flag) {
		for (i = 0; i < strlen(XMLEnd); i++) {
			while (QueueRead(&DataTemp, (void*) pQueues[dev]) != QUEUE_OK) {
				OSSemPend(dev, OutTime, &err);
				if (err != OS_ERR_NONE) {
					printf("[%s][%s][%d]OSSemPend err=%d\n", FILE_LINE, err);
					return err;
				}
			}
			gGprsRec[j++] = DataTemp;
			if (j == (sizeof(gGprsRec) - 1)) {
				fp = fopen(gXML_Rcv_File.XMLFile[gXML_Rcv_File.writepos], "a+");
				debug_info(gDebugModule[GPRS_MODULE], "xml name is %s\n",
						gXML_Rcv_File.XMLFile[gXML_Rcv_File.writepos]);
				fwrite(gGprsRec, 1, strlen(gGprsRec), fp);
				debug_err(gDebugModule[GPRS_MODULE],
						"[%s][%s][%d] strlen(gGprsRec)=%d\n", FILE_LINE,
						strlen(gGprsRec));
				memset(gGprsRec, 0, sizeof(gGprsRec));
				j = 0;
				fclose(fp);
			}
			if (DataTemp == XMLEnd[i]) {
				;
			} else {
				i = 0;
				continue;
			}

		}
		Flag = 0;
	}
	if (j != 0) {
		fp = fopen(gXML_Rcv_File.XMLFile[gXML_Rcv_File.writepos], "a+");
		debug_info(gDebugModule[GPRS_MODULE], "xml name is %s\n",
				gXML_Rcv_File.XMLFile[gXML_Rcv_File.writepos]);
		fwrite(gGprsRec, 1, strlen(gGprsRec), fp);
		debug_err(gDebugModule[GPRS_MODULE],
				"[%s][%s][%d] strlen(gGprsRec)=%d\n", FILE_LINE,
				strlen(gGprsRec));
		memset(gGprsRec, 0, sizeof(gGprsRec));
		j = 0;
		fclose(fp);
	}
	return 0;
}
uint8 UpGetStart(uint8 dev, uint32 OutTime) {
	uint8 err = 0x00;
	int8 XMLHead[] = "<?xml version"; //=\"1.0\" encoding=\"utf-8\" ?>";
	uint8 DataTemp = 0x00;
	uint8 Flag = 0x01;
	uint8 i;
	FILE *fp;
	while (Flag) {
		for (i = 0; i < strlen(XMLHead); i++) {
			while (QueueRead(&DataTemp, (void*) pQueues[dev]) != QUEUE_OK) {
				OSSemPend(dev, OutTime, &err);
				if (err != OS_ERR_NONE) {
					printf("[%s][%s][%d]OSSemPend err=%d\n", FILE_LINE, err);
					return err;
				}
			}
			//printf("get start string is %s\n", DataTemp);

			if (DataTemp == XMLHead[i]) {
			} else {
				i = 0;
				continue;
			}
		}
		Flag = 0;
	}
	printf("start fopen %s  \n", gXML_Rcv_File.XMLFile[gXML_Rcv_File.writepos]);
	fp = fopen(gXML_Rcv_File.XMLFile[gXML_Rcv_File.writepos], "w+");
	fwrite(XMLHead, 1, strlen(XMLHead), fp);
	//printf("gGprsRec = %s\n",XMLHead);
	fclose(fp);
	return 0;
}

uint8 UpGetch(uint8 dev, uint8* data, uint16 OutTime) {
	uint8 err = 0x00;
	uint8 ret;
	while ((ret = QueueRead(data, (void*) pQueues[dev])) != QUEUE_OK) {
		//printf("[%s][%s][%d] ret = %d QUEUE_EMPTY \n",FILE_LINE,ret);
		OSSemPend(dev, (uint32) OutTime, &err);
		if (err != OS_ERR_NONE) {
			//debug_err(gDebugModule[GPRS_MODULE],"[%s][%s][%d]OSSemPend err=%d\n",FILE_LINE,err);
			return err;
		} else {
			//printf("[%s][%s][%d] err = %d\n",FILE_LINE,err);
		}
	}
	//printf("%c",*data);
	return 0;
}

uint8 UpQueueFlush(uint8 dev) {
	QueueFlush((void*) pQueues[dev]);
	return 0;
}
/*
 read n betys buf to dev
 */
//写上行接收队列,说明: 不是所有的操作都调用此函数进行,比如UART3接收队列的写操作没有调用本函数
uint8 UpRecQueueWrite(uint8 dev, uint8* buf, uint32 n) //不可重入
{
	uint32 i;

	for (i = 0; i < n; i++) {
		QueueWrite((void*) pQueues[dev], buf[i]);
	}
	OSSemPost(dev);
	return 0;
}

void UpDevSend(uint8 dev, uint8 *Data, uint32 n) /*不可重入*/
{
	//uint32 i, CirTimes, Residual;

	switch (dev) {
	case UP_COMMU_DEV_ZIGBEE:				//针对于Zigbee发送数据，每次100个字节，每次发送间隔333MS

		break;
	case UP_COMMU_DEV_485:

		break;

	case UP_COMMU_DEV_232:

		break;

	case UP_COMMU_DEV_GPRS:
		GprsSend(g_uiGprsFd, Data, n);
		break;

	default:
		while (1)
			;
	}
}
void GprsSend(int32 fd, uint8 *Data, uint32 n) {
	int status;
	OS_ENTER_CRITICAL();
	status = write(fd, Data, n);
	OS_EXIT_CRITICAL();
}
#if 0
uint8 DuQueueFlush(uint8 dev)
{
	uint8 tmp;
	switch (dev)
	{
		case DOWN_COMM_DEV_MBUS:
		//QueueFlush((void*)USART2RecQueue);			//MBUS接收队列清空
		break;
		/*begin:yangfei added 2013-03-25 for support 485 meter*/
		case DOWN_COMM_DEV_485:
		//QueueFlush((void*)UART5RecQueue);			//MBUS接收队列清空
		break;
		/*end:yangfei added 2013-03-25 */
		default :
		while(1);
	}
	return tmp;
}

uint8 DuGetch(uint8 dev,uint8* data,uint16 OutTime)
{
	uint8 tmp;
	switch (dev)
	{
		case DOWN_COMM_DEV_MBUS:
		//tmp=USART2Getch(data, OutTime); 				//MBUS抄表
		break;
		/*begin:yangfei added 2013-03-25 for support 485 meter*/
		case DOWN_COMM_DEV_485:
		//tmp=UART5Getch(data, OutTime); 				//485抄表
		break;
		/*end:yangfei added 2013-03-25 */
		default :
		while(1);
	}
	return tmp;
}
#endif
void DuSend(uint8 dev, uint8* buf, uint32 n) {
	switch (dev) {
	case DOWN_COMM_DEV_MBUS:
		//USART2Send(buf, n); 							//MBUS抄表---发送数据
		break;
		/*begin:yangfei added 2013-03-25 for support 485 meter*/
	case DOWN_COMM_DEV_485:
		//UART5Send(buf, n); 					//485抄表
		break;
		/*end:yangfei added 2013-03-25 */
	default:
		while (1)
			;
	}
}

/*********************************************************************************************************
 **                            End Of File
 ********************************************************************************************************/
