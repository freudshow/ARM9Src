/****************************************Copyright (c)****************************************************
 **                                      
 **                                 山东华宇空间技术公司(西安分部) 
 **
 **--------------File Info---------------------------------------------------------------------------------
 ** File name:               includes.h
 ** Descriptions:            includes.h for ucos
 **
 **--------------------------------------------------------------------------------------------------------

 ** Modified by:             
 ** Modified date:           
 ** Version:                 
 ** Descriptions:            
 **
 *********************************************************************************************************/
#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <openssl/md5.h>
#include <openssl/aes.h>
//抄表状态
#define STATU_READ_RS485	0x01
#define STATU_READ_MBUS		0x02

//数据收发状态
#define STATU_REV_DATA		0x10
#define STATU_REPORTUP		0x20
#define STATU_CONTINUOUS	0x40

#define STATU_IDEL			0x00

#define CFG_PERIOD	"15"
//#define CFG_IPADDR	"513xy.eicp.net"
#define CFG_IPADDR "122.5.18.174"
//#define CFG_IPADDR	"1.84.149.212"
#define CFG_PORT	"1147"

#define INIT_REPORT_PERIOD  15
#define INIT_SAMPLE_PERIOD  15

#define FIREWARE_VER "V14.0317"

#include "misc.h"
#include "publicfunc.h"
#include "gprs.h"
#include "queue.h"

#include "Centre_Controller.h"
#include "commap.h"
#include "Comm_with_server.h"
#include "tasks_up.h"
#include "gprs_mana.h"
#include "xml.h"
#include "DB_Segment.h"
#include "DELU_Heat_Meter.h"
#include "Elect97.h"
#include "CommandDeal.h"
#include "WaterMeter.h"

#endif
/*********************************************************************************************************
 END FILE
 *********************************************************************************************************/
