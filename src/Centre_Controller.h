#ifndef __CENTRE_CONTROLLER_H
#define __CENTRE_CONTROLLER_H

#include <stdio.h>
#include <stdlib.h>
#include <termio.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sqlite3/sqlite3.h>

#define METER_FUNCTIONID 0

#define INT8      char
#define UINT8    unsigned char
#define INT16    short
#define UINT16  unsigned short
#define INT32     int
#define UINT32  unsigned int

#define CONTROL_RET_FAIL -1
#define CONTROL_RET_SUC  0

/*定义设备文件名称*/
#define DEVICE4851	"/dev/ttyS1"
#define DEVICE4852	"/dev/ttyS2"
#define DEVICEGPRS	"/dev/ttyS3"
#define DEVICEMBUS	"/dev/ttyS6"           

#define IO_CONTROL  "/dev/CONTROL_IO"

#define SQLITE_NAME  "test.db"
/*begin:yangfei added 2013-08-27 for*/
#define IOCTLTEST_MAGIC    0xA4

#define  LED0_A30    _IO (IOCTLTEST_MAGIC, 0)
#define LED1_A31        _IO (IOCTLTEST_MAGIC, 1)
#define MBUS_C0    _IO (IOCTLTEST_MAGIC, 2)
#define MBUS_C1    _IO (IOCTLTEST_MAGIC, 3)
#define MBUS_C2    _IO (IOCTLTEST_MAGIC, 4)
#define MBUS_C3    _IO (IOCTLTEST_MAGIC, 5)
#define MBUS_C4    _IO (IOCTLTEST_MAGIC, 6)
#define MBUS_C5    _IO (IOCTLTEST_MAGIC, 7)

#define GPRS_SWITCH_C28    _IO (IOCTLTEST_MAGIC, 8)
#define GPRS_RESET_C29    _IO (IOCTLTEST_MAGIC, 9)

#define RS4851_C30    _IO (IOCTLTEST_MAGIC, 10)
#define RS4852_C31    _IO (IOCTLTEST_MAGIC, 11)

#define IOCTLTEST_MAXNR        12
/*end:yangfei added 2013-08-27 for*/
#if 0
/* IOCTL 的CMD */
#define LED0_A30   0x00
#define LED1_A31   0x01

#define MBUS_C0   0x12
#define MBUS_C1   0x13
#define MBUS_C2   0x14
#define MBUS_C3   0x15
#define MBUS_C4   0x16
#define MBUS_C5   0x17

#define GPRS_SWITCH_C28   0x08
#define GPRS_RESET_C29   0x09

#define RS4851_C30   0x0a
#define RS4852_C31   0x0b
#endif
/* 485 发送和接受*/
#define RS_485_RECV  0
#define RS_485_SED  1

#define PAR_NONE  0
#define PAR_EVEN  1

#if 0
/* Mbus 串口的配置信息 */
typedef struct
{
	INT32 iMbusByteBits;
	INT32 iMbusDeviceSpeed;
	UINT8 aucMbusDeviceName[15];
}MBUS_INFO_T;

/* COM485 串口的配置信息 */
typedef struct
{
	INT32 iCom485ByteBits;
	INT32 iCom485DeviceSpeed;
	UINT8 aucCom485DeviceName[15];
}COM485_INFO_T;
#endif

typedef struct {
	INT32 ByteBits;INT32 Speed;UINT8 Name[15];
} COM_INFO_T;

struct mdev_meterinfo {
	INT32 fd;INT32 channel;
};

extern INT32 g_uiMbusFd;
extern char gReadingDate[20];
extern volatile INT32 g_uiIoControl;

/* 初始化抄表流程	1:抄485总线, 2 :抄MBUS	总线*/
/* 初始化抄表流程	1:抄485总线, 2 :抄MBUS	总线*/
extern volatile INT32 g_uiReadMeterStatus;
extern volatile INT32 g_uiDataDealStatus;
/*************************************************************************************/
/* 
 双向链表结构:
 head (头结点)                          tail (头结点)
 |                                                   |
 V                                                   V  
 [date]<==>[date]<==>[date]<==>.....[date]-->NULL(结尾指向NULL)
 */
/* 链表节点结构体*/
typedef struct LINK_NODE {
	void *pDate;
	struct LINK_NODE *pstLast;
	struct LINK_NODE *pstNext;
} SIGLE_LIST_T, *SIGLE_LIST_PT, **SIGLE_LIST_PPT;

/* 创建双向链表，即申请头指针 */
#define CREAT_LIST_HEAD(pstHead)  (SIGLE_LIST_T pstHead)
#define CREAT_LIST_TAIL(pstTail)  (SIGLE_LIST_T pstTail)  

extern sqlite3 *sqlitedb;

#endif
