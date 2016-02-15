#ifndef _MISC_H
#define _MISC_H

#define OS_TICKS_PER_SEC       1000u   /* Set the number of ticks in one second                        */
#define OS_ERR_NONE                 0u
#define FALSE					 0
#define TRUE					 1
#define GPRS_ECHO

#define  FILE_LINE   __FILE__,__FUNCTION__,__LINE__

typedef char int8;
typedef short int16;
typedef int int32;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

/*=====================*/
extern sem_t CRITICAL_sem;
extern sem_t GprsXmzSem;
extern sem_t UpSend_Sem; /*数据上报信号量*/
extern sem_t GprsMana_Sem;
extern sem_t XMLRcvFile_Sem;
extern sem_t XMLSendFile_Sem;
extern sem_t ReportUp_Sem;
extern sem_t Xml_Deal_Sem;
extern sem_t Xml_Send_Sem;
extern sem_t gGPRS_Send_Succ_Sem;
extern sem_t Xml_Continuous_Sem;
extern sem_t mbus_Sem;
extern sem_t m485_Sem;
extern pthread_mutex_t mutex;
extern pthread_cond_t cond;
/*begin:yangfei added*/
#define       TASKUP_MODULE    0    /*集中器与主站的通信数据*/
#define       FATFS_MODULE    1
#define       SD_MODULE    2
#define       GPRS_MODULE    3
#define       TASKDOWN_MODULE    4  /*集中器与热表的通信数据*/
#define       UPDATE_MODULE    5
#define       ALL_MODULE    6
#define 	    XML_MODULE	7

#define	     TIMING_METER 8		//定时抄表

#define 	   XML_DEAL		9		//xml 数据处理
#define      MSIC_MODULE    10
#define      QUEUE_MODULE    11
#define	     METER 12		/*抄表*/
#define        SEM     13

#define	KERN_EMERG	0	/* system is unusable			*/
#define	KERN_ALERT	1	/* action must be taken immediately	*/
#define	KERN_CRIT	    2	/* critical conditions			*/
#define	KERN_ERR	    3	/* error conditions			*/
#define	KERN_WARNING	4	/* warning conditions			*/
#define	KERN_NOTICE	5	/* normal but significant condition	*/
#define	KERN_INFO	6	/* informational			*/
#define	KERN_DEBUG	7	/* debug-level messages			*/

/*begin:yangfei added*/
extern uint8 gDebugLevel;
extern uint8 gDebugModule[20];
#define DEBUG   
#ifdef DEBUG
#define debug(fmt,args...)   if(gDebugLevel  >= 1) printf(fmt,##args)
#define debugX(level,fmt,args...)   if(gDebugLevel >= level) printf(fmt,##args);

#define debug_emerg(dev, fmt, args...)		\
	if(dev>=KERN_EMERG) debug(fmt,##args);
#define debug_alert(dev, fmt, args...)		\
	if(dev>=KERN_ALERT) debug(fmt,##args);
#define debug_crit(dev, fmt, args...)		\
	if(dev>=KERN_CRIT) debug(fmt,##args);
#define debug_err(dev, fmt, args...)		\
	if(dev>=KERN_ERR) debug(fmt,##args);
#define debug_warn(dev, fmt, args...)		\
	if(dev>=KERN_WARNING) debug(fmt,##args);
#define debug_notice(dev, fmt, args...)		\
	if(dev>=KERN_NOTICE) debug(fmt,##args);
#define debug_info(dev, fmt, args...)		\
	if(dev>=KERN_INFO) debug(fmt,##args);
#define debug_debug(dev, fmt, args...)		\
   if(dev>=KERN_DEBUG) debug(fmt,##args);
#else
#define debug(fmt,args...)   
#define debugX(level,fmt,args...)  
#define debug_emerg(dev, fmt, args...)
#define debug_alert(dev, fmt, args...)
#define debug_crit(dev, fmt, args...)
#define debug_err(dev, fmt, args...)	
#define debug_warn(dev, fmt, args...)
#define debug_notice(dev, fmt, args...)	
#define debug_info(dev, fmt, args...)	
#define debug_debug(dev, fmt, args...)	
#endif
/*end  :yangfei added*/

void OSTimeDly(uint32 ms);
void OS_ENTER_CRITICAL(void);
void OS_EXIT_CRITICAL(void);
void OSSemPend(uint8 dev, uint32 timeout, uint8 *perr);
void OSSemPost(uint8 dev);

extern void Misc_Init(void);

void ShowTime(void);

void MD5_Password(char*password);

#endif
