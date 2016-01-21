#include "includes.h"

/*******************************************************************************/
unsigned char g_aucServerUartRBuf[SERVER_RCV_BUFF_LEN]; //,UART0R_Buf[rcv0_buf_len];
/*本表的地址*/
unsigned char g_aucServerFamenadd[6] = { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
UINT32 g_ucServerRcvDateLen = 0;
UINT32 g_uiServerFrameLen = 0;
unsigned char g_ucServerHeadLen = 0;
unsigned char g_ucServerRcvState = 0;

//考虑IPDATA连续两次以上发过来,由于对列没有做满了等待而是丢掉,这里定义的比较大

/****************************************************************************/

/*

 */
uint8 Gprs_QueueCreate(void) {

	if (QueueCreate((void *) UpRecQueue_Gprs, sizeof(UpRecQueue_Gprs), NULL,
			NULL) == NOT_OK) {
		while (1) {
			printf("UpRecQueue_Gprs ...[%s][%s][%d] \n", FILE_LINE);
		}
	}
	if (QueueCreate((void *) USART3RecQueue_At, sizeof(USART3RecQueue_At), NULL,
			NULL) == NOT_OK) {
		while (1) {
			printf("USART3RecQueue_At ...[%s][%s][%d] \n", FILE_LINE);
		}
		printf("Max  **** USART3RecQueue_At Max byte num is %d\n",
				QueueSize(USART3RecQueue_At));
	}

	if (QueueCreate((void *) USART3RecQueue_AtIPD, sizeof(USART3RecQueue_AtIPD),
			NULL, NULL) == NOT_OK) {
		while (1) {
			printf("USART3RecQueue_AtIPD ...[%s][%s][%d] \n", FILE_LINE);
		}
	}
	printf("Max  **** USART3RecQueue_AtIPD Max byte num is %d\n",
			QueueSize(USART3RecQueue_AtIPD));
	printf("the USART3RecQueue_AtIPD sizeof is %d\n",
			sizeof(USART3RecQueue_AtIPD));

	return 0;
}
/************************************************************************************************
 注意:处理接受到服务器下发的表地址
 参数1:收到数据的地址(SERVER_HEAD_CMD_PT )
 返回 CONTROL_RET_FAIL 发送失败， CONTROL_RET_SUC 发送成功
 ************************************************************************************************/
INT32 deal_server_send_addr(SERVER_HEAD_CMD_PT * pstServerFrame) {
	if (pstServerFrame == NULL) {
		//jilog
		return (CONTROL_RET_FAIL);
	}

	return (CONTROL_RET_SUC);
}

/************************************************************************************************
 注意:处理接收到SERVER的数据包
 参数1:指针(g_aucServerUartRBuf)接受到数据帧后，存到此数据
 返回 CONTROL_RET_FAIL 发送失败， CONTROL_RET_SUC 发送成功
 ************************************************************************************************/
INT32 server_comm_proc(UINT8 * paucServerFrame) {
	//INT32 uiTempNumber = 0;

	SERVER_HEAD_CMD_PT pstServerCmdFrame = (SERVER_HEAD_CMD_PT) paucServerFrame;

	switch (pstServerCmdFrame->ucFrameMT) {
	/* 服务器配置集中器*/
	case SERVER_SET_CONTROL:

		break;
		/* 服务器下发表地址 */
	case SERVER_SET_INSTRUMENT_ADDR:

		break;
		/* 上传表数据返回*/
	case CONTROL_SEND_DATE_RET:
		break;
	default:
		break; /*出错处理*/
	}

	return (CONTROL_RET_SUC);
}

/************************************************************************************************
 注意:计算CRC的值
 参数1:要组包的存储区域指针
 参数2:数据域的长度
 返回 CONTROL_RET_FAIL 发送失败， CONTROL_RET_SUC 发送成功
 ************************************************************************************************/
UINT8 server_get_crc(UINT8 *pstCommd, UINT8 ucLen) {
	UINT32 u32Crc = 0;
	UINT8 ucTempNumber = 4;

	if (pstCommd == NULL) {
		return CONTROL_RET_FAIL;
	}
	while (ucTempNumber <= 13) {
		u32Crc = u32Crc + pstCommd[ucTempNumber++];
	}

	while (ucLen--) {
		u32Crc = u32Crc + pstCommd[ucTempNumber++];
	}

	return ((UINT8) u32Crc);
}

#if 0
/************************************************************************************************
 组包发送抄表指令，地址在每次开机时从bak区取回赋值，所以组包时不用赋值
 参数1:要组包的存储区域指针
 返回 CONTROL_RET_FAIL 发送失败， CONTROL_RET_SUC 发送成功
 ************************************************************************************************/
int elect97_meter_packag(void * pCommd, UINT8 *pucOutLen)
{
	UINT8 ucTempLoop = 0;
	ELECTR97_METER_CMD_PT pstTempCommd = NULL;
	if(pCommd == NULL)
	{
		return CONTROL_RET_FAIL;
	}

	pstTempCommd = pCommd;

	pstTempCommd->ucLeadFe1 = 0xfe;
	pstTempCommd->ucLeadFe2 = 0xfe;
	pstTempCommd->ucLeadFe3 = 0xfe;
	pstTempCommd->ucLeadFe4 = 0xfe;
	pstTempCommd->ucHead = 0x68;

	pstTempCommd->ucFrameBegin = 0x68;
	pstTempCommd->ucControlCode = 0x01;
	pstTempCommd->ucDateLen = 0x02;

	for(ucTempLoop = 5; ucTempLoop < 11; ucTempLoop++)
	{
		((UINT8 *)pstTempCommd)[ucTempLoop] = g_aucElect97MeterFamenadd[ucTempLoop - 5];
	}
	pstTempCommd->aucDate[0] = 0x43;
	pstTempCommd->aucDate[1] = 0xc3;
	pstTempCommd->aucDate[2] = elect97_neter_get_crc((UINT8 *)pCommd, 0x02);
	pstTempCommd->aucDate[3] = 0x16;
	*pucOutLen = ELECT97_METER_COMMOND_BASE_LEN+ pstTempCommd->ucDateLen + ELECT97_METER_COMMOND_FE_NUMBER +2;

	return CONTROL_RET_SUC;
}
/************************************************************************************************
 发送抄表热表通过Mbus       
 参数1:设备的描述符，在主线程中准备好，在此使用     
 返回 CONTROL_RET_FAIL 发送失败， CONTROL_RET_SUC 发送成功
 ************************************************************************************************/
INT32 elect97_meter_com_send(INT32 u32DeviceFd)
{
	UINT8 ucCommdLen = 0;
	elect97_meter_packag(&g_stElect97_Meter_Commd, &ucCommdLen);
	write(u32DeviceFd, (UINT8 *)&g_stElect97_Meter_Commd, ucCommdLen);

	return 0;
}

#endif
INT32 Gprs_com_send(INT32 u32DeviceFd) {
	char Ats_AT[] = "AT\r";

	write(u32DeviceFd, Ats_AT, strlen(Ats_AT));

	return 0;
}
/************************************************************************************************
 德鲁热表的接受函数
 参数1:设备的描述符，在主线程中准备好，在此使用     
 返回 CONTROL_RET_FAIL 发送失败， CONTROL_RET_SUC 发送成功
 ************************************************************************************************/
INT32 server_com_rev(INT32 u32DeviceFd) {
	unsigned char c;
	INT8 cLoopFlag = 3;
	fd_set ReadSetFD;
	uint8 ret;
	while (1) {
		FD_ZERO(&ReadSetFD);
		FD_SET(u32DeviceFd, &ReadSetFD);
		/* 设置等待的超时时间 */
		if (select(u32DeviceFd + 1, &ReadSetFD, NULL, NULL, NULL) <= 0) {
			printf(" [%s][%s][%d]Server REV is time out .....\n", FILE_LINE);
			cLoopFlag--;
			/* 超时处理*/
		}
		if (FD_ISSET(u32DeviceFd, &ReadSetFD)) {
			if (read(u32DeviceFd, &c, 1) == 1) {
				//printf("[%s][%s][%d]  c= %c \n",FILE_LINE,c);
				ret = QueueWrite((void*) USART3RecQueue_At, c);
				if (ret != QUEUE_OK) {
					printf("[%s][%s][%d] ret=%d queue full\n", FILE_LINE, ret);
				}
				ret = QueueWrite((void*) USART3RecQueue_AtIPD, c);
				if (ret != QUEUE_OK) {

					printf(
							"[%s][%s][%d] ret=%d queue full  The Queue has data Num is %d\n",
							FILE_LINE, ret, QueueNData(USART3RecQueue_AtIPD));
				}
				printf("%c", c);
				OSSemPost(UP_COMMU_DEV_AT);
				OSSemPost(UP_COMMU_DEV_ATIPD);
			}
		}
	}
	return (CONTROL_RET_FAIL);
}

