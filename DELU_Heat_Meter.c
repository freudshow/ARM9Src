#include "includes.h"
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

static uint8 gHeat_RecState = FRAME_START;

/*******************************************************************************/
unsigned char gHeatMeterUartRBuf[HEATMETER_LEN]; //,UART0R_Buf[rcv0_buf_len];
unsigned char gHeatMeterAddrArray_Back[7];
unsigned char gHeatMeterAddrStr[16];
UINT32 gheatmeterDI_Back;

unsigned char rcv1_data_len = 0;
unsigned char frame_len1 = 0;
unsigned char head_len1 = 0;
unsigned char rcv1_state = 0;
unsigned char gHeatMeterSer = 0;
/****************************************************************************/
char gHeatMeter_sqlite_commd[128];
/* 热表命令结构体*/
DELU_HEAT_METER_CMD_T g_stDELU_Heat_Meter_Commd;

/************************************************************************************************
 注意:组装写sqlite的命令
 参数1:协议类型
 参数2:字符串指针
 返回 CONTROL_RET_FAIL 发送失败， CONTROL_RET_SUC 发送成功
 ************************************************************************************************/
UINT8 heat_set_sqlite(MeterFileType *pmf, char *sq_data, char *str) {
	char TempString[100];
	uint8 Heat_MeterAddr[7] = { 0x00 };

	memcpy(Heat_MeterAddr, pmf->Meter_addr, 7);
	PUBLIC_BCDToString(TempString, Heat_MeterAddr, 7);

	debug_info(gDebugModule[TIMING_METER], " the Heatmeter addr is %s\n",
			TempString);

	strcpy(str, "insert into Heat_Meter_Table values ('");
	//拼接表地址
	strcat(str, TempString);
	strcat(str, "',");
	//拼接表ID
	strcat(str, "'");
	strcat(str, (const char *) pmf->Meter_id);
	strcat(str, "',");
	//拼接时间
	strcat(str, "'");
	strcat(str, gReadingDate);
	strcat(str, "',");
	//添加functionID
	strcat(str, "'");
	sprintf(TempString, "%d", METER_FUNCTIONID);
	strcat(str, TempString);
	strcat(str, "',");

	strcat(str, "'");
	sprintf(TempString, "%x", pmf->Subitem);
	strcat(str, TempString);
	strcat(str, "',");
	//debug_info(gDebugModule[TIMING_METER], "[%s]\n", str);

	//拼接数据
	strcat(str, "'");
	strcat(str, sq_data);
	strcat(str, "')");

	//以下数据无用 为了配合数据库表设计结构
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
 注意:接收到完整的数据后写数据库
 参数1:数组指针
 参数2:表类型
 返回 CONTROL_RET_FAIL 发送失败， CONTROL_RET_SUC 发送成功
 ************************************************************************************************/
UINT8 heat_write_date_into_sqlite(MeterFileType *pmf, char *sq_data) {
	char *zErrMsg = 0;
	int rec;
	char sq_dataIn[256] = { 0x00 };

	heat_set_sqlite(pmf, sq_data, sq_dataIn);
	rec = sqlite3_exec(sqlitedb, sq_dataIn, NULL, 0, &zErrMsg);
	if (rec != SQLITE_OK) {
		debug_err(gDebugModule[METER], "[%s][%s][%d] \n", FILE_LINE);
		fprintf(stderr, "SQL error: %s/n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	return rec;

}

/************************************************************************************************
 注意:计算CRC的值
 参数1:要组包的存储区域指针
 参数2:数据域的长度
 返回 CONTROL_RET_FAIL 发送失败， CONTROL_RET_SUC 发送成功
 ************************************************************************************************/
UINT8 Calc_CS_heat(UINT8 *pStart, UINT8 ucLen) {
	UINT8 ucCheck = 0;
	while (ucLen--) {
		ucCheck += *pStart++;
	}
	return ucCheck;
}

/************************************************************************************************
 组包发送抄表指令，地址在每次开机时从bak区取回赋值，所以组包时不用赋值
 参数1:要组包的存储区域指针
 返回 CONTROL_RET_FAIL 发送失败， CONTROL_RET_SUC 发送成功
 ************************************************************************************************/
int CreatFrame_heatmeter(UINT8 * pCommd, DELU_HEAT_METER_CMD_T *pmeter,
		UINT8 *pucOutLen) {
	UINT8 i = 0;
	UINT8 *pTemp = pCommd;
	UINT8 *pCheck = pTemp + DELU_HEAT_METER_COMMOND_FE_NUMBER;

	if (pCommd == NULL) {
		debug_info(gDebugModule[TIMING_METER],
				"[%s][%s][%d] ERROR!  piont is NULL!\n", FILE_LINE);
		return CONTROL_RET_FAIL;
	}

	memset(pTemp, 0xFE, DELU_HEAT_METER_COMMOND_FE_NUMBER);		//前导符
	pTemp += DELU_HEAT_METER_COMMOND_FE_NUMBER;

	pCheck = pTemp;
	*pTemp++ = 0x68;											// 数据帧头
	*pTemp++ = 0x20;

	for (i = 0; i < 7; i++)											// 仪表地址 做逆序
			{
		*pTemp++ = pmeter->ucAddr[i];
	}

	*pTemp++ = pmeter->ucControlCode;							// 控制码
	*pTemp++ = pmeter->ucDataLen;								// 数据域长度

	*pTemp++ = pmeter->ucDILow;
	*pTemp++ = pmeter->ucDIHigh;
	*pTemp++ = gHeatMeterSer;

	memcpy(pTemp, pmeter->aucData, pmeter->ucDataLen - 3);
	pTemp += pmeter->ucDataLen - 3;

	*pTemp++ = Calc_CS_heat(pCheck,
			DELU_HEAT_METER_COMMOND_BASE_LEN + pmeter->ucDataLen);
	*pTemp = 0x16;

	*pucOutLen = DELU_HEAT_METER_COMMOND_BASE_LEN + pmeter->ucDataLen
			+ DELU_HEAT_METER_COMMOND_FE_NUMBER + 2;

	debug_notice(gDebugModule[METER], "the frame is ");
	for (i = 0; i < *pucOutLen; i++) {
		debug_notice(gDebugModule[METER], " %2x", pCommd[i]);
	}
	debug_notice(gDebugModule[METER], "\n");
	return CONTROL_RET_SUC;
}

#if 0
int DELU_Heat_Meter_Packag(void * pCommd, UINT8 *pucOutLen)
{
	UINT8 ucTempLoop = 0;
	DELU_HEAT_METER_CMD_PT pstTempCommd = NULL;
	if((pCommd == NULL) || (pucOutLen == NULL))
	{
		return CONTROL_RET_FAIL;
	}

	pstTempCommd = pCommd;

	pstTempCommd->ucLeadFe1 = 0xfe;
	pstTempCommd->ucLeadFe2 = 0xfe;
	pstTempCommd->ucHead = 0x68;
	pstTempCommd->ucDeviceType = DELU_HEAT_METER_TYPE;
	pstTempCommd->ucControlCode = 0x01;
	pstTempCommd->ucDateLen = 0x03;
	pstTempCommd->ucDateLabel1 = 0x1f;
	pstTempCommd->ucDateLabel2 = 0x90;
	pstTempCommd->ucCmdNumber = 0x00;
	for(ucTempLoop = 4; ucTempLoop < 11; ucTempLoop++)
	{
		((UINT8 *)pstTempCommd)[ucTempLoop] = DeluHeatMeterFamenadd[ucTempLoop - 4];
	}
	pstTempCommd->aucDate[0] = DELU_Heat_Meter_Get_Crc((UINT8 *)pCommd, 0x03);
	pstTempCommd->aucDate[1] = 0x16;
	*pucOutLen = DELU_HEAT_METER_COMMOND_BASE_LEN+ pstTempCommd->ucDateLen +DELU_HEAT_METER_COMMOND_FE_NUMBER+2;

	return CONTROL_RET_SUC;
}
#endif
/*


 INT32 heat_meter_rev_analysis(UINT8 *pOUT, UINT8 *pOUTLen)
 {
 UINT8 i=0;
 UINT32 u32DI = 0;
 DELU_HEAT_METER_CMD_T *pmeter = NULL;

 pmeter = (DELU_HEAT_METER_CMD_T *)gHeatMeterUartRBuf;
 
 while(i++<7)
 {
 if(pmeter->ucAddr[i] != gHeatMeterAddrArray_Back[6-i])	return 1;
 }

 u32DI = (pmeter->ucDIHigh<<8) | pmeter->ucDILow;
 if(u32DI != gheatmeterDI_Back)	return 2;

 for(i=0; i<pmeter->ucDataLen-2; i++)
 {
 *pOUT++ = pmeter->aucData - 0x33;
 }

 *pOUTLen = pmeter->ucDataLen-2;

 return 0;
 }
 */
/************************************************************************************************
 发送抄表热表通过Mbus       
 参数1:设备的描述符，在主线程中准备好，在此使用     
 返回 CONTROL_RET_FAIL 发送失败， CONTROL_RET_SUC 发送成功
 ************************************************************************************************/
INT32 heat_meter_com_send(INT32 u32DeviceFd, DELU_HEAT_METER_CMD_T *pmeter) {
	int i = 0;
	UINT8 ucCommdLen = 0;
	UINT8 CJ188Frame[256] = { 0x00 };

	CreatFrame_heatmeter(CJ188Frame, pmeter, &ucCommdLen);

	debug_notice(gDebugModule[METER], "The Frame is ");

	for (i = 0; i < ucCommdLen; i++) {
		debug_notice(gDebugModule[METER], " %2x", CJ188Frame[i]);
	}
	debug_notice(gDebugModule[METER], "\n");

	if (write(u32DeviceFd, (UINT8 *) &CJ188Frame, ucCommdLen) != ucCommdLen) {
		printf("heat_meter_com_send write error(%d)\n", errno);
	}

	return CONTROL_RET_SUC;
}

uint8 Heat_ReceiveFrameAnalyse(uint8 data) {
	static uint8 Cs = 0x00;
	static uint8 Counter = 0;

	//printf("rev data is %x \n", data);
	switch (gHeat_RecState) {
	case FRAME_START:
		if (data != 0x68)
			return 1;
		Cs = data;
		g_stDELU_Heat_Meter_Commd.ucHead = data;
		gHeat_RecState = FRAME_DEV;
		Counter = 6;
		break;

	case FRAME_DEV:
		if (data != 0x25)
			return 2;
		Cs += data;
		g_stDELU_Heat_Meter_Commd.ucDeviceType = data;
		gHeat_RecState = FRAME_METERADDR;
		Counter = 7;
		break;

	case FRAME_METERADDR:
		debug_debug(gDebugModule[METER], " FRAME_METERADDR data is %x ", data)
		;
		debug_debug(gDebugModule[METER], "Back Addr is %x\n",
				gHeatMeterAddrArray_Back[Counter - 1])
		;
		if (data != gHeatMeterAddrArray_Back[Counter - 1])
			return 3;
		Counter--;
		Cs += data;

		if (Counter == 0x00) {
			memcpy(g_stDELU_Heat_Meter_Commd.ucAddr, gHeatMeterAddrArray_Back,
					7);
			gHeat_RecState = FRAME_CONTROL;
		}
		break;

	case FRAME_CONTROL:
		if (data != 0x81)
			return 4;
		Cs += data;
		g_stDELU_Heat_Meter_Commd.ucControlCode = data;
		gHeat_RecState = FRAME_LENGTH;
		break;

	case FRAME_LENGTH:
		if (data > 0x60)
			return 5;
		Cs += data;
		g_stDELU_Heat_Meter_Commd.ucDataLen = data;
		gHeat_RecState = FRAME_DATA;
		Counter = 0x00;
		break;

	case FRAME_DATA:

		Cs += data;

		//data = data - 0x33;
		if (Counter == 0)
			g_stDELU_Heat_Meter_Commd.ucDILow = data;
		else if (Counter == 1)
			g_stDELU_Heat_Meter_Commd.ucDIHigh = data;
		else if (Counter == 2)
			g_stDELU_Heat_Meter_Commd.ucSer = data;
		{
			g_stDELU_Heat_Meter_Commd.aucData[Counter - 3] = data;
		}
		Counter++;

		if (Counter == (g_stDELU_Heat_Meter_Commd.ucDataLen)) {
			gHeat_RecState = FRAME_CHECK;
		}
		break;

	case FRAME_CHECK:

		if (data != Cs) {
			gHeat_RecState = FRAME_START;
			return 7;
		}

		gHeat_RecState = FRAME_END;
		break;

	case FRAME_END:

		if (data != 0x16)
			return 8;

		gHeat_RecState = FRAME_START;
		return 0;
	default:
		break;
	}
	return 9;
}

/************************************************************************************************
 德鲁热表的接受函数
 参数1:设备的描述符，在主线程中准备好，在此使用     
 返回 CONTROL_RET_FAIL 发送失败， CONTROL_RET_SUC 发送成功
 ************************************************************************************************/
INT32 heat_meter_com_rev(INT32 u32DeviceFd) {
	uint8 ret = 0, c;
	INT8 cLoopFlag = 1;
	fd_set ReadSetFD;
	struct timeval stTimeVal;

	while (cLoopFlag) {
		FD_ZERO(&ReadSetFD);
		FD_SET(u32DeviceFd, &ReadSetFD);
		/* 设置等待的超时时间 */
		stTimeVal.tv_sec = DELU_SEC_TIME_OUT;
		stTimeVal.tv_usec = DELU_USEC_TIME_OUT;
		if (select(u32DeviceFd + 1, &ReadSetFD, NULL, NULL, &stTimeVal) <= 0) {
			debug_notice(gDebugModule[METER],
					"heat_meter REV is time out .....(%d)\n", errno);
			cLoopFlag--;
			/* 超时处理*/
		}
		if (FD_ISSET(u32DeviceFd, &ReadSetFD)) {
			if (read(u32DeviceFd, &c, 1) == 1) {
				ret = Heat_ReceiveFrameAnalyse(c);
				if (ret != 9)
					gHeat_RecState = FRAME_START;

				if (ret == 0) {
					return (CONTROL_RET_SUC);
				} else {
					//printf("Heat meter rev return value is %d\n", ret);
				}
			}
		}
		//printf("\n");
	}
	return (CONTROL_RET_FAIL);
}

UINT8 Heat_meter_com(INT32 u32DeviceFd, MeterFileType *pmf) {
	INT32 ret = 0;
	UINT8 datatemp[50] = { 0x00 };
	INT8 Retry = 1;
	int i = 0;
	char datatempString[100] = { 0x00 };
	char Sq_DataString[200] = { 0x00 };

	//ELECT_DataType elect_data;				//此处可扩展做多数据项读
	DELU_HEAT_METER_CMD_T heat_meter;

	for (i = 0; i < 7; i++) {
		debug_notice(gDebugModule[METER], " %x", pmf->Meter_addr[i]);
	}
	debug_notice(gDebugModule[METER], "\n");

	memcpy(heat_meter.ucAddr, pmf->Meter_addr, 7);
	memcpy(gHeatMeterAddrArray_Back, heat_meter.ucAddr, 7);

	//此处可做循环，来抄读多个数据项,通过传递参数多个标识符的宏定义来
	// 统一判断不同协议下读取相同数据项

	while (Retry-- > 0) {
		debug_notice(gDebugModule[METER], "Protocol_type is %d\n",
				pmf->Protocol_type);
		if (pmf->Protocol_type == 1) {
			heat_meter.ucControlCode = 0x01;
			heat_meter.ucDataLen = 0x03;
			heat_meter.ucDILow = 0x1F;
			heat_meter.ucDIHigh = 0x90;

			gheatmeterDI_Back = (heat_meter.ucDIHigh << 8) | heat_meter.ucDILow;
		} else if (pmf->Protocol_type == 2) {
			heat_meter.ucControlCode = 0x01;
			heat_meter.ucDataLen = 0x03;
			heat_meter.ucDILow = 0x1f;
			heat_meter.ucDIHigh = 0x90;

			gheatmeterDI_Back = (heat_meter.ucDIHigh << 8) | heat_meter.ucDILow;
		} else {
			debug_err(gDebugModule[METER], "Protocol type is not right!\n");
			ret = CONTROL_RET_FAIL;
			goto RealtimeRead;
		}

		debug_debug(gDebugModule[METER], "start com with heat meter!\n");
		heat_meter_com_send(u32DeviceFd, &heat_meter);

		ret = heat_meter_com_rev(u32DeviceFd);
		if (ret) {
			memset(datatemp, 0x00, 3);
			debug_info(gDebugModule[TIMING_METER],
					"[%s][%s][%d]  Analysis HeatMeter Data Error! This Error is %d\n",
					FILE_LINE, ret);
		} else {
			memcpy(datatemp, g_stDELU_Heat_Meter_Commd.aucData,
					g_stDELU_Heat_Meter_Commd.ucDataLen - 3);
			debug_info(gDebugModule[TIMING_METER], "Revcie successful !\n");
			break;
		}

	}
	if (ret)
		strcpy((char *) Sq_DataString, "timout','");
	else
		strcpy((char *) Sq_DataString, "ok','");
	//根据读取的标识符不同来做不同的数据处理，将其转换为字符串格式
	if (datatemp[9] == 0x05) {
		Bcd645DataToAscii((uint8 *) datatempString, (uint8 *) &datatemp[5], 6,
				2);
	} else if (datatemp[9] == 0x08) {

		Bcd645DataToAscii((uint8 *) datatempString, (uint8 *) &datatemp[5], 8,
				0);
		strcat((char *) datatempString, "0");
	} else if (datatemp[9] == 0x0A) {
		Bcd645DataToAscii((uint8 *) datatempString, (uint8 *) &datatemp[5], 8,
				0);
		strcat((char *) datatempString, "000");
	}
	strcat((char *) Sq_DataString, datatempString);

	//多个数据项抄读完成后在此处进行数据库数据的统一插入

	debug_info(gDebugModule[TIMING_METER], "Rcv Heat Data is %s\n",
			Sq_DataString);
	if (pmf->ReadMeter_type == PeriodReadMeter)
		heat_write_date_into_sqlite(pmf, Sq_DataString);
	RealtimeRead: if (pmf->ReadMeter_type == RealtimeReadMeter) {
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

