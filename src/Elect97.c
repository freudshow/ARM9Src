#include "includes.h"
#include "Elect97.h"
#include "CommandDeal.h"

/*******************************************************************************/
#define FRAME_START			0x00
#define FRAME_METERADDR		0x01
#define FRAME_STARTSCE		0x02
#define FRAME_CONTROL		0x03
#define FRAME_LENGTH		0x04
#define FRAME_DATA			0x05
#define FRAME_CHECK			0x06
#define FRAME_END			0x07

static uint8 gElect_RecState = FRAME_START;

/*******************************************************************************/
unsigned char g_aucElect97UartRBuf[ELECT97_RCV_BUFF_LEN]; //,UART0R_Buf[rcv0_buf_len];

UINT8 gelect97FrameAddr_Back[7] = { 0x00 };
UINT32 gelect97DI_Back = 0x00;
unsigned char g_aucElect97MeteraddString[16];
unsigned char g_aucElect97Meteradd[6] = { 0x41, 0x06, 0x09, 0x12, 0x00, 0x00 };
unsigned char g_ucElect97RcvDateLen = 0;
unsigned char g_ucElect97FrameLen = 0;
unsigned char g_ucElect97HeadLen = 0;
unsigned char g_ucElectRcvState = 0;
/****************************************************************************/
/****************************************************************************/
unsigned char elect97_sqlite_commd[256];

/* �ȱ�����ṹ��*/
ELECTR97_METER_CMD_T g_stElect97_Meter_Commd;
ELECTR97_METER_CMD_T gElect97_MeterRev;
/************************************************************************************************
 ע��:��װдsqlite������
 ����1:Э������
 ����2:�ַ���ָ��
 ���� CONTROL_RET_FAIL ����ʧ�ܣ� CONTROL_RET_SUC ���ͳɹ�
 ************************************************************************************************/
UINT8 elect97_set_sqlite(MeterFileType *pmf, char *sq_data, char *str) {
	char TempString[23];
	uint8 Elect_MeterAddr[6] = { 0x00 };

	memcpy(Elect_MeterAddr, pmf->Meter_addr, 6);
	PUBLIC_BCDToString(TempString, Elect_MeterAddr, 6);

	debug_info(gDebugModule[TIMING_METER], " the meter addr is %s\n",
			TempString);

	strcpy(str, "insert into Electric_Meter_Table values ('");
	//ƴ�ӱ��ַ
	strcat(str, TempString);
	strcat(str, "',");
	//ƴ�ӱ�ID
	strcat(str, "'");
	strcat(str, (const char *) pmf->Meter_id);
	strcat(str, "',");

	//ƴ��ʱ��
	strcat(str, "'");
	strcat(str, gReadingDate);
	strcat(str, "',");

	//���functionID
	strcat(str, "'");
	sprintf(TempString, "%d", METER_FUNCTIONID);
	strcat(str, TempString);
	strcat(str, "',");

	strcat(str, "'");
	sprintf(TempString, "%x", pmf->Subitem);
	strcat(str, TempString);
	strcat(str, "',");
	//debug_info(gDebugModule[TIMING_METER], "[%s]\n", str);

	//ƴ������
	strcat(str, "'");
	strcat(str, (const char *) sq_data);
	strcat(str, "',");

	//������������ Ϊ��������ݿ����ƽṹ
	strcat(str, "'");
	strcat(str, "0.00");
	strcat(str, "',");

	strcat(str, "'");
	strcat(str, "0.00");
	strcat(str, "')");

	debug_info(gDebugModule[TIMING_METER], "[%s][%s][%d] \n", FILE_LINE);
	debug_info(gDebugModule[TIMING_METER], "[%s]\n", str);

	return 0;
}

/************************************************************************************************
 ע��:���յ����������ݺ�д���ݿ�
 ����1:����ָ��
 ����2:������
 ���� CONTROL_RET_FAIL ����ʧ�ܣ� CONTROL_RET_SUC ���ͳɹ�
 ************************************************************************************************/
UINT8 elect97_write_date_into_sqlite(MeterFileType *pmf, char *sq_data) {
	char *zErrMsg = 0;
	int rec;
	char sq_dataIn[256] = { 0x00 };

	elect97_set_sqlite(pmf, sq_data, sq_dataIn);
	rec = sqlite3_exec(sqlitedb, sq_dataIn, NULL, 0, &zErrMsg);
	if (rec != SQLITE_OK) {
		printf("[%s][%s][%d] \n", FILE_LINE);
		fprintf(stderr, "SQL error: %s/n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	return rec;
}

UINT8 elect97_meter_com(INT32 u32DeviceFd, MeterFileType *pmf) {
	INT32 ret = 0;
	UINT8 datatemp[50] = { 0x00 };
	INT8 Retry = 3;
	//UINT8 lenTemp = 0;
	unsigned char datatempString[100] = { 0x00 };
	unsigned char Sq_DataString[200] = { 0x00 };
	//ELECT_DataType elect_data;				//�˴�����չ�����������ȡ
	ELECTR97_METER_CMD_T elect_meter;
	int i = 0;

	for (i = 0; i < 6; i++) {
		debug_notice(gDebugModule[METER], " %x", pmf->Meter_addr[i]);
	}
	debug_notice(gDebugModule[METER], "\n");

	memcpy(elect_meter.ucAddr, pmf->Meter_addr, 6);
	memcpy(gelect97FrameAddr_Back, elect_meter.ucAddr, 6);

	//�˴�����ѭ�������������������,ͨ�����ݲ��������ʶ���ĺ궨����
	// ͳһ�жϲ�ͬЭ���¶�ȡ��ͬ������
	while (Retry-- > 0) {
		debug_notice(gDebugModule[METER], "Protocol_type is %d\n",
				pmf->Protocol_type);
		if (pmf->Protocol_type == 1) {
			elect_meter.ucControlCode = 0x01;
			elect_meter.ucDataLen = 0x02;
			elect_meter.ucDILow = 0x00;
			elect_meter.ucDIHigh = 0x00;

			gelect97DI_Back = (elect_meter.ucDIHigh << 8) | elect_meter.ucDILow;
		} else if (pmf->Protocol_type == 2) {
			elect_meter.ucControlCode = 0x01;
			elect_meter.ucDataLen = 0x02;
			elect_meter.ucDILow = 0x10;
			elect_meter.ucDIHigh = 0x90;

			gelect97DI_Back = (elect_meter.ucDIHigh << 8) | elect_meter.ucDILow;
		} else {
			ret = CONTROL_RET_FAIL;
			goto RealtimeReadMeter;
		}

		debug_notice(gDebugModule[METER], "start com by 485\n");
		ioctl(g_uiIoControl, RS4852_C31, RS_485_SED);
		usleep(100);
		elect97_meter_com_send(u32DeviceFd, &elect_meter);
		//��С��
		//usleep(180000);
		//�󻪴��
		usleep(165000);
		ioctl(g_uiIoControl, RS4852_C31, RS_485_RECV);

		ret = elect97_meter_com_rev(u32DeviceFd);
		if (ret) {
			memset(datatemp, 0x00, 3);
			debug_info(gDebugModule[TIMING_METER],
					"[%s][%s][%d]  Analysis electMeter Data Error! This Error is %d\n",
					FILE_LINE, ret);
		} else {
			memcpy(datatemp, gElect97_MeterRev.aucData,
					gElect97_MeterRev.ucDataLen - 2);
			debug_info(gDebugModule[TIMING_METER], "Revcie successful !\n");
			break;
		}
	}
	if (ret)
		strcpy((char *) Sq_DataString, "timout','");
	else
		strcpy((char *) Sq_DataString, "ok','");
	//���ݶ�ȡ�ı�ʶ����ͬ������ͬ�����ݴ�������ת��Ϊ�ַ�����ʽ
	Bcd645DataToAscii(datatempString, datatemp, 4, 2);
	strcat((char*) Sq_DataString, (char*) datatempString);
	//������������ɺ��ڴ˴��������ݿ����ݵ�ͳһ����

	debug_info(gDebugModule[TIMING_METER], "\n********* %s ************\n",
			Sq_DataString);
	if (pmf->ReadMeter_type == PeriodReadMeter)
		elect97_write_date_into_sqlite(pmf, (char *) Sq_DataString);
	RealtimeReadMeter: if (pmf->ReadMeter_type == RealtimeReadMeter) {
		if (ret == CONTROL_RET_SUC)
			sprintf(gxml_realtime_rsp_t.error, "success");
		else
			sprintf(gxml_realtime_rsp_t.error, "fail");
		stringCopy((char *) gxml_realtime_rsp_t.result, (char *) Sq_DataString,
				'\'');
	} else
	debug_info(gDebugModule[TIMING_METER], "the  ReadMeter_type = %d error!\n",
			pmf->ReadMeter_type);
	return 0;
}

/************************************************************************************************
 ע��:����CRC��ֵ
 ����1:Ҫ����Ĵ洢����ָ��
 ����2:������ĳ���
 ���� CONTROL_RET_FAIL ����ʧ�ܣ� CONTROL_RET_SUC ���ͳɹ�
 ************************************************************************************************/
UINT8 Calc_CS_elect(UINT8 *pStart, UINT8 ucLen) {
	UINT8 ucCheck = 0;
	while (ucLen--) {
		ucCheck += *pStart++;
	}
	return ucCheck;
}

/************************************************************************************************
 ������ͳ���ָ���ַ��ÿ�ο���ʱ��bak��ȡ�ظ�ֵ���������ʱ���ø�ֵ
 ����1:Ҫ����Ĵ洢����ָ��
 ���� CONTROL_RET_FAIL ����ʧ�ܣ� CONTROL_RET_SUC ���ͳɹ�
 ************************************************************************************************/
int CreatFrame_elect(UINT8 * pCommd, ELECTR97_METER_CMD_T *pmeter,
		UINT8 *pucOutLen) {
	UINT8 i = 0;
	//UINT8 ucTempLoop = 0;
	UINT8 *pTemp = pCommd;
	UINT8 *pCheck = pTemp + ELECT97_METER_COMMOND_FE_NUMBER;

	if (pCommd == NULL) {
		debug_info(gDebugModule[TIMING_METER],
				"[%s][%s][%d] ERROR!  piont is NULL!\n", FILE_LINE);
		return CONTROL_RET_FAIL;
	}

	memset(pTemp, 0xFE, ELECT97_METER_COMMOND_FE_NUMBER);		//ǰ����
	pTemp += ELECT97_METER_COMMOND_FE_NUMBER;

	*pTemp++ = 0x68;											// ����֡ͷ

	for (i = 0; i < 6; i++)											// �Ǳ��ַ ������
			{
		*pTemp++ = pmeter->ucAddr[i];
	}

	*pTemp++ = 0x68;											// �ڶ���֡ͷ

	*pTemp++ = pmeter->ucControlCode;							// ������
	*pTemp++ = pmeter->ucDataLen;								// �����򳤶�

	*pTemp++ = pmeter->ucDILow + 0x33;
	*pTemp++ = pmeter->ucDIHigh + 0x33;

	for (i = 0; i < pmeter->ucDataLen - 2; i++) {
		pmeter->aucData[i] = pmeter->aucData[i] + 0x33;
	}
	memcpy(pTemp, pmeter->aucData, pmeter->ucDataLen - 2);
	pTemp += pmeter->ucDataLen - 2;

	*pTemp++ = Calc_CS_elect(pCheck,
			ELECT97_METER_COMMOND_BASE_LEN + pmeter->ucDataLen);
	*pTemp = 0x16;

	*pucOutLen = ELECT97_METER_COMMOND_BASE_LEN + pmeter->ucDataLen
			+ ELECT97_METER_COMMOND_FE_NUMBER + 2;

	printf("the frame is ");
	for (i = 0; i < *pucOutLen; i++) {
		printf(" %2x", pCommd[i]);
	}
	printf("\n");
	return CONTROL_RET_SUC;
}
/************************************************************************************************
 ���ͳ����ȱ�ͨ��Mbus       
 ����1:�豸���������������߳���׼���ã��ڴ�ʹ��     
 ���� CONTROL_RET_FAIL ����ʧ�ܣ� CONTROL_RET_SUC ���ͳɹ�
 ************************************************************************************************/
INT32 elect97_meter_com_send(INT32 u32DeviceFd, ELECTR97_METER_CMD_T *pmeter) {
	int i = 0;
	UINT8 ucCommdLen = 0;
	UINT8 DL645Frame[256] = { 0x00 };

	CreatFrame_elect(DL645Frame, pmeter, &ucCommdLen);

	debug_notice(gDebugModule[METER], "The Frame is ");
	for (i = 0; i < ucCommdLen; i++) {
		debug_notice(gDebugModule[METER], " %x", DL645Frame[i]);
	}
	debug_notice(gDebugModule[METER], "\n");

	write(u32DeviceFd, (UINT8 *) &DL645Frame, ucCommdLen);

	return CONTROL_RET_SUC;
}

uint8 elect97_ReceiveFrameAnalyse(uint8 data) {
	static uint8 Cs = 0x00;
	static uint8 Counter = 0;

	//printf("rev data is %x ", data);
	switch (gElect_RecState) {
	case FRAME_START:
		if (data != 0x68)
			return 1;
		Cs = data;
		gElect97_MeterRev.ucFrameHead = data;
		gElect_RecState = FRAME_METERADDR;
		Counter = 6;
		break;

	case FRAME_METERADDR:
		debug_debug(gDebugModule[METER], " FRAME_METERADDR data is %x ", data)
		;
		debug_debug(gDebugModule[METER], "Back Addr is %x\n",
				gelect97FrameAddr_Back[Counter - 1])
		;
		if (data != gelect97FrameAddr_Back[Counter - 1])
			return 2;
		Counter--;
		Cs += data;

		if (Counter == 0x00) {
			memcpy(gElect97_MeterRev.ucAddr, gelect97FrameAddr_Back, 6);
			gElect_RecState = FRAME_STARTSCE;
		}
		break;

	case FRAME_STARTSCE:
		if (data != 0x68)
			return 3;
		Cs += data;
		gElect97_MeterRev.ucSecondHead = data;
		gElect_RecState = FRAME_CONTROL;
		break;

	case FRAME_CONTROL:
		if (data != 0x81)
			return 4;
		Cs += data;
		gElect97_MeterRev.ucControlCode = data;
		gElect_RecState = FRAME_LENGTH;
		break;

	case FRAME_LENGTH:
		if (data > 0x60)
			return 5;
		Cs += data;
		gElect97_MeterRev.ucDataLen = data;
		gElect_RecState = FRAME_DATA;
		Counter = 0x00;
		break;

	case FRAME_DATA:

		Cs += data;

		data = data - 0x33;
		if (Counter == 0)
			gElect97_MeterRev.ucDILow = data;
		else if (Counter == 1)
			gElect97_MeterRev.ucDIHigh = data;
		else {
			gElect97_MeterRev.aucData[Counter - 2] = data;
		}
		Counter++;

		if (Counter == (gElect97_MeterRev.ucDataLen)) {
			gElect_RecState = FRAME_CHECK;
		}
		break;

	case FRAME_CHECK:

		if (data != Cs) {
			gElect_RecState = FRAME_START;
			return 7;
		}

		gElect_RecState = FRAME_END;
		break;

	case FRAME_END:

		if (data != 0x16)
			return 8;

		gElect_RecState = FRAME_START;
		return 0;
	default:
		break;
	}
	return 9;
}

/************************************************************************************************
 ��³�ȱ�Ľ��ܺ���
 ����1:�豸���������������߳���׼���ã��ڴ�ʹ��     
 ���� CONTROL_RET_FAIL ����ʧ�ܣ� CONTROL_RET_SUC ���ͳɹ�
 ************************************************************************************************/

INT32 elect97_meter_com_rev(INT32 u32DeviceFd) {
	unsigned char c;								//,crc,i;
	INT8 cLoopFlag = 1;
	UINT8 ret = 0;

	fd_set ReadSetFD;
	struct timeval stTimeVal;

	gElect_RecState = FRAME_START;
	while (cLoopFlag) {
		FD_ZERO(&ReadSetFD);
		FD_SET(u32DeviceFd, &ReadSetFD);
		/* ���õȴ��ĳ�ʱʱ�� */
		stTimeVal.tv_sec = ELECT97_SEC_TIME_OUT;
		stTimeVal.tv_usec = ELECT97_USEC_TIME_OUT;
		if (select(u32DeviceFd + 1, &ReadSetFD, NULL, NULL, &stTimeVal) <= 0) {
			debug_notice(gDebugModule[METER], "ELECT97 REV is time out .....\n");
			cLoopFlag--;
			/* ��ʱ����*/
		}
		if (FD_ISSET(u32DeviceFd, &ReadSetFD)) {
			if (read(u32DeviceFd, &c, 1) == 1) {
				ret = elect97_ReceiveFrameAnalyse(c);
				if (ret != 9)
					gElect_RecState = FRAME_START;

				//printf("elect97 rev return value is %d", ret);

				if (ret == 0)
					return (CONTROL_RET_SUC);

			}
		}
		//printf("\n");
	}
	return (CONTROL_RET_FAIL);
}

