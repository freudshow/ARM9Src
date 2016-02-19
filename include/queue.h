/****************************************Copyright (c)**************************************************
 **                               广州周立功单片机发展有限公司
 **                                     研    究    所
 **                                        产品一部 
 **
 **                                 http://www.zlgmcu.com
 **
 **--------------文件信息--------------------------------------------------------------------------------
 **文   件   名: queue.h
 **创   建   人: 陈明计
 **最后修改日期: 2003年7月2日
 **描        述: 数据队列的中间件
 **              
 **--------------历史版本信息----------------------------------------------------------------------------
 ** 创建人: 陈明计
 ** 版  本: v1.0
 ** 日　期: 2003年7月2日
 ** 描　述: 原始版本
 **
 **--------------当前版本修订------------------------------------------------------------------------------
 ** 修改人: 陈明计
 ** 日　期: 2004年5月19日
 ** 描　述: 改正注释错误
 **
 **------------------------------------------------------------------------------------------------------
 ********************************************************************************************************/
#ifndef				_QUEUE_H
#define				_QUEUE_H

#include "includes.h"

/* 数据队列的配置 */
#define QUEUE_DATA_TYPE           uint8
#define EN_QUEUE_WRITE            1     /* 禁止(0)或允许(1)FIFO发送数据       */
#define EN_QUEUE_WRITE_FRONT      0     /* 禁止(0)或允许(1)LIFO发送数据       */
#define EN_QUEUE_NDATA            1     /* 禁止(0)或允许(1)取得队列数据数目   */
#define EN_QUEUE_SIZE             1     /* 禁止(0)或允许(1)取得队列数据总容量 */
#define EN_QUEUE_FLUSH            1     /* 禁止(0)或允许(1)清空队列           */

//#define NULL 0
//

#ifndef NOT_OK
#define NOT_OK              0xff                        /* 参数错误                                     */
#endif

#define QUEUE_FULL          8                           /* 队列满                                       */
#define QUEUE_EMPTY         4                           /* 无数据                                       */
#define QUEUE_OK            0                           /* 操作成功                                     */

#define Q_WRITE_MODE        1                           /* 操作成功                                     */
#define Q_WRITE_FRONT_MODE  2                           /* 操作成功                                     */

#ifndef QUEUE_DATA_TYPE
#define QUEUE_DATA_TYPE     uint8
#endif

typedef struct {
	QUEUE_DATA_TYPE *Out; /* 指向数据输出位置         */
	QUEUE_DATA_TYPE *In; /* 指向数据输入位置         */
	QUEUE_DATA_TYPE *End; /* 指向Buf的结束位置        */
	uint32 NData; /* 队列中数据个数           */
	uint32 MaxData; /* 队列中允许存储的数据个数 */

	uint8 (*ReadEmpty)(); /* 读空处理函数             */
	uint8 (*WriteFull)(); /* 写满处理函数             */
	QUEUE_DATA_TYPE Buf[1]; /* 以上成员占有的字节数   */
} DataQueue;

#ifndef IN_QUEUE
#ifdef __cplusplus
extern "C" {
#endif
extern uint8 QueueCreate(void *Buf, uint32 SizeOfBuf, uint8 (*ReadEmpty)(),
		uint8 (*WriteFull)());
/*********************************************************************************************************
 ** 函数名称: QueueCreate
 ** 功能描述: 初始化数据队列
 ** 输　入: Buf      ：为队列分配的存储空间地址
 **         SizeOfBuf：为队列分配的存储空间大小（字节）
 **         ReadEmpty：为队列读空时处理程序
 **         WriteFull：为队列写满时处理程序
 ** 输　出: NOT_OK  :参数错误
 **         QUEUE_OK:成功
 ** 全局变量: 无
 ** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
 ********************************************************************************************************/

extern uint8 QueueRead(QUEUE_DATA_TYPE *Ret, void *Buf);
/*********************************************************************************************************
 ** 函数名称: QueueRead
 ** 功能描述: 获取队列中的数据
 ** 输　入: Ret:存储返回的消息的地址
 **         Buf:指向队列的指针
 ** 输　出: NOT_OK     ：参数错误
 **         QUEUE_OK   ：收到消息
 **         QUEUE_EMPTY：队列空
 ** 全局变量: 无
 ** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
 ********************************************************************************************************/

extern uint8 QueueRead_Int(QUEUE_DATA_TYPE *Ret, void *Buf);
/*********************************************************************************************************
 ** 函数名称: QueueRead_Int
 ** 功能描述: 获取队列中的数据,此函数只能在关中断状态下调用，与函数QueueRead的区别仅是去掉了关开中断
 ** 输　入: Ret:存储返回的消息的地址
 **         Buf:指向队列的指针
 ** 输　出: NOT_OK     ：参数错误
 **         QUEUE_OK   ：收到消息
 **         QUEUE_EMPTY：无消息
 ** 全局变量: 无
 ** 调用模块: 
 ********************************************************************************************************/
extern uint8 QueueWrite(void *Buf, QUEUE_DATA_TYPE Data);
/*********************************************************************************************************
 ** 函数名称: QueueWrite
 ** 功能描述: FIFO方式发送数据
 ** 输　入: Buf :指向队列的指针
 **         Data:发送的数据
 ** 输　出: NOT_OK   ：参数错误
 **         QUEUE_FULL:队列满
 **         QUEUE_OK  :发送成功
 ** 全局变量: 无
 ** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
 ********************************************************************************************************/
extern uint8 QueueWrite_Int(void *Buf, QUEUE_DATA_TYPE Data);
/*********************************************************************************************************
 ** 函数名称: QueueWrite_Int
 ** 功能描述: FIFO方式发送数据,此函数只能在关中断状态下调用，与函数QueueWrite的区别仅是去掉了关开中断
 ** 输　入: Buf :指向队列的指针
 **         Data:消息数据
 ** 输　出: NOT_OK   :参数错误
 **         QUEUE_FULL:队列满
 **         QUEUE_OK  :发送成功
 ** 全局变量: 无
 ** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
 ********************************************************************************************************/

extern void QueueWrite_Int_f(void *Buf, QUEUE_DATA_TYPE Data);
/*********************************************************************************************************
 ** 函数名称: QueueWrite_Int_f
 ** 功能描述: FIFO方式发送数据,此函数只在关中断状态的中断服务程序中调用，与函数QueueWrite的区别仅是去掉了关开中断,
 **           并去掉了参数的合法性判断,和返回值. 因此,特别注意,必须保证在中断使能前确保中断服务程序中使用的队列已经初始化.
 ** 输　入: Buf :指向队列的指针
 **         Data:消息数据
 ** 输　出:   无    
 ** 全局变量: 无
 ** 调用模块: 
 ********************************************************************************************************/

extern uint8 QueueWriteFront(void *Buf, QUEUE_DATA_TYPE Data);
/*********************************************************************************************************
 ** 函数名称: QueueWrite
 ** 功能描述: LIFO方式发送数据
 ** 输　入: Buf:指向队列的指针
 **         Data:消息数据
 ** 输　出: QUEUE_FULL:队列满
 **         QUEUE_OK:发送成功
 ** 全局变量: 无
 ** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
 ********************************************************************************************************/

extern uint16 QueueNData(void *Buf);
/*********************************************************************************************************
 ** 函数名称: QueueNData
 ** 功能描述: 取得队列中数据数
 ** 输　入: Buf:指向队列的指针
 ** 输　出: 消息数
 ** 全局变量: 无
 ** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
 ********************************************************************************************************/

extern uint16 QueueSize(void *Buf);
/*********************************************************************************************************
 ** 函数名称: QueueSize
 ** 功能描述: 取得队列总容量
 ** 输　入: Buf:指向队列的指针
 ** 输　出: 队列总容量
 ** 全局变量: 无
 ** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
 ********************************************************************************************************/

extern void QueueFlush(void *Buf);
/*********************************************************************************************************
 ** 函数名称: OSQFlush
 ** 功能描述: 清空队列
 ** 输　入: Buf:指向队列的指针
 ** 输　出: 无
 ** 全局变量: 无
 ** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
 ********************************************************************************************************/

extern uint8 QueueNotEnoughPend(void* Buf, uint32 n);
/*********************************************************************************************************
 ** 函数名称: QueueNotEnoughPend
 ** 功能描述: 查看队列空间是否足够,若不够,则挂起等待
 ** 输　入: Buf:指向队列的指针;n: 要写入的数据个数
 ** 输　出: 无
 ** 全局变量: 无
 ** 调用模块:
 ********************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
#endif

/*********************************************************************************************************
 **                            End Of File
 ********************************************************************************************************/

