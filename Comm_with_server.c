#include "includes.h"

/*******************************************************************************/
unsigned char g_aucServerUartRBuf[SERVER_RCV_BUFF_LEN]; //,UART0R_Buf[rcv0_buf_len];
/*����ĵ�ַ*/
unsigned char g_aucServerFamenadd[6] = { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
UINT32 g_ucServerRcvDateLen = 0;
UINT32 g_uiServerFrameLen = 0;
unsigned char g_ucServerHeadLen = 0;
unsigned char g_ucServerRcvState = 0;

//����IPDATA�����������Ϸ�����,���ڶ���û�������˵ȴ����Ƕ���,���ﶨ��ıȽϴ�

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
 ע��:������ܵ��������·��ı��ַ
 ����1:�յ����ݵĵ�ַ(SERVER_HEAD_CMD_PT )
 ���� CONTROL_RET_FAIL ����ʧ�ܣ� CONTROL_RET_SUC ���ͳɹ�
 ************************************************************************************************/
INT32 deal_server_send_addr(SERVER_HEAD_CMD_PT * pstServerFrame) {
	if (pstServerFrame == NULL) {
		//jilog
		return (CONTROL_RET_FAIL);
	}

	return (CONTROL_RET_SUC);
}

/************************************************************************************************
 ע��:������յ�SERVER�����ݰ�
 ����1:ָ��(g_aucServerUartRBuf)���ܵ�����֡�󣬴浽������
 ���� CONTROL_RET_FAIL ����ʧ�ܣ� CONTROL_RET_SUC ���ͳɹ�
 ************************************************************************************************/
INT32 server_comm_proc(UINT8 * paucServerFrame) {
	//INT32 uiTempNumber = 0;

	SERVER_HEAD_CMD_PT pstServerCmdFrame = (SERVER_HEAD_CMD_PT) paucServerFrame;

	switch (pstServerCmdFrame->ucFrameMT) {
	/* ���������ü�����*/
	case SERVER_SET_CONTROL:

		break;
		/* �������·����ַ */
	case SERVER_SET_INSTRUMENT_ADDR:

		break;
		/* �ϴ������ݷ���*/
	case CONTROL_SEND_DATE_RET:
		break;
	default:
		break; /*������*/
	}

	return (CONTROL_RET_SUC);
}

/************************************************************************************************
 ע��:����CRC��ֵ
 ����1:Ҫ����Ĵ洢����ָ��
 ����2:������ĳ���
 ���� CONTROL_RET_FAIL ����ʧ�ܣ� CONTROL_RET_SUC ���ͳɹ�
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
 ������ͳ���ָ���ַ��ÿ�ο���ʱ��bak��ȡ�ظ�ֵ���������ʱ���ø�ֵ
 ����1:Ҫ����Ĵ洢����ָ��
 ���� CONTROL_RET_FAIL ����ʧ�ܣ� CONTROL_RET_SUC ���ͳɹ�
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
 ���ͳ����ȱ�ͨ��Mbus       
 ����1:�豸���������������߳���׼���ã��ڴ�ʹ��     
 ���� CONTROL_RET_FAIL ����ʧ�ܣ� CONTROL_RET_SUC ���ͳɹ�
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
 ��³�ȱ�Ľ��ܺ���
 ����1:�豸���������������߳���׼���ã��ڴ�ʹ��     
 ���� CONTROL_RET_FAIL ����ʧ�ܣ� CONTROL_RET_SUC ���ͳɹ�
 ************************************************************************************************/
INT32 server_com_rev(INT32 u32DeviceFd) {
	unsigned char c;
	INT8 cLoopFlag = 3;
	fd_set ReadSetFD;
	uint8 ret;
	while (1) {
		FD_ZERO(&ReadSetFD);
		FD_SET(u32DeviceFd, &ReadSetFD);
		/* ���õȴ��ĳ�ʱʱ�� */
		if (select(u32DeviceFd + 1, &ReadSetFD, NULL, NULL, NULL) <= 0) {
			printf(" [%s][%s][%d]Server REV is time out .....\n", FILE_LINE);
			cLoopFlag--;
			/* ��ʱ����*/
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

