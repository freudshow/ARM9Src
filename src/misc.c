#include "includes.h"

uint8 gDebugLevel = 1;
uint8 gDebugModule[20];

sem_t CRITICAL_sem;
sem_t GprsXmzSem;
sem_t UpSend_Sem; /*数据上报信号量*/
sem_t GprsMana_Sem;

sem_t XMLRcvFile_Sem;
sem_t XMLSendFile_Sem;

sem_t ReportUp_Sem; /* 主动上报信号量*/
struct sem_value {
	sem_t sem;
	int32 value;
} gReportUp_Sem;
/******* gjt add 06061156 **********/
sem_t Xml_Send_Sem;
sem_t Xml_Deal_Sem; /* 有xml有效数据需要处理 通知处理线程 */
sem_t GPRS_Send_Succ_Sem; /* GPRS 发送数据成功信号 */
sem_t Xml_Continuous_Sem; /* 断点续传成功信号 */
/******* gjt add end ***************/

pthread_mutex_t mutex;
pthread_cond_t cond;
/******* add by yangmin 2013.08.29 **********/
sem_t mbus_Sem;
sem_t m485_Sem;
/******* add by yangmin 2013.08.29 **********/
void OSTimeDly(uint32 ms) {
	usleep(ms * 1000);
}

void OS_ENTER_CRITICAL(void) {
	int ret, value = 0;
	ret = sem_getvalue(&CRITICAL_sem, &value);
	if (ret) {
		debug_err(gDebugModule[MSIC_MODULE], "[%s][%s][%d]value=%d \n",
				FILE_LINE, ret);
	}
	debug_debug(gDebugModule[MSIC_MODULE], "[%s][%s][%d]value=%d \n", FILE_LINE,
			value);
    printf("CRITICAL_sem: %d\n", value);
	sem_wait(&CRITICAL_sem); /*减一*/
}

void OS_EXIT_CRITICAL(void) {
	int ret = 0, value = 0;
	ret = sem_getvalue(&CRITICAL_sem, &value);
	if (ret) {
		printf("[%s][%s][%d]value=%d \n", FILE_LINE, ret);
	}
	debug_debug(gDebugModule[MSIC_MODULE], "[%s][%s][%d]value=%d \n", FILE_LINE,
			value);
	sem_post(&CRITICAL_sem); /*加一*/
}

//void  OSSemPend (pthread_cond_t  *cond,uint32  timeout,uint8 *perr)
#if 0
void OSSemPend (uint8 dev,uint32 timeout,uint8 *perr)
{
	int ret=0;
	struct timespec outtime;
	struct timeval now;

	pthread_mutex_lock(&QueueMutex[dev]);
	gettimeofday(&now,NULL);
	//outtime.tv_sec = time(0)+timeout/1000;
	//outtime.tv_nsec = (timeout%1000)*1000*1000;
	outtime.tv_sec = now.tv_sec+timeout/1000;
	//outtime.tv_nsec = now.tv_usec*1000 + (timeout%1000)*1000*1000;
	outtime.tv_nsec = (timeout%1000)*1000*1000;
	//printf("ossem  timeout  is right!\n ");
	if(timeout == (uint32)NULL)
	{
		pthread_cond_wait(&QueueSems[dev],&QueueMutex[dev]);
	}
	else
	{
		ret=pthread_cond_timedwait(&QueueSems[dev],&QueueMutex[dev],&outtime);
		if(ret != 0) {
			debug_warn(gDebugModule[MSIC_MODULE],"[%s][%s][%d]pthread_cond_timedwait %s\n",FILE_LINE,strerror(ret));
		}
	}
	*perr = (uint8)ret;
	pthread_mutex_unlock(&QueueMutex[dev]);

}
#endif
/*

 yangfei 
 2013-07-05

 */
void OSSemPend(uint8 dev, uint32 timeout, uint8 *perr) {
	int ret = 0;
	struct timespec outtime;
	struct timeval now;

	gettimeofday(&now, NULL);

	outtime.tv_sec = now.tv_sec + timeout / 1000;
	outtime.tv_nsec = now.tv_usec + (timeout % 1000) * 1000 * 1000;

	outtime.tv_sec += outtime.tv_nsec / (1000 * 1000 * 1000);
	outtime.tv_nsec %= (1000 * 1000 * 1000);
	if (timeout == (uint32) NULL) {
		sem_wait(&QueueSems[dev]);
	} else {
		ret = sem_timedwait(&QueueSems[dev], &outtime);
		if (ret != 0) {
			debug_err(gDebugModule[MSIC_MODULE],
					"[%s][%s][%d]sem_timedwait %s\n", FILE_LINE,
					strerror(errno));
		}
	}
	*perr = (uint8) ret;
}

void OSSemPost(uint8 dev) {
	sem_post(&QueueSems[dev]); /*加一*/
}
/**/
void Misc_Init(void) {
	int ret = 0;

	ret = sem_init(&CRITICAL_sem, 0, 1);
	if (ret != 0) {
		perror("sem_init\n");
	}
	ret = sem_init(&GprsXmzSem, 0, 1);
	if (ret != 0) {
		perror("sem_init\n");
	}
	ret = sem_init(&UpSend_Sem, 0, 0);
	if (ret != 0) {
		perror("sem_init\n");
	}
	ret = sem_init(&GprsMana_Sem, 0, 0);
	if (ret != 0) {
		perror("sem_init\n");
	}
	ret = sem_init(&XMLRcvFile_Sem, 0, 0);
	if (ret != 0) {
		perror("sem_init\n");
	}
	ret = sem_init(&XMLSendFile_Sem, 0, 0);
	if (ret != 0) {
		perror("sem_init\n");
	}
	ret = sem_init(&ReportUp_Sem, 0, 0);
	if (ret != 0) {
		perror("sem_init\n");
	}
	ret = sem_init(&Xml_Send_Sem, 0, 1);
	if (ret != 0) {
		perror("sem_init\n");
	}
	ret = sem_init(&Xml_Deal_Sem, 0, 0);
	if (ret != 0) {
		perror("sem_init\n");
	}
	ret = sem_init(&GPRS_Send_Succ_Sem, 0, 0);
	if (ret != 0) {
		perror("sem_init\n");
	}
	ret = sem_init(&Xml_Continuous_Sem, 0, 0);
	if (ret != 0) {
		perror("sem_init\n");
	}
	ret = pthread_mutex_init(&mutex, NULL);
	if (ret != 0) {
		perror("pthread_mutex_init mutex error\n");
	}

	ret = sem_init(&UpRecQueSem_Gprs, 0, 0);
	if (ret != 0) {
		perror("UpRecQueSem_Gprs  error\n");
	}
	ret = sem_init(&USART3RecQueSem_At, 0, 0);
	if (ret != 0) {
		perror("USART3RecQueSem_At  error\n");
	}
	ret = sem_init(&USART3RecQueSem_AtIPD, 0, 0);
	if (ret != 0) {
		perror("USART3RecQueSem_AtIPD  error\n");
	}
	ret = sem_init(&Sequence_XML_sem, 0, 0);
	if (ret != 0) {
		perror("USART3RecQueSem_AtIPD  error\n");
	}
	ret = sem_init(&Result_XML_sem, 0, 0);
	if (ret != 0) {
		perror("USART3RecQueSem_AtIPD  error\n");
	}

	ret = sem_init(&mbus_Sem, 0, 1);
	if (ret != 0) {
		perror("mbus_Sem  error\n");
	}

	ret = sem_init(&m485_Sem, 0, 1);
	if (ret != 0) {
		perror("485_Sem  error\n");
	}

	Gprs_QueueCreate();

	UpcomInit();

	printf("this is in  ...[%s][%s][%d] \n", FILE_LINE);

	memset(gDebugModule, 3, sizeof(gDebugModule));
	gDebugModule[GPRS_MODULE] = 6;
	gDebugModule[MSIC_MODULE] = 1;
	//gDebugModule[TIMING_METER]=7;
	gDebugModule[XML_MODULE] = 6;
	gDebugModule[XML_DEAL] = 6;
	gDebugModule[METER] = 6;

	XMLFileInit(&gXML_Rcv_File, RecFileName);
	XMLFileInit(&gXML_Send_File, SendFileName);

}

void XMLFileInit(XML_FILE *XML_file, char *FileName[]) {
	int i, ret;
	int value;
	XML_file->readpos = 0;
	XML_file->writepos = 0;
	ret = sem_init(&XML_file->sem_read, 0, 0);
	if (ret != 0) {
		perror("sem_init\n");
	}
	ret = sem_getvalue(&XML_file->sem_read, &value);
	if (ret != 0) {
		printf("[%s][%s][%d] %s \n", FILE_LINE, strerror(errno));
	}
	debug_debug(gDebugModule[XML_MODULE], "[%s][%s][%d]sem_read=%d \n",
			FILE_LINE, value);

	ret = sem_init(&XML_file->sem_write, 0,
			sizeof(XML_file->XMLFile) / sizeof(XML_file->XMLFile[0]) - 1);
	if (ret != 0) {
		perror("sem_init\n");
	}
	ret = sem_getvalue(&XML_file->sem_write, &value);
	if (ret != 0) {
		printf("[%s][%s][%d] %s \n", FILE_LINE, strerror(errno));
	}
	debug_debug(gDebugModule[XML_MODULE], "[%s][%s][%d]sem_write=%d \n",
			FILE_LINE, value);
	for (i = 0; i < sizeof(XML_file->XMLFile) / sizeof(XML_file->XMLFile[0]);
			i++) {
		XML_file->XMLFile[i] = FileName[i];
		//printf("%s\n",XML_file->XMLFile[i]);
	}

}

void ShowTime(void) {
	time_t timep;
	struct tm *p;

	time(&timep);
	p = localtime(&timep);
	printf("%d:%d:%d ", (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday);
	printf("%d:%d:%d\n", p->tm_hour, p->tm_min, p->tm_sec);

	time(&timep);
	p = gmtime(&timep);
	printf("%d:%d:%d ", (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday);
	printf("%d:%d:%d\n", p->tm_hour, p->tm_min, p->tm_sec);

}

void MD5_Password(char*password) {
	MD5_CTX Md5Ctx;
	char md5_pw[256];
	uint8 HASH[MD5_LBLOCK];
	MD5_Init(&Md5Ctx);

	MD5_Update(&Md5Ctx, (char*) password, strlen(password));/*MD5加密*/

	MD5_Final(HASH, &Md5Ctx);/*将加密后的密文放到HASH中*/

	PUBLIC_HexToString(HASH, sizeof(HASH), md5_pw, 0);
	printf("[%s][%s][%d]%s \n", FILE_LINE, md5_pw);
}

void sem_init_ext(struct sem_value *psem_value, sem_t sem, int32 value) {
	psem_value->sem = sem;
	psem_value->value = value;
}

void sem_post_ext(struct sem_value *psem_value, int32 value) {

	psem_value->value = value;
	sem_post(&psem_value->sem);
}

int32 sem_pend_ext(struct sem_value *psem_value) {

	sem_wait(&psem_value->sem);

	return (psem_value->value);
}

