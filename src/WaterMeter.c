#include "includes.h"
#include "WaterMeter.h"
#include "DELU_Heat_Meter.h"

/****************************************************************************/
#define FRAME_START			0x00
#define FRAME_METERADDR		0x01
#define FRAME_DEV			0x02
#define FRAME_CONTROL		0x03
#define FRAME_LENGTH		0x04
#define FRAME_DATA			0x05
#define FRAME_CHECK			0x06
#define FRAME_END			0x07

static uint8 gWater_RecState = FRAME_START;

/*******************************************************************************/
unsigned char gWaterMeterUartRBuf[HEATMETER_LEN]; //,UART0R_Buf[rcv0_buf_len];
unsigned char gWaterMeterAddrArray_Back[7];
unsigned char gWaterMeterAddrStr[16];
UINT32 gwatermeterDI_Back;

unsigned char gWaterMeterSer = 0;
/****************************************************************************/
char gWaterMeter_sqlite_commd[128];
/* �ȱ�����ṹ��*/
DELU_HEAT_METER_CMD_T g_stWater_Meter_Commd;

/************************************************************************************************
 ע��:��װдsqlite������
 ����1:Э������
 ����2:�ַ���ָ��
 ���� CONTROL_RET_FAIL ����ʧ�ܣ� CONTROL_RET_SUC ���ͳɹ�
 ************************************************************************************************/
UINT8 water_set_sqlite(MeterFileType *pmf, char *sq_data, char *str) {
	char TempString[23];
	uint8 Water_MeterAddr[7] = { 0x00 };

	memcpy(Water_MeterAddr, pmf->Meter_addr, 7);
	PUBLIC_BCDToString(TempString, Water_MeterAddr, 7);

	debug_info(gDebugModule[TIMING_METER], " the Watermeter addr is %s\n",
			TempString);

	strcpy(str, "insert into Water_Meter_Table values ('");
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
	strcat(str, sq_data);
	strcat(str, "')");

	//������������ Ϊ��������ݿ����ƽṹ
#if 0
	strcat(str,"'");
	strcat(str, sq_data);
	strcat(str,"',");

	strcat(str,"'");
	strcat(str, sq_data);
	strcat(str,"')");
#endif 

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
UINT8 water_write_date_into_sqlite(MeterFileType *pmf, char *sq_data) {
	char *zErrMsg = 0;
	int rec;
	char sq_dataIn[256] = { 0x00 };

	water_set_sqlite(pmf, sq_data, sq_dataIn);
	rec = sqlite3_exec(sqlitedb, sq_dataIn, NULL, 0, &zErrMsg);
	if (rec != SQLITE_OK) {
		printf("[%s][%s][%d] \n", FILE_LINE);
		fprintf(stderr, "SQL error: %s/n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	return rec;

}

/************************************************************************************************
 ע��:����CRC��ֵ
 ����1:Ҫ����Ĵ洢����ָ��
 ����2:������ĳ���
 ���� CONTROL_RET_FAIL ����ʧ�ܣ� CONTROL_RET_SUC ���ͳɹ�
 ************************************************************************************************/
UINT8 Calc_CS_water(UINT8 *pStart, UINT8 ucLen) {
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
int CreatFrame_watermeter(UINT8 * pCommd, DELU_HEAT_METER_CMD_T *pmeter,
		UINT8 *pucOutLen) {
	UINT8 i = 0;
	UINT8 *pTemp = pCommd;
	UINT8 *pCheck = pTemp + WATER_METER_COMMOND_FE_NUMBER;

	if (pCommd == NULL) {
		debug_info(gDebugModule[TIMING_METER],
				"[%s][%s][%d] ERROR!  piont is NULL!\n", FILE_LINE);
		return CONTROL_RET_FAIL;
	}

	memset(pTemp, 0xFE, WATER_METER_COMMOND_FE_NUMBER);		//ǰ����
	pTemp += WATER_METER_COMMOND_FE_NUMBER;

	pCheck = pTemp;
	*pTemp++ = 0x68;											// ����֡ͷ
	*pTemp++ = 0x10;

	for (i = 0; i < 7; i++)											// �Ǳ��ַ ������
			{
		*pTemp++ = pmeter->ucAddr[i];
	}

	*pTemp++ = pmeter->ucControlCode;							// ������
	*pTemp++ = pmeter->ucDataLen;								// �����򳤶�

	*pTemp++ = pmeter->ucDILow;
	*pTemp++ = pmeter->ucDIHigh;
	//*pTemp++ = gWaterMeterSer++;
	*pTemp++ = 0x01;

	memcpy(pTemp, pmeter->aucData, pmeter->ucDataLen - 3);
	pTemp += pmeter->ucDataLen - 3;

	*pTemp++ = Calc_CS_water(pCheck,
			WATER_METER_COMMOND_BASE_LEN + pmeter->ucDataLen);
	*pTemp = 0x16;

	*pucOutLen = WATER_METER_COMMOND_BASE_LEN + pmeter->ucDataLen
			+ WATER_METER_COMMOND_FE_NUMBER + 2;

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
INT32 water_meter_com_send(INT32 u32DeviceFd, DELU_HEAT_METER_CMD_T *pmeter) {
	int i = 0;
	UINT8 ucCommdLen = 0;
	UINT8 CJ188Frame[256] = { 0x00 };

	CreatFrame_watermeter(CJ188Frame, pmeter, &ucCommdLen);

	debug_notice(gDebugModule[METER], "The Frame is ");
	for (i = 0; i < ucCommdLen; i++) {
		debug_notice(gDebugModule[METER], " %x", CJ188Frame[i]);
	}
	debug_notice(gDebugModule[METER], "\n");

	write(u32DeviceFd, (UINT8 *) &CJ188Frame, ucCommdLen);

	return CONTROL_RET_SUC;
}

uint8 Water_ReceiveFrameAnalyse(uint8 data) {
	static uint8 Cs = 0x00;
	static uint8 Counter = 0;

	//printf("rev data is %x ", data);
	switch (gWater_RecState) {
	case FRAME_START:
		if (data != 0x68)
			return 1;
		Cs = data;
		g_stWater_Meter_Commd.ucHead = data;
		gWater_RecState = FRAME_DEV;
		Counter = 6;
		break;

	case FRAME_DEV:
		if (data != 0x10)
			return 2;
		Cs += data;
		g_stWater_Meter_Commd.ucDeviceType = data;
		gWater_RecState = FRAME_METERADDR;
		Counter = 7;
		break;

	case FRAME_METERADDR:
		//printf(" FRAME_METERADDR data is %x ", data);
		//printf("Back Addr is %x\n", gWaterMeterAddrArray_Back[Counter-1]);
		debug_debug(gDebugModule[METER], " FRAME_METERADDR data is %x ", data)
		;
		debug_debug(gDebugModule[METER], "Back Addr is %x\n",
				gWaterMeterAddrArray_Back[Counter - 1])
		;
		if (data != gWaterMeterAddrArray_Back[Counter - 1])
			return 3;
		Counter--;
		Cs += data;

		if (Counter == 0x00) {
			memcpy(g_stWater_Meter_Commd.ucAddr, gWaterMeterAddrArray_Back, 7);
			gWater_RecState = FRAME_CONTROL;
		}
		break;

	case FRAME_CONTROL:
		if (data != 0x81)
			return 4;
		Cs += data;
		g_stWater_Meter_Commd.ucControlCode = data;
		gWater_RecState = FRAME_LENGTH;
		break;

	case FRAME_LENGTH:
		if (data > 0x60)
			return 5;
		Cs += data;
		g_stWater_Meter_Commd.ucDataLen = data;
		gWater_RecState = FRAME_DATA;
		Counter = 0x00;
		break;

	case FRAME_DATA:

		Cs += data;

		//data = data - 0x33;
		if (Counter == 0)
			g_stWater_Meter_Commd.ucDILow = data;
		else if (Counter == 1)
			g_stWater_Meter_Commd.ucDIHigh = data;
		else if (Counter == 2)
			g_stWater_Meter_Commd.ucSer = data;
		{
			g_stWater_Meter_Commd.aucData[Counter - 3] = data;
		}
		Counter++;

		if (Counter == (g_stWater_Meter_Commd.ucDataLen)) {
			gWater_RecState = FRAME_CHECK;
		}
		break;

	case FRAME_CHECK:

		if (data != Cs) {
			gWater_RecState = FRAME_START;
			return 7;
		}

		gWater_RecState = FRAME_END;
		break;

	case FRAME_END:

		if (data != 0x16)
			return 8;

		gWater_RecState = FRAME_START;
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
INT32 water_meter_com_rev(INT32 u32DeviceFd) {
	uint8 ret = 0, c;
	INT8 cLoopFlag = 1;
	fd_set ReadSetFD;
	struct timeval stTimeVal;

	while (cLoopFlag) {
		FD_ZERO(&ReadSetFD);
		FD_SET(u32DeviceFd, &ReadSetFD);
		/* ���õȴ��ĳ�ʱʱ�� */
		stTimeVal.tv_sec = WATER_SEC_TIME_OUT;
		stTimeVal.tv_usec = WATER_USEC_TIME_OUT;
		if (select(u32DeviceFd + 1, &ReadSetFD, NULL, NULL, &stTimeVal) <= 0) {
			printf("water_meter REV is time out .....\n");
			cLoopFlag--;
			/* ��ʱ����*/
		}
		if (FD_ISSET(u32DeviceFd, &ReadSetFD)) {
			if (read(u32DeviceFd, &c, 1) == 1) {
				ret = Water_ReceiveFrameAnalyse(c);
				if (ret != 9)
					gWater_RecState = FRAME_START;
				//printf("Water meter rev return value is %d", ret);
				if (ret == 0)
					return (CONTROL_RET_SUC);
			}
		}
		//printf("\n");
	}
	return (CONTROL_RET_FAIL);
}

UINT8 Water_meter_com(INT32 u32DeviceFd, MeterFileType *pmf) {
	INT32 ret = 0;
	int retry = 3;
	UINT8 datatemp[50] = { 0x00 };
	int8 i = 0;
	char datatempString[100] = { 0x00 };
	char Sq_DataString[200] = { 0x00 };

	//ELECT_DataType elect_data;				//�˴�����չ�����������
	DELU_HEAT_METER_CMD_T water_meter;

	for (i = 0; i < 7; i++) {
		printf(" %x", pmf->Meter_addr[i]);
	}
	printf("\n");

	memcpy(water_meter.ucAddr, pmf->Meter_addr, 7);
	memcpy(gWaterMeterAddrArray_Back, water_meter.ucAddr, 7);

	//�˴�����ѭ�������������������,ͨ�����ݲ��������ʶ���ĺ궨����
	// ͳһ�жϲ�ͬЭ���¶�ȡ��ͬ������

	while (retry-- > 0) {
		printf("Protocol_type is %d\n", pmf->Protocol_type);
		if (pmf->Protocol_type == 1) {
			water_meter.ucControlCode = 0x01;
			water_meter.ucDataLen = 0x03;
			water_meter.ucDILow = 0x90;
			water_meter.ucDIHigh = 0x1F;

			gwatermeterDI_Back = (water_meter.ucDIHigh << 8)
					| water_meter.ucDILow;
		} else if (pmf->Protocol_type == 2) {
			water_meter.ucControlCode = 0x01;
			water_meter.ucDataLen = 0x03;
			water_meter.ucDILow = 0x90;
			water_meter.ucDIHigh = 0x1F;

			gwatermeterDI_Back = (water_meter.ucDIHigh << 8)
					| water_meter.ucDILow;
		} else {
			ret = CONTROL_RET_FAIL;
			goto RealtimeReadMeter;
		}

		printf("start com with water meter!\n");
		water_meter_com_send(u32DeviceFd, &water_meter);

		ret = water_meter_com_rev(u32DeviceFd);
		if (ret) {
			memset(datatemp, 0x00, 3);
			debug_info(gDebugModule[TIMING_METER],
					"[%s][%s][%d]  Analysis WaterMeter Data Error! This Error is %d\n",
					FILE_LINE, ret);
		} else {
			memcpy(datatemp, g_stWater_Meter_Commd.aucData,
					g_stWater_Meter_Commd.ucDataLen - 3);
			debug_info(gDebugModule[TIMING_METER], "Revcie successful !\n");
			break;
		}
	}
	if (ret)
		strcpy((char *) Sq_DataString, "timout','");
	else
		strcpy((char *) Sq_DataString, "ok','");
	//���ݶ�ȡ�ı�ʶ����ͬ������ͬ�����ݴ�������ת��Ϊ�ַ�����ʽ
	Bcd645DataToAscii((uint8 *) datatempString, (uint8 *) datatemp, 6, 2);
	strcat((char *) Sq_DataString, datatempString);
	//������������ɺ��ڴ˴��������ݿ����ݵ�ͳһ����

	debug_info(gDebugModule[TIMING_METER], "Rcv Water Data is %s\n",
			Sq_DataString);
	if (pmf->ReadMeter_type == PeriodReadMeter)
		water_write_date_into_sqlite(pmf, Sq_DataString);
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
	return ret;
}

